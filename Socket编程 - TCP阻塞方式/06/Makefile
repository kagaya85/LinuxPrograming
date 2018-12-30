CC = gcc
TARGETS = tcp_server6-1 tcp_server6-2 tcp_server6-3 tcp_server6-4 \
tcp_client6-1 tcp_client6-2 tcp_client6-3 tcp_client6-4
RM = -rm -f

all : $(TARGETS)

tcp_server6-1 : server6-1.o
	$(CC) -o $@ $^

tcp_server6-2 : server6-2.o
	$(CC) -o $@ $^

tcp_server6-3 : server6-3.o
	$(CC) -o $@ $^

tcp_server6-4 : server6-4.o
	$(CC) -o $@ $^

tcp_client6-1 : client6-1.o
	$(CC) -o $@ $^

tcp_client6-2 : client6-2.o
	$(CC) -o $@ $^

tcp_client6-3 : client6-3.o
	$(CC) -o $@ $^

tcp_client6-4 : client6-4.o
	$(CC) -o $@ $^

server6-1.o : 

server6-2.o : 

server6-3.o : 

server6-4.o : 

client6-1.o : 

client6-2.o : 

client6-3.o : 

client6-4.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
