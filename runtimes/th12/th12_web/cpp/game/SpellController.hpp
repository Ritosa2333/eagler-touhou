#pragma once
#include "PortTitle.hpp"
#include "Timer.hpp"
#include "GameEconomy.hpp"
#include <array>
#include <string>
namespace th11 {
struct EnemyFrameWorld;
class AsciiText;
struct SpellRecord {std::array<char,kPortTitle==14?128:64> name{};i32 captures=0,attempts=0;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // Native record +80/+84 and +88/+8c separate ordinary and spell practice.
    i32 practice_captures=0,practice_attempts=0;
#endif
};
// 175 spell records per shot type, plus the aggregate table, in score.dat.
struct SpellRecords {std::array<std::array<SpellRecord,175>,7> entries{};};
struct SpellEffects {
    virtual ~SpellEffects()=default;
    virtual bool spell_begin_visuals(i32 id,i32 timeout,const char* name)=0;
    virtual bool spell_update_visuals()=0;
    virtual void spell_background(bool hidden)=0;
    virtual void spell_title_interrupt(i16 label)=0;
    virtual void spell_circle_position(Vec3)=0;
    virtual void spell_circle_end()=0;
    virtual bool spell_result(bool captured,i32 bonus)=0;
    virtual bool spell_sound(i32 id)=0;
    virtual void spell_award_score(i32 amount,GameEconomy& economy){economy.add_score(amount);}
    virtual bool spell_timer_interrupt(i16){return false;}
    virtual void spell_background_end(){}
};
// 40c650 / 40c0a0 / 40cd60. The world fields are shared with player hits
// and enemy phase transitions; none of these systems owns a stale copy.
class SpellController {
public:
    SpellController(EnemyFrameWorld&,GameEconomy&,SpellEffects&,SpellRecords&,const float*);
    Timer timer;
    i32 initial_bonus=0,timeout=0,frame_count=0;
    Vec3 circle_position{};
    std::array<char,kPortTitle==14?128:64> name{};
    i32 selection=0,stage=1;
#if PORT_TITLE == 16
    i32 th16_effect_variant=0;
#endif
    bool replay=false,bomb_active=false;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    u32 run_flags=0;
#endif
    bool begin(i32 id,i32 timeout,const char* name,Vec3 boss,bool bomb);
    bool update(float player_y,Vec3 boss,bool bomb);
    bool end();
    bool queue_text(AsciiText&,u32 title_color)const;
    void survival();
    void hide_circle();
    void title_orientation(i32);
    void invalidate_capture();
private:
    EnemyFrameWorld& world;GameEconomy& economy;SpellEffects& effects;SpellRecords& records;const float* rate;
};
}
