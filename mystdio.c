#include "mystdio.h"


myFILE* myfopen(const char * pathname, const char * mode)
{
    if (strlen(mode) > 2)
    {
        exit(0);
    }
    myFILE *file = (myFILE *)malloc(sizeof(myFILE));
    if(file == NULL)
    {
        exit(0);
    }
    file->fd = myopen(pathname,0);
    strcpy(file->mod,mode);
    if(*file->mod == 'a')
    {
        file->pointer = inodes[file->fd].size;
    }
    else
    {
        file->pointer = 0;
    }
    file->size = inodes[file->fd].size;
    file->data = malloc(file->size);
    if(*file->mod == 'w')
    {
        for(int i =0; i < file->size;i++)
        {
            file->data[i] = ' ';
        }
        file->data[file->size] = '\0';
    }
    else
    {
        for(int i =0; i < file->size;i++)
        {
            file->data[i] = read_one_byte(file->fd , i);
        }
    }
    myclose(file->fd);
    return file;
}
int myfclose(myFILE *stream)
{
    if(!strcmp(stream->mod,"r"))
    {
        free(stream->data);
        free(stream);
        return 1;
    }
    for(int i=0; i < stream->size; i++)
    {
        write_one_byte(stream->fd,i,stream->data[i]);
    }
    free(stream->data);
    free(stream);
    return 1;
}
size_t myfread(void * ptr, size_t size, size_t nmemb,myFILE * stream)
{
    int read_size = size * nmemb;
    char* buff = malloc(read_size+1);
    int file_pointer = stream->pointer;
    if (stream->mod[0] == 'r' || (stream->mod[0] == 'r' && stream->mod[1] == '+'))
    {
        for(int i =0;i < read_size;i++)
        {
            if(stream->pointer+i > stream->size)
            {
                break;
            }
            buff[i] = stream->data[stream->pointer+i];
            file_pointer++;
        }
    }
    stream->pointer = file_pointer;
    buff[read_size] = '\0';
    strncpy(ptr,buff,read_size);
    free(buff);
    return stream->pointer;
}
size_t myfwrite(const void * ptr, size_t size, size_t nmemb, myFILE * stream)
{
    if(*stream->mod == 'r')
    {
        return -1;
    }
    int write_size = size*nmemb;
    char* buff = (char*)ptr;
    if(stream->pointer + write_size > stream->size)
    {
        char* new_buff = malloc(stream->size+1);
        for(int i=0 ;i < stream->size;i++)
        {
            new_buff[i] = stream->data[i];
        }
        free(stream->data);
        stream->data = malloc(stream->pointer+write_size);
        for(int i =0;i < stream->size;i++)
        {
            stream->data[i] = new_buff[i];
        }
        free(new_buff);
    }
    int pointer = stream->pointer;
    for(int i=0;i < write_size;i++)
    {
        stream->data[stream->pointer+i] = buff[i];
        pointer++;
    }
    stream->pointer = pointer;
    return stream->pointer;
}
int myfseek(myFILE *stream, long offset, int whence)
{
    if(whence == SEEK_CUR)
    {
        stream->pointer += offset;
    }
    if(whence == SEEK_END)
    {
        stream->pointer = stream->size+offset;
    }
    if(whence == SEEK_SET)
    {
        stream->pointer = offset;
    }
    if(stream->pointer < 0)
    {
        stream->pointer = 0;
    }
    return stream->pointer;
}
int myfscanf(myFILE * stream,const char * format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    char* buff; 
    int i = 0;
    int j = 0;
    while (format && format[i])
    {
        if (format[i] == '%')
        {
            if (format[i+1]=='d')
            {
                *(int *)va_arg( arguments, int* ) = strtol(&stream->data[j], &buff, 10);
                j+=strlen(buff) - stream->size;
            }
            else if (format[i+1]=='f')
            {
                *(double *)va_arg( arguments, double* ) = strtol(&stream->data[j], &buff, 10);
                j+=strlen(buff) - stream->size;
            }
            else if (format[i+1]=='c')
            {
                *(char *)va_arg( arguments, char* ) = stream->data[j];
            } 
            i++;
        }
        i++;
        j++;
    }
    return 1;
}
int myfprintf(myFILE * stream, const char * format, ...)
{
    va_list arguments;
    va_start ( arguments, format );
    char *temp_buff = malloc(strlen(format)+5000);
    char *buff = malloc(500);
    int j = 0;
    for (size_t i = 0; i < strlen(format); i++)
    {
        memset(buff, 0, 500);
        if (format[i]=='%')
        {
            if(format[i+1]=='d')
            {
                int currvar = va_arg(arguments, int);
                sprintf(buff,"%d",currvar);
            }
            else if(format[i+1]=='f')
            {
                double currvar = va_arg ( arguments, double);
                sprintf(buff,"%f",currvar);
            }
            else if(format[i+1]=='c')
            {
                char currvar = va_arg(arguments, int);
                buff[0] = currvar;
            } 
            i++;
        }
        else
        {
            buff[0] = format[i];
        }
        for(int k = 0; k < strlen(buff); k++)
        {
            temp_buff[j] = buff[k];
            j++;
        }
    }
    temp_buff[j] = '\0';
    myfwrite(temp_buff, strlen(temp_buff), 1, stream);
    return 1;
}