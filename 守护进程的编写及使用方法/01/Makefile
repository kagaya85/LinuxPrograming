CC = gcc
RM = -rm -rf
TARGET = test1-1 test1-2
OBJS = test1-1.o test1-2.o

all : $(TARGET)

test1-1 : test1-1.o
	$(CC) -o $@ $^

test1-2 : test1-2.o
	$(CC) -o $@ $^

test1-1.o :

test1-2.o :


.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)