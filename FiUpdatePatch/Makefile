default:all

all:fipatch

INC=-I../comm 
fipatch:utility.o main.o Md5.o Markup_linux.o
	g++ $^ -o $@ 
%.o:%.cpp
	g++ -c -g $^ -o $@ ${INC}

%.o:../comm/%.cpp
	g++ -c -g $^ -o $@ ${INC}

.PHONY:clean
clean:
	rm *.o fipatch  -rf
