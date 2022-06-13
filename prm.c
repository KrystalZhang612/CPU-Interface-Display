
//PRSIM file

//to defines miscellaneous symbolic constants and types
//declares miscellaneous functions

#include <unistd.h>

#include "datas.h"

#include "arrange_queue.h"

//maximum quantum as 5

const int QUANTUM_MAX = 5;

//set up the clock 

int clock = 1;

//initialize RR

int round_robin = 0;

//ready queue and I/O queue 

queue_t ready_queue;

queue_t io_queue;

struct resource *sysCPU, *sysIO; 

int cpu_idle = 0;

int cpu_busy = 0;

//current process running in CPU 

struct process* cpu; 

//the non-zero if cpu process is blocking

int requires_blocking = 0; 

//time left to run before blocking

int runTime_left = 0; 

struct process* iodev; 

int wakeup_io = 0;

int io_is_idle = 0;

int dispatches = 0;

int io_time = 0;

//done initializing
//prepare CPU queue 

void preparingCPU() {
    
    
    queue_dequeue(ready_queue, (void**)&cpu);
    
    runTime_left = cpu ->  timeremainedCPU;
    

    if (round_robin) {
        
        runTime_left = runTime_left > QUANTUM_MAX ? QUANTUM_MAX : runTime_left;
    }

    if (cpu-> timeremainedCPU >= 2) {
        
        //Roll a number to test for blocking
        
        double rng = (double)random() / (double)RAND_MAX; // rand between 0 and 1
        
        if (rng < cpu->blocked_probability) {
            //generate time for running until its moved to I/O
            runTime_left = rand() % runTime_left + 1;

            //mark for blocking
            requires_blocking = 1;
        }
    }
    //increase stats
    cpu->CPUprovided++;
    
    sysCPU->number++;
    
    dispatches++;
}



//run CPU


void runCPU() {
    
    //if CPU is idle
    
    if (cpu == NULL && ready_queue->index_counting == 0) { 
        
        sysCPU->idle++;
        
        return;
    }
    //dispatch process into CPU
    
    if (cpu == NULL) { 
        
        preparingCPU();
    }
    
    //if finished, terminate the process
    
    if (cpu-> timeremainedCPU == 0) {
        
        cpu-> time_completed = clock;
        
        //remove the last item from CPU queue and display it 
        
        ProcessDisplay(cpu);
        
        cpu = NULL;
        
        sysCPU->idle++;

        return;
    }
    
    //decrement the CPU time remained 
	
    cpu-> timeremainedCPU--;

    //decrement runtime left 
    
    runTime_left--;
    
    //increment to allocate sysCPU busy time
	
    sysCPU->busy++;
    
    if (requires_blocking) {
        
        //block to i/o
        
        if (runTime_left <= 0) {
            
            wakeup_io = 1;
            
            queue_enqueue(io_queue, cpu);
            
            cpu = NULL;
            
            requires_blocking = 0;
        }
    }
    //else do RR quantum 
    
    else if (round_robin) { 
        
        if (runTime_left <= 0) {
            
            queue_enqueue(ready_queue, cpu);
            
            cpu = NULL;
        }
    }

}


//move I/O to dev 

void movetoiodev(int remaining_runtime, struct process *pointer) {
    
    iodev = pointer;
    
    // determine runtime
    
    int io_runtime; 
    
    if (pointer -> timeremainedCPU == 0){
        
        io_runtime = 1;
    }
    else{ 
        
	    io_runtime = random() % 30 + 1;
    }
    
    pointer -> timeremainedIO = io_runtime;
    
    
    // Update process I/O runtime, total IO busy runtime
    
    pointer-> IOblocked++;
    
    pointer-> IOprocessing += io_runtime;
    
    return;
}

//after that we need to run I/O


void runio() {
    
    //if I/O queue is empty and I/O device is empty as well
    
    if(io_queue->index_counting== 0 && iodev == NULL) { 
        
        sysIO->idle += 1;
        
        io_is_idle = 1;
        
	    return;
    }
    
    //if I/O device is inactive 
    else if(iodev == NULL){ 
        
        if(io_is_idle && wakeup_io) { 
            
            //woke up 
            //begin to append 
            
            sysIO->idle += 1;
            
            io_is_idle = 0;
            
            wakeup_io = 0;
            
            return;
        }
        
        struct process* currentProcess; 
        
        queue_dequeue(io_queue, (void**)&currentProcess);
	    
	    //setup the current position
	    //move to I/O device
        
	    movetoiodev(currentProcess->IOprocessing, currentProcess);
	    
	//keep traversing through I/O system 
        
        sysIO->number++;
    }
	
    //similarly, decrement I/O remaining time. 
	
    iodev-> timeremainedIO--;
    
    //allocate sysIO
    
    sysIO->busy += 1;
    
    //if I/O is completed
    //move to ready queue 
    
    if (iodev-> timeremainedIO <= 0) { 
        
        //back to idle 
        
        io_is_idle = 1; 
        
        queue_enqueue(ready_queue, iodev);
	    
        //move to I/O 
        
        iodev = NULL; 
        
    }
    return;
}


//driver

int main( int argc, char *argv[] ) {
    
    int MAX_LENGTH = 80;
    
    int line_num, proc_run_time, total_args_read;
    
    char* proc_name;
    
    char line[MAX_LENGTH];
    
    float proc_block_prob;
    
    
    // exit out of arguments errors
    
    if ( argc != 3 ) {
        
        return (1);
    }
    
    
    //if 2nd arg neither f nor r
    
    if (strcmp(argv[1], "-r") == 0) {
        
        round_robin = 1;
    }
    
    else if (strcmp(argv[1], "-f") == 0) {
        
        round_robin = 0;
    }
    
    else {
        
        fprintf(stderr, "Usage: ./prsim [-r | -f] file\n");
        
	    return (1);
    }
    

    char workingdir[MAX_LENGTH];
    
    getcwd(workingdir,MAX_LENGTH);
    
//assign the working directory 
    
    strcat(workingdir,"/");
    
    strcat(workingdir,argv[2]);
    
	
    FILE *file = fopen(workingdir, "r");
    
    if (!file) {
        
        return (1);
    }
	
    
    ready_queue = queue_create();
    
    io_queue = queue_create();
    
    // Get each line
    
    while (fgets(line, MAX_LENGTH, file)) { 
        
        line_num++;
        
        proc_name = malloc(MAX_LENGTH);
        
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
        
        
        //properly handled the process interface 
        
        
        //add to ready_queue
        
        struct process* p = generatingProcess(proc_name, proc_run_time,0,0,0,0,proc_block_prob);
        
        queue_enqueue(ready_queue, p);
    }
	
    //initialize variables
    
    (void) srandom(12345);
    
    sysCPU = buildResource("CPU",0,0,0);
    
    sysIO = buildResource("IO",0,0,0);
    
    int process_count = ready_queue->index_counting;
    
    
    ProcessTableSetup();
    
    
    while (ready_queue->index_counting > 0 || io_queue->index_counting > 0 || cpu != NULL || iodev != NULL) {
        
        runCPU();
        
        runio();
        
        clock++;
        
        wakeup_io = 0;
    }

//clock time printed at the end
    
	printf("\nSystem:\n");
    
	printf("The wall clock time at which the simulation finished: %d\n", clock-1);
    

    //caculate values of idle and busy time 
    
    calcResourceStats(sysCPU, process_count);
    
    calcResourceStats(sysIO, process_count);

    //display CPU+I/O
    
    displayResource(sysCPU);
    
    displayResource(sysIO);
    
    return 0;
}


//handled user command line input prsim
