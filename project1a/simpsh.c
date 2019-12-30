//NAME: Alexander Ma
//EMAIL: alexanderzma@hotmail.com
//ID: 105093055

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
	char* file;
	char** args;//first arg is file, last arg is NULL
};

void freeAll(int* files, struct command commandArgs){
	free(files);
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

int main(int argc, char* const argv[]){
	opterr = 0;

	struct option options[5];

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
	
	options[4].name = 0;
	options[4].has_arg = 0;
	options[4].flag = 0;
	options[4].val = 0;
	
	int longInd[1] = {-1};
	int verbose = 0;
	int cmdArrCounter = 0;
	int failed = 0;
	int numFiles = 0;
	int numFailedFiles = 0;
	int invalidFd = 0;
	struct command commandArgs;
	commandArgs.args = NULL;
	int* files = (int*)malloc(argc*sizeof(int));
	if(files == NULL){
		fprintf(stderr, "Memory failure\n");
		freeAll(files, commandArgs);
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
					freeAll(files, commandArgs);
					exit(failed);
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
			commandArgs.args = malloc(sizeof(char*));
			if(commandArgs.args == NULL){
				fprintf(stderr, "Fatal memory failure\n");
				if(failed == 0){
					failed = 1;
				}
				freeAll(files, commandArgs);
				exit(failed);
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
						continue;
					}
					int x = 0;
					commandArgs.args[cmdArrCounter-3] = (char*)NULL;
					if(options[3].flag[0] == 1){
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
						continue;
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
						continue;
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
						continue;
					}
					x = fork();
					if(x == -1){
						fprintf(stderr, "Fork failure: %s. Going to next command.\n", strerror(errno));
						free(commandArgs.args);
						commandArgs.args = NULL;
						cmdArrCounter = 0;
						options[2].flag[0] = 0;
						continue;
					} else if(x == 0){
						x = dup2(files[commandArgs.input], 0);
						if(x == -1){
							fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							if(failed == 0){
								failed = 1;
							}
							closeAll(files, numFiles);
							freeAll(files, commandArgs);
							return failed;
						}
						x = dup2(files[commandArgs.output], 1);
						if(x == -1){
						fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							output = closeAll(files, numFiles);
							if(failed == 0){
								failed = 1;
							}
							closeAll(files, numFiles);
							freeAll(files, commandArgs);
							return failed;
						}
						x = dup2(files[commandArgs.error], 2);
						if(x == -1){
							fprintf(stderr, "Dup failure: %s\n", strerror(errno));
							if(failed == 0){
								failed = 1;
							}
							closeAll(files, numFiles);
							freeAll(files, commandArgs);
							return failed;
						}
						output = execvp(commandArgs.file, commandArgs.args);
						if(output > 0){
							failed = output;
						}
						output = closeAll(files, numFiles);
						if(output == -1){
							fprintf(stderr, "close: %s\n", strerror(errno));
							if(failed == 0){
								failed = 1;
							}
						}
						freeAll(files, commandArgs);
						return failed;
					} else {
						numForks++;
					}
				}
				free(commandArgs.args);
				commandArgs.args = NULL;
				cmdArrCounter = 0;
				options[2].flag[0] = 0;
			}
			if(*longInd == 0){
				if(options[3].flag[0] == 1){
					fprintf(stdout, "--rdonly %s\n", optarg);
					fflush(stdout);
				}
				int x = open(optarg, O_RDONLY);
				if(x == -1){
					fprintf(stderr, "--rdonly %s: %s\n", optarg, strerror(errno));
					files[numFiles] = 0;
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles++;
				} else {
					files[numFiles] = x;
				}
				numFiles++;
			} else if(*longInd == 1){
				if(options[3].flag[0] == 1){
					fprintf(stdout, "--wronly %s\n", optarg);
					fflush(stdout);
				}
				int x = open(optarg, O_WRONLY);
				if(x == -1){
					fprintf(stderr, "--wronly %s: %s\n", optarg, strerror(errno));
					files[numFiles] = 0;
					if(failed == 0){
						failed = 1;
					}
					numFailedFiles++;
				} else {
					files[numFiles] = x;
				}
				numFiles++;
			} else if (*longInd == 2){
				commandArgs.args = malloc(sizeof(char*));
				if(commandArgs.args == NULL){
					fprintf(stderr, "Fatal memory failure\n");
					if(failed == 0){
						failed = 1;
					}
					freeAll(files, commandArgs);
					exit(failed);
				}
				options[2].flag[0] = 1;
			} else if (*longInd == 3){
				if(verbose == 1){
					fprintf(stdout, "--verbose\n");
					fflush(stdout);
				}
				verbose = 1;
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
				freeAll(files, commandArgs);
				return failed;
			}
			int x = 0;
			commandArgs.args[cmdArrCounter-3] = (char*)NULL;
			if(options[3].flag[0] == 1){
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
				freeAll(files, commandArgs);
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
				freeAll(files, commandArgs);
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
				freeAll(files, commandArgs);
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
				freeAll(files, commandArgs);
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
					freeAll(files, commandArgs);
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
					freeAll(files, commandArgs);
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
					freeAll(files, commandArgs);
					return failed;
				}
				output = execvp(commandArgs.file, commandArgs.args);
				if(output > 0){
					failed = output;
				}
				output = closeAll(files, numFiles);
				if(output == -1){
					fprintf(stderr, "close: %s\n", strerror(errno));
					if(failed == 0){
						failed = 1;
					}
				}
				freeAll(files, commandArgs);
				return failed;
			} else {
				numForks++;
			}
		}
	}
	output = closeAll(files, numFiles);
	if(output == -1){
		fprintf(stderr, "close: %s\n", strerror(errno));
	}
	freeAll(files, commandArgs);

	return failed;
}
