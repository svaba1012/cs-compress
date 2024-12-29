#include <stdio.h>
#include "priority_queue.h"
#include "huffman.h"
#include "vcl.h"
#include "fft.h"
#include <complex.h>
#include <string.h>

// progam csp namenjen je za kompresiju fajlova u cs format, dekompresovanje cs formata u originalne formate i za otvaranje cs formata
// detaljni opis u readme fajlu

int main(int argc, char* argv[]){
    vcl_init();
    printf("\n-----------------------------------------------------------------------------------\n");
    printf("----------------CSP-cs program za manipulaciju cs kompresovanim formatima----------\n\n");
    if(argc < 2){
        printf("Unesite \"csp -h\" za  ispis opcija programa\n");
        proba2();
    }else if (argc == 2){
        if(strcmp(argv[1], "-h") == 0){
            printf("Postojece opcije:\n-h{opcija za pomoc}\n-c{za kompresovanje fajla, naredni string uzima kao ulazni fajl za kompajlovanje}\n-d{za dekompresovanje fajla, naredni string uzima kao ulazni fajl za dekompajlovanje}\n-o{opciono ovim se dodaje zeljeni naziv izlaznog fajla}\n-p{opcija za otvaranje kompresovanih formata cs, ne moze se koristiti opciona opcija -o uz ovu}\n\n");
            printf("Primeri upotrebe:\n1)Kompresovanje\na)csp -c {NAZIV_FAJLA}\nb)csp -c {NAZIV_FAJLA} -o {NAZIV_IZALZNOG_FAJLA}\n2)Dekompresovanje\na)csp -d {NAZIV_FAJLA}\nb)csp -d {NAZIV_FAJLA} -o {NAZIV_IZALZNOG_FAJLA}\n3)Otvaranje kompresovanog fajla\ncsp -p {NAZIV_FAJLA}\n4)Pomoc\ncsp -h\n");
        }
    }else if (argc == 3 || argc == 5){
        if(!(argc == 5 && strcmp(argv[1], "-p") == 0)){
            FILE* file_in = fopen(argv[2], "rb");
            if(file_in == NULL){
                printf("Nemoguce je otvoriti fajl \"%s\"!!!\nFajl ili ne postoji ili je doslo do neke greske!!!\n", argv[2]);
                exit(EXIT_SUCCESS);
            }
            char temp_file_name[20] = "./cs_temp_file.cs";
            char temp_file_name2[30] = "./cs_temp_file_org.wav";
            char* file_out_name = temp_file_name2;
            if(strcmp(argv[1], "-c") == 0){
                file_out_name = temp_file_name;
            }
            if(argc == 5){
                file_out_name = argv[4];
            }

            FILE* file_out = fopen(file_out_name, "wb");
            if(file_out == NULL){
                printf("Nemoguce je otvoriti fajl \"%s\"!!!\nDoslo do neke greske!!!\n", file_out_name);
                exit(EXIT_SUCCESS);
            }
            if(strcmp(argv[1], "-p") == 0){
                decompress_file(file_in, file_out);
                fclose(file_out);
                open_file(file_out_name);
                // printf("Proba");
                // remove(file_out_name);
            }else if(strcmp(argv[1], "-c") == 0){
                compress_file(file_in, file_out);
            }else if(strcmp(argv[1], "-d") == 0){
                decompress_file(file_in, file_out);
            }else{
                printf("Neprepoznate opcije i komande\n");
            }
            fclose(file_out);
            fclose(file_in);
            
            
        }

    }else{
        printf("Neprepoznate opcije i komande\n");
    }
    


}