#pragma once
#include "../game/Types.hpp"
namespace th075::sdl {
bool localized_option_description(u8 index,std::string& text);
bool apply_language_patch(const std::string& path,std::vector<u8>& bytes,std::string& error);
}
