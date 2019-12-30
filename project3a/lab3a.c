//NAME: Alexander Ma
//ID: 105093055
//EMAIL: alexanderzma@hotmail.com
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "ext2_fs.h"
#include <stdlib.h>
#include <time.h>

//pread
//open
//close
//fprintf
//memcpy

int recursiveIndirect(int levels, int file, int blocksize, int block, int owner, int indirect, int logical, char mode, int* dirIndex, int isize){
	if(levels < 1){
		return 0;
	}
	int numints = blocksize/(long)sizeof(__u32);
	int output = 0;
	__u32 array[numints];
	int num = 0;
	while(num < blocksize){
		int temp = pread(file, array+num, blocksize-num, block*blocksize+num);
		if(temp == -1 || temp == EOF || temp == 0){
			fprintf(stderr, "Pread: %s\n", strerror(errno));
			exit(2);
		}
		num += temp;
	}
	if(levels == 1){
		int i = 0;
		for(i = 0; i < numints; i++){
			if(array[i] != 0){
				fprintf(stdout, "INDIRECT");
				fprintf(stdout, ",%d", owner);
				fprintf(stdout, ",%d", levels);
				fprintf(stdout, ",%d", logical+i);
				fprintf(stdout, ",%d", indirect);
				fprintf(stdout, ",%d\n", array[i]);
				output++;
				if(mode == 'd'){
					int blockIndex = 0;
					while(blockIndex < blocksize && *dirIndex < isize){
						struct ext2_dir_entry dirent;
						int tracker = 0;
						while(tracker < (int)sizeof(dirent)){
							int temp = pread(file, &dirent+tracker, sizeof(dirent)-tracker, blocksize*array[i]+tracker+blockIndex);
							if(temp == -1 || temp == EOF || temp == 0){
								fprintf(stderr, "Pread: %s\n", strerror(errno));
								return 2;
							}
							tracker += temp;
						}
						if(dirent.name_len == 0){
							break;
						}
						fprintf(stdout, "DIRENT");
						fprintf(stdout, ",%d", owner);
						fprintf(stdout, ",%d", *dirIndex);
						fprintf(stdout, ",%d", dirent.inode);
						fprintf(stdout, ",%d", dirent.rec_len);
						*dirIndex += dirent.rec_len;
						blockIndex += dirent.rec_len;
						fprintf(stdout, ",%d", dirent.name_len);
						char name[EXT2_NAME_LEN+1];
						memcpy(name, dirent.name, dirent.name_len);
						name[dirent.name_len] = '\0';
						fprintf(stdout, ",'");
						fprintf(stdout, "%s", name);
						fprintf(stdout, "'\n");
					}
				}
			}
		}
	}
	if(levels != 1) {
		//recursion
		int i = 0;
		for(i = 0; i < numints; i++){
			if(array[i] == 0){
				if(levels == 2){
					logical += 16*16;
				} else if(levels == 3){
					logical += 16*16*16*16;
				}
				continue;
			}
			fprintf(stdout, "INDIRECT");
			fprintf(stdout, ",%d", owner);
			fprintf(stdout, ",%d", levels);
			fprintf(stdout, ",%d", logical);
			fprintf(stdout, ",%d", indirect);
			fprintf(stdout, ",%d\n", array[i]);
			output++;
			output += recursiveIndirect(levels-1, file, blocksize, array[i], owner, array[i], logical, mode, dirIndex, isize);
			if(levels == 2){
				logical += 16*16;
			} else if(levels == 3){
				logical += 16*16*16*16;
			}
		}
	}
	output++;
	return output;
}

int main(int argc, char* const argv[]){
	if(argc != 2){
		fprintf(stderr, "Invalid number of arguments: %d", argc-1);
		fprintf(stderr, "\n");
		return 1;
	}
	if(argv[1][0] == '-'){
		fprintf(stderr, "This program does not take option commands such as: %s\n", argv[1]);
		return 1;
	}
	int file = open(argv[1], O_RDONLY);
	if(file == -1){
		fprintf(stderr, "Open: %s\n", strerror(errno));
		return 2;
	}
	//read superblock
	struct ext2_super_block superblock;
	int output = 0;
	while(output < (int)sizeof(superblock)){
		int temp = pread(file, &superblock+output, sizeof(superblock)-output, 1024+output);
		if(temp == -1 || temp == EOF || temp == 0){
			fprintf(stderr, "Pread: %s\n", strerror(errno));
			return 2;
		}
		output += temp;
	}
	fprintf(stdout, "SUPERBLOCK");
	int blockscount = superblock.s_blocks_count;
	fprintf(stdout, ",%d", superblock.s_blocks_count);
	int inodescount = superblock.s_inodes_count;
	fprintf(stdout, ",%d", superblock.s_inodes_count);
	int blocksize = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
	fprintf(stdout, ",%d", blocksize);
	fprintf(stdout, ",%d", superblock.s_inode_size);
	fprintf(stdout, ",%d", superblock.s_blocks_per_group);
	fprintf(stdout, ",%d", superblock.s_inodes_per_group);
	fprintf(stdout, ",%d", superblock.s_first_ino);
	fprintf(stdout, "\n");
	//read group summary
	struct ext2_group_desc group;
	int block = 0;
	if(blocksize == 1024){
		output = 0;
		block = 2;
		while(output < (int)sizeof(group)){
			int temp = pread(file, &group+output, sizeof(group)-output, 2048+output);
			if(temp == -1 || temp == EOF || temp == 0){
				fprintf(stderr, "Pread: %s\n", strerror(errno));
				return 2;
			}
			output += temp;
		}
	} else {
		block = 1;
		output = 0;
		while(output < (int)sizeof(group)){
			int temp = pread(file, &group+output, sizeof(group)-output, blocksize+output);
			if(temp == -1 || temp == EOF || temp == 0){
				fprintf(stderr, "Pread: %s\n", strerror(errno));
				return 2;
			}
			output += temp;
		}
	}
	fprintf(stdout, "GROUP");
	fprintf(stdout, ",%d", 0);
	fprintf(stdout, ",%d", superblock.s_blocks_count);
	fprintf(stdout, ",%d", superblock.s_inodes_per_group);
	fprintf(stdout, ",%d", group.bg_free_blocks_count);
	fprintf(stdout, ",%d", group.bg_free_inodes_count);
	fprintf(stdout, ",%d", group.bg_block_bitmap);
	fprintf(stdout, ",%d", group.bg_inode_bitmap);
	fprintf(stdout, ",%d", group.bg_inode_table);
	fprintf(stdout, "\n");
	//block bitmap
	char* blockbitmap = malloc(blocksize*sizeof(char));
	if(blockbitmap == NULL){
		fprintf(stderr, "Malloc error\n");
		return 2;
	}
	block = group.bg_block_bitmap;
	output = 0;
	while(output < blocksize){
		int temp = pread(file, blockbitmap+output, blocksize-output, blocksize*block+output);
		if(temp == -1 || temp == EOF || temp == 0){
			fprintf(stderr, "Pread: %s\n", strerror(errno));
			//free(blockbitmap);
			return 2;
		}
		output += temp;
	}
	int i;
	for(i = 1; i < blockscount; i++){
		int index = 0;
		int offset = 0;
		index = (i-1)/8;
		offset = (i-1)%8;
		if((blockbitmap[index]&(1 << offset)) == 0){
			fprintf(stdout, "BFREE");
			fprintf(stdout, ",%d\n", i);
		}
	}
	//free(blockbitmap); //TODO: Uncomment
	//inode bitmap
	char* inodebitmap = malloc(blocksize*sizeof(char));
	if(inodebitmap == NULL){
		fprintf(stderr, "Malloc error\n");
		return 2;
	}
	block = group.bg_inode_bitmap;
	output = 0;
	while(output < blocksize){
		int temp = pread(file, inodebitmap+output, blocksize*block-output, blocksize*block+output);
		if(temp == -1 || temp == EOF || temp == 0){
			fprintf(stderr, "Pread: %s\n", strerror(errno));
			//free(inodebitmap);
			return 2;
		}
		output += temp;
	}
	for(i = 1; i < inodescount+1; i++){
		int index = 0;
		int offset = 0;
		index = (i-1)/8;
		offset = (i-1)%8;
		if((inodebitmap[index]&(1 << offset)) == 0){
			fprintf(stdout, "IFREE");
			fprintf(stdout, ",%d\n", i);
		}
		index++;
	}
	//free(inodebitmap); //TODO: uncomment
	//inode summary
	int inodeCount = 0;
	int index = 0;
	block = group.bg_inode_table;
	struct ext2_inode inode;
	for(inodeCount = 0; inodeCount < inodescount; inodeCount++){
		output = 0;
		while(output < (int)sizeof(inode)){
			int temp = pread(file, &inode+output, sizeof(inode)-output, blocksize*block+output+index*sizeof(inode));
			if(temp == -1 || temp == EOF || temp == 0){
				fprintf(stderr, "Pread: %s\n", strerror(errno));
				return 2;
			}
			output += temp;
		}
		if(inode.i_mode != 0 && inode.i_links_count != 0){
			fprintf(stdout, "INODE");
			fprintf(stdout, ",%d", index+1);
			char mode = '?';
			if(S_ISDIR(inode.i_mode)){
				fprintf(stdout, ",d");
				mode = 'd';
			} else if(S_ISREG(inode.i_mode)){
				fprintf(stdout, ",f");
				mode = 'f';
			} else if(S_ISLNK(inode.i_mode)){
				fprintf(stdout, ",s");
				mode = 's';
			} else {
				fprintf(stdout, ",?");
			}
			fprintf(stdout, ",%o", inode.i_mode&0xfff);
			fprintf(stdout, ",%d", inode.i_uid);
			fprintf(stdout, ",%d", inode.i_gid);
			fprintf(stdout, ",%d", inode.i_links_count);
			struct tm* time;
			time_t ctime = (time_t)inode.i_ctime;
			time = gmtime(&ctime);
			if(time == NULL){
				fprintf(stderr, "Ctime parsing error\n");
				return 2;
			}
			fprintf(stdout, ",%02d/%02d/%02d %02d:%02d:%02d", time->tm_mon+1, time->tm_mday, time->tm_year%100, time->tm_hour, time->tm_min, time->tm_sec);
			time_t mtime = (time_t)inode.i_mtime;
			time = gmtime(&mtime);
			if(time == NULL){
				fprintf(stderr, "Mtime parsing error\n");
				return 2;
			}
			fprintf(stdout, ",%02d/%02d/%02d %02d:%02d:%02d", time->tm_mon+1, time->tm_mday, time->tm_year%100, time->tm_hour, time->tm_min, time->tm_sec);
			time_t atime = (time_t)inode.i_atime;
			time = gmtime(&atime);
			if(time == NULL){
				fprintf(stderr, "Atime parsing error\n");
				return 2;
			}
			fprintf(stdout, ",%02d/%02d/%02d %02d:%02d:%02d", time->tm_mon+1, time->tm_mday, time->tm_year%100, time->tm_hour, time->tm_min, time->tm_sec);
			fprintf(stdout, ",%d", inode.i_size);
			fprintf(stdout, ",%d", inode.i_blocks);
			if(mode == 'd' || mode == 'f' || (mode == 's' && inode.i_size > 60)){
				int i = 0;
				for(i = 0; i < 15; i++){
					fprintf(stdout, ",%d", inode.i_block[i]);
				}
				fprintf(stdout, "\n");
			}
			int dirIndex = 0;
			if(mode == 'd'){
				int i = 0;
				int blockIndex = 0;
				for(i = 0; i < 12; i++){
					if(inode.i_block[i] == 0){
						continue;
					}
					struct ext2_dir_entry dirent;
					output = 0;
					while(output < (int)sizeof(dirent)){
						int temp = pread(file, &dirent+output, sizeof(dirent)-output, blocksize*inode.i_block[i]+output+blockIndex);
						if(temp == -1 || temp == EOF || temp == 0){
							fprintf(stderr, "Pread: %s\n", strerror(errno));
							return 2;
						}
						output += temp;
					}
					if(dirent.name_len == 0){
						continue;
					}
					fprintf(stdout, "DIRENT");
					fprintf(stdout, ",%d", index+1);
					fprintf(stdout, ",%d", dirIndex);
					fprintf(stdout, ",%d", dirent.inode);
					fprintf(stdout, ",%d", dirent.rec_len);
					dirIndex += dirent.rec_len;
					blockIndex += dirent.rec_len;
					fprintf(stdout, ",%d", dirent.name_len);
					char name[EXT2_NAME_LEN+1];
					memcpy(name, dirent.name, dirent.name_len);
					name[dirent.name_len] = '\0';
					fprintf(stdout, ",'");
					fprintf(stdout, "%s", name);
					fprintf(stdout, "'\n");
					if(blockIndex >= blocksize || dirIndex >= (long)inode.i_size){
						blockIndex = 0;
						continue;
					}
					i--;
				}
			}
			if(inode.i_block[12] != 0 && (mode == 'd' || mode == 'f')){
				recursiveIndirect(1, file, blocksize, inode.i_block[12], index+1, inode.i_block[12], 12, mode, &dirIndex, inode.i_size);
			}
			if(inode.i_block[13] != 0 && (mode == 'd' || mode == 'f')){
				recursiveIndirect(2, file, blocksize, inode.i_block[13], index+1, inode.i_block[13], 12+16*16, mode, &dirIndex, inode.i_size);
			}
			if(inode.i_block[14] != 0 && (mode == 'd' || mode == 'f')){
				recursiveIndirect(3, file, blocksize, inode.i_block[14], index+1, inode.i_block[14], 12+16*16+16*16*16*16, mode, &dirIndex, inode.i_size);
			}
		}
		index++;
	}	
	output = close(file);
	if(output == -1){
		fprintf(stderr, "Close: %s\n", strerror(errno));
		return 2;
	}
	return 0;
}
