#include "huffman.h"
#include "priority_queue.h"
#include <stdlib.h>
#include <memory.h>

// Huffman-ov algoritam za kodovanje podataka radi kompresije


struct cvor_stabla{
    // levo dete
    struct cvor_stabla* levi;
    // desno dete
    struct cvor_stabla* desni;
    // simbol
    HUFF_SYM simbol;
    // broj pojavljivanja
    int verovatnoca;
};

// funkcija za dobijanje prioriteta cvora koja se prosledjuje prioritetnom redu
static int huffman_get_priority(void* ptr){
    struct cvor_stabla* cvor = (struct cvor_stabla*) ptr;
    return cvor->verovatnoca;
}

// funcija za pravljenje stabla
static struct huff_tree* huffman_napravi_stablo(struct simbol_verovatnoca_kod* verovatnoce, int broj_simbola){
    // prosledjuje se tabela verovatnoca, i maksimalan broj razlicitih simbola

    // pravljenje prioritetnog reda
    struct priority_queue* prio_queue = new_pq();
    // broj_verovatnih_simbola <= broj_simbola mora da vazi 
    int broj_verovatnih_simbola = 0;
    // priprema reda pq
    for(int i = 0; i < broj_simbola; i++){
        // iteracija kroz celu tabelu
        if(verovatnoce[i].broj_pojavljivanja <= 0){
            // simbol se ne pojavljuje prelazi se na sledeci
            continue;
        }
        // pravljenje novog cvora stabla
        struct cvor_stabla* cvor = malloc(sizeof(struct cvor_stabla));
        if(cvor == NULL){
            printf("Nema memorije!\n");
            exit(EXIT_FAILURE);
        }
        // u cvor se upisuje simbol i verovatnoca
        cvor->desni = NULL;
        cvor->levi = NULL;
        cvor->simbol = verovatnoce[i].simbol;
        cvor->verovatnoca = verovatnoce[i].broj_pojavljivanja;
        // ubacivanje cvora u red sa prioritetom koji je verovatnoca
        pq_insert(prio_queue, cvor, huffman_get_priority);
        // predstavlja broj listova u stablu
        broj_verovatnih_simbola++;
    }
    // pravljenje stabla
    for(int i = 0; i < broj_verovatnih_simbola - 1; i++){
        // novi cvor
        struct cvor_stabla* new_cvor = malloc(sizeof(struct cvor_stabla));
        if(new_cvor == NULL){
            printf("Nema memorije!\n");
            exit(EXIT_FAILURE);
        }
        struct cvor_stabla* cvorA = (struct cvor_stabla*)pq_delete(prio_queue);
        struct cvor_stabla* cvorB = (struct cvor_stabla*)pq_delete(prio_queue);
        // deca novog cvora -----------> prva dva iz reda
        new_cvor->levi = cvorA;
        new_cvor->desni = cvorB;
        // verovatnoca oca zbir dece
        new_cvor->verovatnoca = cvorA->verovatnoca + cvorB->verovatnoca;
        new_cvor->simbol = 0;
        // ubacivanje oca nazad u red sa prioritetom
        pq_insert(prio_queue, new_cvor, huffman_get_priority);
    }
    // po zavrsetku petlje u redu je koren stabla
    struct cvor_stabla* root = pq_delete(prio_queue);
    // pakovanje podataka
    struct huff_tree* stablo = malloc(sizeof(struct huff_tree));
    if(stablo == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    stablo->root = root;
    stablo->num_of_leaves = broj_verovatnih_simbola;
    stablo->num_of_sym = broj_simbola;
    stablo->simb_kod = verovatnoce;
    // oslobadjanje memorije reda
    pq_free(prio_queue);
    return stablo;
}

// podrazumevana funkcija za pretragu simbola u tabeli verovatnoca ako korisnik ne prosledi svoju
static int huffman_get_index_of_symbol(HUFF_SYM simbol, struct simbol_verovatnoca_kod* simb_kod, int duzina){
    for(int i = 0; i < duzina; i++){
        if(simbol == simb_kod[i].simbol && simb_kod[i].broj_pojavljivanja > 0){
            return i;
        }
    }
    return -1;
}




// funcija za pravljenje tabele verovatnoca
static struct simbol_verovatnoca_kod* huffman_calc_verovatnoce(struct huff_symb_arr* symb_arr,int* max_symb, FGET_INDEX_OF_SYMBOL get_index){
    
    HUFF_SYM* arr = symb_arr->arr;
    // priprema tabele
    struct simbol_verovatnoca_kod* simb_vero = malloc(sizeof(struct simbol_verovatnoca_kod)* *max_symb);
    int symb_count = 0;
    if(simb_vero == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    memset(simb_vero, 0, sizeof(struct simbol_verovatnoca_kod)* *max_symb);
    
    // iteracija kroz niz i brojanje broj pojavljivanja simbola
    for(int i = 0; i < symb_arr->len; i++){
        // pretraga u tabeli da li se simbol vec pojavljivao
        int idx = get_index(arr[i], simb_vero, *max_symb);
        
        if(idx >= 0){
            // ako jeste
            simb_vero[idx].simbol = arr[i];
            simb_vero[idx].broj_pojavljivanja++;
        }else{
            // ako nije dodaj ga na kraj tabele
            simb_vero[symb_count].simbol = arr[i];
            simb_vero[symb_count].broj_pojavljivanja++;
            symb_count++;
        }
    }
    return simb_vero;
}

// funkcija za dobijanje stabla
struct huff_tree* huffman_get_tree(struct huff_symb_arr* symb_arr,int max_symb, FGET_INDEX_OF_SYMBOL get_index, struct simbol_verovatnoca_kod* verovatnoce){
    
    if(get_index == NULL){
        // ako korisnike ne prosledi svoju funkciju
        get_index = huffman_get_index_of_symbol;
    }
    if(verovatnoce == NULL){
        // ako verovatnoce ne postoje izracunaj
        verovatnoce = huffman_calc_verovatnoce(symb_arr, &max_symb, get_index);
    }
    // napravi stablo
    struct huff_tree* tree = huffman_napravi_stablo(verovatnoce, max_symb);
    tree->get_index = get_index;
    return tree;
    
}

// rekurzivno racunanje koda po inorder obilasku
// POTENCIJALNO UBRZANJE SA ITERATIVNOM REALIZACIJOM
static void huffman_inorder_save_codes(struct cvor_stabla* cvor,unsigned int code,int lvl, struct simbol_verovatnoca_kod* simb_kod, int duzina, FGET_INDEX_OF_SYMBOL get_index){
    if(cvor != NULL){
        // broj bita koda jednak je nivou u kom se list nalazi
        lvl++;
        // dodavanje 0 u kod za levo dete
        code = code << 1;
        huffman_inorder_save_codes(cvor->levi, code, lvl, simb_kod, duzina, get_index);
        code = code >> 1;
        if(cvor->levi == NULL && cvor->desni == NULL){
            // u listovima se cuva kod kao i duzina koda u bitima
            int idx = get_index(cvor->simbol, simb_kod, duzina);
            simb_kod[idx].code = code;
            simb_kod[idx].code_num_of_bits = lvl;
        }
        // dodavanje 1 u kod za desno dete
        code = (code << 1) | 1;
        huffman_inorder_save_codes(cvor->desni, code, lvl, simb_kod, duzina, get_index);
    }
}





struct huff_coded_arr* huffman_code(struct huff_tree* tree, struct huff_symb_arr* symb_arr){
    // na osnovu niza i stabla kodira podatke -----> kompresuje
    HUFF_SYM* niz_simbola = symb_arr->arr;
    int duzina_niza = symb_arr->len;
    struct cvor_stabla* stablo = tree->root;
    // izracunavanje kodova preko stabla i cuvanje
    huffman_inorder_save_codes(stablo, 0, -1, tree->simb_kod, tree->num_of_sym, tree->get_index);

    // prprema niza za kompresovane podatke
    HUFF_CODE_BLOCK* kodovani_niz = malloc(sizeof(HUFF_CODE_BLOCK) * duzina_niza);
    if(kodovani_niz == NULL){
        printf("Nema memorije!\n");
    }
    memset(kodovani_niz, 0, sizeof(HUFF_CODE_BLOCK) * duzina_niza);

    // trenutna bajt pozicija u kodovanom nizu
    int j = 0;
    //trenutna bit pozicija u trenutnom bajtu u kodovanom nizu gledano od LSB do MSB
    int bit_pos = sizeof(HUFF_CODE_BLOCK) * 8;

    // iteracija kroz niz
    for(int i = 0; i < duzina_niza; i++){
        // dobijanje ideksa niza
        int idx = tree->get_index(niz_simbola[i], tree->simb_kod, tree->num_of_sym);
        struct simbol_verovatnoca_kod kod = tree->simb_kod[idx];
        // kod trenutnog simbola
        int code = kod.code;
        for(int k = kod.code_num_of_bits - 1; k >= 0; k--){
            bit_pos--;
            // dodavanje koda bit po bit
            kodovani_niz[j] |= (((code & (1<<k)) >> k) << bit_pos);
            if(bit_pos == 0){
                // restartuj na pocetni bit i pomeri se na naredni bajt
                bit_pos = sizeof(HUFF_CODE_BLOCK) * 8;
                j++;
            }
        }
    }
    // upakivanje podataka
    struct huff_coded_arr* coded_arr = malloc(sizeof(struct huff_coded_arr));
    if(coded_arr == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    coded_arr->arr = kodovani_niz;
    coded_arr->len = ++j;
    coded_arr->last_end_bit = bit_pos;
    coded_arr->original_len = duzina_niza;
    return coded_arr;
}

struct huff_symb_arr* huffman_decode(struct huff_tree* tree, struct huff_coded_arr* coded_arr){
    // na osnovu stabla i kodovanog niza dekodira podatke
    HUFF_CODE_BLOCK* niz_kodova = coded_arr->arr; 
    int duzina_niza = coded_arr->len;

    // pravljenje niza za rekonstruisane podatke
    HUFF_SYM* niz_simbola = malloc(sizeof(HUFF_SYM)*coded_arr->original_len);
    if(niz_simbola == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }

    // bajt pozicija u rekonstruisanom nizu
    int j = 0;
    // bit pozicija u kodovanom nizu
    int bit_pos = sizeof(HUFF_CODE_BLOCK) * 8 - 1;
    int i;
    struct cvor_stabla* cvor = tree->root;
    for(i = 0; i < duzina_niza - 1; i++){
        while (1){
            if(cvor->desni == NULL && cvor->levi == NULL){
                // ako je list pronadjen simbol sa datim kodom pa se upisuje 
                niz_simbola[j] = cvor->simbol;
                j++;
                cvor = tree->root;
            }
            if(bit_pos < 0){
                // odlazak na restart bit_pos
                break;
            }
            if((niz_kodova[i] & (1 << bit_pos)) >> bit_pos){
                // ako je trutni bit 1 prelazak na desno dete
                cvor = cvor->desni;
            }else{
                // ako je nula na levo
                cvor = cvor->levi;
            }
            // pomeranje na naredni bit
            bit_pos--;
        }
        // restartovanje bit_pozicije
        bit_pos = sizeof(HUFF_CODE_BLOCK) * 8 - 1;
    }
    // obrada poslednjeg clana kodovanog niza koji je verovatno isecen 
    while (1){
        // isto kao i gore
        if(cvor->desni == NULL && cvor->levi == NULL){
            niz_simbola[j] = cvor->simbol;
            j++;
            cvor = tree->root;
        }
        // granica za izlazak nije vise nula vec sacuvani podatak o poslednjem bitu poslednjeg bajta 
        if(bit_pos < coded_arr->last_end_bit){
            break;
        }
        if((niz_kodova[i] & (1 << bit_pos)) >> bit_pos){
            cvor = cvor->desni;
        }else{
            cvor = cvor->levi;
        }
        bit_pos--;
    }
    
    // upakivanje podataka
    struct huff_symb_arr* symb_arr = malloc(sizeof(struct huff_symb_arr));
    if(symb_arr == NULL){
        printf("Nema memorije!\n");
        exit(EXIT_FAILURE);
    }
    symb_arr->arr = niz_simbola;
    symb_arr->len = j;
    return(symb_arr);
}


static void postorder_recursive_free(struct cvor_stabla* cvor){
    // brisanje cvorova stabla postorder obilaskom
    postorder_recursive_free(cvor->levi);
    postorder_recursive_free(cvor->desni);
    free(cvor);
}

void free_tree(struct huff_tree* tree){
    // oslobodi memoriju koju stablo zauzima
    free(tree->simb_kod);
    postorder_recursive_free(tree->root);
}
