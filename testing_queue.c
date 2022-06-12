#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "arrange_queue.h"


//testing queues

#define ASSERTING_TEST(assert)	\
do {							\
	printf("ASSERT: " #assert " ... ");		\
	if (assert) {					\
		printf("PASS\n");			\
	} else	{					\
		printf("FAIL\n");			\
		exit(1);				\
	}						\
} while(0)



    //create test

void test_create(void) {
    
	fprintf(stderr, "*** TEST create ***\n");

	ASSERTING_TEST(queue_create() != NULL);
}

// Enqueue/Dequeue

void test_queue_simple(void) {
    
	int data = 3;
    
	int data2 = 5;
    
	int data3 = 456;
    
	int *pointer;
    
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
    
	queue_enqueue(q, &data);
    
	queue_enqueue(q, &data2);
    
	queue_enqueue(q, &data3);

	queue_dequeue(q, (void**)&pointer);
    
	ASSERTING_TEST(pointer== &data);
    
	queue_dequeue(q, (void**)& pointer);
    
	ASSERTING_TEST(pointer == &data2);
    
	queue_dequeue(q, (void**)& pointer);
    
	ASSERTING_TEST(pointer == &data3);
}



//check if there is errors 

void test_errors(void) {
    
    int myerror;
    
    int *pointer;
    
    int data = 10;
    
    int data2 = 90;
    
    queue_t q;

    fprintf(stderr, "*** TEST errors ***\n");

    q = queue_create();
    
    // Dequeue from empty
    
    myerror = queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(myerror == -1);
    
    // Enqueue from NULL pointer
    
    myerror = queue_enqueue(q, NULL);
    
    ASSERTING_TEST(myerror == -1);
    
    queue_enqueue(q, &data);
    
    // if the node deleted is detected
    //error found, return error 
    
    myerror = queue_delete(q, (void*)20);
    
    ASSERTING_TEST(myerror == -1);
    
    // error not found
    
    queue_enqueue(q, &data2);
    
    myerror = queue_delete(q, (void*)20);
    
    ASSERTING_TEST(myerror == -1);
}



//Delete iterative function

static int delete_item(queue_t q, void *data, void *arg) {
    
    int *a = (int*)data;
    
    int match = (int)(long)arg;
    
    (void)q; 

    if(*a == match)
        
        queue_delete(q, data);
    
    return 0;
}



//double check deletion 

void test_queue_delete(void) {
    
    queue_t q;
    
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    size_t i;
    
    int *pointer;

    fprintf(stderr, "*** TEST queue_delete ***\n");

    //initialize the queue and enqueue items 
    
    q = queue_create();
    
    for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        
        queue_enqueue(q, &data[i]);
    

    // Delete middle 
    
    queue_iterate(q, delete_item, (void*)5, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 9);
    
    // Delete head node
    
    queue_iterate(q, delete_item, (void*)1, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 8);
    
    // Delete tail node 
    
    queue_iterate(q, delete_item, (void*)10, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 7);
    
    // entirely delete the deleted nodes
    
    queue_iterate(q, delete_item, (void*)10, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 7);
}


//double check deletion

void test_reenqueue(void) {
    
    queue_t q;
    
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    size_t i;
    
    int *pointer;
    
    int data2 = 5;

    fprintf(stderr, "*** TEST queue_reenqueue ***\n");

    //Initialize the queue and enqueue items
    
    q = queue_create();
    
    for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        
        queue_enqueue(q, &data[i]);

    // Delete middle, similarly 
    
    queue_iterate(q, delete_item, (void*)5, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 9);
    
    // Enqueue 
    
    queue_enqueue(q, &data2);
    
    ASSERTING_TEST(queue_length(q) == 10);
    
    // Delete again 
    
    queue_iterate(q, delete_item, (void*)5, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 9);
}



//double test deletion


void test_double_dequeue(void) {
    
    int myerror;
    
    int *pointer;
    
    int data = 10;
    
    int data2 = 90;
    
    queue_t q;

    fprintf(stderr, "*** TEST double dequeue ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_enqueue(q, &data2);
    
    // Dequeue at emptiness
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 1);
    
    queue_dequeue(q, (void**)&pointer);
    
    myerror = queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(myerror == -1);
    
    ASSERTING_TEST(queue_length(q) == 0);
}




//callback function for certain values 

static int inc_item(queue_t q, void *data, void *arg) {
    
    int *a = (int*)data;
    
    int inc = (int)(long)arg;

    if(*a == 9) 
        
        queue_delete(q, data);
    else
        *a += inc;
    return 0;
}


static int find_item(queue_t q, void *data, void *arg) {
    
    int *a = (int*)data;
    
    int match = (int)(long)arg;
    
    (void)q; 

    if(*a == match)
        
        return 1;

    return 0;
}

//iterate test 

void test_iterator(void) {
    
    queue_t q;
    
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    
    size_t i;
    
    int *pointer = NULL;

    fprintf(stderr, "*** TEST queue_iterate ***\n");
    

    //initialize the enqueue
    
    q = queue_create();
    
    for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        
        queue_enqueue(q, &data[i]);

    //add value 
    
    queue_iterate(q, inc_item, (void*)1, NULL);
    

    ASSERTING_TEST(data[0] == 2);
    
    assert(queue_length(q) == 9);

    queue_iterate(q, find_item, (void*)5, (void**)&pointer);
    
    ASSERTING_TEST(pointer != NULL);
    
    ASSERTING_TEST(*pointer == 5);
    
    ASSERTING_TEST(pointer == &data[3]);
}

//destroy test and acess again 

void test_destroy(void) {
    
    queue_t q;
    
    int myerror;
    
    int* pointer;

    fprintf(stderr, "*** TEST queue_destroy ***\n");
    
    //initiliaze the queue 
    
    q = queue_create();

    // Destroy 
    myerror = queue_destroy(q);
    
    ASSERTING_TEST(myerror == 0); 
    
}


//test queue via chars 

void test_character(void) {
    
    int data = 'a';
    
    int data2 = 't';
    
    int* pointer;
    
    queue_t q;

    fprintf(stderr, "*** TEST character ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_enqueue(q, &data2);
    
    ASSERTING_TEST(queue_length(q) == 2);
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 1);
}




//test queue creation and iteration 

void test_queue_create_iter(void) {
    
    queue_t q;
    
    int myerror;
    
    int* pointer;

    fprintf(stderr, "*** TEST queue_create_iter ***\n");

    q = queue_create();
    
    myerror = queue_iterate(q, delete_item, (void*)3, (void**)&pointer);
    
    ASSERTING_TEST(myerror == -1);
}


//testing iteration at queues sized of 1

void test_one_elem(void) {
    
    queue_t q;
    
    char data = 'c';
    
    char* pointer;

    fprintf(stderr, "*** TEST one_elem ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_iterate(q, delete_item, (void*)'a', (void**)&pointer);
    
    ASSERTING_TEST(queue_length(q) == 1);
}

//test emptying out queue and then run iteration 

void test_delete_iter(void) {
    
    queue_t q;
    
    int data = 5;
    
    int data2 = 10;
    
    int* pointer; 
    
    int myerror;

    fprintf(stderr, "*** TEST delete_iter ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_enqueue(q, &data2);
    
    queue_dequeue(q, (void**)pointer);
    
    queue_dequeue(q, (void**)pointer);
    
    ASSERTING_TEST(queue_length(q) == 0);
    
    myerror = queue_iterate(q, delete_item, (void*)6, (void**)&pointer);
    
    ASSERTING_TEST(myerror == -1);
}

//destroy test

void test_delete_create(void) {
    
    queue_t q;
    
    int data = 9;
    
    int data2 = 10;
    
    int* pointer;

    fprintf(stderr, "*** TEST delete_create ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_dequeue(q, (void**)&pointer);
    
    queue_destroy(q);
    
    q = queue_create();
    
    queue_enqueue(q, &data2);
    
    ASSERTING_TEST(queue_length(q) == 1);
}

//test without destroying

void test_create_create(void) {
    
    queue_t q;
    
    int data = 9;
    
    int data2 = 10;
    
    int* pointer;

    fprintf(stderr, "*** TEST create_create ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    q = queue_create();
    
    queue_enqueue(q, &data2);
    
    ASSERTING_TEST(queue_length(q) == 1);
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(*pointer == 10);
}


//test enqueue at the end 

void test_enqueue_dequeue(void) {
    
    queue_t q;
    
    int data = 1;
    
    int data2 = 5;
    
    int* pointer;

    fprintf(stderr, "*** TEST enqueue_dequeue ***\n");

    q = queue_create();
    
    queue_enqueue(q, &data);
    
    queue_enqueue(q, &data2);
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(*pointer == 1);
    
    queue_enqueue(q, &data);
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(*pointer == 5);
    
    queue_dequeue(q, (void**)&pointer);
    
    ASSERTING_TEST(*pointer == 1);
}


//run 2 queues to test 

void test_multi_queue(void) {
    
    queue_t q1;
    
    queue_t q2;
    
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    size_t i;

    fprintf(stderr, "*** TEST multi_queue ***\n");
    
    q1 = queue_create();
    
    q2 = queue_create();
    
    for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        
        queue_enqueue(q1, &data[i]);
    
    for(i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        
        queue_enqueue(q2, &data[i]);

    queue_iterate(q1, inc_item, (void*)1, NULL);
    
    ASSERTING_TEST(data[0] == 2);
    
    ASSERTING_TEST(queue_length(q1) == 9);
    
    ASSERTING_TEST(queue_length(q2) == 10);
    
    queue_iterate(q2, inc_item, (void*)1, NULL);
    
    ASSERTING_TEST(data[0] == 3);  
}


//test our functions in driver code

int main(void) {
	
   //test queue creation 
    
    test_create();
	
   //test simple queue 
    
    test_queue_simple();
    
    
    test_errors();
    
    test_queue_delete();
    
    test_reenqueue();
    
    test_double_dequeue();
    
    test_iterator();
    
    test_destroy();
    
    test_character();
    
    test_queue_create_iter();
    
    test_delete_iter();
    
    test_create_create();
    
    test_enqueue_dequeue();
    
    test_multi_queue();

    return 0;
}
