CC = gcc
RM = -rm -rf
OBJS = test.o
TARGET = test
FLAG = --static

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(FLAG) -o $@ $^

$(OBJS) : %.o : %.c
	$(CC) -c $< -o $@

.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)