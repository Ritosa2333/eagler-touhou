#include "Enemy.hpp"
#include "EnemyDrops.hpp"
namespace th10 {
namespace {
// The inlined RNG in 0x40c9d0 consumes TWO seeds but duplicates the SECOND
// seed in both output words. Calling the normal next_u32 would change drops.
Extended drop_random(Rng& rng){
    rng.next_word();const u32 word=rng.next_word();const auto signed_value=static_cast<i32>((word<<16)|word);
    auto value=Extended::from_int(signed_value);if(signed_value<0)value=value+number(4294967296.0f);return value;
}
}
// 0x40c9d0. All twelve counters are cleared, although only eleven are emitted.
void EnemyDrops::scatter(const Vec3& position,ItemDropEnvironment& env){
    auto& rng=*env.drop_rng;
    float angle=(rng.signed_unit()*number(3.1415927410125732f)).to_float();
    for(i32 kind=1;kind<=11;++kind){
        for(i32 i=0;i<counts[kind-1];++i){
            const float x=(cosine(number(angle))*number(spread.x)).to_float();
            const float y=(sine(number(angle))*number(spread.y)).to_float();
            const auto scale=drop_random(rng)*number(0x1p-33f)+number(0.5f);
            const Vec3 drop{(number(x)*scale+number(position.x)).to_float(),
                (number((number(y)*scale).to_float())+number(position.y)).to_float(),position.z};
            env.spawn_item(drop,kind,-1,-1.5707963705062866f,2.2f);
            const auto turn=(drop_random(rng)*number(0x1p-31f)-number(1.0f))*number(0.7853981852531433f);
            angle=normalize_angle((turn+number(angle)+number(1.5707963705062866f)).to_float()).to_float();
        }
    }
    std::memset(counts,0,sizeof(counts));
}
// 0x40c9a0.
void EnemyDrops::release(const Vec3& position,ItemDropEnvironment& env){
    if(kind>0)env.spawn_item(position,kind,-1,-1.5707963705062866f,2.2f);
    scatter(position,env);kind=0;
}
// 0x40e5f0.
i32 EnemyState::destroy(EnemyDropEnvironment& env){
    if(death_sound>=0)env.play_sound(death_sound,current.position.x);
    if(death_animation>=0)env.spawn_death_animation(death_animation_file,death_animation,current.position);
    drops.release(current.position,env);env.economy->extend_faith_timer(10,env.timer_rate);return 1;
}
}
