#include "PracticeTools.hpp"
#include "Practice.hpp"
#include "GameState.hpp"
#include <algorithm>
namespace th12::practice {
namespace { ToolSettings settings;int last_filtered=0; }
ToolSettings& toolSettings(){return settings;}
void resetToolInput(){
  last_filtered=0;
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  settings.input.reset();
#endif
}
void applyToolInput(int& held,int& pressed){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  if(!enabled()||replay())return;
  uint8_t keys[256]{};
  constexpr int bits[]{1,2,8,16,32,64,128,256,512,0x80000,0x200000};
  constexpr int codes[]{90,88,160,38,40,37,39,27,17,13,82};
  for(int i=0;i<11;i++)keys[codes[i]]=held&bits[i]?128:0;
  settings.input.apply(keys);settings.input.gui_tick();
  int filtered=held;for(int i=0;i<11;i++){if(keys[codes[i]])filtered|=bits[i];else filtered&=~bits[i];}
  if(keys[161])filtered|=8;
  pressed=(pressed&filtered)|(filtered&~last_filtered);held=last_filtered=filtered;
  const auto& input=settings.input;
  if(input.disable_xkey||input.disable_zkey||input.disable_shiftkey||input.force_shiftkey||input.enable_fast_retry)markAssisted();
#else
  (void)held;(void)pressed;
#endif
}
void recordToolTick(int held){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  if(enabled()&&settings.show_keyboard_monitor)settings.key_monitor.record(12,uint32_t(held));
#else
  (void)held;
#endif
  if(enabled()&&settings.lock_frame_pending)++settings.lock_frames;
  settings.lock_frame_pending=false;
}
void resetLockTimer(){settings.lock_frames=0;settings.lock_frame_pending=false;}
void bossMoveRange(float& center,float& range){
  if(!enabled()||replay()||!settings.force_boss_move_down)return;
  const float maximum=center+range*.5f;
  const float minimum=maximum-range*(1.f-std::clamp(settings.boss_move_down_range,0.f,1.f));
  center=(maximum+minimum)*.5f;range=maximum-minimum;markAssisted();
}
}
