#pragma once
#include "EclVm.hpp"
#include "SpellPresentation.hpp"
#include "SpellHistory.hpp"
#include "EffectSystem.hpp"
#include "BackgroundState.hpp"
#include "PlayerBomb.hpp"
#include "PracticeConfig.hpp"
namespace th08 {
struct SpellSystemActions {
    virtual ~SpellSystemActions()=default;
    virtual bool clear_projectiles(i32 mode)=0;
    virtual bool cancel_projectiles(i32 maximum,bool reward,i32& score)=0;
    virtual bool cancel_enemies(i32 maximum,i32& score)=0;
    virtual void bonus(i32 value)=0;
    virtual void spell_bonus(i32 value)=0;
    virtual void popup(i32 value,i32 type)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void item(const Vec3& position,i32 type,i32 mode)=0;
};
// The spell owner is shared with ECL and the enemy manager. Effects and
// records here are the real scene objects, not copies used for display.
class SpellSystem:public EclSpellActions {
    EclGlobals& globals;GameGlobals& numbers;GameValues& values;HighScore& high_score;SpellRecord* records;
    EffectSystem& effects;BackgroundState& background;AnmExecutor& anm;SpellPresentation& presentation;PlayerBombState& bomb;PracticeState& practice;SpellSystemActions& actions;
    bool reward(bool point_value=false);
public:
    SpellSystem(EclGlobals&,GameGlobals&,GameValues&,HighScore&,SpellRecord*,EffectSystem&,BackgroundState&,AnmExecutor&,SpellPresentation&,PlayerBombState&,PracticeState&,SpellSystemActions&);
    ~SpellSystem(){if(globals.spell_actions==this)globals.spell_actions=nullptr;}
    bool begin(EclVm&,const EclInstruction&)override;
    bool begin(EclVm&,u32 number,i32 portrait,u32 bonus,const u8* name,const u8* owner,const u8* comment1,const u8* comment2);
    bool end()override;
    bool update();
    void add_bonus(i32 amount);
    void fail(bool bomb=false){globals.spell_flags&=~4u;globals.spell_bonus=0;if(bomb)globals.spell_flags=(globals.spell_flags&~128u)|((globals.spell_flags&1)<<7);}
};
}
