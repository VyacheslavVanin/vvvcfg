#include <string>
#include <vector>
#include <iostream>

#include "token_parser/dncfg_token_fsm.h"

enum TOKEN_TYPE {
    TOKEN_TYPE_NAME,
    TOKEN_TYPE_VALUE,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_EQ,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_REF
};

struct token_t {
    TOKEN_TYPE type;
    std::string value;
};

const char* to_string(TOKEN_TYPE type);

std::ostream& operator<<(std::ostream& str, const token_t& t);

struct parse_token_result {
    std::vector<token_t> tokens;
    std::string error_message;
    enum class STATUS { OK, ERROR, NEXT_LINE };
    STATUS status = STATUS::OK;
};
parse_token_result line_to_tokens(const std::string& input);

