#include "token_parser.h"
#include <iostream>
#include <string>

struct dncfg_token_data_t {
    char current_char;
    std::vector<token_t> tokens;

    std::string error_message;
};

void on_error(dncfg_token_data_t* data) { data->error_message = "Error "; }
void unexpected_symbol(dncfg_token_data_t* data)
{
    // TODO: add escape to print \n, \t, etc.
    data->error_message += "Error: unexpected symbol \'";
    data->error_message += data->current_char;
    data->error_message += "\'";
}

void start_name_token(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_NAME, {}});
}

void start_string_token(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_VALUE, {}});
}

void start_number_token(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_NUMBER, {}});
}

void start_ref_token(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_REF, {}});
}

void commit_comma(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_COMMA, {}});
}

void commit_eq(dncfg_token_data_t* data)
{
    data->tokens.push_back({TOKEN_TYPE_EQ, {}});
}

void append_symbol(dncfg_token_data_t* data)
{
    data->tokens.back().value += data->current_char;
}

static void append(dncfg_token_data_t* data, char ch)
{
    data->tokens.back().value += ch;
}

void append_escape_symbol(dncfg_token_data_t* data)
{
    const char ch = data->current_char;
    switch (ch) {
    case '?':
    case '\'':
    case '\"':
    case '\\': append(data, ch); break;
    case 'a': append(data, '\a'); break;
    case 'b': append(data, '\b'); break;
    case 'f': append(data, '\f'); break;
    case 'n': append(data, '\n'); break;
    case 'r': append(data, '\r'); break;
    case 't': append(data, '\t'); break;
    case 'v': append(data, '\v'); break;
    default: data->error_message = "Error: invalid escape symbol";
    }
}

int dncfg_token_is_alpha(const dncfg_token_data_t* data)
{
    const char ch = data->current_char;
    if (isalpha(ch))
        return true;
    switch (ch) {
    case '_': return true;
    }
    return false;
}

int dncfg_token_is_quote(const dncfg_token_data_t* data)
{
    return data->current_char == '\"';
}

int dncfg_token_is_number(const dncfg_token_data_t* data)
{
    return isdigit(data->current_char);
}

int dncfg_token_is_dollar(const dncfg_token_data_t* data)
{
    return data->current_char == '$';
}

int dncfg_token_is_comma(const dncfg_token_data_t* data)
{
    return data->current_char == ',';
}

int dncfg_token_is_eq(const dncfg_token_data_t* data)
{
    return data->current_char == '=';
}

int dncfg_token_is_backslash(const dncfg_token_data_t* data)
{
    return data->current_char == '\\';
}

int dncfg_token_is_space(const dncfg_token_data_t* data)
{
    return data->current_char == ' ';
}

int dncfg_token_is_eol(const dncfg_token_data_t* data)
{
    return data->current_char == '\n';
}

int dncfg_token_is_escape(const dncfg_token_data_t* data)
{
    switch (data->current_char) {
    case '?':
    case '\'':
    case '\"':
    case '\\':
    case 'a':
    case 'b':
    case 'f':
    case 'n':
    case 'r':
    case 't':
    case 'v': return true;
    }
    return false;
}

int dncfg_token_is_dot(const dncfg_token_data_t* data)
{
    return data->current_char == '.';
}

int dncfg_token_is_sharp(const dncfg_token_data_t* data)
{
    return data->current_char == '#';
}

const char* to_string(TOKEN_TYPE type)
{
    switch (type) {
    case TOKEN_TYPE_NAME: return "NAME";
    case TOKEN_TYPE_VALUE: return "VALUE";
    case TOKEN_TYPE_EQ: return "EQ";
    case TOKEN_TYPE_COMMA: return "COMMA";
    case TOKEN_TYPE_REF: return "REF";
    }
    return "unknown";
}

std::ostream& operator<<(std::ostream& str, const token_t& t)
{
    str << to_string(t.type) << ": " << t.value;
    return str;
}

parse_token_result line_to_tokens(const std::string& input)
{
    dncfg_token_data_t data;
    dncfg_token_ctx_t ctx = {DNCFG_TOKEN_SPACE, &data};

    parse_token_result ret;
    for (auto ch : input) {
        data.current_char = ch;
        dncfg_token_step(&ctx);
        if (!data.error_message.empty()) {
            ret.error_message = std::move(data.error_message);
            ret.status        = parse_token_result::STATUS::ERROR;
            return ret;
        }
    }

    ret.tokens = std::move(data.tokens);
    ret.status = ctx.state == DNCFG_TOKEN_NEXTLINE
                     ? parse_token_result::STATUS::NEXT_LINE
                     : parse_token_result::STATUS::OK;
    return ret;
}

