#ifndef VCL_H
#define VCL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <huffman.h>

enum CS_TYPE{CS_TYPE_TXT, CS_TYPE_WAV};

typedef bool(*FCS_CHECK_TYPE)(FILE*);


struct cs_type;

// typedef struct huff_symb_arr*(*FCS_COMMPRESSION_PREPARE_ARRAY)(FILE*, struct cs_type*);
// typedef struct huff_coded_arr*(*FCS_DECOMMPRESSION_PREPARE_ARRAY)(FILE*, struct cs_type*);
// typedef void(*FCS_MAKE_ADDITIONAL_HEADER)(FILE*, struct cs_type*, struct huff_coded_arr*);

typedef int(*FCS_COMPRESS_FILE)(struct cs_type*, FILE*, FILE*);
typedef int(*FCS_DECOMPRESS_FILE)(struct cs_type*, FILE*, FILE*);


struct cs_type{
    char extension[4];
    FCS_CHECK_TYPE is_this_type;
    FCS_COMPRESS_FILE compress;
    FCS_DECOMPRESS_FILE decompress;
    char name_of_program_to_open[20];
    int max_possible_symbols;
};

struct cs_header{
    char magic1[4];
    short version;
    char type[4];
    char magic2[4];
    int data_field_size;
    short additional_header_size;
}__attribute__((packed));

void vcl_init();
int vcl_add_new_type(struct cs_type* new_type);
void vcl_cs_header_init(struct cs_header* header, struct cs_type* cs);
void compress_file(FILE* file, FILE* out_file);
void decompress_file(FILE* file, FILE* out_file);
void open_file(char* filename);
void proba2();

#endif