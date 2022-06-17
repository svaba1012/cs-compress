#include "csw.h"
#include "vcl.h"
#include "huffman.h"
#include "complex.h"
#include "fft.h"
#include <math.h>

// kompresovani format audio wav fajla sa gubicima
// Ogranicenje je fft koriscen od druga sa etf-a skinut sa github-a
// !!!Napravio sam malo brzu verziju fft, probacu da je iskoristim
// Ideja je da se ucitani podaci podele u blokove velicine 2^16 zbog ogranicenja fft
// Onda se odradi fft na podacima
// Zatim se kvantizira amplituda i faza fft sa korakom DELTA_DB i DELTA_PHASE

#define CSW_BLOCK_SIZE 65536
#define CSW_BLOCK_POW2 16
#define DELTA_DB 3
#define DELTA_PHASE 0.1
#define MAX_DIF_PHASE ((int)(6.5/DELTA_PHASE))
#define PHASE_OFFSET ((int)(3.2/DELTA_PHASE))

int min_amp_db = 0;

//zaglavlje wav fajla
struct wav_header{
    //string "RIFF"
    char chunkID[4];
    //velicina fajla minus 8 bajtova
    int chunk_size;
    //string "WAVE"
    char format[4];
    //string "fmt "
    char subchunk1ID[4];
    int subchunk1_size;
    //treba da bude 1
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short one_sample_size;
    short bits_per_sample;
    //string "data"
    char subchunk2ID[4];
    int subchunk2_size;
}__attribute__((packed));


//dodatno zaglavlje specifcno za ovaj format
struct csw_additional_header{
    // originalni wav header
    struct wav_header orig_header;
    // podaci neophodni za dekompresiju amplitude fft
    int code_table_size1;
    short last_end_bit1;
    int original_size1;
    int data_size1;
    // podaci neophodni za dekompresiju faze fft
    int data_size2;
    int code_table_size2;
    short last_end_bit2;
    int original_size2;
}__attribute__((packed));


bool csw_check(FILE* file){
    printf("Provera csw formata\n");
    fseek(file, 0, SEEK_SET);
    struct wav_header* wav_header = malloc(sizeof(struct wav_header));
    if(wav_header == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    fread(wav_header, sizeof(struct wav_header), 1, file);
    // Ucitiava se header wav fajla i proveravaju se karakteristicni delovi
    bool is_csw = (strncmp(wav_header->chunkID, "RIFF", 4) == 0) && (strncmp(wav_header->subchunk1ID, "fmt ", 4) == 0) && (strncmp(wav_header->format, "WAVE", 4) == 0) && (wav_header->audio_format == 1) && (wav_header->bits_per_sample == 16); 
    free(wav_header);
    return is_csw;
}

// hash funkcija za semstanje simbola amplitude
static int csw_get_index_of_symbol_amp(HUFF_SYM simbol, struct simbol_verovatnoca_kod* simb_kod, int duzina){
    return simbol - min_amp_db;
}

// hash funkcija za smestanje simbola faze
static int csw_get_index_of_symbol_phase(HUFF_SYM simbol, struct simbol_verovatnoca_kod* simb_kod, int duzina){
    // postavljen offset zbog negativnih vrednosti
    return simbol + PHASE_OFFSET;
}


int csw_compress(struct cs_type* cs, FILE* file, FILE* compressed_file){
    // kompresovanje podataka iz wav fajla i pravljenje novog compresovanog csw formata
    printf("Pripremam podatke\n");
    fseek(file, 0, SEEK_SET);

    // alokacija neophodnih blokova
    struct huff_symb_arr* symb_arr_amp = malloc(sizeof(struct huff_symb_arr));
    if(symb_arr_amp == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    struct huff_symb_arr* symb_arr_phase = malloc(sizeof(struct huff_symb_arr));
    if(symb_arr_phase == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    struct csw_additional_header* header2 = malloc(sizeof(struct csw_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    struct wav_header* wav_header = malloc(sizeof(struct wav_header));
    if(wav_header == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    // Ucitavanje wav zaglavlja
    fread(wav_header, sizeof(struct wav_header), 1, file);
    // Ukoliko wav ima listu informacija o umetnicima, izdanju -----> odbacuje se 
    if(strncmp(wav_header->subchunk2ID, "LIST",4) == 0){
        fseek(file, wav_header->subchunk2_size, SEEK_CUR);
        fread(wav_header->subchunk2ID, 1, 4, file);
        fread(&wav_header->subchunk2_size, 1, 4, file);
    }

    int data_size = wav_header->chunk_size - (sizeof(struct wav_header) - 8);
    int data_len = data_size / 2;
    int num_of_ch = wav_header->num_channels;
    int num_of_blocks = data_len / CSW_BLOCK_SIZE / num_of_ch;
    // prostor za ucitavanje jednog bloka
    short* one_block = malloc(num_of_ch*sizeof(short)*CSW_BLOCK_SIZE);
    // ucitani blok konvertovan u complex
    double complex* data_time_domain = malloc(num_of_ch*sizeof(double complex)*CSW_BLOCK_SIZE);
    // fft ucitanog bloka
    double complex* data_freq_domain = malloc(num_of_ch*sizeof(double complex)*CSW_BLOCK_SIZE);
    // pomocni niz za brzi fft
    double complex* fft_help_arr = malloc(sizeof(double complex)*CSW_BLOCK_SIZE);

    // kvantizirane amplitude prve polovine blokova svih blokova 
    HUFF_SYM* data_freq_domain_amp_q = malloc(num_of_ch*CSW_BLOCK_SIZE * (num_of_blocks-1)*sizeof(HUFF_SYM) / 2 + 1);
    // kvantizirane faze prve polovine blokova svih blokova
    HUFF_SYM* data_freq_domain_phase_q = malloc(num_of_ch*CSW_BLOCK_SIZE * (num_of_blocks-1)*sizeof(HUFF_SYM) / 2 + 1) ;
    printf("Pripremam podatke za fft a zatim za huffman-a, moze da potraje...\n");
    
    int* fft_coef = malloc(CSW_BLOCK_SIZE * sizeof(int));
    fft3_spec_calc_coefs(fft_coef, CSW_BLOCK_SIZE);


    HUFF_SYM max_amp_db = 0;

    // iteracija po blokovima
    // POSLEDNJI BLOK SE ZA SAD ODSECA
    for(int block_count = 0; block_count < num_of_blocks-1; block_count++){
        // !!! POSLEDNJI BLOK POSEBNA OBRADA ili ZANEMARITI!!!
        //! provera
        // ucitavanje blok po blok
        fread(one_block, sizeof(short), num_of_ch * CSW_BLOCK_SIZE, file);
        //prepisivanje podataka u kompleksni niz zbog fft
        for(int k = 0, init_pos = 0; k < num_of_ch; k++){
            int base = k * CSW_BLOCK_SIZE;
            int end = CSW_BLOCK_SIZE + base;
            for(int j = base, pos = init_pos; j < end; j++){
                // razdvajanje kanala i upisivanje kanal po kanal a ne naizmenicno kao u wav
                data_time_domain[j] = (double complex)one_block[pos];
                pos += num_of_ch;
            }
            ++init_pos;
        }

        
        //fft bloka svih kanala
        for(int k = 0; k < num_of_ch; k++){
            fft3_spec(data_time_domain + k * CSW_BLOCK_SIZE, data_freq_domain + k * CSW_BLOCK_SIZE, fft_help_arr, fft_coef, CSW_BLOCK_POW2);
            // fft(data_time_domain + k * CSW_BLOCK_SIZE, data_freq_domain + k * CSW_BLOCK_SIZE, CSW_BLOCK_SIZE);    
        }

        // kvantiziranje amplitude i faze i odsecanje druge polovine fft zbog osobine parnosti realnih delova i neparnosti imaginarnih
        double complex* data_freq = data_freq_domain;
        for(int j = 0; j < num_of_ch; j++){
            int base = block_count * CSW_BLOCK_SIZE * num_of_ch / 2 + j * CSW_BLOCK_SIZE / 2;
            for(int i = 0; i <= CSW_BLOCK_SIZE / 2; i++){
                double temp = cabsl(data_freq[i + j * CSW_BLOCK_SIZE]);
                data_freq_domain_amp_q[base + i] =(HUFF_SYM) (20*log10l(temp) / DELTA_DB);
                if(data_freq_domain_amp_q[base + i] > max_amp_db){
                    max_amp_db = data_freq_domain_amp_q[base+i];
                }
                if(data_freq_domain_amp_q[base + i] < min_amp_db){
                    min_amp_db = data_freq_domain_amp_q[base+i];
                }
            }
            for(int i = 0; i <= CSW_BLOCK_SIZE / 2; i++){
                data_freq_domain_phase_q[base + i] = (HUFF_SYM)(cargl(data_freq[i + j * CSW_BLOCK_SIZE]) / DELTA_PHASE);
            }
        }
    }
    free(one_block);
    free(data_time_domain);
    free(data_freq_domain);
    free(fft_coef);
    free(fft_help_arr);
    max_amp_db -= min_amp_db;
    max_amp_db++;
    // printf("Max je %d\n", max_amp_db);
    // printf("Min je %d\n", min_amp_db);

    // Pakovanje podataka za Huffmanovo kodovanje
    symb_arr_amp->arr = data_freq_domain_amp_q;
    symb_arr_amp->len = num_of_ch * CSW_BLOCK_SIZE * (num_of_blocks - 1)/2+1;
    symb_arr_phase->arr = data_freq_domain_phase_q;
    symb_arr_phase->len = num_of_ch * CSW_BLOCK_SIZE * (num_of_blocks - 1)/2+1;

    // Kodovanje amplitude
    struct huff_tree* tree_amp = huffman_get_tree(symb_arr_amp, cs->max_possible_symbols, csw_get_index_of_symbol_amp, NULL);
    struct huff_coded_arr* compressed_amp = huffman_code(tree_amp, symb_arr_amp);
    // Oslobadjanje originalnih podataka amplitude
    free(symb_arr_amp->arr);
    free(symb_arr_amp);

    // Kodovanje faze
    printf("Pravim huffman-ovo stablo i kodujem\n");
    struct huff_tree* tree_phase = huffman_get_tree(symb_arr_phase, cs->max_possible_symbols, csw_get_index_of_symbol_phase, NULL);
    struct huff_coded_arr* compressed_phase = huffman_code(tree_phase, symb_arr_phase);
    // Oslobadjanje originalnih podataka faze
    free(symb_arr_phase->arr);
    free(symb_arr_phase);

    // pravljenje fajla za dekompresovanje
    printf("Pravim fajl i upisujem\n");
    
    // Cuvanje primarnog hedera
    vcl_cs_header_init(header, cs);
    header->additional_header_size = sizeof(struct csw_additional_header);
    header->data_field_size = compressed_phase->len + compressed_amp->len;
    fwrite(header, sizeof(struct cs_header), 1, compressed_file);
    free(header);

    printf("Pravim drugi heder\n"); 
    header2->code_table_size1 = max_amp_db * sizeof(struct simbol_verovatnoca_kod);
    header2->code_table_size2 = MAX_DIF_PHASE * sizeof(struct simbol_verovatnoca_kod);
    header2->last_end_bit1 = compressed_amp->last_end_bit;
    header2->last_end_bit2 = compressed_phase ->last_end_bit;
    header2->original_size1 = compressed_amp->original_len;
    header2->original_size2 = compressed_phase->original_len;
    // printf("\n%d\n", compressed_phase->original_len);
    header2->data_size1 = compressed_amp->len;
    header2->data_size2 = compressed_phase->len;
    header2->orig_header = *wav_header;
    // Cuvanje sekundarnog hedera
    fwrite(header2, sizeof(struct csw_additional_header), 1 , compressed_file); 
    free(header2);
    free(wav_header);
    // Cuvanje tabele verovatnoca
    printf("Upisujem tabele\n");
    fwrite(tree_amp->simb_kod, sizeof(struct simbol_verovatnoca_kod), max_amp_db, compressed_file);
    fwrite(tree_phase->simb_kod, sizeof(struct simbol_verovatnoca_kod), MAX_DIF_PHASE, compressed_file);
    // oslobadjanje memorije huff stabala
    // !!!
    free_tree(tree_amp);
    free_tree(tree_phase);
    // !!!
    // Cuvanje kompresovanih podataka i oslobadjanje memorije istih
    printf("Upisujem podatke\n");
    fwrite(compressed_amp->arr, 1, compressed_amp->len, compressed_file);
    free(compressed_amp->arr);
    free(compressed_amp);
    fwrite(compressed_phase->arr, 1, compressed_phase->len, compressed_file);
    free(compressed_phase->arr);
    free(compressed_phase);
    printf("Fajl je gotov, cuvam ga\n");
    // Zatvaranje fajla
    
}

int csw_decompress(struct cs_type* cs, FILE* file, FILE* decompressed_file){
    // Dekompresovanje podataka iz csw format i formiranje wav formata
    printf("Pocinjem dekompresovanje\n");
    fseek(file, 0, SEEK_SET);
    // Ucitavanje primarnog hedera csw fajla
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(header, sizeof(struct cs_header), 1 ,file);
    free(header);
    // Ucitavanje sekundarnog hedera csw fajla
    struct csw_additional_header* header2 = malloc(sizeof(struct csw_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    fread(header2, sizeof(struct csw_additional_header), 1, file);
    // Ucitavanje tabela verovatnoce 
    printf("Ucitavanje tabele verovatnoca\n");
    struct simbol_verovatnoca_kod* verovatnoce_amp = malloc(header2->code_table_size1);
    if (verovatnoce_amp == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(verovatnoce_amp,1, header2->code_table_size1, file);
    struct simbol_verovatnoca_kod* verovatnoce_phase = malloc(header2->code_table_size2);
    if (verovatnoce_phase == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(verovatnoce_phase,1, header2->code_table_size2, file);
    // Ucitavanje kompresovanih podataka i upakivanje za Huff dekodovanje
    printf("Ucitavanje podataka\n");
    struct huff_coded_arr* coded_arr_amp = malloc(sizeof(struct huff_coded_arr));
    if(coded_arr_amp == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    coded_arr_amp->len = header2->data_size1;
    coded_arr_amp->last_end_bit = header2->last_end_bit1;
    coded_arr_amp->original_len = header2->original_size1;
    coded_arr_amp->arr = malloc(coded_arr_amp->len);
    if(coded_arr_amp->arr == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    // Ucitavanje kompresovanih amplituda
    fread(coded_arr_amp->arr, 1, coded_arr_amp->len, file);
    struct huff_coded_arr* coded_arr_phase = malloc(sizeof(struct huff_coded_arr));
    if(coded_arr_phase == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    coded_arr_phase->len = header2->data_size2;
    coded_arr_phase->last_end_bit = header2->last_end_bit2;
    coded_arr_phase->original_len = header2->original_size2;
    coded_arr_phase->arr = malloc(coded_arr_phase->len);
    if(coded_arr_phase->arr == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    // Ucitavanje kompresovanih faza
    fread(coded_arr_phase->arr, 1, coded_arr_phase->len, file);

    // Dekodovanje sa Huffman-om
    struct huff_tree* tree_amp = huffman_get_tree(NULL, header2->code_table_size1 / sizeof(struct simbol_verovatnoca_kod), csw_get_index_of_symbol_amp,verovatnoce_amp);
    printf("Izrada huffman-ovog stabla i dekodovanje\n");
    struct huff_symb_arr* symb_arr_amp = huffman_decode(tree_amp, coded_arr_amp);
    free(coded_arr_amp->arr);
    free(coded_arr_amp);
    printf("Izrada huffman-ovog stabla i dekodovanje faze\n");

    struct huff_tree* tree_phase = huffman_get_tree(NULL, header2->code_table_size2 / sizeof(struct simbol_verovatnoca_kod), csw_get_index_of_symbol_phase,verovatnoce_phase);
    struct huff_symb_arr* symb_arr_phase = huffman_decode(tree_phase, coded_arr_phase);
    free(coded_arr_phase->arr);
    free(coded_arr_phase);
    printf("Pravljnje fajla za dekompresovane podatke\n");
    // oslobadjanje memorije huff stabala
    // !!!
    free_tree(tree_phase);
    free_tree(tree_amp);
    // !!!
    // Otvaranje novog fajla za dekompresovanu verziju wav
    if(decompressed_file == NULL){
        printf("Nemoguce je napraviti fajl\n");
        exit(EXIT_FAILURE);
    }
    // Upisivanje wav header-a
    fwrite(&(header2->orig_header), 1, sizeof(struct wav_header), decompressed_file);
    int data_len = symb_arr_amp->len;
    int num_of_ch = header2->orig_header.num_channels;
    free(header2);
    int num_of_blocks = data_len/ (CSW_BLOCK_SIZE/2) / num_of_ch;
    short* one_block = malloc(num_of_ch*sizeof(short)*CSW_BLOCK_SIZE);
    double complex* data_time_domain = malloc(num_of_ch*sizeof(double complex)*CSW_BLOCK_SIZE);
    double complex* data_freq_domain = malloc(num_of_ch*sizeof(double complex)*CSW_BLOCK_SIZE);
    double complex* fft_help_arr = malloc(sizeof(double complex)*CSW_BLOCK_SIZE);


    int* fft_coef = malloc(CSW_BLOCK_SIZE * sizeof(int));
    fft3_spec_calc_coefs(fft_coef, CSW_BLOCK_SIZE);
    printf("Ifft blokova pa cuvanje, moze da potraje...\n");
    // Kretnja po blokovima otpakivanje i ifft zatim upisivanje u fajl
    for(int block_count = 0; block_count < num_of_blocks; block_count++){
        // Rekonstrucija frekvencijskog domena iz amplitude i faze 
        // Rekonstruise se druga polovina fft
        // Po kanalima
        for(int j = 0; j < num_of_ch; j++){
            int base = block_count * CSW_BLOCK_SIZE * num_of_ch / 2 + j * (CSW_BLOCK_SIZE / 2);
            // Nulti clan nema parnja pa je izdvoje van petlje
            double angle = ((double)symb_arr_phase->arr[base]) * DELTA_PHASE;
            double amp = powl(10,((double)symb_arr_amp->arr[base]) * DELTA_DB / 20);
            data_freq_domain[j*CSW_BLOCK_SIZE] = amp * (cos(angle) + I * sin(angle));
            // Osobina simetrije i antisimetrije oko srednje clana
            for(int i = 1; i <= CSW_BLOCK_SIZE / 2; i++){
                amp = powl(10,((double)symb_arr_amp->arr[base + i]) * DELTA_DB / 20);
                angle = ((double)symb_arr_phase->arr[base + i]) * DELTA_PHASE;
                // simtricni elemnti su medjusobno konjugovano kompleksni
                data_freq_domain[CSW_BLOCK_SIZE - i + j * CSW_BLOCK_SIZE] = amp * (cos(angle) - I * sin(angle));
                data_freq_domain[i + j *CSW_BLOCK_SIZE] = amp * (cos(angle) + I * sin(angle));
            }
        }
        // ifft bloka u svim kanalima
        for(int k = 0; k < num_of_ch; k++){
            // printf("%d\n", k);
            ifft3_spec(data_freq_domain + k * CSW_BLOCK_SIZE, data_time_domain + k * CSW_BLOCK_SIZE, fft_help_arr, fft_coef, CSW_BLOCK_POW2);
            
            // ifft(data_freq_domain + k * CSW_BLOCK_SIZE, data_time_domain + k * CSW_BLOCK_SIZE, CSW_BLOCK_SIZE);    
        }

        // Prepisivanje iz kompleksnog niza u short niz
        for(int k = 0, init_pos = 0; k < num_of_ch; k++){
            int base = k * CSW_BLOCK_SIZE;
            int end = CSW_BLOCK_SIZE + base;
            for(int j = base, pos = init_pos; j < end; j++){
                one_block[pos] = (short)creal(data_time_domain[j]);
                pos += num_of_ch;
            }
            ++init_pos;
        }
        // Upisivanje rekonstruisanog bloka 
        fwrite(one_block, 2, num_of_ch * CSW_BLOCK_SIZE, decompressed_file);
        // printf("%d\n", block_count);
    }
    printf("Kraj dekompresije\n");
    // NEKI PROBLEM SA OSLOBADJANJEM MEMORIJE PRILIKOM KORISCENJA NOVE VERZIJE IFFT
    // free(one_block);
    // free(data_time_domain);
    // free(data_freq_domain);
    free(fft_coef);
    free(fft_help_arr);
    
}


// struktura vezana za ovu ekstenziju
struct cs_type csw = {
    .extension = "csw",
    .compress = csw_compress,
    .decompress = csw_decompress,
    .is_this_type = csw_check,
    .name_of_program_to_open = "celluloid",
    .max_possible_symbols = 300,
};

// funcija za dodavanje csw strukture u virtuelni sloj
void csw_init(){
    if(vcl_add_new_type(&csw) != 0){
        printf("Nemoguce je dodati jos tipova");
        exit(EXIT_FAILURE);
    }
}



