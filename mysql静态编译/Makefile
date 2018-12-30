CC = g++
TARGET = mysql_demo
SRCS = mysql_demo.cpp
OBJS = mysql_demo.o
RM = -rm -f

CFLAGS = -I/usr/include/mysql
CLIBS = -L. -lmysqlclient -L/usr/lib64/ -lz -lpthread -ldl

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -static -o $@ $^ $(CFLAGS) $(CLIBS) 

$(OBJS) : $(SRCS)
	$(CC) -c $^ -static $(CFLAGS) $(CLIBS)


.PHONY : clean
clean : 
	$(RM) $(TARGET) $(OBJS)