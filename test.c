#include "fs.h"

int main()
{
    // create_fs ();
    // sync_fs();
    mount_fs();
    int a = allocate_file("first");

    set_filesize(0,1000);
    print_fs();

    set_filesize(0,5000);
    print_fs();

    set_filesize(0,100);
    print_fs();

    sync_fs();
    print_fs();
    printf("done\n");
    return 0;
}//main