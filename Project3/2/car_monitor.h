
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

int cars;
pthread_mutex_t mutex;
int current_dir,passed;
int waiting_cars[2];
pthread_cond_t waitq[2];

void monitor_init();

void enter_bridge(int dir);

void exit_bridge(int dir);
