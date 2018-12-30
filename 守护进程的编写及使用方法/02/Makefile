CC = gcc
RM = -rm -rf
TARGET = test2
OBJS = test2.o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $^

test2.o :


.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)