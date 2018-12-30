SOURCE = $(wildcard *.c)
TARGETS = $(patsubst %.c, %, $(SOURCE)) 
CC = gcc
all:$(TARGETS) 
$(TARGETS):%:%.c    
	$(CC) $< -o $@ 
	
.PHONY:clean all
clean:    
	-rm $(TARGETS)
