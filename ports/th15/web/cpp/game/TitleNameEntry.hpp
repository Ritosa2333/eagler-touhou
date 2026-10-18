#pragma once
#include "MenuCursor.hpp"
namespace th15 {
// Both post-run title screens edit the same original eight-byte name/grid.
struct TitleNameEntry {
 MenuCursor names;std::array<char,9> name{};i32 name_length=0;bool name_not_required=false;
 TitleNameEntry(){names.wrapping=false;}
};
}
