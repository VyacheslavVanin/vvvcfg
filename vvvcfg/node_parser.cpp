#include "vvvcfg.hpp"

#include "cfg_node.hpp"
#include "vvvcfg/generated/dncfg_node_fsm.h"
#include "token_parser/token_parser.hpp"

struct dncfg_node_data_t {
    dncfg_node_data_t()
        : last_lvl(0), tab_width(0), line_number(1), root("root"), stack{&root}
    {
    }

    token_t input;

    size_t last_lvl;
    size_t tab_width;
    size_t line_number;
    vvv::CfgNode root;
    std::vector<vvv::CfgNode*> stack{&root};
    std::string last_prop_name;
};

namespace {
size_t count_spaces(const std::string& str)
{
    return std::find_if(str.begin(), str.end(),
                        [](const auto& ch) { return ch != ' '; }) -
           str.begin();
}

size_t indent_lvl(size_t spaces, size_t indent_width, size_t line)
{
    if (spaces == 0)
        return 0;
    if (spaces % indent_width)
        throw std::logic_error(
            std::string("Invalid indent on line ") + std::to_string(line) +
            ": " + std::to_string(spaces) +
            " spaces, when tab width = " + std::to_string(indent_width));
    return spaces / indent_width;
}
} // namespace

void setup_lvl(dncfg_node_data_t* data)
{
    auto& tab_width = data->tab_width;
    auto& last_lvl = data->last_lvl;
    auto& stack = data->stack;

    const auto spaces = count_spaces(data->input.value);
    tab_width = tab_width ? tab_width : spaces;
    const auto lvl = indent_lvl(spaces, tab_width, data->line_number);
    if (lvl > last_lvl + 1)
        throw std::logic_error("Overindent on line " +
                               std::to_string(data->line_number));

    last_lvl = lvl;
    stack.resize(lvl + 1);
}

void reset_lvl(dncfg_node_data_t* data)
{
    data->last_lvl = 0;
    data->stack.resize(1);
}

void inc_line_count(dncfg_node_data_t* data) { data->line_number++; }

void on_invalid_token(dncfg_node_data_t* data)
{
    (void)data;
    const token_t& token = data->input;
    const std::string& token_type = to_string(token.type);
    const std::string& line = std::to_string(data->line_number);
    throw std::logic_error(std::string("Invalid token: ") + token_type + " (" +
                           token.value + ") on line " + line);
}

void add_subnode(dncfg_node_data_t* data)
{
    auto& stack = data->stack;
    const auto lvl = data->last_lvl;
    const auto& name = data->input.value;
    auto& newNode = stack[lvl]->addChild(name);
    stack.push_back(&newNode);
}

void add_prop_name(dncfg_node_data_t* data)
{
    auto& stack = data->stack;
    const auto& prop_name = data->input.value;
    stack.back()->setProperty(prop_name, "");
    data->last_prop_name = prop_name;
}

void add_ref(dncfg_node_data_t* data)
{
    const auto& name = data->input.value;
    const auto& ref_node = data->root.getChild(name);
    auto& current_node = data->stack.back();
    current_node->copyProperties(ref_node);
    current_node->copyChildren(ref_node);
    current_node->copyValue(ref_node);
}

void add_prop_value_str(dncfg_node_data_t* data)
{
    auto& current_node = data->stack.back();
    const auto& value = data->input.value;
    const auto& name = data->last_prop_name;
    current_node->appendToPropperty(name, value);
}

void add_prop_value_number(dncfg_node_data_t* data)
{
    add_prop_value_str(data);
}

void add_prop_value_ref(dncfg_node_data_t* data)
{
    const auto& name = data->input.value;
    const auto& ref_node = data->root.getChild(name);
    auto& current_node = data->stack.back();
    const auto& prop_name = data->last_prop_name;
    current_node->appendToPropperty(prop_name, ref_node.getValue());
}

void add_value_str(dncfg_node_data_t* data)
{
    auto& current_node = data->stack.back();
    const auto& value = data->input.value;
    current_node->appendToValue(value);
}

void add_value_number(dncfg_node_data_t* data)
{
    add_value_str(data);
}

int dncfg_node_is_space(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_SPACE;
}

int dncfg_node_is_name(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_NAME;
}

int dncfg_node_is_newline(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_NEWLINE;
}

int dncfg_node_is_ref(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_REF;
}

int dncfg_node_is_eq(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_EQ;
}

int dncfg_node_is_comma(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_COMMA;
}

int dncfg_node_is_string(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_STRING;
}

int dncfg_node_is_number(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_NUMBER;
}

int dncfg_node_is_linecount(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_LINECOUNTER;
}

namespace vvv {
CfgNode make_cfg(std::istream& input)
{
    TokenStream ts(input);

    dncfg_node_data_t data;
    dncfg_node_ctx_t ctx{DNCFG_NODE_START, &data};

    while (ts >> data.input)
        dncfg_node_step(&ctx);
    return std::move(data.root);
}
}
