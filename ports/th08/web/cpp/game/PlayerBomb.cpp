#include "PlayerBomb.hpp"
namespace th08 {
PlayerBombKind player_bomb_kind(u8 character,i32 type)noexcept{
    if(character>=12||type<0||type>4)return PlayerBombKind::Invalid;
    if(type==4)return PlayerBombKind::LastWord;
    static constexpr PlayerBombKind teams[4][4]={{PlayerBombKind::Reimu,PlayerBombKind::Yukari,PlayerBombKind::ReimuLast,PlayerBombKind::YukariLast},
        {PlayerBombKind::Marisa,PlayerBombKind::Alice,PlayerBombKind::MarisaLast,PlayerBombKind::AliceLast},
        {PlayerBombKind::Sakuya,PlayerBombKind::Remilia,PlayerBombKind::SakuyaLast,PlayerBombKind::RemiliaLast},
        {PlayerBombKind::Youmu,PlayerBombKind::Yuyuko,PlayerBombKind::YoumuLast,PlayerBombKind::YuyukoLast}};
    return character<4?teams[character][type]:teams[(character-4)/2][(type&2)|(character&1)];
}
bool update_player_bomb(PlayerBombState& s,PlayerBombContext& input,PlayerLifeState& life,PlayerLifeContext& context,PlayerMovementState& movement,AnmVm& animation,const ShotProfile& profile,const FrameTiming& timing,PlayerBombActions& actions){
    const bool automatic=life.auto_bomb&&life.predead_count==1;
    if(!automatic){
        if(s.cooldown)s.cooldown=wrapping_sub(s.cooldown,1);
        if(s.active){
            if(s.timer.changed())context.hud_flags=(context.hud_flags&~0x300u)|0x200;
            if(s.timer.current>=s.duration){
                actions.finish_spell_overlay();s.active=0;movement.multiplier={1,1};
                if(s.type==4){context.game_flags&=~0x180u;for(u32 i=0;i<8;++i)if(input.bosses[i]){actions.defeat_boss(i);if(auto* boss=input.bosses[i]){boss->life=0;boss->flags&=~0x40000000u;}}actions.last_word_flash();}
            }else {const auto kind=player_bomb_kind(context.character,s.type);if(kind==PlayerBombKind::Invalid)return false;actions.update(kind);s.timer.tick(timing);}
            if(s.type<4){if(!s.duration)return false;actions.add_gauge(i16((s.type&1?26000:-26000)/s.duration),true);}
            return true;
        }
        if(!(input.buttons&2)||input.tampered||input.gui_blocked||!life.predead_count||context.bombs<1||s.cooldown){s.triggered=0;return true;}
        if(context.game_flags&0x4180){if((input.buttons&2)!=(input.previous_buttons&2))actions.sound(41,0);return true;}
    }
    context.replay_flags|=1;life.auto_bomb=0;
    if(!(context.game_flags&0x180)){
        animation.flag17=0;if(life.predead_effect){life.predead_effect->active=0;life.predead_effect=nullptr;}context.game_flags&=~0x400u;actions.reset_screen_color();
        s.type=context.focused;if(life.deathbomb)s.type=wrapping_sub(1,s.type);
        if(!life.deathbomb){++input.regular_bombs;if(!(input.cheats&4))actions.add_bombs(-1);}
        else {s.type=wrapping_add(s.type,2);if(automatic||context.bombs<2){s.consumed=context.bombs;if(!(input.cheats&4))actions.set_bombs(0);}else{s.consumed=2;if(!(input.cheats&4))actions.add_bombs(-2);}++input.last_spells;}
        actions.count_bomb(1);
    }else s.type=4;
    life.deathbomb=0;context.hud_flags=(context.hud_flags&~12u)|8;s.active=1;s.triggered=1;s.timer.set(0);s.duration=999;
    const auto kind=player_bomb_kind(context.character,s.type);if(kind==PlayerBombKind::Invalid)return false;
    actions.update(kind);s.timer.tick(timing);actions.subtract_rank(200);actions.fail_spell_with_bomb();life.predead_count=wrapping_add(life.predead_count,6);if(life.predead_count>profile.deathbomb_limit)life.predead_count=profile.deathbomb_limit;
    return true;
}
}
