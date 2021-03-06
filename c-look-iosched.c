/*
 * elevator c_look
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

int currsect = -1;

struct c_look_data {
	struct list_head queue;
};

static void c_look_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int c_look_dispatch(struct request_queue *q, int force)
{
	struct c_look_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		//Assign global variable for C-LOOK seeking
		currsect = blk_rq_pos(rq);
		return 1;
	}
	return 0;
}

static void c_look_add_request(struct request_queue *q, struct request *rq)
{
	struct c_look_data *nd = q->elevator->elevator_data;
	struct list_head *current = NULL;
	
	list_for_each(current, &nd->queue){
		struct request *newr = list_entry(current, struct request, queuelist);
		if(blk_rq_pos(rq) < currsect){
			if((blk_rq_pos(newr) < currsect) && (blk_rq_pos(rq) < blk_rq_pos(newr))){
				//if the new request is behind the current sector place it at back of queue
				break;
			}
		}else{
			//this will go through the nd-queue until it finds a request to insert behind.
			if((blk_rq_pos(rq) < blk_rq_pos(newr))||(blk_rq_pos(c) < currsect)){
				break;
			}
		}
	}
	printk("Adding request at: %d", blk_rq_pos(list_entry(current, struct request, queuelist)))
	list_add_tail(&rq->queuelist, &nd->queue);
}

static struct request *
c_look_former_request(struct request_queue *q, struct request *rq)
{
	struct c_look_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
c_look_latter_request(struct request_queue *q, struct request *rq)
{
	struct c_look_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int c_look_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct c_look_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void c_look_exit_queue(struct elevator_queue *e)
{
	struct c_look_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_c_look = {
	.ops = {
		.elevator_merge_req_fn		= c_look_merged_requests,
		.elevator_dispatch_fn		= c_look_dispatch,
		.elevator_add_req_fn		= c_look_add_request,
		.elevator_former_req_fn		= c_look_former_request,
		.elevator_latter_req_fn		= c_look_latter_request,
		.elevator_init_fn		= c_look_init_queue,
		.elevator_exit_fn		= c_look_exit_queue,
	},
	.elevator_name = "c_look",
	.elevator_owner = THIS_MODULE,
};

static int __init c_look_init(void)
{
	return elv_register(&elevator_c_look);
}

static void __exit c_look_exit(void)
{
	elv_unregister(&elevator_c_look);
}

module_init(c_look_init);
module_exit(c_look_exit);


MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");
