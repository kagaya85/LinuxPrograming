RM = -rm -rf
CC = gcc
TARGET = test2
OBJS = test2.o

all : $(TARGET)


$(TARGET) : $(OBJS) libtest1.so
	$(CC) -o $@ $< -L. -ltest1

$(OBJS) : test2.c
	$(CC) -c $< -o $@ 

libtest1.so : test1.c
	$(CC) -fPIC -shared $< -o $@ 

.PHONY: clean
clean : 
	$(RM) $(TARGET) $(OBJS) libtest1.so