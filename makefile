CC=gcc
FLAGS= -Wall -g 

OBJECTS_FS=fs.o
OBJECTS_STDIO=mystdio.o
OBJECTS_TEST=test.o 

all: test libmylibc.so libmyfs.so
test: $(OBJECTS_TEST) libmylibc.so libmyfs.so
	$(CC) $(OBJECTS_TEST) ./libmylibc.so ./libmyfs.so -o test -fPIC

libmylibc.so: $(OBJECTS_FS) $(OBJECTS_STDIO)
	$(CC) $(FLAGS) -shared -fPIC $(OBJECTS_FS) $(OBJECTS_STDIO) -o libmylibc.so

libmyfs.so: $(OBJECTS_FS)
	$(CC) $(FLAGS) -shared -fPIC $(OBJECTS_FS) -o libmyfs.so

# o files 
$(OBJECTS_FS): fs.c fs.h
	$(CC) $(FLAGS) -c fs.c -fPIC

$(OBJECTS_STDIO):mystdio.c mystdio.h
	$(CC) $(FLAGS) -c mystdio.c -fPIC

$(OBJECTS_TEST):test.c fs.h 
	$(CC) $(FLAGS) -c test.c -fPIC


.PHONY: clean all
clean:
	rm -f *.o  *.a *.so test testfiledata.txt
