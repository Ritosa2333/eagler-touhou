#pragma once
#include "../../cpp/game/BombReimu.hpp"
#include "../../cpp/game/BombMarisa.hpp"
#include "../../cpp/game/BombYoumu.hpp"
struct BombFixture final:BombHost {
    AnmManager& animations;PlayerMotion motion;Timer invulnerability;PlayerLifeSession session;PlayerSpellStatus spell;DamageSources damage;
    u32 roaring_flags=0;i32 enemy_count=1;BombContext context;std::unique_ptr<BombController> bomb;BombTarget target;
    std::vector<u32> audio,cancellations,laser_cancellations,screen_effects;std::array<u32,55> orb_view{};
    BombFixture(AnmManagerFixture& a,i32 character):animations(a.manager),context{a.manager,motion,invulnerability,session,spell,damage,a.environment.game_rng,*this,roaring_flags,enemy_count}{
        if(character==0)bomb=std::make_unique<BombReimu>(context);else if(character==1)bomb=std::make_unique<BombMarisa>(context);else bomb=std::make_unique<BombYoumu>(context);
    }
    bool sound(i32 id,bool queued)override{audio.insert(audio.end(),{u32(id),u32(queued)});return true;}
    bool bomb_hud(i32,i32)override{return true;}
    bool screen_effect(const BombScreenEffect& r)override{screen_effects.insert(screen_effects.end(),{u32(r.kind),u32(r.strength),u32(r.rise),u32(r.hold),u32(r.fall)});return true;}
    bool cancel_bullets(const Vec3& p,float radius,i32 reward,bool honor)override{cancellations.insert(cancellations.end(),{u32(honor),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),0,0,u32(reward)});return true;}
    bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool honor)override{laser_cancellations.insert(laser_cancellations.end(),{0,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),0,0,u32(reward),u32(honor)});return true;}
    bool cancel_bullets_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward)override{cancellations.insert(cancellations.end(),{2,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(size.x),float_to_bits(size.y),float_to_bits(angle),u32(reward)});return true;}
    bool cancel_lasers_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{laser_cancellations.insert(laser_cancellations.end(),{1,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(size.x),float_to_bits(size.y),float_to_bits(angle),u32(reward),u32(honor)});return true;}
    bool nearest_target(const Vec3& p,float range,BombTarget& out)override{out={};if(target.id&&!(target.flags&0xc000021)){const float x=float(p.x-target.position.x),y=float(p.y-target.position.y);if(float(float(x*x)+float(y*y))<float(range*range))out=target;}return true;}
};
API(bomb_create) BombFixture* bomb_create(AnmManagerFixture* a,i32 character){return new BombFixture(*a,character);}
API(bomb_delete) void bomb_delete(BombFixture* f){delete f;}
API(bomb_begin) i32 bomb_begin(BombFixture* f){return f->bomb->begin();}
API(bomb_allowed) i32 bomb_allowed(BombFixture* f){return f->bomb->allowed();}
API(bomb_update) i32 bomb_update(BombFixture* f,float rate){return f->bomb->update(rate);}
API(bomb_stop) i32 bomb_stop(BombFixture* f){return f->bomb->stop();}
API(bomb_damage_tick) void bomb_damage_tick(BombFixture* f,float rate){f->damage.tick(rate);}
API(bomb_error) const char* bomb_error(BombFixture* f){return f->bomb->error.c_str();}
API(bomb_field) void* bomb_field(BombFixture* f,u32 kind){auto& b=*f->bomb;switch(kind){case 0:return &f->session;case 1:return &f->spell;case 2:return &f->motion.position;case 3:return &f->motion.velocity;case 4:return &f->invulnerability;case 5:return &f->motion.behavior_flags;case 6:return &f->motion.movement_scale;case 7:return &b.position;case 8:return &b.angle;case 9:return &b.age;case 10:return &b.main_handle;case 11:return &b.secondary_handle;case 12:return &b.aura_handle;case 13:return &b.failed_spell;case 14:return f->damage.sources.data();case 15:return &f->damage.cursor;case 16:return &f->roaring_flags;case 17:return &f->enemy_count;case 18:return &f->target;default:return nullptr;}}
API(bomb_youmu_field) void* bomb_youmu_field(BombFixture* f,u32 kind){auto& b=static_cast<BombYoumu&>(*f->bomb);switch(kind){case 0:return b.handles.data();case 1:return b.sides.data();case 2:return b.heights.data();case 3:return b.timers.data();case 4:return &b.emitted;case 5:return &b.interval;case 6:return &b.spawn_age;default:return nullptr;}}
API(bomb_orb_field) void* bomb_orb_field(BombFixture* f,u32 index){auto& b=static_cast<BombReimu&>(*f->bomb);if(!b.orbs||index>=24)return nullptr;const auto& orb=(*b.orbs)[index];auto* p=f->orb_view.data();std::memset(p,0,220);p[0]=orb.handle;std::memcpy(p+1,&orb.motion,68);p[40]=orb.active;if(orb.age.flags)std::memcpy(p+41,&orb.age,20);std::memcpy(p+46,&orb.last_velocity,12);p[49]=orb.target;p[51]=orb.index;p[52]=orb.damage_id;p[53]=orb.retired;p[54]=float_to_bits(orb.turn_speed);return p;}
API(bomb_events) u32* bomb_events(BombFixture* f,u32 kind){return kind==0?f->audio.data():kind==1?f->cancellations.data():kind==2?f->screen_effects.data():f->laser_cancellations.data();}
API(bomb_event_count) u32 bomb_event_count(BombFixture* f,u32 kind){return kind==0?f->audio.size():kind==1?f->cancellations.size():kind==2?f->screen_effects.size():f->laser_cancellations.size();}
