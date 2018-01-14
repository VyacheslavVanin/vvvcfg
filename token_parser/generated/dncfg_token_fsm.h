/*
 *  This file was generated by cfsmgen script.
 *  See www.github.com/VyacheslavVanin/cfmgen.git 
 */

#ifndef DNCFG_TOKEN_FSM_H
#define DNCFG_TOKEN_FSM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DNCFG_TOKEN_STATE {
    DNCFG_TOKEN_BEGIN,
    DNCFG_TOKEN_BEGIN_SPACE,
    DNCFG_TOKEN_NAME,
    DNCFG_TOKEN_STRING,
    DNCFG_TOKEN_NUM,
    DNCFG_TOKEN_REF_START,
    DNCFG_TOKEN_SPACE,
    DNCFG_TOKEN_NEXTLINE,
    DNCFG_TOKEN_COMMENT,
    DNCFG_TOKEN_ERROR,
    DNCFG_TOKEN_REF,
    DNCFG_TOKEN_STRING_ESC,
    DNCFG_TOKEN_NUM_DOT,
    DNCFG_TOKEN_NUM_DOT_NUM
} DNCFG_TOKEN_STATE;
#define DNCFG_TOKEN_STATE_count 14

extern const char* dncfg_token_state_names[14];


typedef struct dncfg_token_data_t dncfg_token_data_t;

typedef struct dncfg_token_ctx_t dncfg_token_ctx_t;

struct dncfg_token_ctx_t {
    DNCFG_TOKEN_STATE state;
    dncfg_token_data_t* data;
};

void start_space_token(dncfg_token_data_t* data);
void append_symbol(dncfg_token_data_t* data);
void inc_line_number(dncfg_token_data_t* data);
void line_count_token(dncfg_token_data_t* data);
void commit(dncfg_token_data_t* data);
void start_name_token(dncfg_token_data_t* data);
void start_string_token(dncfg_token_data_t* data);
void start_number_token(dncfg_token_data_t* data);
void start_ref_token(dncfg_token_data_t* data);
void start_comma(dncfg_token_data_t* data);
void start_eq(dncfg_token_data_t* data);
void putback(dncfg_token_data_t* data);
void on_error(dncfg_token_data_t* data);
void unexpected_symbol(dncfg_token_data_t* data);
void start_newline_token(dncfg_token_data_t* data);
void start_open_square_br(dncfg_token_data_t* data);
void start_close_square_br(dncfg_token_data_t* data);
void append_escape_symbol(dncfg_token_data_t* data);
int dncfg_token_is_space(const dncfg_token_data_t* data);
int dncfg_token_is_eol(const dncfg_token_data_t* data);
int dncfg_token_is_alpha(const dncfg_token_data_t* data);
int dncfg_token_is_quote(const dncfg_token_data_t* data);
int dncfg_token_is_number(const dncfg_token_data_t* data);
int dncfg_token_is_dollar(const dncfg_token_data_t* data);
int dncfg_token_is_comma(const dncfg_token_data_t* data);
int dncfg_token_is_eq(const dncfg_token_data_t* data);
int dncfg_token_is_backslash(const dncfg_token_data_t* data);
int dncfg_token_is_sharp(const dncfg_token_data_t* data);
int dncfg_token_is_close_square_br(const dncfg_token_data_t* data);
int dncfg_token_is_open_square_br(const dncfg_token_data_t* data);
int dncfg_token_is_dot(const dncfg_token_data_t* data);
int dncfg_token_is_escape(const dncfg_token_data_t* data);


void dncfg_token_step(dncfg_token_ctx_t* ctx);

#ifdef __cplusplus
}
#endif

#endif
