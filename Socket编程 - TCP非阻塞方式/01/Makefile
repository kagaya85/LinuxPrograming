CC = gcc
TARGETS = tcp_server1-1 tcp_client1-1 tcp_server1-2 tcp_client1-2 tcp_server1-3 tcp_client1-3
RM = -rm -f

all : $(TARGETS)

tcp_server1-1 : server1-1.o
	$(CC) -o $@ $^

tcp_client1-1 : client1-1.o
	$(CC) -o $@ $^

tcp_server1-2 : server1-2.o
	$(CC) -o $@ $^

tcp_client1-2 : client1-2.o
	$(CC) -o $@ $^

tcp_server1-3 : server1-3.o
	$(CC) -o $@ $^

tcp_client1-3 : client1-3.o
	$(CC) -o $@ $^

server1-1.o : 

client1-1.o : 

server1-2.o : 

client1-2.o : 

server1-3.o : 

client1-3.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
