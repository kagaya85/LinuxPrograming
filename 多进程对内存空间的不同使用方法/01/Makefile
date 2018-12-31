COMMON_PATH= ../common/
COMMON_INCLUDE= common.h 
COMMON_CPP= common.cpp 
COMMON_LIBOUT= common.o
COMMON_CPPOUT= commoncpp.o

SOURCE = $(wildcard *.cpp)
TARGETS = $(patsubst %.cpp, %, $(SOURCE)) 

CC = g++

all:$(TARGETS)

$(TARGETS):%:%.cpp $(COMMON_PATH)$(COMMON_INCLUDE) $(COMMON_PATH)$(COMMON_CPP) 
	$(CC) $< $(COMMON_PATH)$(COMMON_INCLUDE) $(COMMON_PATH)$(COMMON_CPP) -o $@ -w
#	$(CC) $< -o $@
.PHONY:clean all
clean:    
	-rm $(TARGETS) *.o level_to_level.share
 
check:
	chmod +x packet_analyze
	./packet_analyze < network.dat

runsend:
	./send-level-5 &
	./send-level-4 &
	./send-level-3 &
	./send-level-2 &
	./send-level-1 &
runrecv:
	./recv-level-1 &
	./recv-level-2 &
	./recv-level-3 &
	./recv-level-4 &
	./recv-level-5 &
run:
	-rm *.send *.dat
	chmod +x sender.sh
	chmod +x receiver.sh
	./sender.sh
	./receiver.sh
