#! /bin/bash
# Use cfsmgen to generate sources (https://github.com/VyacheslavVanin/cfsmgen)
cfsmgen ./token_parser/cicfg_token_parser.fsm -o token_parser/generated
cfsmgen ./node_parser/node_parser.fsm -o node_parser/generated
