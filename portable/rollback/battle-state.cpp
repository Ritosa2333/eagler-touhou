#include "../multiplayer/native-world.hpp"
#include "multiplayer/BattleFrameState.hpp"
#include <chrono>
#include <type_traits>
using th15::multiplayer::BattleFrameState;
struct Digest {
    u64 value=14695981039346656037ull;
    void bytes(const void* source,size_t size){const auto* p=static_cast<const u8*>(source);for(size_t i=0;i<size;++i){value^=p[i];value*=1099511628211ull;}}
    template<class T>void add(const T& v){static_assert(std::is_trivially_copyable_v<T>);bytes(&v,sizeof v);}
    void add(const BulletTransform& v){add(v.floats);add(v.integers);add(v.type);add(v.active);bytes(v.payload.data(),v.payload.size());}
    void add(const BulletShooter& v){add(v.sprite);add(v.color);add(v.position);add(v.angle);add(v.angle_step);add(v.speed);add(v.speed_step);add(v.radius);for(const auto& t:v.transforms)add(t);add(v.extra);add(v.count);add(v.rows);add(v.pattern);add(v.flags);add(v.tail);}
    void add(const EnemyShooters& v){for(const auto& s:v.shooters)add(s);add(v.offset);add(v.origin);add(v.next_transform);}
    void add(const BulletState& v){add(v.motion);add(v.transform_flags);add(v.boost);add(v.acceleration);add(v.approach);add(v.angular);add(v.reflection);add(v.wrapping);add(v.drift);add(v.position_curve);add(v.turn);add(v.saved);for(const auto& t:v.transforms)add(t);add(v.transform_index);add(v.wait);add(v.invulnerability);add(v.freeze);add(v.phase);add(v.flags);add(v.visual_flags);add(v.scale_curve);add(v.scale);add(v.lifetime);add(v.spawn);add(v.graze_flash);add(v.graze_duration);add(v.visual_jitter);add(v.graze_color);add(v.sprite_type);add(v.color);add(v.initial_transform_flags);}
    void vm(const AnmVm& v){add(v.presentation_generation);add(v.presentation_motion);add(v.presentation_part);add(v.variables);add(v.visual);add(v.interpolators);add(v.timer);add(v.age);add(v.instruction_offset);add(v.pending_interrupt);add(v.slowdown);add(v.source_script);add(v.ordering);add(v.visible);}
};
static u64 digest(World& world){
    Digest d;auto& b=*world.battle;d.add(world.environment.game_rng);d.add(world.visual);d.add(world.progress);d.add(b.session);d.add(b.spell);d.add(b.score);d.add(b.bomb_enemies);d.add(b.mp_item_targets);d.add(b.mp_gift_targets);d.add(b.mp_fragments);d.add(b.effects.handles);d.add(b.effects.cursor);
    for(auto& pilot:b.pilots)if(pilot){auto& p=*pilot;auto& player=*p.player;d.add(p.keys);d.add(p.input);d.add(p.ghost);d.add(p.ghost_clock);d.add(p.ghost_random);d.add(p.ghost_dx);d.add(p.ghost_dy);d.add(p.life_hold);d.add(p.power_taps);d.add(p.power_gap);d.add(p.life_latched);d.add(p.owned_session);d.add(p.owned_life_piece_tier);
        d.add(player.resource.header);const auto& m=player.motion;d.add(m.position_fixed);d.add(m.position);d.add(m.velocity);d.add(m.focus);d.add(m.input_frame);d.add(m.barrier_timer);d.add(m.enlargement);d.add(m.touch);
        for(const auto& o:m.options){d.add(o.active);d.add(o.target);d.add(o.position);d.add(o.normal_offset);d.add(o.focus_offset);d.add(o.snap);d.add(o.index);}
        d.add(player.life.state);d.add(player.life.age);d.add(player.life.invulnerability);d.add(player.frame.input_age);d.add(player.frame.ready_age);d.add(player.frame.bounds);d.add(player.frame.collision);d.add(player.frame.shooting.shot);d.add(player.frame.shooting.continuous);
        d.add(player.shots.shots);d.add(player.shots.reward_counters);d.add(player.damage.sources);d.add(player.damage.cursor);d.add(player.damage.score);d.vm(player.visuals.root);d.add(player.visuals.focus_handle);d.add(player.visuals.barrier_handle);d.add(player.visuals.option_handles);
        const auto& bomb=*p.bomb;d.add(bomb.position);d.add(bomb.angle);d.add(bomb.age);d.add(bomb.secondary_age);d.add(bomb.effective_against_spell);
        switch(p.character){case 0:{const auto& v=static_cast<const BombReimu&>(bomb);d.add(v.orbs);d.add(v.orbs_available);d.add(v.aura);break;}case 1:{const auto& v=static_cast<const BombMarisa&>(bomb);d.add(v.beam);d.add(v.aura);break;}case 2:{const auto& v=static_cast<const BombSanae&>(bomb);d.add(v.field);d.add(v.aura);break;}case 3:{const auto& v=static_cast<const BombReisen&>(bomb);d.add(v.barrier);d.add(v.aura);d.add(v.charges);break;}}
    }
    auto& bullets=*b.bullet_scene;d.add(bullets.manager.first_active());d.add(bullets.manager.first_free());d.add(bullets.manager.visible_count);d.add(bullets.manager.cancellation_rewards);d.add(bullets.cancellation_animations);
    for(u32 i=0;i<BulletManager::capacity;++i){d.add(*bullets.manager.state(i));if(bullets.manager.active(i)){auto* v=bullets.visual(i);d.vm(v->body);d.vm(v->overlay);d.add(*bullets.viewpoint_graze(i));}}
    for(u32 i=1;i<=ItemManager::pool_size;++i){const auto& item=*b.items->find(i);d.add(item.state);d.add(item.arrow_ended);if(item.state.state&&item.body)d.vm(*item.body);if(item.arrow)d.vm(*item.arrow);}
    d.add(b.items->requests);d.add(b.items->cancel_stamp);d.add(b.items->cancel_density);d.add(b.items->free_head(false));d.add(b.items->free_head(true));d.add(b.items->motion_scale);
    auto& registry=world.animations.registry;d.add(registry.count());d.add(registry.generation_counter());
    for(unsigned group=0;group<2;++group)for(u32 i=0;i<registry.ordered_count(group);++i){const auto handle=registry.ordered_handle(group,i);d.add(handle);d.vm(*registry.find(handle));}
    d.add(b.enemies->count());d.add(b.enemies->next_identifier);d.add(b.enemies->timer);
    for(u32 i=0;i<b.enemies->count();++i){auto& e=*b.enemies->at(i);d.add(e.state.id);d.add(e.state.motion);d.add(e.state.life);d.add(e.state.flags);d.add(e.state.shooters);d.add(e.state.animation_handles);d.add(e.state.lifetime_timer);d.add(e.state.phase_timer);d.add(e.state.integer_registers);d.add(e.state.float_registers);
        for(u32 t=0;t<e.scripts.count();++t){const auto& c=*e.scripts.at(t);d.add(c.time);d.add(c.routine);d.add(c.instruction_offset);d.add(c.stack);d.add(c.stack_pointer);d.add(c.local_base);d.add(c.interpolators);}
    }
    d.add(b.laser_scene->manager.count());d.add(b.laser_scene->manager.generation);
    b.laser_scene->manager.each([&](LaserObject& o){d.add(o.kind);d.add(o.id);d.add(o.flags);d.add(o.state);d.add(o.age);if(auto* m=b.laser_scene->moving_motion(o.id))d.add(*m);if(auto* m=b.laser_scene->stationary_motion(o.id))d.add(*m);if(auto* m=b.laser_scene->curve_motion(o.id)){d.add(m->position);d.add(m->path_age);for(const auto& p:m->points)d.add(p);}});
    return d.value;
}
static std::array<MultiplayerInput,3> input(unsigned frame){std::array<MultiplayerInput,3> out{};for(unsigned s=0;s<3;++s){out[s].held=1|((frame/37+s)%2?8:0)|((frame/91+s)%2?0x40:0x80);if(frame%173==s*13)out[s].held|=2;}return out;}
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");RetailAssets assets(argv[1]);unsigned replayed=0;double capture_ms=0,restore_ms=0;unsigned captures=0;
    for(const auto selection:{std::array<int,3>{0,1,2},std::array<int,3>{3,3,0}}){
        World world(assets,3,2,selection);auto& b=*world.battle;EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
        BattleFrameState saved;
        for(unsigned frame=0;frame<720;){
            for(unsigned n=0;n<48&&frame<720;++n,++frame)world.step(input(frame));
            if(frame>=720)break;
            b.items->spawn(2,{-90,150,0},.3f,1);b.items->spawn(9,{70,100,0},.4f,1);
            auto start=std::chrono::steady_clock::now();require(saved.capture(b),saved.error);capture_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();++captures;
            const auto before=digest(world);const auto identity=AnmVm::presentation_counter;
            std::array<u64,12> expected{};for(unsigned n=0;n<12;++n){world.step(input(frame+n));expected[n]=digest(world);}
            start=std::chrono::steady_clock::now();require(saved.restore(b),saved.error);restore_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
            // An abandoned future uses different movement and edge-triggered
            // bombs. Its allocations, damage and RNG must all disappear.
            for(unsigned n=0;n<12;++n){auto wrong=input(frame+n+99);if(n==0)for(auto& lane:wrong)lane.held|=2;world.step(wrong);}
            require(saved.restore(b),saved.error);
            require(digest(world)==before,"Full battle differs immediately after restore at "+std::to_string(frame));require(AnmVm::presentation_counter==identity,"capture/restore changed presentation identity");
            for(unsigned n=0;n<12;++n){world.step(input(frame+n));require(digest(world)==expected[n],"Full battle resimulation mismatch at "+std::to_string(frame+n));++replayed;}
            frame+=12;
        }
        saved.discard();
    }
    std::printf("PASS: %u exact retail battle resimulations; mean capture %.3f ms, restore %.3f ms\n",replayed,capture_ms/captures,restore_ms/captures);
}
