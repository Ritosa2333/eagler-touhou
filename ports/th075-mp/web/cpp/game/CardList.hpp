#pragma once
#include "Types.hpp"
namespace th075 {
class CardList {
public:
 std::vector<std::string> names;std::string error;
 bool load(const void*,std::size_t,bool encrypted=true);
};
}
