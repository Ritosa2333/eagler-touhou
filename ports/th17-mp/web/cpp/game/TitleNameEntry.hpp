#pragma once
#include "MenuCursor.hpp"
#include <array>
namespace th17 {
// Both post-run title screens edit the same original eight-byte name/grid.
struct TitleNameEntry {
 MenuCursor names;std::array<char,9> name{};i32 name_length=0;bool name_not_required=false;
 TitleNameEntry(){names.wrap=false;}
};
}
