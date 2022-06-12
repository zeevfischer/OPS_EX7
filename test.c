#include "fs.h"
#include "mystdio.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    
    printf("creat root\n");
    mymkfs(10000); 
    int dir1 = myopendir("root/test");
    int dir2 = myopendir("root/test");
    if(dir1 == dir2)
    {
        printf("myopendir PASSED\n");
    }
    else
    {
        printf("myopendir FAILED\n");
    }


    int fd1 = myopen("root/test/file1", 0);
    int fd2 = myopen("root/test/file1", 0);
    if(fd1 == fd2)
    {
        printf("myopen PASSED\n");
    }
    else
    {
        printf("myopen FAILED\n");
    }
    int close = myclose(fd2);
    if(close == 0)
    {
        printf("myclose PASSED\n");
    }
    else
    {
        printf("myclose FAILED\n");
    }

    fd1 = myopen("root/test/file1", 0);
    fd2 = myopen("root/test/file1", 0);

    char* text = "test\n"; 
    int ptr =  mywrite(fd1, text, 4);
    // printf("ptr = %d\n",ptr);
    char buf[5];
    mylseek(fd1, -4, SEEK_CUR);
    myread(fd1, buf, 4);
    if(strncmp(buf, text,4) ==0)
    {
        printf("read test PASSED\n");
    }
    else
    {
        printf("read test FAILED\n");
    }

    char buf2[5];
    mylseek(fd2,-4,SEEK_CUR);
    myread(fd2, buf2, 2);
    if(strncmp(buf2, "te",2) == 0)
    {
        printf("seek test PASSED\n\n");
    }
    else
    {
        printf("seek test FAILED\n\n");
    }

    mymount(NULL, "testfiledata.txt", NULL,NULL,NULL);
    //////////////////////////////test part 2/////////////////////////////
    mymount("testfiledata.txt", NULL, NULL, NULL, NULL);
    char buf3[5];
    myFILE *file1 = myfopen("root/test/file1", "r+");
    if(strncmp(file1->data,"test",4) == 0)
    {
        printf("myfopen PASSED\n");
    }
    else
    {
        printf("myfopen FAILED\n");
    }
    myfread(buf3,1,4, file1);
    if(strcmp("test", buf3) == 0)
    {
        printf("myfread PASSED\n");
    }
    else
    {
        printf("myfread FAILED\n");
    }
    print_fs();
    return 0;
}