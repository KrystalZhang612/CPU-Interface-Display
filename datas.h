#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct process {

//name of different processes 
    
	char *name;
    
//the total CPU
    
    int CPU_in_total;
    
//the completed time 
    
    int time_completed;
    
//the given CPU
    
    int CPUprovided;
    
//the I/O that was blocked
    
    int IOblocked;

//while processing I/O
    
    int  IOprocessing; 

//regarding to the usages of CPU and I/O
    
    //the probability that CPU is blocked

    double blocked_probability;
    
    //the time left in CPU
    
    int timeremainedCPU;
    
    //the time left in I/O
    
    int timeremainedIO;
    
};



//resources allocation 

struct resource {
    
    //either CPU or I/O
    
	char *name; 
    
    //busy
    
    int busy;
    
    //idle status 
    
    int idle;
    
    //util
    
    double util; 
    
    //dispatches
    
    int number; 
    
    double throughput;
};


//generate process

struct process* generatingProcess(char *name, int CPU_in_total, int time_completed, int CPUprovided, int  IOblocked, int IOprocessing , double prob) {
    
    struct process* temp = (struct process*)malloc(sizeof(struct process));
    
    //use temp to hold the variables 
    //point 
    
    temp->name = name;
    
    temp->CPU_in_total= CPU_in_total;
    
	temp->time_completed = time_completed;
    
	temp->CPUprovided = CPUprovided;

	temp->IOblocked = IOblocked;

    temp->IOprocessing = IOprocessing;
    
    //the probability of blocking 

    temp->blocked_probability = prob;
    
    temp->timeremainedCPU = CPU_in_total;
    
    temp->timeremainedIO= 0;
    
    return temp;
    
    //done pointing variables and initializing    
}


//setting up the process table 

void ProcessTableSetup() {
    
    
    printf("Processes:\n\n");
    
    //set up the required interface display 
    
	printf("   name     CPU time  when done  cpu disp  i/o disp  i/o time\n");
}



//process interface display

void ProcessDisplay(struct process* p) {
    
    printf("%-10s %6d     %6d    %6d    %6d    %6d\n", p->name, p->CPU_in_total, p->time_completed, p->CPUprovided, p-> IOblocked, p-> IOprocessing);
}



//build the resources 

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


//calculating the statistics of the resource 

void calcResourceStats(struct resource* res, int count) {
    
	res->util = (double)res->busy / (double)(res->busy + res->idle);
    
    res->throughput = (double)count / (double)(res->busy + res->idle);
}




//display the resource

void displayResource(struct resource* res) {
    
    if(strcmp(res->name, "CPU") == 0) {
        
        printf("\n%s:\n", res->name);
        
        printf("Total time spent busy: %d\n", res->busy);
        
        printf("Total time spent idle: %d\n", res->idle);
        
        printf("CPU utilization: %.2f\n", res->util);
        
        printf("Number of dispatches: %d\n", res->number);
        
        printf("Overall throughput: %.2f\n", res->throughput);
        
    }
    
    
    //error exception handling 
    
    else if(strcmp(res->name, "IO") == 0) {
        
        printf("\nI/O device:\n");
        
        printf("Total time spent busy: %d\n", res->busy);
        
        printf("Total time spent idle: %d\n", res->idle);
        
        printf("I/O utilization: %.2f\n", res->util);
        
        printf("Number of dispatches: %d\n", res->number);
        
        printf("Overall throughput: %.2f\n", res->throughput);
        
    }
    else {
        printf("Error Resource Type");
    }
}





