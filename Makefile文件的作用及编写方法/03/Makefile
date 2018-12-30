OBJS = test1.o test2.o test3.o
TARGET = test1 test2 test3
CC = gcc
RM = -rm -rf

all : $(TARGET)

test1 : test1.o
	$(CC) -o test1 test1.o 
test2 : test2.o
	$(CC) -o test2 test2.o
test3 : test3.o
	$(CC) -o test3 test3.o

.PHONY : clean
clean : 
	$(RM) $(OBJS) $(TARGET)