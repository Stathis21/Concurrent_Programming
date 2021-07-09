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
#include "prime_calc_monitor.h"

int number_of_threads,jobs_left;
int terminate = 0;




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
	while (1) {
			
		get_job(*id,thread_array,&terminate);
		
		if (terminate ==1){ 						//Terminate flag
			break;
		}
		thread_array[*id].thread_job->is_prime = prime_test(thread_array[*id].thread_job->number); //writing result
		
		job_finished(*id,thread_array);
	}
	
	return;
}

int main(int argc,char* argv[]) {
	int i;
	
	if (argc<3) {
		printf("Please enter as first argument the number of threads, and at least one number to check if it is prime\n");
		return 0;
	}	
	

	number_of_threads = atoi(argv[1]);	//Number of threads given as arguemnt.
	clock_t begin = clock();
	
	thread_array = (thread_info*)calloc(number_of_threads, sizeof(thread_info));//Dimiourgia pinaka thread
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
	
	for(i=0; i<jobs_left; i++){					//job array init
		job_array[i].number = atoi(argv[i+2]);
		job_array[i].is_prime = -1; 			//no calculations have been made
	}

	for (i=0; i<number_of_threads; i++) {		//thread array init
		
		thread_array[i].available = 1;
		thread_array[i].thread_index = i;

	}
	
	monitor_init(number_of_threads);
	
	for (i=0; i<number_of_threads; i++) {			//creating threads
		if(pthread_create(&thread_array[i].thread_id,NULL,(void*)worker_thread,(void*)&thread_array[i].thread_index))
			printf("Error creating thread!!\n");
	}
	
	
	distribute_jobs(thread_array, job_array,jobs_left);
	
	wait_on_threads(thread_array);
	terminate = 1;
	broadcast_workers(thread_array);
	
	
// 	job_array[argc-3].is_prime = prime_test(job_array[argc-3].number);
 	for (i=0; i<number_of_threads; i++)
 		pthread_join(thread_array[i].thread_id, NULL);

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
// // 	
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time was: %lf\n",time_spent);
	return(0);
}



