#include "SortedList.h"
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

char lockVal = '\0';
pthread_mutex_t* plock = NULL;
char* syncLock = NULL;
char syncLock2 = 0;
SortedListElement_t** takenElements = NULL;
unsigned long long numThreads = 1;
unsigned long long lockTime = 0;
unsigned long long numLocks = 0;
unsigned long long numLists = 1;
struct thread_input* input = NULL;
pthread_t* pthreads = NULL;
SortedListElement_t* elements = NULL;

void handler(int sigNum){
	if(sigNum != SIGSEGV){
		char buf[] = "Signal caught\n";
		write(2, buf, strlen(buf));
		_exit(2);
	} else {
		char buf[] = "Segmentation fault\n";
		write(2, buf, strlen(buf));
		_exit(2);
	}
}

struct thread_input{
	SortedList_t* list;
	SortedListElement_t* elements;
	int iterations;
	int thread_num;
};

void* thread_method(void* pointer){
	struct thread_input input = *(struct thread_input*)(pointer);
	SortedList_t* list = input.list;
	SortedListElement_t* elements = input.elements;
	int iterations = input.iterations;
	int thread_num = input.thread_num;
	int i = 0;
	for(i = 0; i < iterations; i++){
		if(lockVal == 'm'){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			pthread_mutex_lock(&plock[elements[i].key[0]%numLists]);
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			SortedList_insert(list+elements[i].key[0]%numLists, &elements[i]);
			pthread_mutex_unlock(&plock[elements[i].key[0]%numLists]);
		}
		if(lockVal == 's'){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			while(__sync_lock_test_and_set(&syncLock[elements[i].key[0]%numLists], 1)){continue;}
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			SortedList_insert(list+elements[i].key[0]%numLists, &elements[i]);
			__sync_lock_release(&syncLock[elements[i].key[0]%numLists]);
		}
		if(lockVal == '\0'){
			SortedList_insert(list+elements[i].key[0]%numLists, &elements[i]);
		}
	}
	int length;
	if(lockVal == 'm'){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			pthread_mutex_lock(&plock[i]);
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			length = SortedList_length(&list[i]);
			if(length == -1){
				char buf[] = "Corrupted list1\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			pthread_mutex_unlock(&plock[i]);
		}
	}
	if(lockVal == 's'){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			while(__sync_lock_test_and_set(&syncLock[i], 1)){continue;}
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			length = SortedList_length(&list[i]);
			if(length == -1){
				char buf[] = "Corrupted list1\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			__sync_lock_release(&syncLock[i]);
		}
	}
	if(lockVal == '\0'){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			length = SortedList_length(&list[i]);
			if(length == -1){
				char buf[] = "Corrupted list1\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
		}
	}
	for(i = 0; i < iterations; i++){
		SortedListElement_t* lookup;
		while(i != -1){
			if(lockVal == 'm'){
				struct timespec init_time;
				int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
				if(output == -1){
					char buf[] = "Clock error\n";
					write(2, buf, strlen(buf));
					_exit(2);
				}
				pthread_mutex_lock(&plock[elements[i].key[0]%numLists]);
				struct timespec fin_time;
				output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
				if(output == -1){
					char buf[] = "Clock error\n";
					write(2, buf, strlen(buf));
					_exit(2);
				}
				numLocks++;
				lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			}
			if(lockVal == 's'){
				struct timespec init_time;
				int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
				if(output == -1){
					char buf[] = "Clock error\n";
					write(2, buf, strlen(buf));
					_exit(2);
				}
				while(__sync_lock_test_and_set(&syncLock[elements[i].key[0]%numLists], 1)){continue;}
				struct timespec fin_time;
				output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
				if(output == -1){
					char buf[] = "Clock error\n";
					write(2, buf, strlen(buf));
					_exit(2);
				}
				numLocks++;
				lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			}
			lookup = SortedList_lookup(list+elements[i].key[0]%numLists, elements[i].key);
			if(lookup == NULL){
				char buf[] = "Corrupted list3\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			unsigned long long j = 0;
			for(j = 0; j < numThreads; j++){
				if(takenElements[j] == lookup){
					break;
				}
			}
			if(j == numThreads){
				takenElements[thread_num] = lookup;
				if(lockVal == 'm'){
					pthread_mutex_unlock(&plock[elements[i].key[0]%numLists]);
				}
				if(lockVal == 's'){
					__sync_lock_release(&syncLock[elements[i].key[0]%numLists]);
				}
				break;
			}
			if(lockVal == 'm'){
				pthread_mutex_unlock(&plock[elements[i].key[0]%numLists]);
			}
			if(lockVal == 's'){
				__sync_lock_release(&syncLock[elements[i].key[0]%numLists]);
			}
		}
		int status;
		if(lockVal == 'm'){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			pthread_mutex_lock(&plock[lookup->key[0]%numLists]);
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			status = SortedList_delete(lookup);
			pthread_mutex_unlock(&plock[lookup->key[0]%numLists]);
		}
		if(lockVal == 's'){
			struct timespec init_time;
			int output = clock_gettime(CLOCK_MONOTONIC, &init_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			while(__sync_lock_test_and_set(&syncLock[lookup->key[0]%numLists], 1)){continue;}
			struct timespec fin_time;
			output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
			if(output == -1){
				char buf[] = "Clock error\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			numLocks++;
			lockTime += fin_time.tv_sec*1000000000 + fin_time.tv_nsec - init_time.tv_sec*1000000000 - init_time.tv_nsec;
			status = SortedList_delete(lookup);
			__sync_lock_release(&syncLock[lookup->key[0]%numLists]);
		}
		if(lockVal == '\0'){
			status = SortedList_delete(lookup);
		}
		if(status == 1){
			char buf[] = "Corrupted list2\n";
			write(2, buf, strlen(buf));
			_exit(2);
		}
	}
	return NULL;
}

int main(int argc, char* const argv[]){
	opterr = 0;
	struct option options[6];
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
	options[2].has_arg = 1;
	int flag2[1] = {0};
	options[2].flag = flag2;
	options[2].val = 1;

	options[3].name = "sync";
	options[3].has_arg = 1;
	int flag3[1] = {0};
	options[3].flag = flag3;
	options[3].val = 1;

	options[4].name = "lists";
	options[4].has_arg = 1;
	int flag4[1] = {0};
	options[4].flag = flag4;
	options[4].val = 0;

	options[5].name = 0;
	options[5].has_arg = 0;
	options[5].flag = 0;
	options[5].val = 0;

	int longInd = 0;
	unsigned long long iterations = 1;
	char* yieldopts = NULL;
	char* syncopts = NULL;
	int output = getopt_long(argc, argv, "-", options, &longInd);
	while(output != -1){
		if(output == '?'){
			fprintf(stderr,	"Invalid Usage\n");
			return 1;
		}
		if(longInd == 0){
			int len = strlen(optarg);
                        int i = 0;
                        for(i = 0; i < len; i++){
                                if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
                                        return 1;
                                }
                        }
                        numThreads = atoi(optarg);
		} else if(longInd == 1){
			int len = strlen(optarg);
                        int i = 0;
                        for(i = 0; i < len; i++){
                                if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
                                        return 1;
                                }
                        }
                        iterations = atoi(optarg);
		} else if(longInd == 2){
			int len = strlen(optarg);
			int i = 0;
			for(i = 0; i < len; i++){
				switch(optarg[i]){
					case 'i':
						opt_yield = opt_yield | 1;
						break;
					case 'd':
						opt_yield = opt_yield | 2;
						break;
					case 'l':
						opt_yield = opt_yield | 4;
						break;
					default:
						fprintf(stderr,	"Invalid Usage\n");
						return 1;
				}
			}
			yieldopts = optarg;
		} else if(longInd == 3){
			int len = strlen(optarg);
			if(len != 1){
				fprintf(stderr,	"Invalid Usage\n");
				return 1;
			}
			switch(optarg[0]){
				case 'm':
					lockVal = 'm';
					break;
				case 's':
					lockVal = 's';
					break;
				default:
					fprintf(stderr,	"Invalid Usage\n");
					return 1;
			}
			syncopts = optarg;
		} else if(longInd == 4){
			int len = strlen(optarg);
                        int i = 0;
                        for(i = 0; i < len; i++){
                                if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
                                        return 1;
                                }
                        }
			numLists = atoi(optarg);
		}
		output = getopt_long(argc, argv, "-", options, &longInd);
	}
	unsigned long long elementNum = numThreads*iterations;
	elements = malloc(elementNum*sizeof(SortedListElement_t));
	if(elements == NULL){
		fprintf(stderr, "Malloc error\n");
		return 1;
	}
	srand(time(0));
	unsigned long long i = 0;
	for(i = 0; i < elementNum; i++){
		char* temp = malloc(21*sizeof(char));
		temp[0] = rand()%128;
		temp[1] = rand()%128;
		temp[2] = rand()%128;
		temp[3] = rand()%128;
		temp[4] = rand()%128;
		temp[5] = rand()%128;
		temp[6] = rand()%128;
		temp[7] = rand()%128;
		temp[8] = rand()%128;
		temp[9] = rand()%128;
		temp[10] = rand()%128;
		temp[11] = rand()%128;
		temp[12] = rand()%128;
		temp[13] = rand()%128;
		temp[14] = rand()%128;
		temp[15] = rand()%128;
		temp[16] = rand()%128;
		temp[17] = rand()%128;
		temp[18] = rand()%128;
		temp[19] = rand()%128;
		temp[20] = '\0';
		elements[i].key = temp;
	}
	SortedList_t* list = malloc(numLists*sizeof(SortedList_t));
	if(list == NULL){
		fprintf(stderr, "Malloc error\n");
		goto freeAll;
	}
	i = 0;
	for(i = 0; i < numLists; i++){
		list[i].prev = list+i;
		list[i].next = list+i;
		list[i].key = NULL;
	}
	if(lockVal == 'm'){
		plock = malloc(numLists*sizeof(pthread_mutex_t));
		if(plock == NULL){
			fprintf(stderr, "Malloc error\n");
			goto freeAll;
		}
		i = 0;
		for(i = 0; i < numLists; i++){
			int output = pthread_mutex_init(&plock[i], NULL);
			if(output != 0){
				fprintf(stderr, "Mutex error\n");
				goto freeAll;
			}
		}
	}
	if(lockVal == 's'){
		syncLock = malloc(numLists*sizeof(char));
		if(syncLock == NULL){
			fprintf(stderr, "Malloc error\n");
			goto freeAll;
		}
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			syncLock[i] = 0;
		}
	}
	__sighandler_t sigoutput = signal(SIGSEGV, &handler);
	if(sigoutput == SIG_ERR){
		fprintf(stderr, "Signal error\n");
		goto freeAll;
	}
	pthreads = malloc(numThreads*sizeof(pthread_t));
	if(pthreads == NULL){
		fprintf(stderr, "Malloc error\n");
		goto freeAll;
	}
	input = malloc(numThreads*sizeof(struct thread_input));
	if(input == NULL){
		fprintf(stderr, "Malloc error\n");
		goto freeAll;
	}
	int offset = 0;
	for(i = 0; i < numThreads; i++){
		input[i].list = list;
		input[i].elements = elements+offset;
		input[i].iterations = iterations;
		input[i].thread_num = i;
		offset += iterations;
	}
	takenElements = malloc(numThreads*sizeof(SortedListElement_t*));
	if(takenElements == NULL){
		fprintf(stderr, "Malloc error\n");
		goto freeAll;
	}
	for(i = 0; i < numThreads; i++){
		takenElements[i] = NULL;
	}
	struct timespec init_time;
        output = clock_gettime(CLOCK_MONOTONIC, &init_time);
        if(output == -1){
		fprintf(stderr, "Clock error\n");
		goto freeAll;
        }
	for(i = 0; i < numThreads; i++){
		int output = pthread_create(&pthreads[i], NULL, &thread_method, &input[i]);
		if(output != 0){
			fprintf(stderr, "Pthread error\n");
			goto freeAll;
		}
	}
	for(i = 0; i < numThreads; i++){
		int output = pthread_join(pthreads[i], NULL);
		if(output != 0){
			fprintf(stderr, "Pthread error\n");
			goto freeAll;
		}
	}
	struct timespec fin_time;
	output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
	if(output == -1){
		fprintf(stderr, "Clock error\n");
		goto freeAll;
	}
	unsigned long long nanoseconds = fin_time.tv_nsec + fin_time.tv_sec*1000000000 - init_time.tv_nsec - init_time.tv_sec*1000000000;
	if(SortedList_length(list) != 0){
		fprintf(stderr, "Corrupted list4\n");
		goto freeAll2;
	}
	fprintf(stdout, "list");
	if(yieldopts != NULL){
		fprintf(stdout, "-%s", yieldopts);
	} else {
		fprintf(stdout, "-none");
	}
	if(syncopts != NULL){
		fprintf(stdout, "-%s", syncopts);
	} else {
		fprintf(stdout, "-none");
	}
	fprintf(stdout, ",%llu", numThreads);
	fprintf(stdout, ",%llu", iterations);
	fprintf(stdout, ",%llu", numLists);
	fprintf(stdout, ",%llu", numThreads*iterations*3);
	fprintf(stdout, ",%llu", nanoseconds);
	fprintf(stdout, ",%llu", nanoseconds/numThreads/iterations/3);
	if(numLocks != 0){
		fprintf(stdout, ",%llu", lockTime/numLocks);
	} else {
		fprintf(stdout, ",0");
	}
	fprintf(stdout, "\n");
	free(list);
	free(takenElements);
	free(input);
	free(pthreads);
	i = 0;
	for(i = 0; i < numThreads*iterations; i++){
		free((char*)elements[i].key);
	}
	free(elements);
	if(plock != NULL){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			pthread_mutex_destroy(&plock[i]);
		}
	}
	if(lockVal == 's'){
		free(syncLock);
	}
	free(plock);
	return 0;
freeAll:
	free(list);
	free(takenElements);
	free(input);
	free(pthreads);
	i = 0;
	for(i = 0; i < numThreads*iterations; i++){
		free((char*)elements[i].key);
	}
	free(elements);
	if(plock != NULL){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			pthread_mutex_destroy(&plock[i]);
		}
	}
	free(plock);
	if(lockVal == 's'){
		free(syncLock);
	}
	return 1;
freeAll2:
	free(list);
	free(takenElements);
	free(input);
	free(pthreads);
	i = 0;
	for(i = 0; i < numThreads*iterations; i++){
		free((char*)elements[i].key);
	}
	free(elements);
	if(plock != NULL){
		unsigned long long i = 0;
		for(i = 0; i < numLists; i++){
			pthread_mutex_destroy(&plock[i]);
		}
	}
	free(plock);
	if(lockVal == 's'){
		free(syncLock);
	}
	return 1;	
}
