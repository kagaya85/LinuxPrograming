CC = gcc
TARGETS = tcp_server4-1 tcp_client4-1-1 tcp_client4-1-2 tcp_server4-2 tcp_client4-2-1 tcp_client4-2-2
RM = -rm -f

all : $(TARGETS)

tcp_server4-1 : server1.o
	$(CC) -o $@ $^

tcp_server4-2 : server2.o
	$(CC) -o $@ $^

tcp_client4-1-1 : client1-1.o
	$(CC) -o $@ $^

tcp_client4-1-2 : client1-2.o
	$(CC) -o $@ $^

tcp_client4-2-1 : client2-1.o
	$(CC) -o $@ $^

tcp_client4-2-2 : client2-2.o
	$(CC) -o $@ $^

server1.o : server1.c
	$(CC) -c $^ -o $@

server2.o : server2.c
	$(CC) -c $^ -o $@

client1-1.o : 

client1-2.o : 

client2-1.o : 

client2-2.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
