#pragma once
#include "enemy_path.hpp"
#include "stage_progress.hpp"
#include <optional>
namespace soew {
// Headless integration harness only: no damage, art, automatic firing or actual
// midboss. It stops at the FIRST missing consumer/path byte, never skips it.
struct EnemyPathRuntime {
    StageScroll scroll;
    EnemySlots enemies;
    FormationDispatch dispatch;
    RandomRing random;
    unsigned page=0;
    std::uint64_t ticks=0,spawns=0;
    struct Block {unsigned slot=0,type=0;EnemyPathResult result;};
    std::optional<Block> blocked;
    using BoundaryExit=std::function<void(const EnemySlots::Slot&,int,int)>;
    struct CombatHooks {
        // true follows original positive ordinary-hit branch: skip auto-fire.
        std::function<bool(unsigned,EnemySlots::Slot&,const StageScript::EnemyTemplate&)> damage;
        std::function<void(unsigned,EnemySlots::Slot&,const StageScript::EnemyTemplate&)> death;
        bool suppress_formations=false;
    };
    explicit EnemyPathRuntime(int stage,std::uint32_t seed):scroll(stage){IRand rng(seed);random.fill(rng);}
    bool frame_prepared=false;
    std::optional<unsigned> deferred_scroll_delta;
    void prepare_frame(const StageScript &script,bool suppress=false){
        if(blocked||frame_prepared)return;
        enemies.begin_frame(page);
        if(scroll.take_progress_event())dispatch.dispatch(script,scroll.progress,suppress,[&](auto intent){auto slot=enemies.spawn(script,intent.template_id,intent.lane,page,random);spawns+=slot.has_value();return slot.has_value();});
        if(dispatch.incomplete_boundary){EnemyPathResult missing;missing.reason="declared formation row unavailable";blocked=Block{0,0,std::move(missing)};return;}
        frame_prepared=true;
    }
    template<class CommitTerrain> void finish_deferred_scroll(unsigned map_sections,CommitTerrain commit_terrain){
        const auto delta=deferred_scroll_delta.value();
        const auto reload=scroll.commit_tick(map_sections,delta);
        if(delta&&!scroll.done)commit_terrain(reload);
        deferred_scroll_delta.reset();
        page^=1;++ticks;
    }
    void tick(const StageScript &script,int player_x=208,int player_y=336,EnemyEffectSink *effects=nullptr,CombatHooks *combat=nullptr,
              bool defer_scroll=false,const BoundaryExit &on_exit={}){
        if(blocked)return;
        if(deferred_scroll_delta)throw std::logic_error("Uncommitted enemy frame");
        prepare_frame(script,combat&&combat->suppress_formations);if(blocked)return;frame_prepared=false;
        const auto old_attempt=scroll.attempted_pixels;
        unsigned delta;
        if(defer_scroll){
            StageScroll::validate_map_length(static_cast<unsigned>(script.map.size()));
            delta=scroll.calculate_tick();deferred_scroll_delta=delta;
        }else{
            scroll.finish_tick(static_cast<unsigned>(script.map.size()));
            delta=scroll.attempted_pixels-old_attempt;
        }
        for(unsigned i=0;i<enemies.high_water;++i){auto &s=enemies.slots[i];if(s.raw[0xe]!=1)continue;
            const unsigned type=s.word(0xc);if(type>=script.enemies.size())throw std::runtime_error("Enemy type outside script");const auto slot=script.enemies[type].path_slot;
            const int old_x=enemy_s16(s.word(page*4)),old_y=enemy_s16(s.word(page*4+2));
            bool repositioned=false;
            if(!s.raw[0x10]){
                auto result=run_enemy_path(s,script.paths.at(slot).bytes,random,{page,delta,player_x,player_y,scroll.line,&script.enemies[type],effects});
                repositioned=result.repositioned;if(repositioned)++enemies.motion_generation[i];
                if(result.status==EnemyPathResult::Blocked){blocked=Block{i,type,std::move(result)};return;}
                if(result.status==EnemyPathResult::Ended)continue;
            }
            if(s.raw[0x10]&&(!combat||!combat->death)){EnemyPathResult missing;missing.reason="enemy death animation/score consumer not connected";missing.pc=s.word(8);blocked=Block{i,type,std::move(missing)};return;}
            const int x=enemy_s16(s.word(page*4)),y=enemy_s16(s.word(page*4+2));
            const bool left=x<0,right=x>418,top=s.raw[0x1e]&&y<=-16,bottom=s.raw[0x1e]&&y>=384;
            if(left||right||top||bottom){
                if(on_exit&&!repositioned&&!s.raw[0x10]&&((left&&x<old_x)||(right&&x>old_x)||(top&&y<old_y)||(bottom&&y>old_y)))
                    on_exit(s,old_x,old_y);
                s.raw[0xe]=2;
            }
            // Original automatic fire is outside path bytecode, at template+1A.
            const auto &raw=script.enemies[type].raw;const int fire_interval=enemy_s16(unsigned(raw[26])|(unsigned(raw[27])<<8));
            if(s.raw[0xe]==1){
                ++s.raw[0xf];
                const int cels=enemy_s16(unsigned(raw[10])|(unsigned(raw[11])<<8));
                const int interval=enemy_s16(unsigned(raw[12])|(unsigned(raw[13])<<8));
                if(cels!=0&&s.word(0x14)==1){
                    if(interval==0){EnemyPathResult missing;missing.reason="original animation divide by zero";missing.pc=s.word(8);blocked=Block{i,type,std::move(missing)};return;}
                    if(int(s.raw[0xf])%interval==0){const auto next=static_cast<std::uint16_t>(s.word(0x12)+1);s.word(0x12,enemy_s16(next)>=cels?0:next);}
                }
            }
            if(s.raw[0xe]==1&&combat){
                const unsigned hp=unsigned(raw[22])|(unsigned(raw[23])<<8);
                if(hp!=65535&&!s.raw[0x22]){
                    if(s.raw[0x10]){combat->death(i,s,script.enemies[type]);continue;}
                    if(combat->damage&&combat->damage(i,s,script.enemies[type]))continue;
                }
            }
            if(s.raw[0xe]==1&&fire_interval>1&&unsigned(s.raw[0xf])%unsigned(fire_interval)==0){
                const unsigned emit_x=unsigned(raw[4])|(unsigned(raw[5])<<8),emit_y=unsigned(raw[6])|(unsigned(raw[7])<<8);
                EnemyBulletRequest request;request.left=enemy_s16(s.word(page*4)+emit_x);request.top=enemy_s16(s.word(page*4+2)+emit_y);request.group=s.raw[0x24];request.speed=s.raw[0x25];
                if(!effects||!effects->bullet||!effects->bullet(request)){EnemyPathResult missing;missing.reason="automatic fire consumer not connected";missing.pc=s.word(8);blocked=Block{i,type,std::move(missing)};return;}
            }
        }
        if(!defer_scroll){page^=1;++ticks;}
    }
};
}
