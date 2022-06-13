#ifndef HUFFMAN_H
#define HUFFMAN_H

// tipovi za simbole i kodove
typedef int HUFF_SYM;
typedef unsigned char HUFF_CODE_BLOCK;

// pokazivac na funkcije pretrage
typedef int(*FGET_INDEX_OF_SYMBOL)(HUFF_SYM, struct simbol_verovatnoca_kod*, int);

// struktura za niz simbola koji se kompresuje
struct huff_symb_arr{
    HUFF_SYM* arr;
    int len;
};

// struktura za kodovani, kompresovani niz
struct huff_coded_arr{
    HUFF_CODE_BLOCK* arr;
    int len;
    // broj bitova u poslednjem bajtu koji nose korisnu informaciju
    int last_end_bit;
    // duzina originalnog nekompresovanog niza
    int original_len;
};

// jedno polje tabele verovatnoca
struct simbol_verovatnoca_kod{
    HUFF_SYM simbol;
    int broj_pojavljivanja;
    // duzina koda u bitima
    int code_num_of_bits;
    int code;
};

// struktura stabla
struct huff_tree{
    struct cvor_stabla* root;
    // broj listova
    int num_of_leaves;
    // tabela verovatnoca simbola sa kodovima
    struct simbol_verovatnoca_kod* simb_kod;
    // duzina tabele
    int num_of_sym;
};

struct huff_tree* huffman_get_tree(struct huff_symb_arr* symb_arr,int max_symb, FGET_INDEX_OF_SYMBOL get_index, struct simbol_verovatnoca_kod* verovatnoce);
struct huff_coded_arr* huffman_code(struct huff_tree* tree, struct huff_symb_arr* symb_arr);
struct huff_symb_arr* huffman_decode(struct huff_tree* tree, struct huff_coded_arr* coded_arr);
void free_tree(struct huff_tree* tree);


#endif


