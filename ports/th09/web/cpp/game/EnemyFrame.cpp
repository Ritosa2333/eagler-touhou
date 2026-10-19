#include "EnemyManager.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
namespace {
void clear_async(EclVm& e){for(auto& c:e.asynchronous)c.reset();e.active=&e.primary;e.active_slot=-1;e.bind_context();}
void clear_thresholds(EclVm& e){for(auto& v:e.values.life_thresholds)v=-1;e.status.timeout=-1;}
Vec3 resolve(const EclVm& e){return {e.values.position.x+e.position_offset.x,e.values.position.y+e.position_offset.y,0};}
bool on_screen(const Vec3& p,const AnmLoadedSprite& sprite){
    const float w=sprite.widthPx*.5f,h=sprite.heightPx*.5f;
    return !(p.x+w<-144.f||144.f<p.x-w||p.y+h<0.f||448.f<p.y-h);
}
void collide(EnemyFrameActions& a,const Vec3& p,const Vec3& box){constexpr float inverse=1.f/1.5f;a.body_collision(p,{inverse*box.x,inverse*box.y,inverse*box.z});}
struct TimelineActions:EnemyTimelineActions {
    EnemyManager& m;explicit TimelineActions(EnemyManager& manager):m(manager){}
    EclVm* spawn(const EnemySpawn& s)override{return m.create(s);}
    EclVm* boss(i32 id)override{return id>=0&&u32(id)<m.bosses.size()?m.bosses[id]:nullptr;}
    i32* timeline_events()override{return m.timeline_events;}
    void finish_match()override{m.frame_actions->finish_match();}
};
}
void EnemyManager::remove(EclVm& e){
    if(checkpoint)checkpoint->BeforeEnemy(e);
    if(!(e.behavior_flags&0xe0000))e.behavior_flags&=~1u;
    if((e.behavior_flags&2)&&e.values.boss_id<4){bosses[e.values.boss_id]=nullptr;e.behavior_flags&=~2u;}
    if(e.status.attached_effect_count&&frame_actions)frame_actions->release_attached_effects(e);
    if((e.behavior_flags&2)&&e.values.boss_id<bosses.size())bosses[e.values.boss_id]=nullptr;
    clear_thresholds(e);clear_async(e);if(player&&player->target==&e)player->target=nullptr;
}
bool EnemyManager::step_frame(const EnemyFrameSettings& settings){
    if(settings.game_flags&0x1800)return true;
    if(!player||!frame_actions)return false;auto& p=*player;auto& a=*frame_actions;
    a.attack_position(field.side,{-999,0,0});for(auto& list:draw_lists)list.clear();
    if(!settings.timeline_blocked){
        if(timeline.completed){
            if(normal_alive==0||focus_time.current>0){
                const u8 pattern=settings.patterns[pattern_index++];const Timer wait=timeline.wait;
                if(!timeline.start(common_program,pattern&0x7f,pattern>>7))return false;
                timeline.wait=wait;
            }
        }else {TimelineActions actions(*this);if(timeline.step(timing,frame_step,u8(difficulty_mask),settings.dialogue,world.random,actions)<0)return false;}
    }
    alive=normal_alive=attack_alive=spirit_alive=0;priority_target=first_target=nullptr;
    if(p.focus==0)focus_time.tick(timing);else focus_time.reset();
    i32 collision_token=0;
    for(u32 index=0;index<capacity;++index){
        auto& e=enemies[index];auto& v=e.values;auto& s=e.status;auto& anim=e.animation.layers[0];
        if(!(e.behavior_flags&1)){if(p.target==&e)p.target=nullptr;continue;}
        const auto queue_draw=[&]{if(!(e.behavior_flags&0x10)&&s.draw_group<draw_lists.size())draw_lists[s.draw_group].push_back(&e);};
        const u32 attack_kind=(v.flags>>10)&3;
        if((field.flags&1)&&attack_kind!=1&&attack_kind!=2){queue_draw();continue;}
        bool hit=false,dying=bool(e.behavior_flags&0x100);i32 kill_source=0;
        if(!dying){
            ++alive;if(!(v.flags&0x4dc0))++normal_alive;if(v.flags&0xc00)++attack_alive;if(v.flags&0x1c0)++spirit_alive;
            if(v.flags&0x1000){
                const auto& t=e.movement.capture_time;if(t.current!=t.previous&&t.current%2==0)a.effect(9,v.position);
                if(t.current>capture_lifetime){
                    BulletEmission shot;shot.sprite=1;shot.color=2;shot.position=v.position;shot.speed=float(world.rank)*.1f+1.f;
                    shot.spread=.15707963705062866f;shot.count=field.character==13?1:3;shot.layers=1;shot.flags=4;
                    if(!a.emit_capture_bullets(shot))return false;
                    e.behavior_flags&=~1u;remove(e);continue;
                }
            }
            if(!((e.behavior_flags&0x8000000)&&p.state!=0)&&!(v.flags&0x10)){
                if(!run_script(e)){e.behavior_flags&=~1u;remove(e);if(e.invalid)return false;continue;}
                if(!(e.behavior_flags&0x4000000)){e.movement.clamp(e);e.movement.integrate_position(e,timing);e.movement.clamp(e);}
                v.resolved_position=resolve(e);
                auto& trail=e.trail;
                if(trail.flags){
                    if(trail.length<1||u32(trail.length)>trail.history.size())return false;
                    for(i32 n=trail.length-1;n>0;--n)trail.history[n]=trail.history[n-1];
                    trail.history[0]={v.resolved_position,e.velocity,v.direction};
                }
                if(!anim.loadedSprite)e.behavior_flags|=0x10;
                // First entry and subsequent exit use separate branches: the
                // frame which first becomes visible cannot also remove it.
                const bool entered=!(e.behavior_flags&0x10)&&!(e.behavior_flags&0x200000)&&on_screen(v.resolved_position,*anim.loadedSprite);
                if(entered)e.behavior_flags|=0x200000;
                else if((e.behavior_flags&0x200000)&&!(e.behavior_flags&0x2000000)){
                    if(!anim.loadedSprite)return false;
                    if(!on_screen(v.resolved_position,*anim.loadedSprite)&&(!trail.flags||!on_screen(trail.history[trail.length-1].position,*anim.loadedSprite))){e.behavior_flags&=~1u;remove(e);continue;}
                }
                anim.color1.d3dColor=i32(s.saved_color);a.advance_animation(anim);s.saved_color=u32(anim.color1.d3dColor);
                for(u32 n=1;n<3;++n){auto& extra=e.animation.layers[n];if(extra.scriptIndex>=0&&a.advance_animation(extra))extra.scriptIndex=-1;}
                if(!(e.behavior_flags&0x30)){
                    if((e.behavior_flags&4)&&(!(v.flags&0x1c0)||field.character!=11)){
                        collide(a,v.resolved_position,e.movement.hitbox);
                        if(trail.flags){
                            if(trail.collision_length>i32(trail.history.size()))return false;
                            for(i32 n=1;n<trail.collision_length;n+=6){
                                Vec3 box=e.movement.hitbox;
                                if(trail.flags&2){const float i=float(n),inverse=1.f/float(trail.collision_length);box={box.x-inverse*(box.x*i),box.y-inverse*(box.y*i),box.z-inverse*(box.z*i)};}
                                collide(a,trail.history[n].position,box);
                            }
                        }
                    }
                    v.last_damage=0;
                    if(e.behavior_flags&0x40){
                        i32 damage=a.shot_damage(v.resolved_position,e.movement.hitbox,s.shot_damage[0],collision_token,s.shot_damage[1]);
                        if(e.movement.low_damage_hitbox.x>0)a.shot_damage(v.resolved_position,e.movement.low_damage_hitbox,s.shot_damage[0],collision_token,s.shot_damage[1]);
                        if(damage>0){
                            if(e.behavior_flags&8){
                                if(s.invulnerable.current>0)damage=e.behavior_flags&2?damage/9:0;
                                if(damage==s.shot_damage[0])kill_source=1;if(damage==s.shot_damage[1])kill_source=2;
                                if(v.flags&0x1c0)damage=wrapping_add(s.shot_damage[0]/(v.flags&0x1000?2:4),signed_bits(u32(wrapping_sub(damage,s.shot_damage[0]))*4));
                                v.life=wrapping_sub(v.life,damage);v.last_damage=damage;
                            }hit=true;
                        }
                        const float nx=p.nearest_position.x-field.player.x,ny=p.nearest_position.y-field.player.y;
                        const float dx=v.resolved_position.x-field.player.x,dy=v.resolved_position.y-field.player.y;
                        if(dx*dx+dy*dy<nx*nx+ny*ny)p.nearest_position=v.resolved_position;
                        if(std::fabs(dx)<64&&(!p.target||v.resolved_position.y<p.target->values.position.y))p.target=&e;
                    }
                }
                if((v.flags&1)&&v.life>0)v.flags&=~1u;
                dying=v.life<1&&!(v.flags&9);
            }else v.lifetime.decrement(1,timing);
        }else v.resolved_position=resolve(e);
        if(dying){
            v.flags|=1;s.remaining_seconds=wrapping_sub(s.timeout,v.lifetime.current)/60;clear_thresholds(e);clear_async(e);
            switch((e.behavior_flags>>17)&7){
            case 0:case 1:{const bool persist=bool(e.behavior_flags&0xe0000);a.add_score(v.score_reward);
                e.behavior_flags=persist?(e.behavior_flags&0xffffffb3u)|0x100000:e.behavior_flags&~1u;
                if(e.behavior_flags&2)a.release_attached_effects(e);v.life=0;break;}
            case 2:v.life=0;break;
            case 3:
                e.behavior_flags&=0xfff1fff7u;v.life=1;
                if(s.effects[0]>=0)for(u32 n=0;n<3;++n)a.effect(s.effects[0]+20,v.resolved_position);
                if(p.state==0){p.protection.reset(90);p.state=3;}e.behavior_flags&=0xe7ffffffu;break;
            default:break;
            }
            if(!(e.behavior_flags&0x100)&&!a.enemy_death(e,kill_source))return false;
            if(s.death_subroutine>=0){
                s.motion_range={-.5f,.5f};std::fill_n(s.motion_modes,4,i16(0));e.scratch_depth=0;clear_thresholds(e);clear_async(e);
                e.emitter.parameters=prototype.emitter.parameters;e.emitter.period=0;
                EclExecutor executor(timing,frame_step,difficulty_mask,bindings);if(!executor.enter_main(e,s.death_subroutine))return false;s.death_subroutine=-1;
            }
        }
        if(s.damage_flash==0&&hit){
            a.play_positioned_sound((v.flags&6)<4?20:37,v.resolved_position.x);anim.color2={};anim.color2.b=0x80;anim.color2.g=0x60;anim.color2.r=0xff;anim.color2.a=anim.color1.a;anim.flag17=1;s.damage_flash=1;
        }else {if(s.damage_flash)--s.damage_flash;anim.flag17=0;}
        if(e.behavior_flags&2){
            a.boss_position(v.boss_id,{e.behavior_flags&0x10?-999.f:v.resolved_position.x+32.f,472,0});
            const u32 state=(v.flags>>1)&3;a.boss_state(v.boss_id,state?state+1:anim.flag17);
        }
        if(((v.flags&0x200)&&!priority_target)||(v.flags&0x1000))priority_target=&e;
        if(!first_target)first_target=&e;
        if((v.flags&0xc00)==0xc00){a.attack_position(field.side,v.position);priority_target=&e;}
        else if(v.flags&0x2000)priority_target=&e;
        a.update_attached_effects(e);v.lifetime.tick(timing);if(s.invulnerable.current>0)s.invulnerable.decrement(1,timing);
        if(e.behavior_flags&1)queue_draw();
    }
    for(auto& list:draw_lists)std::reverse(list.begin(),list.end());frame_time.tick(timing);return true;
}
}
