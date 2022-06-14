#include "vcl.h"
#include "cst.h"
#include "csw.h"

// vcl - virtual compressing layer
// virtualni sloj kompresije cs na kom se odredjuje fizicki format

#define MAX_CS_TYPES 10

// broj fizickih formata
int num_of_types = 0;
// niz fizickih formata
struct cs_type* ALL_CS_TYPES[MAX_CS_TYPES] = {NULL};
// trenutni fizicki format
struct cs_type* cur_type = NULL;


void vcl_cs_header_init(struct cs_header* header, struct cs_type* cs){
    // inicijalizacija galvnog hedera koji je zajednicki za sve cs formate
    header->version = 1;
    header->magic1[0] = 'C';
    header->magic1[1] = 'S';
    header->magic1[2] = '2';
    header->magic1[3] = '6';

    header->magic2[0] = 'C';
    header->magic2[1] = 'S';
    header->magic2[2] = '1';
    header->magic2[3] = '1';

    strcpy(header->type, cs->extension);
}

void vcl_init(){
    //add init for other
    csw_init();

    // dodaje se na kraj niza 
    // kada ostali formati nisu prepoznati
    // posmatra se trenutni fajl kao tekstualni 
    cst_init();
}

int vcl_add_new_type(struct cs_type* new_type){
    // dodavanje noog tipa u tabelu
    if(new_type == NULL || cur_type >= MAX_CS_TYPES){
        return -1;
    }
    ALL_CS_TYPES[num_of_types++] = new_type;
    return 0;
}

void vcl_determine_type(FILE* file){
    // odredjivanje formata fajla
    // da li je podrzan za kompresiju
    for(int i = 0; i < num_of_types; i++){
        if(ALL_CS_TYPES[i]->is_this_type != NULL && ALL_CS_TYPES[i]->is_this_type(file)){
            cur_type = ALL_CS_TYPES[i];
            return;
        }
    }
}

void vcl_determine_compressed_type(struct cs_header* header){
    // odredjivanje da li je cs format 
    // provera glavnog hedera
    if(strncmp(header->magic1, "CS26", 4) != 0 || strncmp(header->magic2, "CS11", 4) != 0){
        cur_type = NULL;
        return;
    }
    // jeste cs format

    // provera koji je fizicki format
    for(int i = 0; i < num_of_types; i++){    
        if(strcmp(ALL_CS_TYPES[i]->extension, header->type) == 0){
            cur_type = ALL_CS_TYPES[i];
            return;
        }
    }
}

void compress_file(FILE* file){
    // kompresuje fajl 
    if(file == NULL){
        printf("Nepostojeci fajl\n");
        exit(EXIT_FAILURE);
    }
    // odredjivanje tipa fajla
    vcl_determine_type(file);
    if(cur_type == NULL){
        printf("Format fajla je nepodrzan za kompresiju\n");
        return;
    }
    // spustanje na fizicki format i resavanje na tom nivou
    cur_type->compress(cur_type, file);
    
}



void decompress_file(FILE* file){
    // dekompresuje fajl
    if(file == NULL){
        printf("Nepostojeci fajl\n");
        exit(EXIT_FAILURE);
    }
    // ucitava primarni heder radi provere
    fseek(file, 0, SEEK_SET);
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(header, sizeof(struct cs_header), 1 ,file);
    // odredjuje da li je cs format u pitanju kao i koji je fizicki format
    vcl_determine_compressed_type(header);
    if(cur_type == NULL){
        printf("Format fajla je nepodrzan za dekompresiju\n");
        return;
    }
    // spustanje na fizicki format i resavanje na tom nivou
    cur_type->decompress(cur_type, file);

}


void proba2(){
    vcl_init();
    FILE* file  = fopen("./test_files/sample2.wav", "rb");

    if(file == NULL){
        printf("Nista\n");
        exit(EXIT_FAILURE);
    }
    printf("Pocinjem kompresiju\n");
    compress_file(file);
    file = fopen("./compressed_file.csw", "rb");   
    if(file == NULL){
        printf("Nista\n");
        exit(EXIT_FAILURE);
    }
    decompress_file(file);
}