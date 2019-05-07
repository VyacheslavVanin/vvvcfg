#pragma once
#include <iostream>

#include "cfg_node.hpp"

namespace vvv {

CfgNode make_cfg(std::istream& input);
CfgNode make_cfg(const std::string& input);
} // namespace vvv
