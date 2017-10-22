#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "generated/dncfg_token_fsm.h"

enum TOKEN_TYPE {
    TOKEN_TYPE_NAME,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_EQ,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_REF,
    TOKEN_TYPE_SPACE,
    TOKEN_TYPE_NEWLINE,
    TOKEN_TYPE_LINECOUNTER
};

struct token_t {
    TOKEN_TYPE type;
    std::string value;
    size_t line;
};

const char* to_string(TOKEN_TYPE type);

std::ostream& operator<<(std::ostream& str, const token_t& t);

class TokenStream {
public:
    TokenStream(std::istream& str);

    friend TokenStream& operator>>(TokenStream& str, token_t& out);

    operator bool() const;

    ~TokenStream();

private:
    struct pImpl;
    std::unique_ptr<pImpl> pimpl;
};
