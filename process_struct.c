//assert.h tp test the accuracies of our tests assumptions 

#include <assert.h>

//arange queues 

#include "arrange_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//process structure 

struct process {
	
	//different processes name 
    
	char *name;
    
    //number of CPU in total 
	
    int CPU_in_total;
    
    //current CPU time completed track
    
    int time_completed;
    
    //CPU provided 
    
    int CPUprovided;
	
    //I/O blocked
    
    int IOblocked;
    
    //I/O system processing
    
    int IOprocessing;
};


//resource 

struct resource {
	
	//different resources names 
    
	char *name; 
    
	//busy time 
	
    int busy;
	
    //idle time 
	
    int idle;
	
    //utilization time 
    
    double util; 
    
    //number of dispatches
    
    int number; 
    
   //number of throughput 
    
    double throughput;
};

//from diffrent queues 

//setup the ready quque 

queue_t ready_queue;

//setup the I/O queue 

queue_t io_queue;

//setup the statistic queue 

queue_t stats_queue;

//setup CPU system,I/O system 

struct resource *sysCPU, *sysIO;

//generating process attributes  

struct process* generatingProcess(char *name, int CPU_in_total, int time_completed, int CPUprovided, int IOblocked, int IOprocessing) {
    
    //use temp to assign and point variables 
    
    struct process* temp = (struct process*)malloc(sizeof(struct process));
    
    temp->name = name;
    
    temp->CPU_in_total  = CPU_in_total;
    
	temp-> time_completed =  time_completed;
    
	temp->CPUprovided = CPUprovided;
    
	temp-> IOblocked = IOblocked;

    temp->IOprocessing = IOprocessing;
    
    return temp;
}


//display the process

void ProcessDisplay(struct process* p) {
    
    //properly print in order as intended 
    
    printf("name\tCPU_in_total\ttime_completed\tCPUprovided\tIOblocked\tIOprocessing\n");
    
    printf("%s\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", p->name, p-> CPU_in_total, p->time_completed, p->CPUprovided, p->IOblocked, p-> IOprocessing);
}

//arranging resources variables to build resources

struct resource* buildResource(char *name, int busy, int idle, int number) {
    
    struct resource* temp = (struct resource*)malloc(sizeof(struct resource));
    
    temp->name = name;
    
    temp->busy = busy;
    
	temp->idle = idle;
    
	temp->util = (double)busy / (double)(busy + idle);
    
	temp->number = number;

    temp->throughput = (double)number / (double)(busy + idle);
    
    return temp;
}


//display resource 

void displayResource(struct resource* res) {
	
	//CPU resources 
    
    if(strcmp(res->name, "CPU")) {
        
	//display current CPU resources name 
	    
        printf("%s:\n", res->name);
	    
	//display current CPU busy time
        
        printf("Total time spent busy: %d\n", res->busy);
	
	//display current CPU idle time 
        
        printf("Total time spent idle: %d\n", res->idle);
	
	//display current CPU number of ultilization 
        
        printf("CPU utilization: %.2f\n", res->util);
	    
        //display the number of dispatches 
	    
        printf("Number of dispatches: %d\n", res->number);
	    
	//display the overall throughputs
        
        printf("Overall throughput: %.2f\n", res->throughput);
        
    }
	//current I/O resources display
	
    else if(strcmp(res->name, "IO")) {
	    
        //display the current I/O name 
	    
        printf("%s:\n", res->name);
	  
	    //display the current I/O total time spent busy 
        
        printf("Total time spent busy: %d\n", res->busy);
        
	    //display the current I/O total time spent idle 
	    
        printf("Total time spent idle: %d\n", res->idle);
	    
	    //display the current I/O device ultilization 
        
        printf("I/O device utilization: %.2f\n", res->util);
	    
	    //display the current number of times I/O was started 
        
        printf("Number of times I/O was started: %d\n", res->number);
	    
	    //display the current I/O overall throughput number 
        
        printf("Overall throughput: %.2f\n", res->throughput);
    }
    else {
        //error exception handling 
        
        printf("Error Resource Type");
    }
}


//driver 


int main(void) {
    
     //generate random

    (void) srandom(12345);
	
	//create ready queue
	
    ready_queue = queue_create();
	
	//create I/O queue 
    
    io_queue = queue_create();
	
	//create status queue to track status 

    stats_queue = queue_create();
	
	//initialize all times 

    int clock = 0;
    
    int busy_time = 0;
    
    int idle_time = 0;
    
    int dispatches = 0;

    //read the process functionaloties into queue

    int process_count = ready_queue->index_counting;
    
    //filled ready queue 
    
    while (ready_queue->index_counting > 0 && io_queue->index_counting > 0) {
        
        //cpu processsing 
        
        struct process* cpu; 
        
        //runtime left in cpu
        
        int remaining_job_runtime = 0; 
        
        //non zero if blocking
        
        int requires_blocking = 0; 

        //setup next CPU and reset 
        
        if (ready_queue->index_counting > 0 && cpu == NULL) {
            
            cpu = ready_queue->head; //process at start of the queue
            
            int remaining_job_runtime = cpu->time_completed;
            
            //rand num [0,1]
            
            double rng = (double)random() / (double)RAND_MAX;
            
            //at least 2 units 
            
            if (remaining_job_runtime >= 2 && rng < cpu->IOblocked) {
                
                //range between 1 and runtime inclusively
                
                remaining_job_runtime = rand();
            }
            
		//return requiring blocking is true 
		
            requires_blocking = 1;
		
		//increment dispatches 
            
            dispatches++;
        }

        //run it 
        
        clock++;
        
	    //if there is still jobs remained with run time 
	    
        if (remaining_job_runtime > 0) {
		
		//decrement it 
            
            remaining_job_runtime--;
		
		//increment busy time 
            
            busy_time++;
            
            //return true if job is accomplished 
            
            if (remaining_job_runtime == 0) {
            
            //move if blocked
            
                if (requires_blocking != 0) {
                    
                    queue_enqueue(io_queue, cpu);
                }
                else {
                    //finihing up otherwise
                    
                    int ioblock_count = 0;
                    
                    int io_time = 0;
                    
                    //lacking of args 

                    struct process* stats = generatingProcess(cpu->name, cpu->time_completed, clock, ioblock_count, io_time);
                    
                    queue_enqueue(stats_queue, stats);
                
                    cpu = NULL;
                }
            }
        }
        else {
            
            //incrementing idle time 
            
            idle_time++;
        }
    }
    return 0;
}

