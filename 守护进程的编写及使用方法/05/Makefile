CC = gcc
RM = -rm -rf
TARGET = test5-1 test5-2 test5-3
OBJS = test5-1.o test5-2.o test5-3.o

all : $(TARGET)

test5-1 : test5-1.o
	$(CC) -o $@ $^

test5-1.o :

test5-2 : test5-2.o
	$(CC) -o $@ $^

test5-2.o :

test5-3 : test5-3.o
	$(CC) -o $@ $^

test5-3.o :


.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)