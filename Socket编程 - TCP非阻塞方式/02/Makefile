CC = gcc
TARGETS = tcp_server2-1 tcp_client2-1 tcp_server2-2 tcp_client2-2
RM = -rm -f

all : $(TARGETS)

tcp_server2-1 : server2-1.o
	$(CC) -o $@ $^

tcp_client2-1 : client2-1.o
	$(CC) -o $@ $^

tcp_server2-2 : server2-2.o
	$(CC) -o $@ $^

tcp_client2-2 : client2-2.o
	$(CC) -o $@ $^

server2-1.o : 

client2-1.o : 

server2-2.o : 

client2-2.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
