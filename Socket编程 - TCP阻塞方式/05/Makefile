CC = gcc
TARGETS = tcp_server5-1 tcp_client5-1 tcp_client5-2 tcp_server5-2 tcp_server5-3 tcp_client5-3
RM = -rm -f

all : $(TARGETS)

tcp_server5-1 : server5-1.o
	$(CC) -o $@ $^

tcp_server5-2 : server5-2.o
	$(CC) -o $@ $^

tcp_server5-3 : server5-3.o
	$(CC) -o $@ $^

tcp_client5-1 : client5-1.o
	$(CC) -o $@ $^

tcp_client5-2 : client5-2.o
	$(CC) -o $@ $^

tcp_client5-3 : client5-3.o
	$(CC) -o $@ $^

server5-1.o : 

server5-2.o : 

server5-3.o : 

client5-1.o : 

client5-2.o : 

client5-3.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
