CC = gcc
RM = -rm -rf
TARGET = test3-1 test3-2
OBJS = test3-1.o test3-2.o

all : $(TARGET)

test3-1 : test3-1.o
	$(CC) -o $@ $^

test3-2 : test3-2.o
	$(CC) -o $@ $^

test3-2.o :

test3-2.o :

.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)