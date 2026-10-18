#pragma once
#include "Timer.hpp"
#include "GameEconomy.hpp"
#include <array>
#include <string>
namespace th11 {
struct EnemyFrameWorld;
class AsciiText;
struct SpellRecord {std::array<char,64> name{};i32 captures=0,attempts=0;};
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
};
// 40c650 / 40c0a0 / 40cd60. The world fields are shared with player hits
// and enemy phase transitions; none of these systems owns a stale copy.
class SpellController {
public:
    SpellController(EnemyFrameWorld&,GameEconomy&,SpellEffects&,SpellRecords&,const float*);
    Timer timer;
    i32 initial_bonus=0,timeout=0,frame_count=0;
    Vec3 circle_position{};
    std::array<char,64> name{};
    i32 selection=0,stage=1;
    u32 translation_rank=0;
    bool replay=false,bomb_active=false;
    bool practice_time_lock=false;
    bool begin(i32 id,i32 timeout,const char* name,Vec3 boss,bool bomb);
    bool update(float player_y,Vec3 boss,bool bomb);
    bool end();
    bool queue_text(AsciiText&,u32 title_color)const;
    void survival();
    void hide_circle();
private:
    EnemyFrameWorld& world;GameEconomy& economy;SpellEffects& effects;SpellRecords& records;const float* rate;
};
}
