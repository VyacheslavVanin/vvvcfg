#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include <vvvcfg/vvvcfg.hpp>

GTEST_TEST(vvvcfg, value)
{
    using Value = vvv::CfgNode::value_type;
    Value v(Value::DATA_TYPE::DICT);
    auto& dict = v.asDict();
    dict["key1"] = Value("value1");
    dict["key2"] = Value("value2");
    dict["key3"] = std::string("value3");
    dict["key4"] = std::vector<std::string>{"111", "222"};
    dict["key5"] = std::unordered_map<std::string, std::string>{{"a", "aaa"},
                                                                {"b", "bbb"}};
}

GTEST_TEST(vvvcfg, plain)
{
    const auto input = R"(
#sadsadsa
#Hello
#sadsadsa

message= "Hello World"
list= []
list2= ["Hello", "List"]
list3= $list2
integer_number= 42
float_number = 3.1415
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    EXPECT_EQ(root.getNumChildren(), 6);

    const auto& message_value = root.getChild("message").getValue();
    EXPECT_TRUE(message_value.isString());
    EXPECT_FALSE(message_value.isList());
    EXPECT_ANY_THROW(message_value.asStringList());
    const auto& message = message_value.asString();
    EXPECT_EQ(message, "Hello World");

    const auto& list_value = root.getChild("list").getValue();
    EXPECT_FALSE(list_value.isString());
    EXPECT_TRUE(list_value.isList());
    EXPECT_ANY_THROW(list_value.asString());
    const auto& list = list_value.asStringList();
    EXPECT_EQ(list, std::vector<std::string>());

    const auto& list2 = root.getChild(2).getValue().asStringList();
    const auto& expected_list2 = std::vector<std::string>{"Hello", "List"};
    EXPECT_EQ(list2, expected_list2);

    const auto& list3_value = root.getChild("list3").getValue();
    EXPECT_FALSE(list3_value.isString());
    EXPECT_TRUE(list3_value.isList());
    EXPECT_ANY_THROW(list3_value.asString());
    const auto& list3 = list3_value.asStringList();
    EXPECT_EQ(list3, list2);

    const auto& integer = root.getChild("integer_number").getValue();
    EXPECT_TRUE(integer.isString());
    EXPECT_FALSE(integer.isList());
    const auto& int_val = integer.asString();
    EXPECT_EQ(int_val, "42");

    const auto& floating = root.getChild("float_number").getValue();
    EXPECT_TRUE(floating.isString());
    EXPECT_FALSE(floating.isList());
    const auto& float_val = floating.asString();
    EXPECT_EQ(float_val, "3.1415");
}

GTEST_TEST(vvvcfg, node_value_nested_list_simple)
{
    const auto input = R"(
node1 = [[]]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& list = node1.getValue().asList();
    EXPECT_EQ(list.size(), 1);
    EXPECT_TRUE(list[0].isList());
    const auto& list_list = list[0].asList();
    EXPECT_TRUE(list_list.empty());
}

GTEST_TEST(vvvcfg, node_value_nested_list)
{
    const auto input = R"(
node1 = [["text"], [42, 3.14]]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& list = node1.getValue().asList();
    EXPECT_EQ(list.size(), 2);
    EXPECT_TRUE(list[0].isList());
    const auto& list_list1 = list[0].asList();
    EXPECT_EQ(list_list1.size(), 1);
    EXPECT_EQ(list_list1[0].asString(), "text");

    const auto& list_list2 = list[1].asList();
    EXPECT_EQ(list_list2.size(), 2);
    EXPECT_EQ(list_list2[0].asString(), "42");
    EXPECT_EQ(list_list2[1].asString(), "3.14");
}

GTEST_TEST(vvvcfg, node_value_nested_list_complex)
{
    const auto input = R"(
node1 = [["text"], [42, [3.14], [42, "123"], []]]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& list = node1.getValue().asList();
    EXPECT_EQ(list.size(), 2);
    EXPECT_TRUE(list[0].isList());
    const auto& list_list1 = list[0].asList();
    EXPECT_EQ(list_list1.size(), 1);
    EXPECT_EQ(list_list1[0].asString(), "text");

    const auto& list_list2 = list[1].asList();
    EXPECT_EQ(list_list2.size(), 4);
    EXPECT_EQ(list_list2[0].asString(), "42");
    EXPECT_EQ(list_list2[1].asStringList(),
              vvv::CfgNode::value_list_type{"3.14"});
    const auto& expected = vvv::CfgNode::value_list_type{"42", "123"};
    EXPECT_EQ(list_list2[2].asStringList(), expected);
    EXPECT_TRUE(list_list2[3].asList().empty());
}

GTEST_TEST(vvvcfg, node_value_nested_list_ref)
{
    const auto input = R"(
node = [["text"], [42, [3.14], [42, "123"], []]]
node2 = $node
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    const auto& node2 = root.getChild("node2");
    EXPECT_EQ(node.getValue(), node2.getValue());
}

GTEST_TEST(vvvcfg, node_value_nested_multiline)
{
    const auto input = R"(
node = [
        "111",
        "222"
        ,
        "333"
        ]
node2 = ["111", "222", "333"]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    const auto& node2 = root.getChild("node2");
    EXPECT_EQ(node.getValue(), node2.getValue());
}

GTEST_TEST(vvvcfg, nested_value_ref)
{
    const auto input = R"(
node  = "text"
node1 = "other"
node2 = [$node, $node1, [$node1, $node], "333"]
node3 = ["text", "other", ["other", "text"], "333"]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node2 = root.getChild("node2");
    const auto& node3 = root.getChild("node3");
    EXPECT_EQ(node2.getValue(), node3.getValue());
}

GTEST_TEST(vvvcfg, string_properties)
{
    const auto input = R"(
node prop1="value", prop2="2", prop3, prop4="val" "ue",
        prop5="prop on newline after comma" \
        prop6 = "prop on newline after line continuation"
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    EXPECT_EQ(node.getProperties().size(), 6);
    EXPECT_TRUE(node.hasProperty("prop1"));
    EXPECT_TRUE(node.hasProperty("prop2"));
    EXPECT_TRUE(node.hasProperty("prop3"));
    EXPECT_FALSE(node.hasProperty("missing"));

    const auto& prop1 = node.getProperty("prop1");
    EXPECT_TRUE(prop1.isString());
    EXPECT_FALSE(prop1.isList());
    EXPECT_EQ(prop1.asString(), "value");

    const auto& prop2 = node.getProperty("prop2");
    EXPECT_TRUE(prop2.isString());
    EXPECT_FALSE(prop2.isList());
    EXPECT_EQ(prop2.asString(), "2");
    EXPECT_EQ(node.getPropertyAsLong("prop2"), 2);
    EXPECT_EQ(node.getPropertyAsDouble("prop2"), 2.0);

    const auto& prop3 = node.getProperty("prop3");
    EXPECT_TRUE(prop3.isString());
    EXPECT_FALSE(prop3.isList());
    EXPECT_EQ(prop3.asString(), "");

    const auto& prop4 = node.getProperty("prop4");
    EXPECT_TRUE(prop4.isString());
    EXPECT_FALSE(prop4.isList());
    EXPECT_EQ(prop4.asString(), "value");

    const auto& prop5 = node.getProperty("prop5");
    EXPECT_EQ(prop5.asString(), "prop on newline after comma");

    const auto& prop6 = node.getProperty("prop6");
    EXPECT_EQ(prop6.asString(), "prop on newline after line continuation");
}

GTEST_TEST(vvvcfg, list_properties)
{
    const auto input = R"(
node prop1=["value", "42", 3.14], prop2=["2" "4"], prop3=[]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    EXPECT_EQ(node.getProperties().size(), 3);
    EXPECT_TRUE(node.hasProperty("prop1"));
    EXPECT_TRUE(node.hasProperty("prop2"));
    EXPECT_TRUE(node.hasProperty("prop3"));
    EXPECT_FALSE(node.hasProperty("missing"));

    const auto& prop1 = node.getProperty("prop1");
    EXPECT_TRUE(prop1.isList());
    EXPECT_FALSE(prop1.isString());
    const auto& list1 = prop1.asStringList();
    EXPECT_EQ(list1.size(), 3);
    const std::vector<std::string> list1_expected = {"value", "42", "3.14"};
    EXPECT_EQ(list1, list1_expected);

    const auto& prop2 = node.getProperty("prop2");
    EXPECT_TRUE(prop2.isList());
    EXPECT_FALSE(prop2.isString());
    const auto& list2 = prop2.asStringList();
    EXPECT_EQ(list2.size(), 1);
    const std::vector<std::string> list2_expected = {"24"};
    EXPECT_EQ(list2, list2_expected);

    const auto& prop3 = node.getProperty("prop3");
    EXPECT_TRUE(prop3.isList());
    EXPECT_FALSE(prop3.isString());
    const auto& list3 = prop3.asStringList();
    EXPECT_TRUE(list3.empty());
}

GTEST_TEST(vvvcfg, nested_list_properties)
{
    const auto input = R"(
node prop1=[[]],
     prop2=["text", [42, 3.1415]]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    const auto& prop1 = node.getProperty("prop1");
    EXPECT_TRUE(prop1.isList());
    const auto& list1 = prop1.asList();
    EXPECT_EQ(list1.size(), 1);
    EXPECT_TRUE(list1[0].isList());
    const auto& list1_list = list1[0].asList();
    EXPECT_TRUE(list1_list.empty());

    const auto& prop2 = node.getProperty("prop2");
    EXPECT_TRUE(prop2.isList());
    const auto& list2 = prop2.asList();
    EXPECT_EQ(list2.size(), 2);
    const auto& list2_0 = list2[0];
    EXPECT_TRUE(list2_0.isString());
    EXPECT_EQ(list2_0.asString(), "text");
    const auto& list2_1 = list2[1];
    EXPECT_TRUE(list2_1.isList());
    const std::vector<std::string> expected{"42", "3.1415"};
    EXPECT_EQ(list2_1.asStringList(), expected);
}

GTEST_TEST(vvvcfg, ref_in_properties_lists)
{
    const auto input = R"(
node  = "text"
node1 = "other"
node2 prop=[$node, $node1, [$node1, $node], "333"]
node3 prop=["text", "other", ["other", "text"], "333"]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node2 = root.getChild("node2");
    const auto& node3 = root.getChild("node3");
    EXPECT_EQ(node2.getValue(), node3.getValue());
}

GTEST_TEST(vvvcfg, linecontinuation)
{
    const auto input = R"(
node \
    list \
    = \
    [ \
    "Hello " \
    "World" \
    , \
    "42" \
    ] \
    flag1 \
    flag2 \
    , \
    value \
    = \
    "42"
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    const auto& properties = node.getProperties();
    EXPECT_EQ(properties.size(), 4);
    EXPECT_TRUE(node.hasProperty("list"));
    EXPECT_TRUE(node.hasProperty("flag1"));
    EXPECT_TRUE(node.hasProperty("flag2"));
    EXPECT_TRUE(node.hasProperty("value"));
    EXPECT_FALSE(node.hasProperty("missing"));

    const auto& list = node.getProperty("list").asStringList();
    const std::vector<std::string> list_expected = {"Hello World", "42"};
    EXPECT_EQ(list, list_expected);

    EXPECT_EQ(node.getPropertyAsLong("value"), 42);
}

GTEST_TEST(vvvcfg, assignment)
{
    const auto input = R"(
node1 = "value"
node2 = $node1

node3 = [1, 2, 3]
node4 = $node3

node5 = "value"
    subnode1
    subnode2
        subsubnode1
    subnode3
node6 = $node5
node7 $node5
node8 = $node5
    subnode1 = "12345"
node9 = $node5
    subnode1 value="12345"
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1_value = root.getChild("node1").getValue().asString();
    const auto& node2_value = root.getChild("node2").getValue().asString();
    EXPECT_EQ(node1_value, node2_value);

    const auto& node3_value = root.getChild("node3").getValue().asStringList();
    const auto& node4_value = root.getChild("node4").getValue().asStringList();
    EXPECT_EQ(node3_value, node4_value);

    const auto& node5 = root.getChild("node5");
    const auto& node6 = root.getChild("node6");
    const auto& node7 = root.getChild("node7");
    const auto& node8 = root.getChild("node8");
    const auto& node9 = root.getChild("node9");
    const auto& node5_value = node5.getValue().asString();
    const auto& node6_value = node6.getValue().asString();
    const auto& node7_value = node7.getValue().asString();
    EXPECT_EQ(node5_value, node6_value);
    EXPECT_TRUE(node6.hasChild("subnode1"));
    EXPECT_EQ(node5_value, node7_value);
    EXPECT_TRUE(node7.hasChild("subnode1"));

    EXPECT_EQ(node7.getChildren(), node5.getChildren());
    EXPECT_EQ(node7.getProperties(), node5.getProperties());
    EXPECT_EQ(node7.getValue(), node5.getValue());

    EXPECT_EQ(node7.getChildren(), node6.getChildren());
    EXPECT_EQ(node7.getProperties(), node6.getProperties());
    EXPECT_EQ(node7.getValue(), node6.getValue());

    EXPECT_NE(node8.getChildren(), node5.getChildren());
    EXPECT_NE(node9.getChildren(), node5.getChildren());
}

GTEST_TEST(vvvcfg, hierarchy)
{
    const auto input = R"(
node
    subnode1
        another_level = "42"
    subnode2
        another_level = "43"
    subnode3 p=[800, 600]
        subsubnode
node2
    subnode1
    subnode4
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node = root.getChild("node");
    EXPECT_EQ(node.getNumChildren(), 3);
    EXPECT_TRUE(node.hasChild("subnode1"));
    EXPECT_TRUE(node.hasChild("subnode2"));
    EXPECT_TRUE(node.hasChild("subnode3"));
    EXPECT_FALSE(node.hasChild("subnode4"));

    EXPECT_EQ(
        root.getChild("node.subnode1.another_level").getValue().asString(),
        "42");
    EXPECT_EQ(
        root.getChild("node.subnode2.another_level").getValue().asString(),
        "43");

    const std::vector<std::string> expected_list{"800", "600"};
    EXPECT_EQ(root.getChild("node.subnode3").getProperty("p").asStringList(),
              expected_list);

    EXPECT_EQ(&root.getChild("node.subnode1.another_level"),
              &node.getChild("subnode1.another_level"));
    const std::vector<std::string> path = {"node", "subnode1", "another_level"};
    EXPECT_EQ(&root.getChild("node.subnode1.another_level"),
              &root.getChild({"node", "subnode1", "another_level"}));
}

GTEST_TEST(vvvcfg, complex_test)
{
    const auto input = R"(
default_style \
    color="00ff00", font="DejaVu"

#sadsadsa
#Hello
#sadsadsa

message= "Hello World"
list= []
list2= ["Hello", "List"]
list3= [1, 2, 3, 4]

list4= $list3
list_ex= [3.14, 42, "foo"]

node1 = [[]]
node2 = [["text"], [42, 3.14]]
node3 = [["text"], [42, [3.14], ["123"], []]]

view type="vlayout"
    l1
        text1 $default_style font="Droid",
                              type="label mabel",
                              width=12, height=123
        line1 type="line_edit"
        button1 type="button"
    l2
        text2 type="label" list=["aaa", "bbb", "cccc"] list2=$list4
    l3
        text3 type="label"
        combobox1 type="combobox"
            item1 type="item", text="1", flag1, flag2
            item2 type="item", text="Hello"
            item3 type="item", text="World"

    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
}

GTEST_TEST(vvvcfg, dict)
{
    const auto input = R"(
node1 = {"key": "value", "key2": "value2",
         "key3": 123,
         4: 444}
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& value = node1.getValue();
    EXPECT_TRUE(value.isDict());
    auto& dict = value.asDict();
    EXPECT_EQ(dict.size(), 4);
    auto& val = dict.at(std::string("key"));
    EXPECT_TRUE(val.isString());
    EXPECT_EQ(val.asString(), "value");
    EXPECT_EQ(dict.at("key2").asString(), "value2");
    EXPECT_EQ(dict.at("key3").asString(), "123");
    EXPECT_EQ(dict.at("4").asString(), "444");
}

GTEST_TEST(vvvcfg, dict_nested_dict)
{
    const auto input = R"(
node1 = {"key": "value",
         "nested": { 1: 111,
                     2: 222 }
        }
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& value = node1.getValue();
    EXPECT_TRUE(value.isDict());
    const auto& dict1 = value.asDict();
    const auto& nested = dict1.at("nested").asDict();
    EXPECT_EQ(nested.size(), 2);
    EXPECT_EQ(nested.at("1").asString(), "111");
    EXPECT_EQ(nested.at("2").asString(), "222");
}

GTEST_TEST(vvvcfg, dict_nested_list)
{
    const auto input = R"(
node1 = {"key": "value",
         "nested": [1, 2, 3]
        }
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& value = node1.getValue();
    EXPECT_TRUE(value.isDict());
    const auto& dict1 = value.asDict();
    const auto& nested = dict1.at("nested").asStringList();
    const std::vector<std::string> expected {"1", "2", "3"};
    EXPECT_EQ(nested, expected);
}

GTEST_TEST(vvvcfg, list_nested_dict)
{
    const auto input = R"(
node1 = [1, 2, {1: 111, 2: 222}, {3: 333}, 4]
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    const auto& node1 = root.getChild("node1");
    const auto& value = node1.getValue();
    EXPECT_TRUE(value.isList());
    const auto& list = value.asList();
    EXPECT_TRUE(list[2].isDict());
    EXPECT_TRUE(list[3].isDict());
    EXPECT_TRUE(list[4].isString());

    const auto& dict1 = list[2].asDict();
    EXPECT_EQ(dict1.at("1").asString(), "111");
    EXPECT_EQ(dict1.at("2").asString(), "222");

    const auto& dict2 = list[3].asDict();
    EXPECT_EQ(dict2.at("3").asString(), "333");
}

GTEST_TEST(vvvcfg, numbers)
{
    const auto input = R"(
int_neg = -42
int_pos = +42
float_neg = -3.1415
float_pos = +3.1415
    )";
    vvv::CfgNode root("");
    EXPECT_NO_THROW(root = vvv::make_cfg(input));
    EXPECT_EQ(root.getChild("int_neg").getValueAsString(), "-42");
    EXPECT_EQ(root.getChild("int_pos").getValueAsString(), "+42");
    EXPECT_EQ(root.getChild("float_neg").getValueAsString(), "-3.1415");
    EXPECT_EQ(root.getChild("float_pos").getValueAsString(), "+3.1415");
}

GTEST_TEST(vvvcfg, numbers_sign_throw)
{
    EXPECT_THROW(vvv::make_cfg("int_neg = --42"), std::logic_error);
    EXPECT_THROW(vvv::make_cfg("int_neg = ++42"), std::logic_error);
}

GTEST_TEST(vvvcfg, names_as_strings)
{
    const auto input = R"(
node1 = text
node2 prop=text2
node3 = [val1, val2, val3]
node4 = {key: value, key2: value2}
    )";
    vvv::CfgNode root("");
    root = vvv::make_cfg(input);
    EXPECT_EQ(root.getChild("node1").getValue().asString(), "text");
    EXPECT_EQ(root.getChild("node2").getProperty("prop").asString(), "text2");
    const std::vector<std::string> expected{"val1", "val2", "val3"};
    EXPECT_EQ(root.getChild("node3").getValue().asStringList(), expected);
    const auto& dict = root.getChild("node4").getValue().asDict();
    EXPECT_EQ(dict.at("key").asString(), "value");
    EXPECT_EQ(dict.at("key2").asString(), "value2");
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
