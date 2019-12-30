//NAME: Alexander Ma
//EMAIL: alexanderzma@hotmail.com
//ID: 105093055

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>

struct command {
	int input;
	int output;
	int error;
	int pid;
	char* file;
	char** args;//first arg is file, last arg is NULL
};

void freeAll(int* files, struct command commandArgs, struct command* commands, int numCommands){
	free(files);
	int i = 0;
	for(i = 0; i < numCommands; i++){
		free(commands[numCommands].args);
	}
	free(commands);
	free(commandArgs.args);
}

int closeAll(int* files, int numFiles){
	int failed = 0;
	int temp = 0;
	int temp2 = 0;
	int i = 0;
	failed = close(0);
	if(failed == -1){
		temp2 = errno;
	}
	temp = close(1);
	if(temp == -1){
		temp2 = errno;
		failed = temp;
	}
	temp = close(2);
	if(temp == -1){
		temp2 = errno;
		failed = temp;
	}
	for(i = 0; i < numFiles; i++){
		if(files[i] == 1){
			temp = close(i+3);
			if(temp == -1){
				temp2 = errno;
				failed = temp;
			}
		}
	}
	errno = temp2;
	return failed;
}

void zeroOpenFlags(struct option* options){
	options[5].flag[0] = 0;
	options[6].flag[0] = 0;
	options[7].flag[0] = 0;
	options[8].flag[0] = 0;
	options[9].flag[0] = 0;
	options[10].flag[0] = 0;
	options[11].flag[0] = 0;
	options[12].flag[0] = 0;
	options[13].flag[0] = 0;
	options[14].flag[0] = 0;
	options[15].flag[0] = 0;
}

int flagsOred(struct option* options){
	return (options[5].flag[0] | options[6].flag[0] | options[7].flag[0] | options[8].flag[0] | options[9].flag[0] | options[10].flag[0] | options[11].flag[0] | options[12].flag[0] | options[13].flag[0] | options[14].flag[0] | options[15].flag[0]);
}

int closeNotStd(int* files, int numFiles){
	int i = 0;
	int failed = 0;
	int temp = 0;
	int temp2 = 0;
	for(i = 0; i < numFiles; i++){
		if(files[i] == 0){
			continue;
		}
		temp = close(files[i]);
		if(temp == -1){
			failed = -1;
			temp2 = errno;
		}
	}
	errno = temp2;
	return failed;
}

int closeStd(){
	int failed = 0;
	int temp = 0;
	int temp2 = 0;
	failed = close(0);
	if(failed == -1){
		temp2 = errno;
	}
	temp = close(1);
	if(temp == -1){
		failed = temp;
		temp2 = errno;
	}
	temp = close(2);
	if(temp == -1){
		failed = temp;
		temp2 = errno;
	}
	errno = temp2;
	return failed;
}

void catcher(int signal){
	char arr[10];
	sprintf(arr, "%d caught\n", signal);
	int written = 0;
	while(written < (int)strlen(arr)){
		written += write(2, arr+written, strlen(arr));
	}
	exit(signal);
}

int main(int argc, char* const argv[]){
	opterr = 0;

	struct option options[27];

	options[0].name = "rdonly";
	options[0].has_arg = 1;
	int flag0[1] = {0};
	options[0].flag = flag0;
	options[0].val = 1;

	options[1].name = "wronly";
	options[1].has_arg = 1;
	int flag1[1] = {0};
	options[1].flag = flag1;
	options[1].val = 1;

	options[2].name = "command";
	options[2].has_arg = 0;
	int flag2[1] = {0};
	options[2].flag = flag2;
	options[2].val = 1;

	options[3].name = "verbose";
	options[3].has_arg = 0;
	int flag3[1] = {0};
	options[3].flag = flag3;
	options[3].val = 1;

	options[4].name = "wait";
	options[4].has_arg = 0;
	int flag4[1] = {0};
	options[4].flag = flag4;
	options[4].val = 1;

	options[5].name = "append";
	options[5].has_arg = 0;
	int flag5[1] = {0};
	options[5].flag = flag5;
	options[5].val = O_APPEND;

	options[6].name = "cloexec";
	options[6].has_arg = 0;
	int flag6[1] = {0};
	options[6].flag = flag6;
	options[6].val = O_CLOEXEC;

	options[7].name = "creat";
	options[7].has_arg = 0;
	int flag7[1] = {0};
	options[7].flag = flag7;
	options[7].val = O_CREAT;

	options[8].name = "directory";
	options[8].has_arg = 0;
	int flag8[1] = {0};
	options[8].flag = flag8;
	options[8].val = O_DIRECTORY;

	options[9].name = "dsync";
	options[9].has_arg = 0;
	int flag9[1] = {0};
	options[9].flag = flag9;
	options[9].val = O_DSYNC;

	options[10].name = "excl";
	options[10].has_arg = 0;
	int flag10[1] = {0};
	options[10].flag = flag10;
	options[10].val = O_EXCL;

	options[11].name = "nofollow";
	options[11].has_arg = 0;
	int flag11[1] = {0};
	options[11].flag = flag11;
	options[11].val = O_NOFOLLOW;

	options[12].name = "nonblock";
	options[12].has_arg = 0;
	int flag12[1] = {0};
	options[12].flag = flag12;
	options[12].val = O_NONBLOCK;

	options[13].name = "rsync";
	options[13].has_arg = 0;
	int flag13[1] = {0};
	options[13].flag = flag13;
	options[13].val = O_RSYNC;

	options[14].name = "sync";
	options[14].has_arg = 0;
	int flag14[1] = {0};
	options[14].flag = flag14;
	options[14].val = O_SYNC;

	options[15].name = "trunc";
	options[15].has_arg = 0;
	int flag15[1] = {0};
	options[15].flag = flag15;
	options[15].val = O_TRUNC;

	options[16].name = "rdwr";
	options[16].has_arg = 1;
	int flag16[1] = {0};
	options[16].flag = flag16;
	options[16].val = 1;

	options[17].name = "pipe";
	options[17].has_arg = 0;
	int flag17[1] = {0};
	options[17].flag = flag17;
	options[17].val = 1;

	options[18].name = "chdir";
	options[18].has_arg = 1;
	int flag18[1] = {0};
	options[18].flag = flag18;
	options[18].val = 1;

	options[19].name = "close";
	options[19].has_arg = 1;
	int flag19[1] = {0};
	options[19].flag = flag19;
	options[19].val = 1;

	options[20].name = "abort";
	options[20].has_arg = 0;
	int flag20[1] = {0};
	options[20].flag = flag20;
	options[20].val = 1;

	options[21].name = "catch";
	options[21].has_arg = 1;
	int flag21[1] = {0};
	options[21].flag = flag21;
	options[21].val = 1;

	options[22].name = "ignore";
	options[22].has_arg = 1;
	int flag22[1] = {0};
	options[22].flag = flag22;
	options[22].val = 1;

	options[23].name = "default";
	options[23].has_arg = 1;
	int flag23[1] = {0};
	options[23].flag = flag23;
	options[23].val = 1;

	options[24].name = "pause";
	options[24].has_arg = 0;
	int flag24[1] = {0};
	options[24].flag = flag24;
	options[24].val = 1;

	options[25].name = "profile";
	options[25].has_arg = 0;
	int flag25[1] = {0};
	options[25].flag = flag25;
	options[25].val = 1;
	
	options[26].name = 0;
	options[26].has_arg = 0;
	options[26].flag = 0;
	options[26].val = 0;
	
	int longInd[1] = {-1};
	int verbose = 0;
	int profile = 0;
	int cmdArrCounter = 0;
	int failed = 0;
	int numFiles = 0;
	int numFailedFiles = 0;
	int invalidFd = 0;
	int signalFailed = 0;
	struct rusage prior;
	int profileFlag = 0;
	struct command commandArgs;
	commandArgs.args = NULL;
	struct command* commands = malloc(argc*sizeof(struct command));
	if(commands == NULL){
		fprintf(stderr, "Memory failure\n");
		freeAll(NULL, commandArgs, commands, 0);
		exit(1);
	}
	int numCommands = 0;
	int* files = (int*)malloc(argc*sizeof(int));
	if(files == NULL){
		fprintf(stderr, "Memory failure\n");
		freeAll(files, commandArgs, commands, numCommands);
		exit(1);
	}
	int numForks = 0;

	int output = getopt_long(argc, argv, "-:", options, longInd);

	while(output != -1){
		if(output == '?' && options[2].flag[0] != 1){
			if(optopt != '\0'){
                                fprintf(stderr, "-%c: Invalid option\n", optopt);
				optopt = '\0';
                        } else {
                                fprintf(stderr, "%s: Invalid option\n", argv[optind-1]);
                        }
			failed = 1;
		} else if(output == ':'){
			fprintf(stderr, "%s: Requires argument\n", argv[optind-1]);
			failed = 1;
		} else if(*longInd == -1 && options[2].flag[0] != 1){
			fprintf(stderr, "%s: Invalid option\n", argv[optind-1]);
			failed = 1;
		} else if(*longInd == -1){
			//add element to execvp array (char array)
			if(cmdArrCounter >= 3){
				//realloc
				commandArgs.args = (char**)realloc(commandArgs.args, sizeof(char*)*(cmdArrCounter-1));
				if(commandArgs.args == NULL){
					fprintf(stderr, "Fatal memory failure\n");
					if(failed == 0){
						failed = 1;
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				commandArgs.args[cmdArrCounter-3] = argv[optind-1];
				if(cmdArrCounter == 3){
					commandArgs.file = argv[optind-1];
				}
				if(optopt != '\0'){
					if(optind < argc && strlen(argv[optind]) > 1 && argv[optind][0] == '-' && argv[optind][1] != '-' && argv[optind][strlen(argv[optind])-1] != optopt){
						cmdArrCounter--;
					}
					optopt = '\0';
				}
			} else if(cmdArrCounter == 2){
				int len = strlen(argv[optind-1]);
				int i = 0;
				while(i < len){
					if(argv[optind-1][i] < '0' || argv[optind-1][i] > '9'){
						fprintf(stderr, "Invalid command file number: %s\n", argv[optind-1]);
						invalidFd = 1;
						break;
					}
					i++;
				}
				commandArgs.error = atoi(argv[optind-1]);
			} else if(cmdArrCounter == 1){
				int len = strlen(argv[optind-1]);
				int i = 0;
				while(i < len){
					if(argv[optind-1][i] < '0' || argv[optind-1][i] > '9'){
						fprintf(stderr, "Invalid command file number: %s\n", argv[optind-1]);
						invalidFd = 1;
						break;
					}
					i++;
				}
				commandArgs.output = atoi(argv[optind-1]);
			} else {
				int len = strlen(argv[optind-1]);
				int i = 0;
				while(i < len){
					if(argv[optind-1][i] < '0' || argv[optind-1][i] > '9'){
						fprintf(stderr, "Invalid command file number: %s\n", argv[optind-1]);
						invalidFd = 1;
						break;
					}
					i++;
				}
				commandArgs.input = atoi(argv[optind-1]);
			}
			cmdArrCounter++;
		} else if(options[2].flag[0] == 1 && commandArgs.args == NULL){
			//start new execvp array
			profileFlag = 0;
			if(profile == 1){
				int output = getrusage(RUSAGE_SELF, &prior);
				if(output == -1){
					if(failed == 0){
						failed = 1;
					}
					profileFlag = 1;
					fprintf(stderr, "getrusage: %s\n", strerror(errno));
				}
			}
			commandArgs.args = malloc(sizeof(char*));
			if(commandArgs.args == NULL){
				fprintf(stderr, "Fatal memory failure\n");
				if(failed == 0){
					failed = 1;
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
				}
				return failed;
			}
		} else {
			if(options[2].flag[0] == 1){
				if(cmdArrCounter < 4){
					fprintf(stderr, "--command: command does not have enough arguments\n");
					free(commandArgs.args);
					commandArgs.args = NULL;
					cmdArrCounter = 0;
					options[2].flag[0] = 0;
					if(failed == 0){
						failed = 1;
					}
				} else {
					if(invalidFd == 1){
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						if(failed == 0){
							failed = 1;
						}
						goto longIndParse;
					}
					int x = 0;
					commandArgs.args[cmdArrCounter-3] = (char*)NULL;
					if(verbose == 1){
						fprintf(stdout, "--command");
						int i = 0;
						fprintf(stdout, " %d", commandArgs.input);
						fprintf(stdout, " %d", commandArgs.output);
						fprintf(stdout, " %d", commandArgs.error);
	                               	        while(commandArgs.args[i] != NULL){
        	                              	        fprintf(stdout, " %s", commandArgs.args[i]);
                	       	                        i++;
               	        	                }
       	                        	        fprintf(stdout, "\n");
                                        	fflush(stdout);
					}
					if(commandArgs.input >= 0 && commandArgs.input < numFiles && files[commandArgs.input] > 0){
						//DO NOTHING THIS IS GOOD
					} else {
						fprintf(stderr, "File descriptor %d is not open\n", commandArgs.input);
						if(failed == 0){
							failed = 1;
						}
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						goto longIndParse;
					}
					if(commandArgs.output >= 0 && commandArgs.output < numFiles && files[commandArgs.output] > 0){
						//DO NOTHING THIS IS GOOD
					} else {
						fprintf(stderr, "File descriptor %d is not open\n", commandArgs.output);
						if(failed == 0){
							failed = 1;
						}
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						goto longIndParse;
					}
					if(commandArgs.error >= 0 && commandArgs.error < numFiles && files[commandArgs.error] > 0){
						//DO NOTHING THIS IS GOOD
					} else {
						fprintf(stderr, "File descriptor %d is not open\n", commandArgs.error);
						if(failed == 0){
							failed = 1;
						}
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						goto longIndParse;
					}
					x = fork();
					if(x == -1){
						fprintf(stderr, "Fork failure: %s. Going to next command.\n", strerror(errno));
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						goto longIndParse;
					} else if(x == 0){
						x = dup2(files[commandArgs.input], 0);
						if(x == -1){
							fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							failed = 1;
							closeAll(files, numFiles);
							freeAll(files, commandArgs, commands, numCommands);
							if(signalFailed != 0){
								signal(signalFailed, SIG_DFL);
								raise(signalFailed);
							}
							return failed;
						}
						x = dup2(files[commandArgs.output], 1);
						if(x == -1){
						fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							output = closeAll(files, numFiles);
							failed = 1;
							closeAll(files, numFiles);
							freeAll(files, commandArgs, commands, numCommands);
							if(signalFailed != 0){
								signal(signalFailed, SIG_DFL);
								raise(signalFailed);
							}
							return failed;
						}
						x = dup2(files[commandArgs.error], 2);
						if(x == -1){
							fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							failed = 1;
							closeAll(files, numFiles);
							freeAll(files, commandArgs, commands, numCommands);
							if(signalFailed != 0){
								signal(signalFailed, SIG_DFL);
								raise(signalFailed);
							}
							return failed;
						}
						output = closeNotStd(files, numFiles);
						if(output == -1){
							fprintf(stderr, "close: %s\n", strerror(errno));
							int output = closeAll(files, numFiles);
							if(output == -1){
								fprintf(stderr, "close: %s\n", strerror(errno));
							}
							freeAll(files, commandArgs, commands, numCommands);
							if(signalFailed != 0){
								signal(signalFailed, SIG_DFL);
								raise(signalFailed);
							}
							return 1;
						}
						output = execvp(commandArgs.file, commandArgs.args);
						fprintf(stderr, "execvp: %s\n", strerror(errno));
						failed = 1;
						output = closeStd();
						if(output == -1){
							fprintf(stderr, "close: %s\n", strerror(errno));
						}
						freeAll(files, commandArgs, commands, numCommands);
						if(signalFailed != 0){
							signal(signalFailed, SIG_DFL);
							raise(signalFailed);
						}
						return failed;
					} else {
						numForks++;
						commands[numCommands] = commandArgs;
						commands[numCommands].pid = x;
						numCommands++;
					}
				}
				commandArgs.args = NULL;
				cmdArrCounter = 0;
				options[2].flag[0] = 0;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
			}
			longIndParse:
			if(*longInd == 0){
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--rdonly %s\n", optarg);
					fflush(stdout);
				}
				int x = open(optarg, O_RDONLY | flagsOred(options), 0777);
				zeroOpenFlags(options);
				if(x == -1){
					fprintf(stderr, "--rdonly %s: %s\n", optarg, strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles++;
				} else {
					files[numFiles] = x;
				}
				numFiles++;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
			} else if(*longInd == 1){
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--wronly %s\n", optarg);
					fflush(stdout);
				}
				int x = open(optarg, O_WRONLY | flagsOred(options), 0777);
				zeroOpenFlags(options);
				if(x == -1){
					fprintf(stderr, "--wronly %s: %s\n", optarg, strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles++;
				} else {
					files[numFiles] = x;
				}
				numFiles++;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
			} else if (*longInd == 2){
				//new execvp array
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				commandArgs.args = malloc(sizeof(char*));
				if(commandArgs.args == NULL){
					fprintf(stderr, "Fatal memory failure\n");
					if(failed == 0){
						failed = 1;
					}
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				options[2].flag[0] = 1;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
			} else if (*longInd == 3){
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--verbose\n");
					fflush(stdout);
				}
				verbose = 1;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
			} else if (*longInd == 4){
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--wait\n");
					fflush(stdout);
				}
				while(numForks > 0){
					int status = 0;
					int output = wait(&status);
					if(output == -1){
						fprintf(stderr, "wait: %s\n", strerror(errno));
						if(failed == 0){
							failed = 1;
						}
						int output = closeAll(files, numFiles);
						if(output == -1){
							fprintf(stderr, "close: %s\n", strerror(errno));
						}
						freeAll(files, commandArgs, commands, numCommands);
						if(signalFailed != 0){
							signal(signalFailed, SIG_DFL);
							raise(signalFailed);
						}
						return failed;
					} else {
						int i = 0;
						for(i = 0; i < numCommands; i++){
							if(commands[i].pid == output){
								break;
							}
						}
						if(i == numCommands){
							fprintf(stderr, "Unknown pid: %d\n", output);
						} else if(WIFEXITED(status)){
							int temp = WEXITSTATUS(status);
							if(temp > failed){
								failed = temp;
							}
							fprintf(stdout, "exit %d", temp);
							int iterator = 0;
							while(commands[i].args[iterator] != NULL){
								fprintf(stdout, " %s", commands[i].args[iterator]);
								iterator++;
							}
							fprintf(stdout, "\n");
							fflush(stdout);
							numForks--;
						} else if(WIFSIGNALED(status)){
							int temp = WTERMSIG(status);
							if(temp > signalFailed){
								signalFailed = temp;
							}
							fprintf(stdout, "signal %d", temp);
							int iterator = 0;
							while(commands[i].args[iterator] != NULL){
								fprintf(stdout, " %s", commands[i].args[iterator]);
								iterator++;
							}
							fprintf(stdout, "\n");
							fflush(stdout);
							numForks--;
						}
					}
				}
				if(profile == 1){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)(temp.ru_utime.tv_sec-prior.ru_utime.tv_sec), (int)(temp.ru_utime.tv_usec-prior.ru_utime.tv_usec), (int)(temp.ru_stime.tv_sec-prior.ru_stime.tv_sec), (int)(temp.ru_stime.tv_usec-prior.ru_stime.tv_usec));
						fflush(stdout);
					}
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_CHILDREN, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "CHILDREN: User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 5){
				profileFlag = 0;
				if(profile == 1){
					int output = getrusage(RUSAGE_SELF, &prior);
					if(output == -1){
						if(failed == 0){
							failed = 1;
						}
						profileFlag = 1;
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--append\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 6){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--cloexec\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 7){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--creat\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 8){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--directory\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 9){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--dsync\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 10){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--excl\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 11){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--nofollow\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 12){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--nonblock\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 13){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--rsync\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 14){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--sync\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 15){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--trunc\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 16){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--rdwr %s\n", optarg);
					fflush(stdout);
				}
				int x = open(optarg, O_RDWR | flagsOred(options), 0777);
				zeroOpenFlags(options);
				if(x == -1){
					fprintf(stderr, "--rdwr %s: %s\n", optarg, strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles++;
				} else {
					files[numFiles] = x;
				}
				numFiles++;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 17){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--pipe %s\n", optarg);
					fflush(stdout);
				}
				int fds[2] = {0, 0};
				int x = pipe(fds);
				if(x == -1){
					fprintf(stderr, "--pipe: %s\n", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles+=2;
				} else {
					files[numFiles] = fds[0];
					files[numFiles+1] = fds[1];
				}
				numFiles+=2;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 18){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--chdir %s\n", optarg);
					fflush(stdout);
				}
				int output = chdir(optarg);
				if(output == -1){
					fprintf(stderr, "--chdir %s: %s\n", optarg, strerror(errno));
					if(failed == 0){
						failed = 1;
					}
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 19){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				int i = 0;
				for(i = 0; i < (int)strlen(optarg); i++){
					if(optarg[i] < '0' || optarg[i] > '9'){
						fprintf(stderr, "%s: invalid file descriptor\n", optarg);
						*longInd = -1;
						output = getopt_long(argc, argv, "-:", options, longInd);
						continue;
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--close %s\n", optarg);
					fflush(stdout);
				}
				int fd = atoi(optarg);
				if(fd < 0 || fd >= numFiles || files[fd] == 0){
					fprintf(stderr, "%s: invalid file descriptor\n", optarg);
					*longInd = -1;
					output = getopt_long(argc, argv, "-:", options, longInd);
					continue;
				}
				int output = close(files[fd]);
				if(output == -1){
					fprintf(stderr, "close %s: %s\n", optarg, strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				files[fd] = 0;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 20){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--abort\n");
					fflush(stdout);
				}
				raise(SIGSEGV);
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 21){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				int i = 0;
				for(i = 0; i < (int)strlen(optarg); i++){
					if(optarg[i] < '0' || optarg[i] > '9'){
						fprintf(stderr, "%s: invalid signal number", optarg);
						*longInd = -1;
						output = getopt_long(argc, argv, "-:", options, longInd);
						continue;
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--catch %s\n", optarg);
					fflush(stdout);
				}
				int number = atoi(optarg);
				if(number < 0){
					fprintf(stderr, "%s: invalid signal number\n", optarg);
					*longInd = -1;
					output = getopt_long(argc, argv, "-:", options, longInd);
					continue;
				}
				struct sigaction sa;
				sa.sa_handler = catcher;
				sigemptyset(&sa.sa_mask);
				sa.sa_flags = SA_RESTART;
				int output = sigaction(number, &sa, NULL);
				if(output == -1){
					fprintf(stderr, "signal: %s", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed == 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 22){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				int i = 0;
				for(i = 0; i < (int)strlen(optarg); i++){
					if(optarg[i] < '0' || optarg[i] > '9'){
						fprintf(stderr, "%s: invalid signal number", optarg);
						*longInd = -1;
						output = getopt_long(argc, argv, "-:", options, longInd);
						continue;
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--ignore %s\n", optarg);
					fflush(stdout);
				}
				int number = atoi(optarg);
				if(number < 0){
					fprintf(stderr, "%s: invalid signal number\n", optarg);
					*longInd = -1;
					output = getopt_long(argc, argv, "-:", options, longInd);
					continue;
				}
				__sighandler_t output = signal(number, SIG_IGN);
				if(output == SIG_ERR){
					fprintf(stderr, "signal: %s", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed == 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 23){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				int i = 0;
				for(i = 0; i < (int)strlen(optarg); i++){
					if(optarg[i] < '0' || optarg[i] > '9'){
						fprintf(stderr, "%s: invalid signal number", optarg);
						*longInd = -1;
						output = getopt_long(argc, argv, "-:", options, longInd);
						continue;
					}
				}
				if(verbose == 1){
					fprintf(stdout, "--default %s\n", optarg);
					fflush(stdout);
				}
				int number = atoi(optarg);
				if(number < 0){
					fprintf(stderr, "%s: invalid signal number\n", optarg);
					*longInd = -1;
					output = getopt_long(argc, argv, "-:", options, longInd);
					continue;
				}
				__sighandler_t output = signal(number, SIG_DFL);
				if(output == SIG_ERR){
					fprintf(stderr, "signal: %s", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed == 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			} else if(*longInd == 24){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--pause\n");
					fflush(stdout);
				}
				pause();
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
				int output = closeAll(files, numFiles);
				if(output == -1){
					if(failed == 0){
						failed = 1;
					}
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			} else if(*longInd == 25){
				profileFlag = 0;
                                if(profile == 1){
                                        int output = getrusage(RUSAGE_SELF, &prior);
                                        if(output == -1){
                                                if(failed == 0){
                                                        failed = 1;
                                                }
                                                profileFlag = 1;
                                                fprintf(stderr, "getrusage: %s\n", strerror(errno));
                                        }
                                }
				if(verbose == 1){
					fprintf(stdout, "--profile\n");
					fflush(stdout);
				}
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
				profile = 1;
				*longInd = -1;
				output = getopt_long(argc, argv, "-:", options, longInd);
				continue;
			}
		}
		*longInd = -1;
		output = getopt_long(argc, argv, "-:", options, longInd);
	}
	if(options[2].flag[0] == 1){
		if(cmdArrCounter < 4){
			fprintf(stderr, "--command: command does not have enough arguments\n");
			if(failed == 0){
				failed = 1;
			}
		} else {
			if(invalidFd){
				if(failed == 0){
					failed = 1;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			}
			int x = 0;
			commandArgs.args[cmdArrCounter-3] = (char*)NULL;
			if(verbose == 1){
				fprintf(stdout, "--command");
				fprintf(stdout, " %d", commandArgs.input);
				fprintf(stdout, " %d", commandArgs.output);
				fprintf(stdout, " %d", commandArgs.error);
				int i = 0;
				while(commandArgs.args[i] != NULL){
					fprintf(stdout, " %s", commandArgs.args[i]);
					i++;
				}
				fprintf(stdout, "\n");
				fflush(stdout);
			}
			if(commandArgs.input >= 0 && commandArgs.input < numFiles && files[commandArgs.input] > 0){
				//DO NOTHING THIS IS GOOD
			} else {
				fprintf(stderr, "File descriptor %d is not open\n", commandArgs.input);
				if(failed == 0){
					failed = 1;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			}
			if(commandArgs.output >= 0 && commandArgs.output < numFiles && files[commandArgs.output] > 0){
				//DO NOTHING THIS IS GOOD
			} else {
				fprintf(stderr, "File descriptor %d is not open\n", commandArgs.output);
				if(failed == 0){
					failed = 1;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			}
			if(commandArgs.error >= 0 && commandArgs.error < numFiles && files[commandArgs.error] > 0){
				//DO NOTHING THIS IS GOOD
			} else {
				fprintf(stderr, "File descriptor %d is not open\n", commandArgs.error);
				if(failed == 0){
					failed = 1;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			}
			x = fork();
			if(x == -1){
				fprintf(stderr, "Fork failure: %s\n", strerror(errno));
				if(failed == 0){
					failed = 1;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			} else if(x == 0){
				x = dup2(files[commandArgs.input], 0);
				if(x == -1){
					fprintf(stderr, "Dup failure: %s\n", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				x = dup2(files[commandArgs.output], 1);
				if(x == -1){
					fprintf(stderr, "Dup failure: %s\n", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				x = dup2(files[commandArgs.error], 2);
				if(x == -1){
					fprintf(stderr, "Dup failure: %s\n", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
					output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					if(signalFailed != 0){
						signal(signalFailed, SIG_DFL);
						raise(signalFailed);
					}
					return failed;
				}
				int output = closeNotStd(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
					int output = closeAll(files, numFiles);
					if(output == -1){
						fprintf(stderr, "close: %s\n", strerror(errno));
					}
					freeAll(files, commandArgs, commands, numCommands);
					return 1;
				}
				output = execvp(commandArgs.file, commandArgs.args);
				failed = 1;
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
				}
				freeAll(files, commandArgs, commands, numCommands);
				if(signalFailed != 0){
					signal(signalFailed, SIG_DFL);
					raise(signalFailed);
				}
				return failed;
			} else {
				numForks++;
				if(profile == 1 && profileFlag == 0){
					struct rusage temp;
					int output = getrusage(RUSAGE_SELF, &temp);
					if(output == -1){
						fprintf(stderr, "getrusage: %s\n", strerror(errno));
					} else {
						fprintf(stdout, "User time: %d s, %d µs. System time: %d s, %d µs.\n", (int)temp.ru_utime.tv_sec, (int)temp.ru_utime.tv_usec, (int)temp.ru_stime.tv_sec, (int)temp.ru_stime.tv_usec);
						fflush(stdout);
					}
				}
			}
		}
	}
	output = closeAll(files, numFiles);
	if(output == -1){
		fprintf(stderr, "close: %s\n", strerror(errno));
	}
	freeAll(files, commandArgs, commands, numCommands);
	if(signalFailed != 0){
		signal(signalFailed, SIG_DFL);
		raise(signalFailed);
	}
	return failed;
}
