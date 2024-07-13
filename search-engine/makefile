PARSER=parser
DEBUG=debug_server
cc=g++

.PHONY:all
all:$(PARSER) $(DEBUG)

$(PARSER):./src/parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11 -g
$(DEBUG):./src/debug.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -ljsoncpp -std=c++11 -g
.PHONY:clean
clean:
	rm -f $(PARSER) $(DEBUG)