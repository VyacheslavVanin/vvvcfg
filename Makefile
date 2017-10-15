SRC=$(shell find -name '*.cpp' -or -name '*.c')

all:
	cfsmgen ./cicfg_token_parser.fsm -o token_parser
	cfsmgen ./node_parser.fsm -o node_parser
	g++ $(SRC)
