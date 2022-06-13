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

typedef int(*FCS_COMPRESS_FILE)(struct cs_type*, FILE*);
typedef int(*FCS_DECOMPRESS_FILE)(struct cs_type*, FILE*);


struct cs_type{
    char extension[4];
    FCS_CHECK_TYPE is_this_type;
    FCS_COMPRESS_FILE compress;
    FCS_DECOMPRESS_FILE decompress;
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

int vcl_add_new_type(struct cs_type* new_type);
void vcl_cs_header_init(struct cs_header* header, struct cs_type* cs);
void proba2();

#endif