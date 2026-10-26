#pragma once
#include "../multiplayer/native-world.hpp"
#include "multiplayer/BattleFrameState.hpp"
#include <chrono>
#include <type_traits>
using th15::multiplayer::BattleFrameState;
using DigestTrace=std::vector<std::pair<std::string,u64>>;
inline DigestTrace* digest_trace=nullptr;
inline bool ignore_presentation_identity=false;
struct Digest {
    u64 value=14695981039346656037ull;
    void bytes(const void* source,size_t size){const auto* p=static_cast<const u8*>(source);for(size_t i=0;i<size;++i){value^=p[i];value*=1099511628211ull;}}
    template<class T>void add(const T& v){static_assert(std::is_trivially_copyable_v<T>);bytes(&v,sizeof v);}
    void add(const BulletTransform& v){add(v.floats);add(v.integers);add(v.type);add(v.active);bytes(v.payload.data(),v.payload.size());}
    void add(const BulletShooter& v){add(v.sprite);add(v.color);add(v.position);add(v.angle);add(v.angle_step);add(v.speed);add(v.speed_step);add(v.radius);for(const auto& t:v.transforms)add(t);add(v.extra);add(v.count);add(v.rows);add(v.pattern);add(v.flags);add(v.tail);}
    void add(const EnemyShooters& v){for(const auto& s:v.shooters)add(s);add(v.offset);add(v.origin);add(v.next_transform);}
    void add(const BulletState& v){add(v.motion);add(v.transform_flags);add(v.boost);add(v.acceleration);add(v.approach);add(v.angular);add(v.reflection);add(v.wrapping);add(v.drift);add(v.position_curve);add(v.turn);add(v.saved);for(const auto& t:v.transforms)add(t);add(v.transform_index);add(v.wait);add(v.invulnerability);add(v.freeze);add(v.phase);add(v.flags);add(v.visual_flags);add(v.scale_curve);add(v.scale);add(v.lifetime);add(v.spawn);add(v.graze_flash);add(v.graze_duration);add(v.visual_jitter);add(v.graze_color);add(v.sprite_type);add(v.color);add(v.initial_transform_flags);}
    void vm(const AnmVm& v){if(!ignore_presentation_identity)add(v.presentation_generation);if(digest_trace)digest_trace->push_back({"vm.presentation_generation",value});add(v.presentation_motion);if(digest_trace)digest_trace->push_back({"vm.presentation_motion",value});add(v.presentation_part);if(digest_trace)digest_trace->push_back({"vm.presentation_part",value});add(v.variables);if(digest_trace)digest_trace->push_back({"vm.variables",value});add(v.visual);if(digest_trace)digest_trace->push_back({"vm.visual",value});add(v.interpolators);if(digest_trace)digest_trace->push_back({"vm.interpolators",value});add(v.timer);if(digest_trace)digest_trace->push_back({"vm.timer",value});add(v.age);if(digest_trace)digest_trace->push_back({"vm.age",value});add(v.instruction_offset);if(digest_trace)digest_trace->push_back({"vm.instruction_offset",value});add(v.pending_interrupt);if(digest_trace)digest_trace->push_back({"vm.pending_interrupt",value});add(v.slowdown);if(digest_trace)digest_trace->push_back({"vm.slowdown",value});add(v.source_script);if(digest_trace)digest_trace->push_back({"vm.source_script",value});add(v.ordering);if(digest_trace)digest_trace->push_back({"vm.ordering",value});add(v.visible);if(digest_trace)digest_trace->push_back({"vm.visible",value});}
};
static u64 digest(World& world){
    Digest d;const auto mark=[&](const std::string& label){if(digest_trace)digest_trace->push_back({label,d.value});};auto& b=*world.battle;d.add(world.environment.game_rng);d.add(world.visual);d.add(world.progress);d.add(b.session);d.add(b.spell);d.add(b.score);d.add(b.bomb_enemies);d.add(b.mp_item_targets);d.add(b.mp_gift_targets);d.add(b.mp_fragments);d.add(b.effects.handles);d.add(b.effects.cursor);if(digest_trace)mark("header");
    for(auto& pilot:b.pilots)if(pilot){auto& p=*pilot;auto& player=*p.player;d.add(p.keys);d.add(p.input);d.add(p.ghost);d.add(p.ghost_clock);d.add(p.ghost_random);d.add(p.ghost_dx);d.add(p.ghost_dy);d.add(p.life_hold);d.add(p.power_taps);d.add(p.power_gap);d.add(p.life_latched);d.add(p.owned_session);d.add(p.owned_life_piece_tier);
        d.add(player.resource.header);const auto& m=player.motion;d.add(m.position_fixed);d.add(m.position);d.add(m.velocity);d.add(m.focus);d.add(m.input_frame);d.add(m.barrier_timer);d.add(m.enlargement);d.add(m.touch);
        for(const auto& o:m.options){d.add(o.active);d.add(o.target);d.add(o.position);d.add(o.normal_offset);d.add(o.focus_offset);d.add(o.snap);d.add(o.index);}
        d.add(player.life.state);d.add(player.life.age);d.add(player.life.invulnerability);d.add(player.frame.input_age);d.add(player.frame.ready_age);d.add(player.frame.bounds);d.add(player.frame.collision);d.add(player.frame.shooting.shot);d.add(player.frame.shooting.continuous);
        d.add(player.shots.shots);d.add(player.shots.reward_counters);d.add(player.damage.sources);d.add(player.damage.cursor);d.add(player.damage.score);d.vm(player.visuals.root);d.add(player.visuals.focus_handle);d.add(player.visuals.barrier_handle);d.add(player.visuals.option_handles);
        const auto& bomb=*p.bomb;d.add(bomb.position);d.add(bomb.angle);d.add(bomb.age);d.add(bomb.secondary_age);d.add(bomb.effective_against_spell);
        switch(p.character){case 0:{const auto& v=static_cast<const BombReimu&>(bomb);d.add(v.orbs);d.add(v.orbs_available);d.add(v.aura);break;}case 1:{const auto& v=static_cast<const BombMarisa&>(bomb);d.add(v.beam);d.add(v.aura);break;}case 2:{const auto& v=static_cast<const BombSanae&>(bomb);d.add(v.field);d.add(v.aura);break;}case 3:{const auto& v=static_cast<const BombReisen&>(bomb);d.add(v.barrier);d.add(v.aura);d.add(v.charges);break;}}if(digest_trace)mark("pilot:"+std::to_string(p.character));
    }
    auto& bullets=*b.bullet_scene;d.add(bullets.manager.first_active());d.add(bullets.manager.first_free());d.add(bullets.manager.visible_count);d.add(bullets.manager.cancellation_rewards);d.add(bullets.cancellation_animations);
    for(u32 i=0;i<BulletManager::capacity;++i){d.add(*bullets.manager.state(i));if(bullets.manager.active(i)){auto* v=bullets.visual(i);d.vm(v->body);d.vm(v->overlay);d.add(*bullets.viewpoint_graze(i));}if(digest_trace)mark("bullet:"+std::to_string(i));}
    for(u32 i=1;i<=ItemManager::pool_size;++i){const auto& item=*b.items->find(i);d.add(item.state);if(digest_trace)mark("item-state:"+std::to_string(i));d.add(item.arrow_ended);if(item.state.state&&item.body)d.vm(*item.body);if(digest_trace)mark("item-body:"+std::to_string(i));if(item.arrow)d.vm(*item.arrow);if(digest_trace)mark("item-arrow:"+std::to_string(i));}
    d.add(b.items->requests);d.add(b.items->cancel_stamp);d.add(b.items->cancel_density);d.add(b.items->free_head(false));d.add(b.items->free_head(true));d.add(b.items->motion_scale);
    auto& registry=world.animations.registry;d.add(registry.count());d.add(registry.generation_counter());
    std::vector<u32> ordered;for(unsigned group=0;group<2;++group){registry.ordered_handles(group,ordered);for(const auto handle:ordered){d.add(handle);d.vm(*registry.find(handle));if(digest_trace)mark("animation:"+std::to_string(handle));}}
    d.add(b.enemies->count());d.add(b.enemies->next_identifier);d.add(b.enemies->timer);
    for(u32 i=0;i<b.enemies->count();++i){auto& e=*b.enemies->at(i);d.add(e.state.id);d.add(e.state.motion);d.add(e.state.life);d.add(e.state.flags);d.add(e.state.shooters);d.add(e.state.animation_handles);d.add(e.state.lifetime_timer);d.add(e.state.phase_timer);d.add(e.state.integer_registers);d.add(e.state.float_registers);
        for(u32 t=0;t<e.scripts.count();++t){const auto& c=*e.scripts.at(t);d.add(c.time);d.add(c.routine);d.add(c.instruction_offset);d.add(c.stack);d.add(c.stack_pointer);d.add(c.local_base);d.add(c.interpolators);}if(digest_trace)mark("enemy:"+std::to_string(e.state.id));
    }
    d.add(b.laser_scene->manager.count());d.add(b.laser_scene->manager.generation);
    b.laser_scene->manager.each([&](LaserObject& o){d.add(o.kind);d.add(o.id);d.add(o.flags);d.add(o.state);d.add(o.age);if(auto* m=b.laser_scene->moving_motion(o.id))d.add(*m);if(auto* m=b.laser_scene->stationary_motion(o.id))d.add(*m);if(auto* m=b.laser_scene->curve_motion(o.id)){d.add(m->position);d.add(m->path_age);for(const auto& p:m->points)d.add(p);}if(digest_trace)mark("laser:"+std::to_string(o.id));});
    return d.value;
}
static std::array<MultiplayerInput,3> input(unsigned frame){std::array<MultiplayerInput,3> out{};for(unsigned s=0;s<3;++s){out[s].held=1|((frame/37+s)%2?8:0)|((frame/91+s)%2?0x40:0x80);if(frame%173==s*13)out[s].held|=2;}return out;}
