#include "queue.h"

int empty(Queue* qu){
    if(qu->head == 0 && qu->tail == 0)return 1;
    return 0;
}
void queue_init(Queue *qu) {
    qu->sz = 0;
    qu->head = 0;
    qu->tail = 0;
}

void push(Queue* qu, int inp_val){
    qu->sz++;
    if(qu->head == 0 && qu->tail == 0){
        Queue_Node* inp = malloc(sizeof(Queue_Node));
        inp->val = inp_val;
        inp->last = inp->next = 0;
        qu->head = inp;
        qu->tail = inp;
    }else{
        Queue_Node* inp = malloc(sizeof(Queue_Node));
        inp->val = inp_val;
        inp->last = qu->tail;
        qu->tail->next = inp;
        qu->head = inp;
    }
}

int pop(Queue* qu){
    qu->sz--;
    int re = qu->head->val;
    Queue* tmp = qu->head->last;
    qu->head = tmp;
    free(tmp);
    if(qu->head == 0)qu->tail = 0;
    return re;
}

int qu_size(Queue* qu) {
    return qu->sz;
}