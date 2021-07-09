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
#define N 5

int inqueue,terminate;
pthread_mutex_t mutex;

pthread_cond_t queue;
pthread_cond_t ridin;
pthread_cond_t train;

void monitor_init();

void get_in_queue();

void idle_train();

void mad_max();

void monitor_kill();
