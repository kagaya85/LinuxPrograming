CC = gcc
TARGETS = tcp_server5 tcp_client5-1 tcp_client5-2
RM = -rm -f

all : $(TARGETS)

tcp_server5 : server5.o
	$(CC) -o $@ $^

tcp_client5-1 : client5-1.o
	$(CC) -o $@ $^

tcp_client5-2 : client5-2.o
	$(CC) -o $@ $^

server5.o : 

client5-1.o : 

client5-2.o : 

.PHONY : clean
clean:
	$(RM) $(TARGETS) *.o 
