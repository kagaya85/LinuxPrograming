CC = gcc
TARGETS = tcp_server4-1 tcp_client4-1 tcp_server4-2 tcp_client4-2
RM = -rm -f

all : $(TARGETS)

tcp_server4-1 : server4-1.o
	$(CC) -o $@ $^

tcp_client4-1 : client4-1.o
	$(CC) -o $@ $^

tcp_server4-2 : server4-2.o
	$(CC) -o $@ $^

tcp_client4-2 : client4-2.o
	$(CC) -o $@ $^

server4-1.o : 

client4-1.o : 

server4-2.o : 

client4-2.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
