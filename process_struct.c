#include <assert.h>
#include "arrange_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//process struct

struct process {
    
	char *name;
    
    int CPU_in_total;
    
    int time_completed;
    
    int CPUprovided;
    
    int IOblocked;
    
    int IOprocessing;
};


//resource 

struct resource {
    
	char *name; 
    
    int busy;
    
    int idle;
    
    double util; 
    
    //dispatches
    
    int number; 
    
    double throughput;
};

//from diffrent queues 

queue_t ready_queue;

queue_t io_queue;

queue_t stats_queue;

struct resource *sysCPU, *sysIO;

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
    
    if(strcmp(res->name, "CPU")) {
        
        printf("%s:\n", res->name);
        
        printf("Total time spent busy: %d\n", res->busy);
        
        printf("Total time spent idle: %d\n", res->idle);
        
        printf("CPU utilization: %.2f\n", res->util);
        
        printf("Number of dispatches: %d\n", res->number);
        
        printf("Overall throughput: %.2f\n", res->throughput);
        
    }
    else if(strcmp(res->name, "IO")) {
        
        printf("%s:\n", res->name);
        
        printf("Total time spent busy: %d\n", res->busy);
        
        printf("Total time spent idle: %d\n", res->idle);
        
        printf("I/O device utilization: %.2f\n", res->util);
        
        printf("Number of times I/O was started: %d\n", res->number);
        
        printf("Overall throughput: %.2f\n", res->throughput);
    }
    else {
        //error exception handling 
        
        printf("Error Resource Type");
    }
}


//driver 


int main(void) {
    

    (void) srandom(12345);
    
    ready_queue = queue_create();
    
    io_queue = queue_create();

    stats_queue = queue_create();

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
            
            requires_blocking = 1;
            
            dispatches++;
        }

        //run it 
        
        clock++;
        
        if (remaining_job_runtime > 0) {
            
            remaining_job_runtime--;
            
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

