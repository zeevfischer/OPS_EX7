#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#pragma once 

typedef struct dir 
{
    int temp;
    char *name;
} myDIR;

struct myopenfile {
    int fd;// file discriptor
    int pos; //location
};

struct superblock
{
    int num_inodes;
    int num_blocks;
    int size_block;
};

struct inode
{
    int size;
    char name[100];
    int  first_block;
    int dir; // 0 for file 1 for dir

};

struct disk_block
{
    int next_block_num;
    char data[512];
};

struct mydirent { 
    int size;
    int fds[10];
    char name[100];
};

extern struct myopenfile openfiles[10000];
extern struct super_block super_block;
extern struct inode *inodes;
extern struct disk_block *disk_blocks;

///////////////////////// vidio function /////////////////////////

void create_fs(); //initialize new filesystem
void mount_fs(); //load a file system
void sync_fs(); //write the file system
int allocate_file(int size,char name[50]);//return file number
void set_filesize(int filenum , int size);
// void write_data(int filenum,int pos,char *data);
void print_fs(); //print out info about the filesystem

///////////////////////// vidio function /////////////////////////


/////////////////////////  my functions  /////////////////////////

void write_one_byte(int , int, char); 
char read_one_byte(int, int);
void files();

/////////////////////////  my functions  /////////////////////////




/////////////////////////  part 1 functions  /////////////////////////


void mymkfs(int size);

int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data); 

int myopen(const char *pathname, int flags);

int myclose(int myfd);

size_t myread(int myfd, void *buf, size_t count);

size_t mywrite(int myfd, const void *buf, size_t count);

off_t mylseek(int myfd, int offset, int whence);

int myopendir(const char *name);

struct mydirent *myreaddir(int fd); 

int myclosedir(int fd); 


/////////////////////////  part 1 functions  /////////////////////////
