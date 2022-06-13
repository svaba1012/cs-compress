#include "vcl.h"
#include "cst.h"
#include "csw.h"

// vcl - virtual compressing layer

#define MAX_CS_TYPES 10

int num_of_types = 0;
struct cs_type* ALL_CS_TYPES[MAX_CS_TYPES] = {NULL};
struct cs_type* cur_type = NULL;


void vcl_cs_header_init(struct cs_header* header, struct cs_type* cs){
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
    cst_init();
}

int vcl_add_new_type(struct cs_type* new_type){
    if(new_type == NULL || cur_type >= MAX_CS_TYPES){
        return -1;
    }
    ALL_CS_TYPES[num_of_types++] = new_type;
    return 0;
}

void vcl_determine_type(FILE* file){
    for(int i = 0; i < num_of_types; i++){
        // if(ALL_CS_TYPES[i]->extension[0] == '\0'){
        //     break;
        // }
        if(ALL_CS_TYPES[i]->is_this_type != NULL && ALL_CS_TYPES[i]->is_this_type(file)){
            cur_type = ALL_CS_TYPES[i];
            printf("%d\n", i);
            return;
        }
    }
}

void vcl_determine_compressed_type(struct cs_header* header){
    if(strncmp(header->magic1, "CS26", 4) != 0 || strncmp(header->magic2, "CS11", 4) != 0){
        cur_type = NULL;
        return;
    }

    for(int i = 0; i < num_of_types; i++){
        // if(ALL_CS_TYPES[i]->extension[0] == '\0'){
        //     break;
        // }
        if(strcmp(ALL_CS_TYPES[i]->extension, header->type) == 0){
            cur_type = ALL_CS_TYPES[i];
            return;
        }
    }
}

void compress_file(FILE* file){
    if(file == NULL){
        printf("Nepostojeci fajl\n");
        exit(EXIT_FAILURE);
    }
    vcl_determine_type(file);
    if(cur_type == NULL){
        printf("Format fajla je nepodrzan za kompresiju\n");
        return;
    }

    cur_type->compress(cur_type, file);
       
    // //compress data
    // printf("\nProba\n");
    // struct huff_symb_arr* symb_arr = cur_type->prepare_data(file, cur_type);
    // struct huff_tree* tree = huffman_get_tree(symb_arr, cur_type->max_possible_symbols, NULL, NULL);
    // struct huff_coded_arr* compressed_data = huffman_code(tree, symb_arr);

    // // for(int i = 0; i < compressed_data->len; i++){
    // //     printf("%c", compressed_data->arr[i]);
    // // }printf("\n");

    // // struct huff_symb_arr* symb2 = huffman_decode(tree, compressed_data);

    // // for(int i = 0; i < symb2->len; i++){
    // //     printf("%c", symb2->arr[i]);
    // // }printf("\n");


    // FILE* compressed_file = fopen("./compressed_file.cst", "wb");
    // if(cur_type->make_additional_header == NULL){
    //     cs_header.additional_header_size = 0;
    // }else{
    //     // ....
    // }
    // cs_header.last_end_bit = compressed_data->last_end_bit;
    // cs_header.data_field_size = compressed_data->len;
    // cs_header.original_data_size = compressed_data->original_len;
    // cs_header.code_table_size = tree->num_of_leaves * sizeof(struct simbol_verovatnoca_kod);
    // //change

    // printf("\n\n%d\n\n", tree->num_of_leaves);
    // strcpy(cs_header.type, cur_type->extension);
    // //...
    // fwrite(&cs_header, sizeof(struct cs_header), 1, compressed_file);
    // fwrite(tree->simb_kod, cs_header.code_table_size, 1, compressed_file);
    // // printf("\n\n%d\n\n", compressed_data->len);
    // fwrite(compressed_data->arr, 1, compressed_data->len, compressed_file);
    // if(cs_header.additional_header_size != 0){
    //     cur_type->make_additional_header(compressed_file, cur_type, compressed_data);
    // }
    // fclose(file);
    // fclose(compressed_file);
    // //...
    // //napravi fajl 
    // //upisi heder
    // //tabelu
    // //pa additional heder
}



void decompress_file(FILE* file){
    if(file == NULL){
        printf("Nepostojeci fajl\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_SET);
    struct cs_header* header = malloc(sizeof(struct cs_header));
    if(header == NULL){
        printf("Nema vise memorije\n");
        exit(EXIT_FAILURE);
    }
    fread(header, sizeof(struct cs_header), 1 ,file);
    vcl_determine_compressed_type(header);
    if(cur_type == NULL){
        printf("Format fajla je nepodrzan za dekompresiju\n");
        return;
    }
    
    cur_type->decompress(cur_type, file);

//     struct simbol_verovatnoca_kod* verovatnoce = malloc(header->code_table_size);
//     if (verovatnoce == NULL){
//         printf("Nema vise memorije\n");
//         exit(EXIT_FAILURE);
//     }
//     fread(verovatnoce,1, header->code_table_size, file);

//     struct huff_coded_arr* coded_arr = malloc(sizeof(struct huff_coded_arr));
//     if(coded_arr == NULL){
//         printf("Nema vise memorije\n");
//         exit(EXIT_FAILURE);
//     }
//     coded_arr->last_end_bit = header->last_end_bit;
//     coded_arr->len = header->data_field_size;
//     coded_arr->original_len = coded_arr->original_len;
//     coded_arr ->arr = malloc(coded_arr->len);
//     if(coded_arr->arr == NULL){
//         printf("Nema vise memorije\n");
//         exit(EXIT_FAILURE);
//     }
//     fread(coded_arr->arr, 1, coded_arr->len, file);




//     //struct huff_coded_arr* coded_arr = cur_type->prepare_compressed_data(file, cur_type);

//     struct huff_tree* tree = huffman_get_tree(NULL, header->code_table_size / sizeof(struct simbol_verovatnoca_kod), NULL,verovatnoce);
//     struct huff_symb_arr* original_data = huffman_decode(tree, coded_arr);

//     FILE* decompressed_file = fopen("./decompressed_file.txt", "wb");
//     if(decompress_file == NULL){
//         printf("Nemoguce je napraviti fajl\n");
//         exit(EXIT_FAILURE);
//     }
//     printf("\n\nIdemo\n\n");
//     fwrite(original_data->arr, 1, original_data->len, decompressed_file);
    
//    fclose(decompressed_file);
//     fclose(file);
}


void proba2(){
    vcl_init();
    FILE* file  = fopen("./test_files/pesma.wav", "rb");

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