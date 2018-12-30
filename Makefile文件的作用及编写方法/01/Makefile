objects = test1.o test2.o test3.o
RM = -rm -rf

all : $(objects)
	gcc -o test $(objects)

$(objects) : 

.PHONY : clean
clean : 
	$(RM) test $(objects)