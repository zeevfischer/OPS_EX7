#include "fs.h"

struct myopenfile myfiles[10000];
struct superblock sb;
struct inode *inodes;
struct disk_block *diskB;


///////////////////////// vidio function /////////////////////////
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
void mount_fs(const char *source)
{
     FILE *file = fopen(source, "r+");
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
void sync_fs(const char* target)
{
    FILE *file = fopen(target, "w+");
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
int allocate_file(int size ,char name[50])
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
    if (inode == -1 || block == -1)
    {
        return -1;
    }

    inodes[inode].size = size; 
    inodes[inode].first_block = block;
    diskB[block].next_block_num = -2;
    strcpy(inodes[inode].name, name);

    if (size > 512) 
    {
        int new_size = -(3*512)/4;
        int next_block;
        while (new_size < size)
        {
            next_block = find_empty_block();
            if (next_block == -1) {
                return -1;
            }
            diskB[block].next_block_num = next_block;
            block = next_block;
            new_size += 512;
        }
    }
    diskB[block].next_block_num = -2;
    return inode; 
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
    int temp = size + 512 -1;
    int num = temp / 512;
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
void write_one_byte(int fd, int opos, char data) 
{ 
    int save_pos = opos;
    int rb = inodes[fd].first_block;
    while (save_pos >= 512)
    {
        save_pos -= 512;
        if (diskB[rb].next_block_num == -1)
        {
            return -1;
        }
        else if (diskB[rb].next_block_num == -2)
        { 
            diskB[rb].next_block_num = find_empty_block(); 
            rb = diskB[rb].next_block_num;
            diskB[rb].next_block_num = -2; 
        } 
        else
        {
            rb = diskB[rb].next_block_num;
        }
    }
    if(opos > inodes[fd].size)
    {
        inodes[fd].size = opos+1;
    }
    diskB[rb].data[save_pos] = data;
}

char read_one_byte(int fd, int pos)
{
    int rb = inodes[fd].first_block;
    while(pos >= 512)
    {
        pos -= 512;
        rb = diskB[rb].next_block_num;
        if(rb==-1)
        {
            return -1;
        }
        else if (rb == -2)
        {
            return -1;
        }
    }
    // printf("pass\n");
    return diskB[rb].data[pos];
}
void write_data(int filenum, int pos, char *data)
{
    //calculate witch block
    int relative_block = pos/512;
    //find the block number
    int bn = get_block_num(filenum, relative_block);
    //calculate the offset in the block
    int offset = pos % 512;
    //wrote the data
    strcpy(&diskB[bn].data[offset], data);
}
// char* read_data(int filenum, int pos)
// {
//     //calculate witch block
//     int relative_block = pos/BLOCKSIZE;
//     //find the block number
//     int bn = get_block_num(filenum, relative_block);
//     //calculate the offset in the block
//     int offset = pos % BLOCKSIZE;
//     //read the data
//     char* res = malloc(sizeof(char)*1024);
//     strcpy(res, &(diskB[bn].data[offset]));
// }
///////////////////////// vidio function /////////////////////////


/////////////////////////  part 1 functions  /////////////////////////
int make_dir(const char *path, const char* name)
{
    int fd = myopendir(path);
    if(fd == -1 || inodes[fd].dir != 1)
    {
        return -1;
    }
    int block = inodes[fd].first_block;
    struct mydirent* dir = (struct mydirent*)diskB[block].data;
    if(dir->size >= 10)
    {
        return -1;  
    }
    int new_fd = allocate_file(sizeof(struct mydirent), name);
    dir->fds[dir->size++] = new_fd;
    inodes[new_fd].dir = 1;
    struct mydirent* newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for(size_t i = 0; i < 10; i++)
    {
        newdir->fds[i] = -1;
    }
    
    char *char_dir = (char*)newdir;

    for(size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        write_one_byte(new_fd, i, char_dir[i]);        
    }
    strcpy(newdir->name, name);
    return new_fd;
}
int myopendir(const char *name)
{
    char path[100];
    strcpy(path, name);
    char *temp;
    const char root[2] = "/";
    temp = strtok(path, root);
    char this_path[100] = "";
    char last_path[100] = "";
    
    while (temp != NULL)
    {
        if(temp == NULL)
        {
            strcpy(last_path, this_path);
            strcpy(this_path, temp);
        }
        temp = strtok(NULL, root);
    }
    
    for (int i = 0; i < sb.num_inodes; i++)
    {
        if (!strcmp(inodes[i].name, this_path))
        {
            if (inodes[i].dir != 1)
            {
                return -1;
            }
            return i;
        }
    }
    return make_dir(last_path,this_path);
}


void mymkfs(int s)
{
    int size_mines_sb = s - sizeof(struct superblock);
    sb.num_inodes = (size_mines_sb/10) / (sizeof(struct inode));
    sb.num_blocks = (size_mines_sb-(size_mines_sb/10)) / (sizeof(struct disk_block));

    inodes = malloc(sb.num_inodes*sizeof(struct inode));
    diskB = malloc(sb.num_blocks*sizeof(struct disk_block));

    for(int i =0; i < sb.num_inodes; i++)
    {
        strcpy(inodes[i].name, "");
        inodes[i].first_block = -1;
        inodes[i].first_block = -1;
        inodes[i].dir = 0;
    }

    for(int i =0; i < sb.num_blocks; i++)
    {
        strcpy(diskB[i].data, "");
        diskB[i].next_block_num = -1;
    }
    // create root
    root();
}
void root()
{
    int root = allocate_file(sizeof(struct mydirent), "root");
    if(root != 0)
    {
        return -1;
    }
    inodes[root].dir = 1;
    struct mydirent* root_dir = malloc(sizeof(struct mydirent));
    for(int i =0; i < 10; i++)
    {
        root_dir->fds[i] = -1;
    }
    strcmp(root_dir->name, "root");
    root_dir->size = 0;
    char* char_root = (char*)root_dir;
    for(int i =0; i < sizeof(struct mydirent); i++)
    {
        write_one_byte(root,i,char_root[i]);
    }
    free(root_dir);
}
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data)
{
    if(source == NULL && target == NULL)
    {
        return -1;
    }
    if(source != NULL)
    {
        mount_fs(source);
    }
    if(target != NULL)
    {
        sync_fs(target);
    }
}
int create_file(const char *path, const char *name)
{
    int new_fd = allocate_file(1,name);
    int dir_fd = myopendir(path);
    struct mydirent *curdir = myreaddir(dir_fd);
    curdir->fds[curdir->size++] = new_fd;
    return new_fd;

}
int myopen(const char *pathname, int flags)
{
    char path[100];
    strcpy(path, pathname);
    char *temp;
    const char root[2] = "/";
    temp = strtok(path, root);
    char this_path[100] = "";
    char last_path[100] = "";
    
    while (temp != NULL)
    {
         if(temp == NULL)
        {
            strcpy(last_path, this_path);
            strcpy(this_path, temp);
        }
        temp = strtok(NULL, root);
    }
    
    for (int i = 0; i < sb.num_inodes; i++)
    {
        if (!strcmp(inodes[i].name, this_path))
        {
            if (inodes[i].dir != 0)
            {
                return -1;
            }
            myfiles[i].fd = i;
            myfiles[i].pos = 0;
            return i;
        }
    }
    int j = create_file(last_path,this_path);
    myfiles[j].fd = j;
    myfiles[j].pos = 0;
    return j;
}
void files()
{
    for(int i =0; i < 10;i++)
    {
        printf("myfilest[i] = %d\n",myfiles[i].fd);
    }
}
int myclose(int myfd)
{
    myfiles[myfd].fd = -1;
    myfiles[myfd].pos = -1;
    return 0;
}
size_t myread(int myfd, void *buf, size_t count)
{
    if(inodes[myfd].dir == 1)
    {
        return -1;
    }
    if(myfiles[myfd].fd != myfd)
    {
        return -1;
    }
    // printf("pass\n");
    char *buff = malloc(count + 1);
    for(int i =0; i < count; i++)
    {
        // printf("fd = %d\n",myfd);
        // files();
        buff[i] = read_one_byte(myfd, myfiles[myfd].pos);
        // printf("buff[i] = %c\n",buff[i]);
        myfiles[myfd].pos++;
    }
    // printf("count = %d\n",count);
    buff[count] = '\0';
    strncpy(buf,buff,count);
    free(buff);
    // printf("buf = %s")
    return myfiles[myfd].pos;
}
size_t mywrite(int myfd, const void *buf, size_t count)
{
    // printf("myfiles[myfd].fd = %d\n",myfiles[myfd].fd);
    // printf("myfd = %d\n",myfd);
    if(inodes[myfd].dir == 1 || myfiles[myfd].fd != myfd)
    {
        return -1;
    }
    char *buff = (char *)buf;
    for(int i =0; i < count;i++)
    {
        write_one_byte(myfd,myfiles[myfd].pos,buff[i]);
        myfiles[myfd].pos++;
    }
    // for(int i=0;i < 4;i++)
    // {
    //     printf("buff = %c\n",read_one_byte(myfd,myfiles[myfd].pos-4+i));
    // }
    return myfiles[myfd].pos;
}
off_t mylseek(int myfd, int offset, int whence)
{
    if(myfiles[myfd].fd != myfd)
    {
        return -1;
    }
    if(whence == SEEK_CUR)
    {
        myfiles[myfd].pos += offset;
    }
    else if(whence == SEEK_END)
    {
        myfiles[myfd].pos = inodes[myfd].size + offset;
    }
    else if(whence == SEEK_SET)
    {
        myfiles[myfd].pos = offset;
    }
    if(myfiles[myfd].pos < 0)
    {
        myfiles[myfd].pos = 0;
    }
    return myfiles[myfd].pos;
}
struct mydirent *myreaddir(int fd)
{
    if(inodes[fd].dir == 0)
    {
        return -1; 
    }
    return (struct mydirent *)diskB[inodes[fd].first_block].data;
}
int myclosedir(int fd)
{
    printf("was not imlemented because it was not needed");
    return 0;
}
