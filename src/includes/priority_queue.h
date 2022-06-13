#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


typedef int(*CALC_PRIO)(void*);

struct priority_queue{
    struct pq_node* head;

    //mozda nepotrebno
    // struct pq_node* rear;
};

struct priority_queue* new_pq();
void* pq_delete(struct priority_queue* pq);
void pq_insert(struct priority_queue* pq, void* data, CALC_PRIO get_priority);
void pq_free(struct priority_queue* pq);


#endif