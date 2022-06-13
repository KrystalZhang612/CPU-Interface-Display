
//assert.h to check accuarcies of our tests assumptions 

#include <assert.h>

#include "arrange_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//same from proces_struct 

struct process {
    
    char *name;
    
    int CPU_in_total;
    
    int time_completed;
    
    int CPUprovided;
    
    int IOblocked;
    
    int IOprocessing;
};




//same from the resources building process

struct resource {
    
    char *name; 
    
    int busy;
    
    int idle;
    
    double util; 
    
    //dispatches
    
    int number; 
    
    double throughput;
};


queue_t ready_queue;

queue_t io_queue;

queue_t stats_queue;

struct resource *sysCPU, *sysIO;

//I/O devices

struct process* iodev; 

struct process* generatingProcess(char *name, int CPU_in_total, int time_completed, int CPUprovided, int IOblocked, int IOprocessing) {
    
    struct process* temp = (struct process*)malloc(sizeof(struct process));
    
    temp->name = name;
    
    temp->CPU_in_total = CPU_in_total;
    
	temp->time_completed = time_completed;
    
	temp->CPUprovided = CPUprovided;
    
	temp->IOblocked = IOblocked;
    
    temp->IOprocessing= IOprocessing;
    
    return temp;
}




//display process 

void ProcessDisplay(struct process* p) {
    
    printf("name\tCPU_in_total\ttime_completed\tCPUprovided\tIOblocked\tIOprocessing\n");

    printf("%s\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", p->name, p->CPU_in_total, p->time_completed, p->CPUprovided, p->IOblocked, p->IOprocessing);
}

//build resources

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

//display resources

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
        //else error exception handling 
        
        printf("Error Resource Type");
    }
}

//move to IO device

void movetoiodev(int remaining_runtime, struct process *pointer) {
    
    iodev = pointer;
    
    // determine runtime
    
    int io_runtime; 
    
    if (remaining_runtime == 0){
        
        io_runtime = 1;
    }
    else{ 
	 // generate random number
    
	 io_runtime = random()/30;
    }

    // Update process I/O runtime
    
    pointer ->IOprocessing = pointer->IOprocessing  + io_runtime;
    
    sysIO->busy = sysIO->busy + io_runtime; 
    
    return;
}


//run IO

void runio(int remaining_runtime) {
    
    //if empty 
    
    if(io_queue->index_counting == 0 && iodev == NULL) { 
        
        sysIO->idle += 1;
        
	return;
    }
    else if(iodev == NULL){ 
        
        //I/O device inactive,move first
        
        struct process* currentProcess = io_queue->head;
        
	movetoiodev(remaining_runtime, currentProcess);
    }
	
    // if completed process 
    //move to ready queue
    
    queue_dequeue(io_queue, (void**)&iodev);
    
    queue_enqueue(ready_queue, iodev);
    
    iodev = NULL; 
    
    sysIO->idle += 1;
    
    return;
}

//driver 

int main( int argc, char *argv[] ) {
    
    //set up the maximum length 
    int MAX_LENGTH = 80;
    
    int line_num, proc_run_time, total_args_read;
    
    char proc_name[MAX_LENGTH], line[MAX_LENGTH];
    
    float proc_block_prob;
    
    // args is 3
    //complain, exit
    
    if ( argc != 3 ) {
        
        return (1);
    }
	
    // if 2nd arg is not r or f
    
    if (strcmp(argv[1],"-r") != 0 && strcmp(argv[1],"-f") != 0) {
        
        fprintf(stderr, "Usage: ./prsim [-r | -f] file\n");
        
	return (1);
    }
	
    FILE *file = fopen(argv[2], "r");
    if (!file) {
        return (1);
    }
    
    // Get each line
    while (fgets(line, MAX_LENGTH, file)) {  
        
        line_num++;
        
        total_args_read = sscanf(line, "%s %d %f", proc_name, &proc_run_time, &proc_block_prob);
        
        if (total_args_read != 3) {
            
		fprintf(stderr, "Malformed line %s(%d)\n", argv[2], line_num);
        
		return (1);
	}
	if (strlen(proc_name) > 10) {
        
		fprintf(stderr, "name is too long %s(%d)\n", argv[2], line_num);
        
		return (1);
	}
	if (proc_run_time <= 0) {
        
		fprintf(stderr, "runtime is not positive integer %s(%d)\n", argv[2], line_num);
        
		return (1);
	}
	if (proc_block_prob < 0 || proc_block_prob > 1) {
        
		fprintf(stderr, "probability <0 or >1 %s(%d)\n", argv[2], line_num);
        
		return (1);
	}
    }
	
    // fcfs scheduling
    
    (void) srandom(12345);
    
    ready_queue = queue_create();
    
    io_queue = queue_create();

    stats_queue = queue_create();

    int clock = 0;
    
    int busy_time = 0;
    
    int idle_time = 0;
    
    int dispatches = 0;
    

    //read into ready queue
    
    int process_count = ready_queue->index_counting;
    
    //filled ready_queue
    
    while (ready_queue->index_counting> 0 && io_queue->index_counting > 0) {
        
        //process cpu
        
        struct process* cpu; 
        
        //runtime left
        
        int remaining_job_runtime = 0;
        
        //non zero, 0 otherwise
        
        int requires_blocking = 0; 
	    
	    //similarly to the process structure implementation 
	    
	    //track various runtimes 
	    
	   //to strictly adhere to the first-come-first served scheduling orders 

        if (ready_queue->index_counting > 0 && cpu == NULL) {
            
            cpu = ready_queue->head; 
            
            int remaining_job_runtime = cpu->time_completed;
            
            //generate rand num [0,1]

            double rng = (double)random() / (double)RAND_MAX;
            
            //Task blocks only if has at least 2 time units
            
            if (remaining_job_runtime >= 2 && rng < cpu->IOblocked) {
                
                //runtime [0,1] 
                
                remaining_job_runtime = rand();
            }
            requires_blocking = 1;
            
            dispatches++;
        }
        
        

        //run remaining jobs 
        
        clock++;
        
        if (remaining_job_runtime > 0) {
            
            remaining_job_runtime--;
            
            busy_time++;
            
            //return true if jpb is done
            
            if (remaining_job_runtime == 0) {
                
                if (requires_blocking != 0) {
                    
                    queue_enqueue(io_queue, cpu);
                }
                else {
                    
                    int ioblock_count = 0;
                    
                    int io_time = 0;
                    
                    //lacking of args, ignored. 

                    struct process* stats = generatingProcess(cpu->name, cpu->time_completed, clock, ioblock_count, io_time); 
                    
                    queue_enqueue(stats_queue, stats);
                    
                    //move to CPU

                    cpu = NULL;
                }
            }
        }
        else {
		
		//keep allocating the idle runtime
            
            idle_time++;
        }
    }
    return 0;
}

