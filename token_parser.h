#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "token_parser/dncfg_token_fsm.h"

enum TOKEN_TYPE {
    TOKEN_TYPE_NAME,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_EQ,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_REF,
    TOKEN_TYPE_SPACE
};

struct token_t {
    TOKEN_TYPE type;
    std::string value;
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

