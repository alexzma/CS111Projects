#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>

long long count = 0;
int opt_yield = 0;
char syncVal = 0;
pthread_mutex_t mutexLock;
char syncLock = 0;

void add(long long *pointer, unsigned long long value) {
	if(syncVal == 'c'){
		long long val;
		long long sum;
		do{
			val = *pointer;
			sum = val+value;
			if(opt_yield)
				sched_yield();
		} while(__sync_val_compare_and_swap(pointer, val, sum) != val);
		return;
	}
	else if(syncVal == 'm'){
		pthread_mutex_lock(&mutexLock);
	} else if(syncVal == 's'){
		while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
	}
	unsigned long long sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
	if(syncVal == 'm'){
		pthread_mutex_unlock(&mutexLock);
	} else if(syncVal == 's'){
		__sync_lock_release(&syncLock);
	}
}

void addMultiple(void* pointer) {
	unsigned long long i = 0;
	unsigned long long iterations = *((unsigned long long*)pointer);
	for(i = 0; i < iterations; i++){
		add(&count, 1);
		add(&count, -1);
	}
}

int main(int argc, char* const argv[]){
	opterr = 0;
	struct option options[5];
	
	options[0].name = "threads";
	options[0].has_arg = 1;
	int flag0[1] = {0};
	options[0].flag = flag0;
	options[0].val = 1;

	options[1].name = "iterations";
	options[1].has_arg = 1;
	int flag1[1] = {0};
	options[1].flag = flag1;
	options[1].val = 1;

	options[2].name = "yield";
	options[2].has_arg = 0;
	int flag2[1] = {0};
	options[2].flag = flag2;
	options[2].val = 1;

	options[3].name = "sync";
	options[3].has_arg = 1;
	int flag3[1] = {0};
	options[3].flag = flag3;
	options[3].val = 1;

	options[4].name = 0;
	options[4].has_arg = 0;
	options[4].flag = 0;
	options[4].val = 0;

	int longInd = 0;
	unsigned long long numThreads = 1;
	unsigned long long iterations = 1;
	char name[15];
	name[0] = 'a';
	name[1] = 'd';
	name[2] = 'd';
	name[3] = '-';
	name[4] = 'n';
	name[5] = 'o';
	name[6] = 'n';
	name[7] = 'e';
	name[8] = '\0';
	int output = getopt_long(argc, argv, "-", options, &longInd);
	while(output != -1){
		if(output == '?'){
			fprintf(stderr, "Invalid Usage\n");
			return 1;
		}
		if(longInd == 0){//threads
			int len = strlen(optarg);
			int i = 0;
			for(i = 0; i < len; i++){
				if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
					return 1;
				}
			}
			numThreads = atoi(optarg);
		} else if(longInd == 1){//iterations
			int len = strlen(optarg);
			int i = 0;
			for(i = 0; i < len; i++){
				if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
					return 1;
				}
			}
			iterations = atoi(optarg);
		} else if(longInd == 2){//yield
			opt_yield = 1;
		} else if (longInd == 3){//sync
			if(strlen(optarg) != 1){
				fprintf(stderr,	"Invalid Usage\n");
				return 1;
			}
			if(optarg[0] != 'm' && optarg[0] != 's' && optarg[0] != 'c'){
				fprintf(stderr,	"Invalid Usage\n");
				return 1;
			}
			syncVal= optarg[0];
		} else {
			fprintf(stderr,	"Invalid Usage\n");
			return 1;
		}
		output = getopt_long(argc, argv, "-", options, &longInd);
	}
	if(syncVal== 'm'){
		pthread_mutex_init(&mutexLock, NULL);
	}
	pthread_t* threads = malloc(numThreads*sizeof(pthread_t));
	if(threads == NULL){
		fprintf(stderr, "Malloc error\n");
		return 1;
	}
	struct timespec init_time;
	output = clock_gettime(CLOCK_MONOTONIC, &init_time);
	if(output == -1){
		fprintf(stderr, "Malloc error\n");
		free(threads);
		return 1;
	}
	unsigned long long i = 0;
	for(i = 0; i < numThreads; i++){
		pthread_create(&threads[i], NULL, (void*)&addMultiple, &iterations);
	}
	for(i = 0; i < numThreads; i++){
		output = pthread_join(threads[i], (void*)NULL);
		if(output != 0){
			fprintf(stderr, "Malloc error\n");
			free(threads);
			return 1;
		}
	}
	struct timespec fin_time;
	output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
	if(output == -1){
		fprintf(stderr, "Malloc error\n");
		free(threads);
		return 1;
	}
	if(options[2].flag[0] == 1){
		name[4] = 'y';
		name[5] = 'i';
		name[6] = 'e';
		name[7] = 'l';
		name[8] = 'd';
		name[9] = '-';
		switch(syncVal){
			case 'm':
				name[10] = 'm';
				name[11] = '\0';
				break;
			case 's':
				name[10] = 's';
				name[11] = '\0';
				break;
			case 'c':
				name[10] = 'c';
				name[11] = '\0';
				break;
			default:
				name[10] = 'n';
				name[11] = 'o';
				name[12] = 'n';
				name[13] = 'e';
				name[14] = '\0';
		}
	} else {
		switch(syncVal){
			case 'm':
				name[4] = 'm';
				name[5] = '\0';
				break;
			case 's':
				name[4] = 's';
				name[5] = '\0';
				break;
			case 'c':
				name[4] = 'c';
				name[5] = '\0';
				break;
		}
	}
	unsigned long long nanoseconds = (fin_time.tv_nsec+fin_time.tv_sec*1000000000 - init_time.tv_nsec - init_time.tv_sec*1000000000);
	fprintf(stdout, "%s", name);
	fprintf(stdout, ",%llu", numThreads);
	fprintf(stdout, ",%llu", iterations);
	fprintf(stdout, ",%llu", numThreads*iterations*2);
	fprintf(stdout, ",%llu", nanoseconds);
	fprintf(stdout, ",%llu", nanoseconds/numThreads/iterations/2);
	fprintf(stdout, ",%lld", count);
	fprintf(stdout, "\n");
	if(syncVal== 'm'){
		output = pthread_mutex_destroy(&mutexLock);
		if(output != 0){
			free(threads);
			return 2;
		}
	}
	free(threads);
	return 0;
}
