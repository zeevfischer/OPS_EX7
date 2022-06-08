CC=gcc
FLAGS= -Wall -g 

OBJECTS_FS=fs.o
OBJECTS_TEST=test.o 

all: test
test:$(OBJECTS_TEST) $(OBJECTS_FS)
	$(CC) $(FLAGS) -o test $(OBJECTS_TEST) $(OBJECTS_FS)

# o files 
$(OBJECTS_FS): fs.c fs.h
	$(CC) $(FLAGS) -c fs.c

$(OBJECTS_TEST):test.c fs.h 
	$(CC) $(FLAGS) -c test.c


.PHONY: clean all
clean:
	rm -f *.o  *.a *.so test
