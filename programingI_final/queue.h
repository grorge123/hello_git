typedef struct Queue_Node Queue_Node;
typedef struct Queue Queue;
struct Queue_Node{
    int val; 
    Queue_Node *next;
    Queue_Node *last;
};

struct Queue{
    Queue_Node* head;
    Queue_Node* tail;
    int sz;
};

int empty(Queue qu);
void push(Queue qu,int inp_val);
int pop(Queue qu);
int qu_size(Queue qu);
void queue_init(Queue* qu);