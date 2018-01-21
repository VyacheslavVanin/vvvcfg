#include "vvvcfg.hpp"
#include <sstream>

#include "cfg_node.hpp"
#include "token_parser/token_parser.hpp"
#include "vvvcfg/generated/dncfg_node_fsm.h"

enum VALUE_DST { VALUE_DST_NONE, VALUE_DST_NODE, VALUE_DST_PROP };

struct dncfg_node_data_t {
    dncfg_node_data_t(TokenStream& stream)
        : last_lvl(0), tab_width(0), line_number(1),
          root("root"), node_stack{&root}, value(), value_stack(),
          last_prop_name(), stream(&stream), value_dst(VALUE_DST_NONE)
    {
    }

    token_t input;

    size_t last_lvl;
    size_t tab_width;
    size_t line_number;
    vvv::CfgNode root;
    std::vector<vvv::CfgNode*> node_stack{&root};
    vvv::CfgNode::value_type value;
    std::vector<vvv::CfgNode::value_type*> value_stack;
    std::string last_prop_name;
    TokenStream* stream;
    VALUE_DST value_dst;
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
    auto& stack = data->node_stack;

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
    data->node_stack.resize(1);
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
    auto& stack = data->node_stack;
    const auto lvl = data->last_lvl;
    const auto& name = data->input.value;
    auto& newNode = stack[lvl]->addChild(name);
    stack.push_back(&newNode);
}

void add_prop_name(dncfg_node_data_t* data)
{
    auto& stack = data->node_stack;
    const auto& prop_name = data->input.value;
    stack.back()->setProperty(prop_name, "");
    data->last_prop_name = prop_name;
}

void add_ref(dncfg_node_data_t* data)
{
    const auto& name = data->input.value;
    const auto& ref_node = data->root.getChild(name);
    auto& current_node = data->node_stack.back();
    current_node->copyProperties(ref_node);
    current_node->copyChildren(ref_node);
    current_node->copyValue(ref_node);
}

void add_prop_value_str(dncfg_node_data_t* data)
{
    auto& current_node = data->node_stack.back();
    const auto& value = data->input.value;
    const auto& name = data->last_prop_name;
    current_node->appendToStringProperty(name, value);
}

void add_prop_value_number(dncfg_node_data_t* data)
{
    add_prop_value_str(data);
}

void add_prop_value_ref(dncfg_node_data_t* data)
{
    const auto& name = data->input.value;
    const auto& ref_node = data->root.getChild(name);
    auto& current_node = data->node_stack.back();
    const auto& prop_name = data->last_prop_name;
    current_node->setProperty(prop_name, ref_node.getValue());
}

void add_value_str(dncfg_node_data_t* data)
{
    auto& current_node = data->node_stack.back();
    const auto& value = data->input.value;
    current_node->appendToValue(value);
}

void add_value_number(dncfg_node_data_t* data) { add_value_str(data); }

void append_to_list(dncfg_node_data_t* data)
{
    using Value = vvv::CfgNode::value_type;
    auto& value_stack = data->value_stack;
    auto* last_value = value_stack.back();
    auto& last_value_list = last_value->asList();
    last_value_list.push_back(Value(data->input.value));
}

void append_ref_to_list(dncfg_node_data_t* data)
{
    using Value = vvv::CfgNode::value_type;
    const auto& name = data->input.value;
    const auto& ref_node = data->root.getChild(name);
    auto& value_stack = data->value_stack;
    auto* last_value = value_stack.back();
    auto& last_value_list = last_value->asList();
    last_value_list.push_back(ref_node.getValue());
}

void start_list(dncfg_node_data_t* data)
{
    data->value = vvv::Value(vvv::Value::DATA_TYPE::LIST);
    data->value_stack.push_back(&data->value);
}

void pop_value_stack(dncfg_node_data_t* data) { data->value_stack.pop_back(); }

namespace {
void push_subnode_to_list(dncfg_node_data_t* data, vvv::Value::DATA_TYPE type)
{
    using Value = vvv::CfgNode::value_type;
    auto& value_stack = data->value_stack;
    auto* last_value = value_stack.back();
    auto& last_value_list = last_value->asList();
    last_value_list.push_back(Value(type));
    Value* new_last = &last_value_list.back();
    value_stack.push_back(new_last);
}
} // namespace

void push_list_to_value_stack(dncfg_node_data_t* data)
{
    push_subnode_to_list(data, vvv::Value::DATA_TYPE::LIST);
}

void push_dict_to_value_stack(dncfg_node_data_t* data)
{
    push_subnode_to_list(data, vvv::Value::DATA_TYPE::DICT);
}

void append_to_last_in_prop_list(dncfg_node_data_t* data)
{
    auto& current_node = data->node_stack.back();
    const auto& name = data->last_prop_name;
    const auto& value = data->input.value;
    current_node->appendToLastListProperty(name, value);
}

void append_to_last_in_list(dncfg_node_data_t* data)
{
    const auto& value = data->input.value;
    data->value_stack.back()->asList().back().asString() += value;
}

void node_putback(dncfg_node_data_t* data)
{
    data->stream->putback(data->input);
}

void set_value_dst_node(dncfg_node_data_t* data)
{
    data->value_dst = VALUE_DST_NODE;
}

void set_value_dst_prop(dncfg_node_data_t* data)
{
    data->value_dst = VALUE_DST_PROP;
}

void commit_value(dncfg_node_data_t* data)
{
    auto& current_node = data->node_stack.back();
    switch (data->value_dst) {
    case VALUE_DST_NODE: current_node->setValue(data->value); break;
    case VALUE_DST_PROP:
        current_node->setProperty(data->last_prop_name, data->value);
        break;
    default: throw std::logic_error("Shouldn't be here");
    }
    data->value_stack.clear();
}

void dict_set_string_value(dncfg_node_data_t* data)
{
    data->value_stack.back()->asString() = data->input.value;
}

void dict_set_dict_value(dncfg_node_data_t* data)
{
    using Value = vvv::CfgNode::value_type;
    *data->value_stack.back() = Value(Value::DATA_TYPE::DICT);
}

void dict_set_list_value(dncfg_node_data_t* data)
{
    using Value = vvv::CfgNode::value_type;
    *data->value_stack.back() = Value(Value::DATA_TYPE::LIST);
}

void append_string_to_dict_value(dncfg_node_data_t* data)
{
    data->value_stack.back()->asString() += data->input.value;
}

void dict_set_key(dncfg_node_data_t* data)
{
    const auto& key = data->input.value;
    auto& dict = data->value_stack.back()->asDict();
    auto inserted = dict.emplace(key, vvv::Value());
    data->value_stack.push_back(&inserted.first->second);
}

void start_dict(dncfg_node_data_t* data)
{
    data->value = vvv::Value(vvv::Value::DATA_TYPE::DICT);
    data->value_stack.push_back(&data->value);
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

int dncfg_node_is_open_square_br(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_OPEN_SQUARE_BR;
}

namespace {
int is_close_square_br(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_CLOSE_SQUARE_BR;
}

int dncfg_node_is_close_square_br_lvl_e0(const dncfg_node_data_t* data)
{
    return is_close_square_br(data) && data->value_stack.size() == 1;
}
} // namespace

int dncfg_node_is_close_square_br_lvl_0_dst_node(const dncfg_node_data_t* data)
{
    return dncfg_node_is_close_square_br_lvl_e0(data) &&
           data->value_dst == VALUE_DST_NODE;
}

int dncfg_node_is_close_square_br_lvl_0_dst_prop(const dncfg_node_data_t* data)
{
    return dncfg_node_is_close_square_br_lvl_e0(data) &&
           data->value_dst == VALUE_DST_PROP;
}

int dncfg_node_is_close_square_br_lvl_ne0(const dncfg_node_data_t* data)
{
    return is_close_square_br(data) && data->value_stack.size() > 1;
}

namespace {
int is_close_curly_br(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_CLOSE_CURLY_BR;
}

int has_prev_value_lvl(const dncfg_node_data_t* data)
{
    return data->value_stack.size() > 1;
}

const vvv::Value& get_prev_value_lvl(const dncfg_node_data_t* data)
{
    return *(*(data->value_stack.end() - 2));
}

int is_prev_lvl_dict(const dncfg_node_data_t* data)
{
    return has_prev_value_lvl(data) && get_prev_value_lvl(data).isDict();
}

int is_prev_lvl_list(const dncfg_node_data_t* data)
{
    return has_prev_value_lvl(data) && get_prev_value_lvl(data).isList();
}

int is_dst_node(const dncfg_node_data_t* data)
{
    return data->value_dst == VALUE_DST_NODE;
}

int is_dst_prop(const dncfg_node_data_t* data)
{
    return data->value_dst == VALUE_DST_PROP;
}
} // namespace

int dncfg_node_is_close_curly_br_lvl_0_dst_node(const dncfg_node_data_t* data)
{
    return is_close_curly_br(data) && !has_prev_value_lvl(data) &&
           is_dst_node(data);
}

int dncfg_node_is_close_curly_br_lvl_0_dst_prop(const dncfg_node_data_t* data)
{
    return is_close_curly_br(data) && !has_prev_value_lvl(data) &&
           is_dst_prop(data);
}

int dncfg_node_is_close_curly_br_prev_lvl_dict(const dncfg_node_data_t* data)
{
    return is_close_curly_br(data) && is_prev_lvl_dict(data);
}

int dncfg_node_is_close_curly_br_prev_lvl_list(const dncfg_node_data_t* data)
{
    return is_close_curly_br(data) && is_prev_lvl_list(data);
}

int dncfg_node_is_open_curly_br(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_OPEN_CURLY_BR;
}

int dncfg_node_is_colon(const dncfg_node_data_t* data)
{
    return data->input.type == TOKEN_TYPE_COLON;
}

int dncfg_node_is_close_square_br_prev_lvl_list(const dncfg_node_data_t* data)
{
    return is_close_square_br(data) && is_prev_lvl_list(data);
}

int dncfg_node_is_close_square_br_prev_lvl_dict(const dncfg_node_data_t* data)
{
    return is_close_square_br(data) && is_prev_lvl_dict(data);
}

namespace vvv {
CfgNode make_cfg(std::istream& input)
{
    TokenStream ts(input);

    dncfg_node_data_t data(ts);
    dncfg_node_ctx_t ctx{DNCFG_NODE_START, &data};

    while (ts >> data.input)
        dncfg_node_step(&ctx);
    return std::move(data.root);
}

CfgNode make_cfg(const std::string& input)
{
    std::stringstream str(input);
    return make_cfg(str);
}

} // namespace vvv
