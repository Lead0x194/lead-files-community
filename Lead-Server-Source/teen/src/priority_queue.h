#ifndef __QUEUE_H__
#define __QUEUE_H__
/* number of priority queues to use (reduces enqueue cost) */
#define NUM_PRIORITY_QUEUES   509

typedef struct priority_queue
{
    struct priority_q_element    *head[NUM_PRIORITY_QUEUES];
    struct priority_q_element    *tail[NUM_PRIORITY_QUEUES];
} PRIORITY_QUEUE;

typedef struct priority_q_element
{
    struct priority_q_element* prev, *next;

    void*		data;
    int	                key;
} PRIORITY_Q_ELEMENT;

/* function protos need by other modules */
PRIORITY_QUEUE*		queue_init(void);
PRIORITY_Q_ELEMENT*	queue_enq(PRIORITY_QUEUE* q, void* data, int key);
void			queue_deq(PRIORITY_QUEUE* q, PRIORITY_Q_ELEMENT* qe);
PRIORITY_Q_ELEMENT*	queue_reque(PRIORITY_QUEUE* q, PRIORITY_Q_ELEMENT* qe, int new_key);
void*			queue_head(PRIORITY_QUEUE* q);
int			queue_key(PRIORITY_QUEUE* q);
int			queue_elmt_key(PRIORITY_Q_ELEMENT* qe);
void			queue_free(PRIORITY_QUEUE* q);
int			queue_count(PRIORITY_QUEUE* q);

#endif // __QUEUE_H__
