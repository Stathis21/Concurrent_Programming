#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <math.h>
#include <time.h>

int jobs_left;
short int terminate = 0;

typedef struct job {				//job struct containing the number and the result
	int number;
	int is_prime;
}job;


typedef struct thread_info {
	pthread_t thread_id;
	int thread_index;				//Thread_array index
	short int available;			//available for job,also used by main to synchronize 
	job* thread_job;				//pointer used by threads,to write the result.
}thread_info;


job* job_array;
thread_info* thread_array;





int prime_test(int number) {
    int i;
	
    for (i=2; i<=sqrt(number); i++) {
        if (number % i == 0)
		return 0;
    }
    return 1;
}





void worker_thread(int* id) {
	int err;
	while (1) {
		if (terminate ==1) 						//Terminate flag
			break;


	
		while (thread_array[*id].available) {		//Waiting for job
			err = sched_yield();
			if (err == -1) {
				printf("Error yielding CPU!\n");
			}
			if (terminate ==1) 
				break;
		}
		if (terminate ==1) 
				break;
		
		thread_array[*id].thread_job->is_prime = prime_test(thread_array[*id].thread_job->number); //writing result
		thread_array[*id].available = 1;			//Job done,waiting for new

	}
	thread_array[*id].available = 0;				//notifying main that i will terminate
	return;
	
	
	
	
	
	
	
}

int main(int argc,char* argv[]) {
	int number_of_threads,i,finished_sum;
	
	if (argc<3) {
		printf("Please enter as first argument the number of threads, and at least one number to check if it is prime\n");
		return 0;
	}	
	
	number_of_threads = atoi(argv[1]);	//Number of threads given as arguemnt.
	clock_t begin = clock();
	
	thread_array = (thread_info*)calloc(number_of_threads, sizeof(thread_info));
	if (thread_array == NULL) {
		printf("Error allocating memory for the thread_array.\n");
		return 0;
	}
	jobs_left= argc-2 ;
	job_array = (job*)calloc(jobs_left, sizeof(job));
	if (job_array == NULL) {
		printf("Error allocating memory for the job_array.\n");
		return 0;
	}
	
	
	for(i=0; i<jobs_left; i++){			//job array init
		job_array[i].number = atoi(argv[i+2]);
		job_array[i].is_prime = -1; 			//no calculations have been made
	}

	for (i=0; i<number_of_threads; i++) {		//thread array init
		
		thread_array[i].available = 1;
		thread_array[i].thread_index = i;
	}
	
	for (i=0; i<number_of_threads; i++) {			//creating threads
		if(pthread_create(&thread_array[i].thread_id,NULL,(void*)worker_thread,(void*)&thread_array[i].thread_index))
			printf("Error creating thread!!\n");
	}
	
	i=0;
	while(jobs_left>0){					//As long as there are jobs
		
		
		
		while(1){
			if(thread_array[i].available){				//Search for available thread
				break;
			}
			i = (i+1)%number_of_threads;
		}
		thread_array[i].thread_job = &job_array[jobs_left-1];		//assign job
		jobs_left--;
		thread_array[i].available = 0;								//thread no longer available
	}
		
	while (1) {										//wait for all workers to become available
		finished_sum = 0;
		for (i=0; i<number_of_threads; i++) {
			finished_sum += thread_array[i].available;
		}
		
		if (finished_sum == number_of_threads) {
			terminate = 1;							//notify workers to terminate
			break;
		}
		
	}
	while (1) {										//wait for all workers to be terminated
		finished_sum = 0;
		for (i=0; i<number_of_threads; i++) {
			finished_sum += thread_array[i].available;
		}
		if (finished_sum == 0)
			break;
	}
	

	for(i=0; i<argc-2; i++){
		if(job_array[i].is_prime){
			printf("Number %d is a prime.\n",job_array[i].number);
		}
		else{
			printf("Number %d is not a prime.\n",job_array[i].number);
		}
	}
	
	
	free(thread_array);
	free(job_array);
	
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time was: %lf\n",time_spent);
	return(0);
}



