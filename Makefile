cc=g++
server=ChatServer
client=ChatClient
INCLUDE=-I./lib/jsoncpp-src-0.5.0/include
LIB_PATH=-L./lib/jsoncpp-src-0.5.0/libs/linux-gcc-4.8.5
LDFLAGS=-std=c++11 -lpthread -ljson_linux-gcc-4.8.5_libmt

.PHONY:all
all:$(server) $(client)

$(server):ChatServer.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS) -static

$(client):ChatClient.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS) -lncurses

.PHONY:clean
clean:
	rm -f $(server) $(client)
