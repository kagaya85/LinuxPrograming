CC = gcc
TARGETS = tcp_server3-1 tcp_client3-1 tcp_server3-2 tcp_client3-2
RM = -rm -f

all : $(TARGETS)

tcp_server3-1 : server3-1.o
	$(CC) -o $@ $^

tcp_client3-1 : client3-1.o
	$(CC) -o $@ $^

tcp_server3-2 : server3-2.o
	$(CC) -o $@ $^

tcp_client3-2 : client3-2.o
	$(CC) -o $@ $^

server3-1.o : 

client3-1.o : 

server3-2.o : 

client3-2.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
