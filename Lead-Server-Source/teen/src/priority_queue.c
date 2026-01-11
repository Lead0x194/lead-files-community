/*
 *    Filename: queue.c
 * Description: 큐 처리
 *
 *      Author: 김한주 (aka. 비엽, Cronan), 송영진 (aka. myevan, 빗자루)
 */
#include "structs.h"
#include "utils.h"
#include "priority_queue.h"

extern int global_pulse;

/* 큐를 초기화 한다 */
PRIORITY_QUEUE *queue_init(void)
{/*{{{*/
    PRIORITY_QUEUE *q;

    CREATE(q, PRIORITY_QUEUE, 1);

    return (q);
}
/*}}}*/

/* 큐를 enqueue 한다. */
PRIORITY_Q_ELEMENT *queue_enq(PRIORITY_QUEUE *q, void *data, int key)
{/*{{{*/
    PRIORITY_Q_ELEMENT	*qe, *i;
    int			bucket;

    /* fast hasing */
    bucket	= key % NUM_PRIORITY_QUEUES;

    CREATE(qe, PRIORITY_Q_ELEMENT, 1);

    qe->data	= data;
    qe->key	= key;
    qe->prev	= NULL;
    qe->next	= NULL;

    if (!q->head[bucket]) 
    {
	// 큐가 비었다.
	q->head[bucket] = qe;
	q->tail[bucket] = qe;
    }
    else
    {
	for (i = q->tail[bucket]; i; i = i->prev) 
	{
	    if (i->key < key) 
	    { 
		// 넣을 곳을 찾았다.
		if (i == q->tail[bucket])
		    q->tail[bucket]	= qe;
		else 
		{
		    qe->next		= i->next;
		    i->next->prev	= qe;
		}

		qe->prev = i;
		i->next = qe;
		break;
	    }
	}

	if (i == NULL) 
	{ 
	    // 넣을 곳이 리스트의 처음 이다.
	    qe->next		= q->head[bucket];
	    q->head[bucket]	= qe;
	    qe->next->prev	= qe;
	}
    }

    return (qe);
}/*}}}*/

/* 큐를 dequeue 한다. */
void queue_deq(PRIORITY_QUEUE *q, PRIORITY_Q_ELEMENT *qe)
{/*{{{*/
    int i;

    if (qe == NULL) 
    {
	//core_dump();
	return;
    }

    i = qe->key % NUM_PRIORITY_QUEUES;

    if (qe->prev == NULL)
	q->head[i] = qe->next;
    else
	qe->prev->next = qe->next;

    if (qe->next == NULL)
	q->tail[i] = qe->prev;
    else
	qe->next->prev = qe->prev;

    free(qe);
}
/*}}}*/

PRIORITY_Q_ELEMENT *queue_reque(PRIORITY_QUEUE *q, PRIORITY_Q_ELEMENT *qe, int new_key)
{/*{{{*/
    void	*data;

    data = qe->data;

    queue_deq(q, qe);
    return (queue_enq(q, data, new_key));
}
/*}}}*/

/*
 * 큐의 헤드를 dequeue 후 리턴한다.
 */
void *queue_head(PRIORITY_QUEUE *q)
{/*{{{*/
    void *data;
    int i;

    i = (global_pulse % NUM_PRIORITY_QUEUES);

    if (!q->head[i])
	return (NULL);

    data = q->head[i]->data;
    queue_deq(q, q->head[i]);

    return (data);
}
/*}}}*/

/*
 * 숫자가 제일 작은 (큐의 헤드) key 를 리턴한다.
 * 큐가 NULL 일 경우 unsinged number 의 최고수를 리턴한다.
 */
int queue_key(PRIORITY_QUEUE *q)
{/*{{{*/
    int i;

    i = global_pulse % NUM_PRIORITY_QUEUES;

    if (q->head[i])
	return q->head[i]->key;
    else
	return LONG_MAX;
}
/*}}}*/

/* key 를 리턴한다. */
int queue_elmt_key(PRIORITY_Q_ELEMENT *qe)
{/*{{{*/
    return qe->key;
}
/*}}}*/

/* 큐의 갯수를 리턴한다. */
int queue_count(PRIORITY_QUEUE *q)
{/*{{{*/
    int i, count = 0;
    PRIORITY_Q_ELEMENT *qe, *next_qe;

    for (i = 0; i < NUM_PRIORITY_QUEUES; i++)
    {
	for (qe = q->head[i]; qe; qe = next_qe)
	{
	    next_qe = qe->next;
	    count++;
	}
    }
    
    return (count);
}
/*}}}*/

/* 큐를 free 한다. */
void queue_free(PRIORITY_QUEUE *q)
{/*{{{*/
    int i;
    PRIORITY_Q_ELEMENT *qe, *next_qe;

    for (i = 0; i < NUM_PRIORITY_QUEUES; i++)
    {
	for (qe = q->head[i]; qe; qe = next_qe) 
	{
	    next_qe = qe->next;
	    free(qe);
	}
    }

    free(q);
}/*}}}*/
