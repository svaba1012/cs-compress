#include "TEMPLATE.h"
#include "vcl.h"
#include "huffman.h"

// !!!
// Zameni TEMPLATE za odgovarajuci naziv i implementiraj zadate funcije

struct TEMPLATE_additional_header{
    // int code_table_size;
    // short last_end_bit;
    // int original_size;

    //uredi dodatni heder za ovaj tip
};


bool TEMPLATE_check(FILE* file){
    // kod za tvoj fajl
}

int TEMPLATE_compress(struct cs_type* cs, FILE* file){
    //read data

    //dodaj kod, ispod je ponavljajuci kod
    fseek(file, 0, SEEK_END);
    
    struct huff_symb_arr* symb_arr = malloc(sizeof(struct huff_symb_arr));
    if(symb_arr == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    struct cst_additional_header* header2 = malloc(sizeof(struct cst_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    struct huff_tree* tree = huffman_get_tree(symb_arr, cs->max_possible_symbols, NULL, NULL);
    struct huff_coded_arr* compressed_data = huffman_code(tree, symb_arr);

    FILE* compressed_file = fopen("./compressed_file.cst", "wb");
    
    vcl_cs_header_init(header, cs);
    header->additional_header_size = sizeof(struct TEMPLATE_additional_header);
    
    //kod

    fwrite(header, sizeof(struct cs_header), 1, compressed_file);
     
    //kod za dodatni heder

    fwrite(header2, sizeof(struct cst_additional_header), 1 , compressed_file); 
    
    //kod
    fclose(file);
    fclose(compressed_file);

}

int TEMPLATE_decompress(struct cs_type* cs, FILE* file){
    //kod
    fseek(file, 0, SEEK_SET);
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(header, sizeof(struct cs_header), 1 ,file);
    struct cst_additional_header* header2 = malloc(sizeof(struct TEMPLATE_additional_header));
    if(header2 == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    fread(header2, sizeof(struct TEMPLATE_additional_header), 1, file);

    // kod

    FILE* decompressed_file = fopen("./decompressed_file.txt", "wb");
    if(decompressed_file == NULL){
        printf("Nemoguce je napraviti fajl\n");
        exit(EXIT_FAILURE);
    }
    
    //kod

    fclose(decompressed_file);
    fclose(file);


}


struct cs_type TEMPLATE = {
    .extension = "???",
    .compress = TEMPLATE_compress,
    .decompress = TEMPLATE_decompress,
    .is_this_type = TEMPLATE_check,
    //tvoj broj
    .max_possible_symbols = ,
    
};

void TEMPLATE_init(){
    if(vcl_add_new_type(&TEMPLATE) != 0){
        printf("Nemoguce je dodati jos tipova");
        exit(EXIT_FAILURE);
    }
}



