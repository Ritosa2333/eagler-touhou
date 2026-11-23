#pragma once
#include "game/ItemHost.hpp"
namespace th128::coopitems {
void born(unsigned id);
void removed(unsigned id);
void target(unsigned id,item::Position at,item::Position& pos,int& state,bool& focus);
}
