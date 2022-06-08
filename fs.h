#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 1024
#define NAMESIZE 1024

struct superblock
{
    int num_inodes;
    int num_blocks;
    int size_block;
};

struct inode
{
    int size;
    char name[50];
    int  first_block;
    int dir; // 0 for file 1 for dir

};

struct disk_block
{
    int next_block_num;
    char data[512];
};

void create_fs(); //initialize new filesystem
void mount_fs(); //load a file system
void sync_fs(); //write the file system

int allocate_file(char name[50]);//return file number
void set_filesize(int filenum , int size);
void write_data(int filenum,int pos,char *data);

void print_fs(); //print out info about the filesystem