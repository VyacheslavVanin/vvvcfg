#! /bin/bash
# Use cfsmgen to generate sources (https://github.com/VyacheslavVanin/cfsmgen)
cfsmgen ./token_parser/cicfg_token_parser.fsm -o token_parser/generated
cfsmgen ./vvvcfg/node_parser.fsm -o vvvcfg/generated
