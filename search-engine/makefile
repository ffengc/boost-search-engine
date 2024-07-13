PARSER=parser
DEBUG=debug_server
SERVER=server
cc=g++

.PHONY:all
all:$(PARSER) $(DEBUG) $(SERVER)

$(PARSER):./src/parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11 -g
$(DEBUG):./src/debug.cc
	$(cc) -o $@ $^ -ljsoncpp -std=c++11 -g
$(SERVER): ./src/server.cc
	$(cc) -o $@ $^ -ljsoncpp -lpthread -std=c++11 -g
.PHONY:clean
clean:
	rm -f $(PARSER) $(DEBUG) $(SERVER)