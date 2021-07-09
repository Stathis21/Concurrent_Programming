#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>


int available_threads, no_of_threads,main_done;
pthread_cond_t zero_available_threads,waiting_on_threads;
pthread_cond_t* waiting_for_job;
pthread_mutex_t mutex;
int* has_job;

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






void monitor_init(int number_of_threads);

void distribute_jobs(thread_info* thread_array, job* job_array,int jobs);


void get_job(int i,thread_info* thread_array,int* terminate);

void job_finished(int i,thread_info* thread_array);


void wait_on_threads(thread_info* thread_array);

void broadcast_workers(thread_info* thread_array);



























