CC = gcc
RM = -rm -rf
TARGET = test4-1 test4-2
OBJS = test4-1.o test4-2.o

all : $(TARGET)

test4-1 : test4-1.o
	$(CC) -o $@ $^

test4-1.o :

test4-2 : test4-2.o
	$(CC) -o $@ $^

test4-2.o :


.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)