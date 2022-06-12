#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//node struct 

struct node {
	
	void* value;
	
	struct node* next;
};

//queue struct 

struct queue {
	
	int index_counting;
	
	struct node* head;
	
	struct node* back;
};


typedef struct queue* queue_t;

//create qeuue 

queue_t queue_create(void);

//destroy queue wether accessing or not

int queue_destroy(queue_t queue);

//enqueue

int queue_enqueue(queue_t queue, void *data);

//dequeue

int queue_dequeue(queue_t queue, void **data);

//delete queue whether already accessed or not 

int queue_delete(queue_t queue, void *data);

typedef int (*queue_func_t)(queue_t queue, void *data, void *arg);

//iteration

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data);

//length of queue

int queue_length(queue_t queue);

//we need to initialize heads and tail

queue_t queue_create(void) {

	struct queue* newQueue = (struct queue*)malloc(sizeof(struct queue));
	
	if(newQueue == NULL)
		
		return NULL;

	newQueue->head = NULL;
	
	newQueue->back = NULL;
	
	newQueue->index_counting = 0;
	
	return newQueue;
}

//done initializing, start counting

int queue_destroy(queue_t queue) {
	
	// if emtpy for deletion
	
	if (queue->head == NULL && queue->back == NULL) {
		
		free(queue);
		
		return 0;
	}
	
	return -1;
}

//enqueue

int queue_enqueue(queue_t queue, void *data) {
	
	struct node* newNode;
	
	void* newValue;
	
	if(queue == NULL || data == NULL)
		
		return -1;

	newValue= data;
	
	newNode = (struct node*)malloc(sizeof(struct node));
	
	if(newNode == NULL)
		return -1;

	newNode->next = NULL;
	
	newNode->value = newValue;

	// If the queue is empty
	//the enqueued value becomes the head
	
	if(queue->head == NULL) {
		
		queue->head = newNode;
		
		queue->back = newNode;
		
		queue->index_counting++;
		
		//keep counting 
		
		
	} else { 
		// Else, the new value is added
		// the end of the queue to implement FIFO
		
		queue->back->next = newNode;
		
		queue->back = queue->back->next;
		
		queue->index_counting++;
	}
	return 0;
}

int queue_dequeue(queue_t queue, void **data) {
	
	if(queue == NULL || queue->index_counting == 0 || data == NULL)
		
		return -1;

	// If there is only one element in the queue
	//dequeue it and ensure both head and back are null
	
	if(queue->head == queue->back) {
		
		*data = queue->head->value;
		
		queue->head = NULL;
		
		queue->back = NULL;
		
		queue->index_counting--;
	}
	else { 
		// Else, take remove the value 
		//the head and shift the head to the next node
		
		*data = queue->head->value;
		
		queue->head = queue->head->next;
		
		queue->index_counting--;
	}
	return 0;
}

//deletion 
int queue_delete(queue_t queue, void* data) {
	
	if(queue == NULL || data == NULL)
		
		return -1;

	struct node* currentNode = queue->head;
	

	if(currentNode->value == data) {
		
		queue->head = queue->head->next;
		
		free(currentNode);
		
		queue->index_counting--;
		
		return 0;
	}
	else { 
		//check if match 
		
		while(currentNode->next != NULL) {
			
			if(currentNode->next->value == data) {
				
				currentNode->next = currentNode->next->next;
				
				queue->index_counting--;
				
				currentNode = NULL;
			
				free(currentNode);
				
				return 0;
				
			} else {
				// Check whether to keep going
				
				if(currentNode->next->next != NULL) 
					
					currentNode = currentNode->next;
				else 
					break; //break if stop traversing 
				
			}
		}
	}
	
	
	currentNode = NULL;
	
	free(currentNode);
	
	return -1;
}


//iterate queue 

int queue_iterate(queue_t queue, queue_func_t func, void* arg, void** data) {
	
	if(queue == NULL || func == NULL || queue->head == NULL)
		
		return -1;
	
	struct node* currentNode = queue->head;
	
	struct node* nextNode = queue->head->next;
	
	
	if(func(queue, currentNode->value, arg)) {
		
		if(data != NULL) {
			
			*data = currentNode->value;
			
			return 0;
		}
	}

	// Check if more than oen elements present 
	
	if(nextNode == NULL) 
		
        return 0;
    
	currentNode = nextNode;
	
	nextNode = nextNode->next;

	while (currentNode != NULL) {
		
		if (func(queue, currentNode->value, arg)) {
			
			if (data != NULL) {
				
				*data = currentNode->value;
				
				return 0;
			}
		}

		// reassign current element if deleted 
		
		if(currentNode->value == NULL) {
			
			currentNode = nextNode;
			
			nextNode = nextNode->next;
		}

		currentNode = currentNode->next;
		
		if(nextNode->next != NULL)
			
			nextNode = nextNode->next;
	}
	
	return 0;
}

int queue_length(queue_t queue) {
	
	if (queue == NULL)
		
		return -1;
	
	return queue->index_counting;
}