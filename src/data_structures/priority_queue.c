#include "priority_queue.h"
#include <stdio.h>
#include <stdlib.h>

// Prioritetni red - pq

struct pq_node{
    // pokazivac na podatak
    void* data;
    // pokazivac na sledeceg clana
    struct pq_node* next;
};

struct priority_queue* new_pq(){
    // napravi novi pq
    struct priority_queue* pq = malloc(sizeof(struct priority_queue));
    if(pq == NULL){
        printf("Nema memorije\n");
        exit(EXIT_FAILURE);
    }
    pq->head = NULL;
    return pq;
}

void pq_free(struct priority_queue* pq){
    // obrisi , oslobodi memoriju pq
    if(pq == NULL){
        return;
    }
    struct pq_node* node;
    for(node = pq->head; node!=NULL; node = node->next){
        free(node->data);
        free(node);
    }
    free(pq);
}



void* pq_delete(struct priority_queue* pq){
    // skini prvog clana iz reda
    if(pq->head == NULL){
        return NULL;
    }
    struct pq_node* temp;
    void* data;
    temp = pq->head;
    data = temp->data;
    pq->head = pq->head->next;
    // vrati prvog clana head, pomeri head na sledeceg
    free(temp);

    return data;
} 

void pq_insert(struct priority_queue* pq, void* data, CALC_PRIO get_priority){
    // ubaci u red sa prioritetom
    int priority = get_priority(data);
    struct pq_node* prev = NULL;
    struct pq_node* qur;
    // prolazak kroz listu (red) dok je veci prioritet
    for(qur = pq->head; (qur != NULL) && (priority >= get_priority(qur->data)); prev = qur, qur = qur->next){
        ;
    }
    // izmedju prev i qur ulazi novi clan
    struct pq_node* new_node = malloc(sizeof(struct pq_node));
    if(new_node == NULL){
        printf("Nema memorije\n");
        exit(EXIT_FAILURE);
    }
    // umetanje novog clana
    new_node->data = data;
    new_node->next = qur;
    if(prev == NULL){
        pq->head = new_node;
    }else{
        prev->next = new_node;
    }
    return;
}

