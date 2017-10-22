SRC=$(shell find -name '*.cpp' -or -name '*.c')
INCLUDE=-I.
CXXFLAGS=$(INCLUDE) -Wall -Wextra

all:
	cfsmgen ./token_parser/cicfg_token_parser.fsm -o token_parser/generated
	cfsmgen ./node_parser/node_parser.fsm -o node_parser/generated
	g++ $(CXXFLAGS) $(SRC)
