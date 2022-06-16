#include "cst.h"
#include "vcl.h"
#include "huffman.h"

//cst je kopresovani tekstualni (.txt) format 
// sastoji se od glavnog cs zaglavljha pomocnog zaglavlje, jedne tabele verovatnoca i na kraju od kompresovanih podataka

// dodatno zaglavlje 
struct cst_additional_header{
    // velicina tabele u bajtovima
    int code_table_size;
    // poslednji validni bit u poslednjem kompresovanom bajtu 
    short last_end_bit;
    // velicina originalnih, nekompresovanih podataka
    int original_size;
};

// svaki fajl moze da se gleda kao tesktualni
bool cst_check(FILE* file){
    if(file != NULL){
        return true;
    }
    return false;
}


int cst_compress(struct cs_type* cs, FILE* file, FILE* compressed_file){
    // pravi novi kompresovani fajl .cst bez gubitaka od .txt
    // odredjivanje velicine fajla
    printf("Pocinjem kompresiju\n");
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    // ucitavanje celog fajla
    printf("Ucitavam podatke iz fajla\n");
    char* data = malloc(file_size);
    if(data == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    fread(data, 1,  file_size, file);

    // prepakivanje podataka u niz odgovarajuceg tipa
    printf("Prepakujem podatke\n");
    HUFF_SYM* data_huff = malloc(sizeof(HUFF_SYM) * file_size);
    if(data_huff == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < file_size; i++){
        data_huff[i] = (HUFF_SYM)data[i];
    }
    free(data);
    // priprema neophodnih struktura
    struct huff_symb_arr* symb_arr = malloc(sizeof(struct huff_symb_arr));
    if(symb_arr == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    symb_arr->arr = data_huff;
    symb_arr->len = file_size;
    printf("Pravim stablo i kodujem\n");
    // pravljenje huffmanovog stabla i kodovanje
    struct huff_tree* tree = huffman_get_tree(symb_arr, cs->max_possible_symbols, NULL, NULL);
    struct huff_coded_arr* compressed_data = huffman_code(tree, symb_arr);
    free(symb_arr->arr);
    free(symb_arr);

    
    
    // pravljenje glavnog hedera i upisivanje
    printf("Upisivanje glavnog hedera\n");
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    vcl_cs_header_init(header, cs);
    header->additional_header_size = sizeof(struct cst_additional_header);
    header->data_field_size = compressed_data->len;
    fwrite(header, sizeof(struct cs_header), 1, compressed_file);
    free(header);
    // pravljenje pomocnog hedera i upisivanje    
    printf("Upisivanje pomocnog hedera\n");

    struct cst_additional_header* header2 = malloc(sizeof(struct cst_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    } 
    header2->code_table_size = tree->num_of_leaves * sizeof(struct simbol_verovatnoca_kod);
    header2->last_end_bit = compressed_data->last_end_bit;
    header2->original_size = compressed_data->original_len;
    fwrite(header2, sizeof(struct cst_additional_header), 1 , compressed_file); 
    
    // upisivanje tabele verovatnoca
    printf("Upisivanje tabele verovatnoca\n");
    fwrite(tree->simb_kod, header2->code_table_size, 1, compressed_file);
    free(header2);
    // upisivanje kodovanih podataka
    // oslobadjanje memorije huff stabala
    // free_tree(tree);
    printf("Upisivanje podataka\n");
    fwrite(compressed_data->arr, 1, compressed_data->len, compressed_file);
    free(compressed_data->arr);
    free(compressed_data);

    
}


int cst_decompress(struct cs_type* cs, FILE* file, FILE* decompressed_file){
    // dekompresuje .cst fajl u .txt fajl
    // ucitavanje hedera
    fseek(file, 0, SEEK_SET);
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(header, sizeof(struct cs_header), 1 ,file);
    // ucitavanje pomocnog hedera
    struct cst_additional_header* header2 = malloc(sizeof(struct cst_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    fread(header2, sizeof(struct cst_additional_header), 1, file);
    // ucitavavanje tabele verovatnoca
    struct simbol_verovatnoca_kod* verovatnoce = malloc(header2->code_table_size);
    if (verovatnoce == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(verovatnoce,1, header2->code_table_size, file);
    // ucitavanje i upakivanje ucitanih kompresovanih podataka 
    struct huff_coded_arr* coded_arr = malloc(sizeof(struct huff_coded_arr));
    if(coded_arr == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    coded_arr->last_end_bit = header2->last_end_bit;
    coded_arr->len = header->data_field_size;
    coded_arr->original_len = header2->original_size;
    coded_arr ->arr = malloc(coded_arr->len);
    if(coded_arr->arr == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(coded_arr->arr, 1, coded_arr->len, file);
    free(header);

    // izrada stabla i dekodovanje
    struct huff_tree* tree = huffman_get_tree(NULL, header2->code_table_size / sizeof(struct simbol_verovatnoca_kod), NULL,verovatnoce);
    struct huff_symb_arr* original_data = huffman_decode(tree, coded_arr);
    free(header2);
    free(coded_arr->arr);
    free(coded_arr);

    // oslobadjanje memorije huff stabala
    // free_tree(tree);

    // otvaranje fajla za upisi dekompresovanih podataka
    
    if(decompressed_file == NULL){
        printf("Nemoguce je napraviti fajl\n");
        exit(EXIT_FAILURE);
    }
    char* text = malloc(original_data->len);
    if(text == NULL){
        printf("Nemoguce je napraviti fajl\n");
        exit(EXIT_FAILURE);
    }
    // prepakivanje podataka u niz odgovarajuceg tipa
    for(int i = 0; i < original_data->len; i++){
        text[i] = (char)original_data->arr[i];
    }
    free(original_data->arr);
    // upisivanje rekonstruisanih podataka
    fwrite(text, 1, original_data->len, decompressed_file);
    free(text);
    free(original_data);
    
}



// struktura koja opisuje cst format
struct cs_type cst = {
    .extension = "cst",
    .compress = cst_compress,
    .decompress = cst_decompress,
    .is_this_type = cst_check,
    .name_of_program_to_open = "gedit",
    .max_possible_symbols = 256,
};

void cst_init(){
    // dodavanje cst formata u visi virtuelni sloj cs formata
    if(vcl_add_new_type(&cst) != 0){
        printf("Nemoguce je dodati jos tipova");
        exit(EXIT_FAILURE);
    }
}



