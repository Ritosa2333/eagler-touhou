#pragma once
#include <cstdint>
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
#include <eagler/thprac/PracticeInput.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#endif
namespace th12::practice {
struct ToolSettings {
  bool show_keyboard_monitor=false, map_inf_life_to_no_continue=false;
  bool force_boss_move_down=false, disable_master_display=false, show_lock_timer=false;
  bool chromatic_ufo=false;
  float boss_move_down_range=.5f;
  int lock_frames=0;bool lock_frame_pending=false;
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  eagler::thprac::PracticeInput input;
  eagler::thprac::PracticeKeyMonitor key_monitor;
#endif
};
ToolSettings& toolSettings();
void resetToolInput();
void applyToolInput(int& held,int& pressed);
void recordToolTick(int held);
void resetLockTimer();
void bossMoveRange(float& center,float& range);
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
eagler::thprac::PracticeSpeed& speedSettings();
#endif
}
