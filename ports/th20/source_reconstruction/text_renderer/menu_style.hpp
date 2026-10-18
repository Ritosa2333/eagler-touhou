#pragma once
#include "text.hpp"
#include "../stone_menu/cursor.hpp"
namespace th20::source::text {
void style_menu_line(Renderer&,sprite::Vec3&,int index,const menu::Cursor&,int flash,int jitter); //46b590
}
