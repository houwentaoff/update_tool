default:all

all:fimerge
CFG_LIB=-Wl,-Bdynamic -lpthread -lrt -ldl -Wl,-Bstatic -lomniORB4 \
	-lomnithread -lomniDynamic4 -Wl,-Bdynamic
INC=-I../comm -I../FiUpdateLoader

fimerge:utility.o patch.o main.o Md5.o Markup_linux.o QueryUpdateSK.o FiUpdateAssistant.o SAXParserHandler.o 
	g++ $^ -o $@ ${CFG_LIB}
%.o:%.cpp
	g++ -c -fpermissive -g $^ -o $@ ${INC}
%.o: ../FiUpdateLoader/%.cpp
	g++ -c -fpermissive -g $^ -o $@ ${INC}
%.o: ../idl/%.cc
	g++ -c -fpermissive -g $^ -o $@ ${INC}	
%.o:../comm/%.cpp
	g++ -c -fpermissive -g $^ -o $@ ${INC}

.PHONY:clean
clean:
	rm *.o fimerge  -rf
