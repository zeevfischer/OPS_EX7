#include "fs.h"

struct superblock sb;
struct inode *inodes;
struct disk_block *diskB;

//initialize new filesystem
void create_fs()
{
    sb.num_inodes = 10;
    sb.num_blocks = 100;
    sb.size_block = sizeof (struct disk_block);

    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    for(int i =0;i<sb.num_inodes;i++)
    {
        inodes[i].size = -1;
        inodes[i].first_block = -1;
        strcpy(inodes[i].name, "empty file");
    }//init inodes

    diskB = malloc(sizeof(struct disk_block) * sb.num_blocks);
    for(int i =0;i<sb.num_blocks;i++)
    {
        diskB[i].next_block_num = -1;
    }//init disc

}//create_fs

//load a file system
void mount_fs()
{
     FILE *file = fopen("fs_data", "r+");
    // super block
    fread(&sb, sizeof(struct superblock), 1, file);

    inodes = malloc(sizeof(struct inode) * sb.num_inodes);
    diskB = malloc(sizeof(struct disk_block) * sb.num_blocks);

    // inodes
    fread(inodes, sizeof(struct inode), sb.num_inodes,file);

    // diskB
    fread(diskB, sizeof(struct disk_block), sb.num_blocks,file);
    
    fclose(file);
}//mount_fs

//write the file system
void sync_fs()
{
    FILE *file = fopen("fs_data", "w+");
    // super block
    fwrite(&sb, sizeof(struct superblock), 1, file);
    // inodes
    fwrite(inodes, sizeof(struct inode), sb.num_inodes,file);
    // diskB
    fwrite(diskB, sizeof(struct disk_block), sb.num_blocks,file);
    
    fclose(file);
}//sync_fs


//print out info about the filesystem
void print_fs()
{
    printf("superblok info\n\n");
    printf("\tnum_inodes %d\n", sb.num_inodes);
    printf("\tnum_blocks %d\n", sb.num_blocks);
    printf("\tsize_blocks %d\n", sb.size_block);

    //inodes
    printf("inodes:\n");
    for (int i = 0; i < sb.num_inodes; i++)
    {
        printf("\tname: %s\n", inodes[i].name);
        printf("\tsize: %d\n", inodes[i].size);
        printf("\tfirst_block: %d\n", inodes[i].first_block);
    }
    // diskB
    printf("block:\n");
    for (int i = 0; i < sb.num_blocks; i++)
    {
        printf("block num: %d next block %d\n", i, diskB[i].next_block_num);
    }
}

//extras for allocate file
int find_empty_inode()
{
    for(int i =0;i<sb.num_inodes;i++)
    {
        if(inodes[i].first_block == -1)
        {
            return i;
        }
    }
    return -1;
}

int find_empty_block()
{
    for(int i =0;i<sb.num_blocks;i++)
    {
        if(diskB[i].next_block_num == -1)
        {
            return i;
        }
    }
    return -1;
}

//return file number
int allocate_file(char name[50])
{
    //Find if the file is already exists
    for (int i = 0; i < sb.num_inodes; i++)
    {
        if (strcmp(inodes[i].name, name)==0)
        {
            return inodes[i].first_block;
        }
        
    }
    
    int inode = find_empty_inode();//find an empty inode 
    int block = find_empty_block();//find an empty block

    inodes[inode].first_block = block;
    strcpy(inodes[inode].name, name);
    diskB[block].next_block_num = -2;
    return block;
}//allocate file

//extra for set file 
void shorten_file(int bn)
{
    int nn = diskB[bn].next_block_num;
    if(diskB[bn].next_block_num >= 0)
    {
        shorten_file(nn);
    }
    diskB[bn].next_block_num = -1;
}

// add/ deleat blocks
void set_filesize(int filenum , int size)
{
    int temp = size + BLOCKSIZE -1;
    int num = temp / BLOCKSIZE;
    int bn = inodes[filenum].first_block;
    //grow the file if necessary
    for (num-- ; num > 0; num--)
    {
        //check next block number
        int next_num = diskB[bn].next_block_num;
        if (next_num == -2)
        {
            int empty = find_empty_block();
            diskB[bn].next_block_num = empty;
            diskB[empty].next_block_num = -2;
        }
        bn = diskB[bn].next_block_num;
    }
    //short the file if necessary 
    shorten_file(bn);
    diskB[bn].next_block_num = -2;
}

int get_block_num(int file, int offeset)
{
    int bn = inodes[file].first_block;
    for(int togo = offeset; togo > 0; togo--)
    {
        bn = diskB[bn].next_block_num;
    }
    return bn;
}
void write_data(int filenum, int pos, char *data)
{
    //calculate witch block
    int relative_block = pos/BLOCKSIZE;
    //find the block number
    int bn = get_block_num(filenum, relative_block);
    //calculate the offset in the block
    int offset = pos % BLOCKSIZE;
    //wrote the data
    strcpy(&diskB[bn].data[offset], data);
}
char* read_data(int filenum, int pos)
{
    //calculate witch block
    int relative_block = pos/BLOCKSIZE;
    //find the block number
    int bn = get_block_num(filenum, relative_block);
    //calculate the offset in the block
    int offset = pos % BLOCKSIZE;
    //read the data
    char* res = malloc(sizeof(char)*1024);
    strcpy(res, &(diskB[bn].data[offset]));
}