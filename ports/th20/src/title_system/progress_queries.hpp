#pragma once
#include "../progress_state/manager.hpp"
namespace th20::source::title {
bool character_extra_unlocked(progress::SaveManager&,int); //52c870, checks indices0..7
bool any_extra_unlocked(progress::SaveManager&); //52c7b0
bool notice_pending(progress::SaveManager&); //52c9f0
int pop_notice(progress::SaveManager&); //52c320
int profile_clear_count(progress::SaveManager&,int difficulty,int character,int index); //52c440
bool character_all_cleared(progress::SaveManager&,int difficulty,int character); //52c4f0
bool difficulty_all_cleared(progress::SaveManager&,int difficulty); //52c640
}
