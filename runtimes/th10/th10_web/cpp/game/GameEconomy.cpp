#include "GameEconomy.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeRuntime.hpp"
#endif
namespace th10 {
// 0x412ed0. Re-selecting the same section preserves its elapsed frame count.
void GameEconomy::select_section(i32 next) noexcept {const auto previous=section;section=next;if(previous!=next)section_frames=0;}
// 0x409d90. Stored score is in tens; division happens before saturation.
void GameEconomy::add_score(i32 points) noexcept {score=wrapping_add(score,points/10);if(score>999999999)score=999999999;}
// 0x41be80 / 0x405b60. Addition wraps before applying the original limits.
void GameEconomy::add_item_value(i32 points) noexcept {item_value=wrapping_add(item_value,points/10);if(item_value>99999)item_value=99999;}
void GameEconomy::add_rank(i32 delta) noexcept {rank=wrapping_add(rank,delta);if(rank>1024)rank=1024;else if(rank<-1024)rank=-1024;}
// 0x418930. Power is a signed 16-bit value; the upper neighboring word is retained.
bool GameEconomy::add_power(std::int16_t delta,EconomyEnvironment& env){
#ifdef TH_ENABLE_THPRAC
    // F3 infinite power: suppress only the decrement path (0x425ABD upstream).
    if(delta<0&&env.practice&&practice_infinite_power(*env.practice))return false;
#endif
    if(power>=100)return false;
    const u16 bits=static_cast<u16>(power)+static_cast<u16>(delta);std::memcpy(&power,&bits,2);
    if(power>100){power=100;env.show_notification(0x49);}
    return (static_cast<i32>(power)-delta)/20!=static_cast<i32>(power)/20;
}
// 0x4188a0. The cap path refreshes the life display without a notification.
void GameEconomy::add_lives(i32 delta,EconomyEnvironment& env){
#ifdef TH_ENABLE_THPRAC
    // F2 infinite lives: block any life decrement here as well. Normal callers
    // add positive 1-ups; the death decrement is handled in PlayerLifecycle.
    if(delta<0&&env.practice&&practice_infinite_lives(*env.practice))return;
#endif
    lives=wrapping_add(lives,delta);
    if(lives>9){lives=9;env.update_lives(lives);return;}
    env.play_global_sound(0x2c);env.show_notification(0x4b);env.update_lives(lives);
}
// 0x412ff0. Adding time only acts while below the 130-frame ceiling.
void GameEconomy::extend_faith_timer(i32 frames,const float* default_rate) noexcept {
    if(faith_timer.current>=130)return;
    faith_timer.advance(Extended::from_int(frames).to_float());
    if(faith_timer.current<=130)return;
    if(!(faith_timer_flags&1)){faith_timer.rate=default_rate;faith_timer_flags|=1;}
    faith_timer.previous=129;faith_timer.current=130;faith_timer.fractional=130.0f;
}
}
