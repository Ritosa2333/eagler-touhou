#pragma once
#include "../progress_state/manager.hpp"
namespace th20::source::progress {
bool unlock_flag(SaveManager&,unsigned); //4bd780
void notify_unlock(SaveManager&,unsigned); //4bddc0
void grant_stone(SaveManager&,unsigned); //4bcf60
}
