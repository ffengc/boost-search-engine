cc=g++

parser: ./src/parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11 -ljsoncpp -g
.PHONY:clean
clean:
	rm -f parser