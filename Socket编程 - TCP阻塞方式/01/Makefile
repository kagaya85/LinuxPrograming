CC = gcc
TARGETS = tcp_server1 tcp_client1
SRCS = server.c client.c
OBJS = server.o client.o
RM = -rm -f

all : $(TARGETS)

tcp_server1 : server.o
	$(CC) -o $@ $^

tcp_client1 : client.o
	$(CC) -o $@ $^

server.o : server.c
	$(CC) -c $^ -o $@

client.o : client.c 
	$(CC) -c $^ -o $@

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
