//NAME: Alexander Ma
//EMAIL: alexanderzma@hotmail.com
//ID: 105093055
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>

void catch(int signal){
	if(signal == SIGSEGV){
		fprintf(stderr, "Segmentation fault\n");
		exit(4);
	}
}

int readWrite(int inFd, int outFd){
	char buffer[1024];
	int readCount = read(inFd, buffer, 1024);
	while(readCount != 0){
		if(readCount == -1){
			return -1;
		}
		char* temp = buffer;
		int writeCount = write(outFd, buffer, readCount);
		while(readCount != 0){
			if(writeCount == -1){
				return -2;
			}
			readCount -= writeCount;
			temp += writeCount;
			writeCount = write(outFd, temp, readCount);
		}
		readCount = read(inFd, buffer, 1024);
	}
	return 0;
}

int main(int argc, char* const argv[]){
	opterr = 0;

	struct option options[6];

	options[0].name = "input";
	options[0].has_arg = 1;
	int flag0[1] = {0};
	options[0].flag = flag0;
	options[0].val = 1;

	options[1].name = "output";
	options[1].has_arg = 1;
	int flag1[1] = {0};
	options[1].flag = flag1;
	options[1].val = 1;

	options[2].name = "segfault";
	options[2].has_arg = 0;
	int flag2[1] = {0};
	options[2].flag = flag2;
	options[2].val = 1;

	options[3].name = "catch";
	options[3].has_arg = 0;
	int flag3[1] = {0};
	options[3].flag = flag3;
	options[3].val = 1;

	options[4].name = "dump-core";
	options[4].has_arg = 0;
	int flag4[1] = {0};
	options[4].flag = flag4;
	options[4].val = 1;

	options[5].name = 0;
	options[5].has_arg = 0;
	options[5].flag = 0;
	options[5].val = 0;
	
	int argPos[1] = {-1};
	char* inFile = NULL;
	char* outFile = NULL;
	int output = getopt_long(argc, argv, "-:", options, argPos);

	while(output != -1){
		if(output == '?'){
			if(argc>optind && argv[optind][0] == '-' && argv[optind][1] != '-'){
				fprintf(stderr, "Invalid argument: %s\n", argv[optind]);
			} else {
				fprintf(stderr, "Invalid argument: %s\n", argv[optind-1]);
			}
			fprintf(stderr, "Usage: lab0 --input=file --output=file --segfault --catch --dump-core\n");
			free(inFile);
			free(outFile);
			exit(1);
		}
		if(output == ':'){
			fprintf(stderr, "Argument %s requires a value\n", argv[optind-1]);
			fprintf(stderr, "Usage: lab0 --input=file --output=file --segfault --catch --dump-core\n");
			free(inFile);
			free(outFile);
			exit(1);
		}
		if(*argPos == -1){
			fprintf(stderr, "Invalid argument: %s\n", argv[optind-1]);
			fprintf(stderr, "Usage: lab0 --input=file --output=file --segfault --catch --dump-core\n");
			free(inFile);
			free(outFile);
			exit(1);
		}
		if(*argPos == 0){
			free(inFile);
			inFile = malloc(strlen(optarg)*sizeof(char)+sizeof(char));
			if(inFile == NULL){
				free(outFile);
				fprintf(stderr, "Memory allocation failure\n");
				exit(-1);
			}
			strcpy(inFile, optarg);
		}
		if(*argPos == 1){
			free(outFile);
			outFile = malloc(strlen(optarg)*sizeof(char)+sizeof(char));
			if(outFile == NULL){
				free(inFile);
				fprintf(stderr, "Memory allocation failure\n");
				exit(-1);
			}
			strcpy(outFile, optarg);
		}
		if(options[3].flag[0] == 1 && options[4].flag[0] == 1){
			options[3].flag[0] = 0;
			options[4].flag[0] = 0;
		}
		argPos[0] = -1;
		output = getopt_long(argc, argv, "", options, argPos);
	}

	if(options[3].flag[0] == 1 && options[4].flag[0] != 1){
		signal(SIGSEGV, catch);
	}

	if(options[2].flag[0] == 1){
		free(inFile);
		free(outFile);
		char* crash = NULL;
		crash[0] = 'f';
	}

	if(inFile == NULL){
		if(outFile == NULL){
			//read stdin and write stdout
			output = readWrite(0, 1);
		} else {
			//read stdin and write outFile
			const char* out2 = outFile;
			int outFd = open(out2, O_CREAT | O_TRUNC | O_WRONLY, 0777);
			if(outFd == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				free(outFile);
				exit(3);
			}
			int closed = close(1);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", "stdout", strerror(errno));
				free(outFile);
				exit(-1);
			}
			int newFd = dup(outFd);
			if(newFd == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				free(outFile);
				exit(-1);
			}
			closed = close(outFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				free(outFile);
				exit(-1);
			}
			output = readWrite(0, newFd);
			closed = close(newFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				free(outFile);
				exit(-1);
			}
		}
	} else {
		if(outFile == NULL){
			//read inFile and write stdout
			const char* in2 = inFile;
			int inFd = open(in2, O_RDONLY);
			if(inFd == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				exit(2);
			}
			int closed = close(0);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", "stdin", strerror(errno));
				free(inFile);
				exit(-1);
			}
			int newFd = dup(inFd);
			if(newFd == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				exit(-1);
			}
			closed = close(inFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				exit(-1);
			}
			output = readWrite(newFd, 1);
			closed = close(newFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				exit(-1);
			}
		} else {
			//read inFile and write outFile
			const char* in2 = inFile;
			int inFd = open(in2, O_RDONLY);
			if(inFd == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				free(outFile);
				exit(2);
			}
			int closed = close(0);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", "stdin", strerror(errno));
				free(inFile);
				free(outFile);
				exit(-1);
			}
			int newFd1 = dup(inFd);
			if(newFd1 == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				free(outFile);
				exit(-1);
			}
			closed = close(inFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				free(inFile);
				free(outFile);
				exit(-1);
			}
			const char* out2 = outFile;
			int outFd = open(out2, O_CREAT | O_TRUNC | O_WRONLY, 0777);
			if(outFd == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				int closed = close(newFd1);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(3);
			}
			closed = close(1);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", "stdout", strerror(errno));
				int closed = close(newFd1);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(-1);
			}
			int newFd2 = dup(outFd);
			if(newFd2 == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				int closed = close(newFd1);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(-1);
			}
			closed = close(outFd);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				int closed = close(newFd1);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(-1);
			}
			output = readWrite(newFd1, newFd2);
			closed = close(newFd1);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				int closed = close(newFd2);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(-1);
			}
			closed = close(newFd2);
			if(closed == -1){
				fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
				int closed = close(newFd1);
				if(closed == -1){
					fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
				}
				free(inFile);
				free(outFile);
				exit(-1);
			}
		}
	}

	if(output == -1){//input file read error
		if(inFile != NULL){
			fprintf(stderr, "%s: %s\n", inFile, strerror(errno));
			free(inFile);
		} else {
			fprintf(stderr, "stdin: %s\n", strerror(errno));
		}
		free(outFile);
		return -1;
	}
	if(output == -2){//output file write error
		if(outFile != NULL){
			fprintf(stderr, "%s: %s\n", outFile, strerror(errno));
			free(outFile);
		} else {
			fprintf(stderr, "stdout: %s\n", strerror(errno));
		}
		free(inFile);
		return -1;
	}

	free(inFile);
	free(outFile);

	return 0;
}
