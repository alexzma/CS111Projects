//NAME: Alexander Ma
//EMAIL: alexanderzma@hotmail.com
//ID: 105093055

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* const argv[]){
	if(argc != 2){
		fprintf(stderr, "Invalid number of arguments: %d", argc-1);
		return 1;
	}
	FILE* file = fopen(argv[1], "r");
	if(file == NULL){
		fprintf(stderr, "fopen: %s\n", strerror(errno));
		return 1;
	}
	int failed = 0;
	int output = 0;
	//read first line using getline (superblock)
	char* line = NULL;
	size_t len = 0;
	ssize_t num = getline(&line, &len, file);
	if(num == -1){
		fprintf(stderr, "getline: %s\n", strerror(errno));
		return 1;
	}
	line[num-1] = '\0';
	//parse numblocks and numinodes and inodesize and blocksize
	long long numblocks = -1;
	long long blocksize = -1;
	long long numinodes = -1;
	long long inodesize = -1;
	int commacount = 0;
	char* temp = malloc(num*sizeof(char));
	int tempcounter = 0;
	for(int i = 11; i < num-1; i++){
		if(line[i] == ','){
			commacount++;
			if(commacount == 1){
				temp[tempcounter] = '\0';
				numblocks = atoi(temp);
				tempcounter = 0;
			} else if(commacount == 2){
				temp[tempcounter] = '\0';
				numinodes = atoi(temp);
				tempcounter = 0;
			} else if(commacount == 3){
				temp[tempcounter] = '\0';
				blocksize = atoi(temp);
				tempcounter = 0;
			} else if(commacount == 4){
				temp[tempcounter] = '\0';
				inodesize = atoi(temp);
				tempcounter = 0;
			} else {
				tempcounter = 0;
			}
		} else if(line[i] < '0' || line[i] > '9'){
			fprintf(stderr, "Invalid superblock: %s\n", line);
			free(line);
			return 1;
		} else {
			temp[tempcounter] = line[i];
			tempcounter++;
		}
	}
	if(numblocks == -1 || numinodes == -1 || commacount != 6){
		fprintf(stderr, "Invalid superblock: %s\n", line);
		free(temp);
		free(line);
		return 1;
	}	
	free(line);

	int* reservedblocks = malloc((numblocks)*sizeof(int));
	//-1: not found
	//-2: reserved
	//-3: found
	//-4: found and printed
	//-5: free
	if(reservedblocks == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	for(int i = 0; i < numblocks; i++){
		reservedblocks[i] = -1;
	}
	int* blockinodes = malloc((numblocks)*sizeof(int));
	if(blockinodes == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	long long* blockoffsets = malloc((numblocks)*sizeof(long long));
	if(blockoffsets == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	int* blocklevels = malloc((numblocks)*sizeof(int));
	//0: direct block
	//1: singly indirect block
	//2: doubly indirect block
	//3: triply indirect block
	if(blocklevels == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}

	int* inodestatus = malloc((numinodes+1)*sizeof(int));
	//-1: not found
	//-2: free
	//-3: allocated
	if(inodestatus == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	for(int i = 0; i < numinodes+1; i++){
		inodestatus[i] = -1;
	}
	inodestatus[0] = -2;
	int* inodelinks = malloc((numinodes+1)*sizeof(int));
	if(inodelinks == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	memset(inodelinks, 0, (numinodes+1)*sizeof(int));
	int* inodefoundlinks = malloc((numinodes+1)*sizeof(int));
	if(inodefoundlinks == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	memset(inodefoundlinks, 0, (numinodes+1)*sizeof(int));
	int* inodeparents = malloc((numinodes+1)*sizeof(int));
	if(inodeparents == NULL){
		fprintf(stderr, "Malloc failure\n");
		return 1;
	}
	for(int i = 0; i < numinodes+1; i++){
		inodeparents[i] = -1;
	}
	if(tempcounter != 0){
		temp[tempcounter] = '\0';
		int endreserved = atoi(temp)+1;
		for(int i = 0; i < endreserved; i++){
			inodestatus[i] = -3;
		}
		tempcounter = 0;
	} else {
		fprintf(stderr, "Invalid superblock: %s\n", line);
		return 1;
	}
	free(temp);

	//read second line using getline (group)
	line = NULL;
	num = getline(&line, &len, file);
	if(num == -1){
		fprintf(stderr, "getline: %s\n", strerror(errno));
		return 1;
	}
	line[num-1] = '\0';

	//parse special blocks nums
	commacount = 0;
	temp = malloc(num*sizeof(char));
	tempcounter = 0;
	for(int i = 8; i < num-1; i++){
		if(line[i] == ','){
			commacount++;
			tempcounter = 0;
		} else if(line[i] < '0' || line[i] > '9'){
			fprintf(stderr, "Invalid superblock: %s\n", line);
			free(temp);
			free(line);
			return 1;
		} else {
			temp[tempcounter] = line[i];
			tempcounter++;
		}
	}
	if(tempcounter != 0 && commacount == 6){
		temp[tempcounter] = '\0';
		int numpaststart = (inodesize*numinodes)/blocksize;
		int endreserved = atoi(temp)+numpaststart;
		for(int j = 0; j < endreserved; j++){
			reservedblocks[j] = -2;
		}
		tempcounter = 0;
	} else {
		fprintf(stderr, "Invalid group: %s\n", line);
		return 1;
	}
	free(temp);
	free(line);

	//read bfrees
	line = NULL;
	num = getline(&line, &len, file);
	if(num == -1){
		fprintf(stderr, "getline: %s\n", strerror(errno));
		return 1;
	}
	line[num-1] = '\0';
	if(num < 7){
		fprintf(stderr, "Invalid line: %s\n", line);
		return 1;
	}
	while(line[0] == 'B'){
		temp = malloc(num*sizeof(char));
		tempcounter = 0;
		for(int i = 6; i < num-1; i++){
			if(line[i] < '0' || line[i] > '9'){
				fprintf(stderr, "Invalid bfree: %s\n", line);
				free(temp);
				free(line);
				return 1;
			}
			temp[tempcounter] = line[i];
			tempcounter++;
		}
		temp[tempcounter] = '\0';
		reservedblocks[atoi(temp)] = -5;
		free(temp);
		free(line);
		//read next
		line = NULL;
		num = getline(&line, &len, file);
		if(num == -1){
			fprintf(stderr, "getline: %s\n", strerror(errno));
			return 1;
		}
		line[num-1] = '\0';
		if(num < 7){
			fprintf(stderr, "Invalid line: %s\n", line);
			return 1;
		}
	}

	//read ifrees
	while(line[0] == 'I' && line[1] == 'F'){
		temp = malloc(num*sizeof(char));
		tempcounter = 0;
		for(int i = 6; i < num-1; i++){
			if(line[i] < '0' || line[i] > '9'){
				fprintf(stderr, "Invalid ifree: %s\n", line);
				free(temp);
				free(line);
				return 1;
			}
			temp[tempcounter] = line[i];
			tempcounter++;
		}
		temp[tempcounter] = '\0';
		inodestatus[atoi(temp)] = -2;
		free(temp);
		free(line);
		//read next
		line = NULL;
		num = getline(&line, &len, file);
		if(num == -1){
			fprintf(stderr, "getline: %s\n", strerror(errno));
			return 1;
		}
		line[num-1] = '\0';
		if(num < 7){
			fprintf(stderr, "Invalid line: %s\n", line);
			return 1;
		}
	}

	//inodes: num links, directory or no, output blocks correctness
	while(line[0] == 'I' && line[1] == 'N' && line[2] == 'O'){
		commacount = 0;
		temp = malloc(num*sizeof(char));
		tempcounter = 0;
		int inodenum = 0;
		for(int i = 6; i < num-1; i++){
			if(line[i] == ','){
				commacount++;
				temp[tempcounter] = '\0';
				if(commacount == 1){
					if(inodestatus[atoi(temp)] == -2){
						fprintf(stdout, "ALLOCATED INODE %d ON FREELIST\n", atoi(temp));
						inodestatus[atoi(temp)] = -3;
						failed = 2;
					} else {
						inodestatus[atoi(temp)] = -3;
					}
					inodenum = atoi(temp);
				} else if(commacount == 6){
					inodelinks[inodenum] = atoi(temp);
				} else if(commacount > 11 && commacount < 24){
					int value = atoi(temp);
					if(value != 0){
						if(value < 0 || value >= numblocks){
							fprintf(stdout, "INVALID BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, commacount-12);
							failed = 2;
						} else if(reservedblocks[value] == -2){
							fprintf(stdout, "RESERVED BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, commacount-12);
							failed = 2;
						} else if(reservedblocks[value] == -3){
							if(blocklevels[value] == 0){
								fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 1){
								fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 2){
								fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else {
								fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							}
							fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, commacount-12);
							reservedblocks[value] = -4;
							failed = 2;
						} else if(reservedblocks[value] == -4){
							fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, commacount-12);
							failed = 2;
						} else if(reservedblocks[value] == -5){
							fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", value);
						} else {
							reservedblocks[value] = -3;
							blockinodes[value] = inodenum;
							blockoffsets[value] = commacount-12;
							blocklevels[value] = 0;
						}
					}
				} else if(commacount == 24){
					int value = atoi(temp);
					if(value != 0){
						if(value < 0 || value >= numblocks){
							fprintf(stdout, "INVALID INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 12);
							failed = 2;
						} else if(reservedblocks[value] == -2){
							fprintf(stdout, "RESERVED INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 12);
							failed = 2;
						} else if(reservedblocks[value] == -3){
							failed = 2;
							if(blocklevels[value] == 0){
								fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 1){
								fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 2){
								fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else {
								fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							}
							fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 12);
							reservedblocks[value] = -4;
						} else if(reservedblocks[value] == -4){
							fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 12);
							failed = 2;
						} else if(reservedblocks[value] == -5){
							fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", value);
							failed = 2;
						} else {
							reservedblocks[value] = -3;
							blockinodes[value] = inodenum;
							blockoffsets[value] = 12;
							blocklevels[value] = 1;
						}
					}
				} else if(commacount == 25){
					int value = atoi(temp);
					if(value != 0){
						if(value < 0 || value >= numblocks){
							fprintf(stdout, "INVALID DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 268);
							failed = 2;
						} else if(reservedblocks[value] == -2){
							fprintf(stdout, "RESERVED DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 268);
							failed = 2;
						} else if(reservedblocks[value] == -3){
							failed = 2;
							if(blocklevels[value] == 0){
								fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 1){
								fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else if(blocklevels[value] == 2){
								fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							} else {
								fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
							}
							fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 268);
							reservedblocks[value] = -4;
						} else if(reservedblocks[value] == -4){
							fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 268);
							failed = 2;
						} else if(reservedblocks[value] == -5){
							fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", value);
							failed = 2;
						} else {
							reservedblocks[value] = -3;
							blockinodes[value] = inodenum;
							blockoffsets[value] = 268;
							blocklevels[value] = 2;
						}
					}
				}
				tempcounter = 0;
			} else {
				temp[tempcounter] = line[i];
				tempcounter++;
			}
		}
		if(tempcounter != 0){
			if(commacount == 25){
				temp[tempcounter] = '\0';
				int value = atoi(temp);
				if(value != 0){
					if(value < 0 || value >= numblocks){
						fprintf(stdout, "INVALID TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 65804);
						failed = 2;
					} else if(reservedblocks[value] == -2){
						fprintf(stdout, "RESERVED TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 65804);
						failed = 2;
					} else if(reservedblocks[value] == -3){
						failed = 2;
						if(blocklevels[value] == 0){
							fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
						} else if(blocklevels[value] == 1){
							fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
						} else if(blocklevels[value] == 2){
							fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
						} else {
							fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
						}
						fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 65804);
						reservedblocks[value] = -4;
					} else if(reservedblocks[value] == -4){
						fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inodenum, 65804);
						failed = 2;
					} else if(reservedblocks[value] == -5){
						fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", value);
					} else {
						reservedblocks[value] = -3;
						blockinodes[value] = inodenum;
						blockoffsets[value] = 65804;
						blocklevels[value] = 3;
					}
				}
			}
		} else {
			fprintf(stderr, "Invalid line: %s\n", line);
			return 1;
		}
		free(temp);
		free(line);
		line = NULL;
		num = getline(&line, &len, file);
		if(num == -1){
			fprintf(stderr, "getline: %s\n", strerror(errno));
			return 1;
		}
		if(num < 10){
			fprintf(stderr, "Invalid line: %s\n", line);
			return 1;
		}
		line[num-1] = '\0';
	}

	//indirect: output blocks correctness
	while(line[0] == 'I' && line[1] == 'N' && line[2] == 'D'){
		commacount = 0;
		temp = malloc(num*sizeof(char));
		tempcounter = 0;
		int inum = 0;
		int level = 0;
		int offset = 0;
		for(int i = 9; i < num-1; i++){
			if(line[i] == ','){
				commacount++;
				temp[tempcounter] = '\0';
				if(commacount == 1){
					inum = atoi(temp);
				} else if(commacount == 2){
					level = atoi(temp);
				} else if(commacount == 3){
					offset = atoi(temp);
				}
				tempcounter = 0;
			} else {
				if(line[i] < '0' || line[i] > '9'){
					fprintf(stderr, "Invalid indirect: %s\n", line);
					return 1;
				}
				temp[tempcounter] = line[i];
				tempcounter++;
			}
		}
		if(tempcounter != 0 && inum != 0 && level != 0){
			temp[tempcounter] = '\0';
			int value = atoi(temp);
			if(value != 0){
				if(value < 0 || value >= numblocks){
					failed = 2;
					if(level == 1){
						fprintf(stdout, "INVALID INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else if(level == 2){
						fprintf(stdout, "INVALID DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else {
						fprintf(stdout, "INVALID TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					}
				} else if(reservedblocks[value] == -2){
					failed = 2;
					if(level == 1){
						fprintf(stdout, "RESERVED INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else if(level == 2){
						fprintf(stdout, "RESERVED DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else {
						fprintf(stdout, "RESERVED TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					}
				} else if(reservedblocks[value] == -3){
					failed = 2;
					if(blocklevels[value] == 0){
						fprintf(stdout, "DUPLICATE BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
					} else if(blocklevels[value] == 1){
						fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
					} else if(blocklevels[value] == 2){
						fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
					} else {
						fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %lld\n", value, blockinodes[value], blockoffsets[value]);
					}
					if(level == 1){
						fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else if(level == 2){
						fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d", value, inum, offset);
					} else {
						fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					}
					reservedblocks[value] = -4;
				} else if(reservedblocks[value] == -4){
					if(level == 1){
						fprintf(stdout, "DUPLICATE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else if(level == 2){
						fprintf(stdout, "DUPLICATE DOUBLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					} else {
						fprintf(stdout, "DUPLICATE TRIPLE INDIRECT BLOCK %d IN INODE %d AT OFFSET %d\n", value, inum, offset);
					}
					failed = 2;
				} else if(reservedblocks[value] == -5){
					fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", value);
				} else {
					reservedblocks[value] = -3;
					blockinodes[value] = inum;
					blockoffsets[value] = offset;
					blocklevels[value] = level;
				}
			}
		} else {
			fprintf(stderr, "Invalid indirect: %s\n", line);
			return 1;
		}
		free(temp);
		free(line);
		line = NULL;
		num = getline(&line, &len, file);
		if(num == -1){
			fprintf(stderr, "getline: %s\n", strerror(errno));
			return 1;
		}
		if(num < 10){
			fprintf(stderr, "Invalid line: %s\n", line);
			return 1;
		}
		line[num-1] = '\0';
	}

	//dirents: num links, output inode correctness, name correctness
	while(num != -1){
		line[num-1] = '\0';
		if(line[0] == 'D' && num > 7){
			//DIRENT
			commacount = 0;
			temp = malloc(num*sizeof(char));
			tempcounter = 0;
			int parentinum = 0;
			int inum = 0;
			for(int i = 7; i < num-1; i++){
				if(line[i] == ','){
					commacount++;
					temp[tempcounter] = '\0';
					if(commacount == 1){
						parentinum = atoi(temp);
					} else if(commacount == 3){
						inum = atoi(temp);
					}
					tempcounter = 0;
				} else {
					temp[tempcounter] = line[i];
					tempcounter++;
				}
			}
			if(tempcounter != 0){
				temp[tempcounter] = '\0';
				if(inum < 1 || inum > numinodes){
					failed = 2;
					fprintf(stdout, "DIRECTORY INODE %d NAME %s INVALID INODE %d\n", parentinum, temp, inum);
				} else if(inodestatus[inum] == -2){
					failed = 2;
					fprintf(stdout, "DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d\n", parentinum, temp, inum);
				} else {
					inodefoundlinks[inum] += 1;
					if(strcmp(temp, "'.'") == 0){
						if(parentinum != inum){
							failed = 2;
							fprintf(stdout, "DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n", parentinum, temp, inum, parentinum);
						}
					} else if(strcmp(temp, "'..'") == 0){
						if(parentinum == 2){
							if(inum != 2){
								failed = 2;
								fprintf(stdout, "DIRECTORY INODE 2 NAME '..' LINK TO INODE %d SHOULD BE 2\n", inum);
							}
						} else if(inum != inodeparents[parentinum]){
							if(inodeparents[parentinum] == -1){
								inodeparents[parentinum] = inum;
							} else {
								failed = 2;
								fprintf(stdout, "DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n", parentinum, temp, inum, inodeparents[parentinum]);
							}
						}
					} else {
						if(inodeparents[inum] != -1){
							if(parentinum != inodeparents[inum]){
								fprintf(stdout, "DIRECTORY INODE %d NAME '..' LINK TO INODE %d SHOULD BE %d\n", inum, inodeparents[inum], parentinum);
							}
						}
						inodeparents[inum] = parentinum;
					}
				}
			} else {
				fprintf(stderr, "Invalid dirent: %s\n", line);
				return 1;
			}
			free(temp);
		} else {
			fprintf(stderr, "Invalid dirent: %s\n", line);
			return 1;
		}
		free(line);
		line = NULL;
		num = getline(&line, &len, file);
	}
	if(errno == EINVAL || errno == ENOMEM){
		fprintf(stderr, "getline: %s\n", strerror(errno));
		return 1;
	}
	free(line);
	//check if blocks not found
	for(int i = 1; i < numblocks; i++){
		if(reservedblocks[i] == -1){
			fprintf(stdout, "UNREFERENCED BLOCK %d\n", i);
			failed = 2;
		}
	}
	//check if inodes not found
	for(int i = 1; i < numinodes+1; i++){
		if(inodestatus[i] == -1){
			fprintf(stdout, "UNALLOCATED INODE %d NOT ON FREELIST\n", i);
			failed = 2;
		}
	}
	for(int i = 1; i < numinodes+1; i++){
		if(inodelinks[i] != inodefoundlinks[i]){
			fprintf(stdout, "INODE %d HAS %d LINKS BUT LINKCOUNT IS %d\n", i, inodefoundlinks[i], inodelinks[i]);
			failed = 2;
		}
	}
	//aim for O(n) efficiency or O(nlogn) if sorting
	output = fclose(file);
	if(output == EOF){
		fprintf(stderr, "fclose: %s\n", strerror(errno));
		return 1;
	}
	free(reservedblocks);
	free(blockinodes);
	free(blockoffsets);
	free(blocklevels);
	free(inodestatus);
	free(inodelinks);
	free(inodefoundlinks);
	free(inodeparents);
	return failed;
}
