#include "EnemyFlow.hpp"
#include "EnemyRetirement.hpp"
namespace th08 {
namespace {
constexpr const char* stage_names[]{"stg1enm.anm","stg2enm.anm","stg3enm.anm","stg4aenm.anm","stg4benm.anm","stg5enm.anm","stg6enm.anm","stg7enm.anm","stg8enm.anm"};
constexpr const char* stage_ecl[]{"ecldata1.ecl","ecldata2.ecl","ecldata3.ecl","ecldata4a.ecl","ecldata4b.ecl","ecldata5.ecl","ecldata6.ecl","ecldata7.ecl","ecldata8.ecl"};
constexpr const char* practice_ecl[]{"ecldata1sp.ecl","ecldata2sp.ecl","ecldata3sp.ecl","ecldata4asp.ecl","ecldata4bsp.ecl","ecldata5sp.ecl","ecldata6sp.ecl","ecldata7sp.ecl","ecldata8sp.ecl"};
constexpr const char* last_names[]{"stg1enm.anm","stg2enm.anm","stg3enm.anm","stg5enm.anm","stg6enm.anm","stg7enm.anm","stg8enm.anm","stg5enm.anm","stg8enm.anm","stg4aenm.anm","stg4benm.anm","stgenm_sk.anm","stgenm_ym.anm","stgenm_al.anm","stgenm_rm.anm","stgenm_yy.anm","stgenm_yk.anm"};
constexpr const char* last_ecl[]{"ecldata1sp.ecl","ecldata2sp.ecl","ecldata3sp.ecl","ecldata5sp.ecl","ecldata6sp.ecl","ecldata7sp.ecl","ecldata8sp.ecl","ecldata5sp.ecl","ecldata8sp.ecl","ecldata4asp.ecl","ecldata4bsp.ecl","ecldata_sk.ecl","ecldata_ym.ecl","ecldata_al.ecl","ecldata_rm.ecl","ecldata_yy.ecl","ecldata_yk.ecl"};
}
bool EnemyFlow::setup(){
    globals.enemy_animation_files[0]=context.initial?resources.load(7,"enemy.anm"):resources.get(7);
    if(context.initial&&!globals.enemy_animation_files[0])return false;
    const bool practice=globals.game_flags&0x4000,last=practice&&globals.current_spell>=205;
    const char* script=nullptr;
    if(context.keep_resources)globals.enemy_animation_files[1]=resources.get(8);
    else{
        if((last&&globals.current_spell>=222)||(!last&&globals.stage>=9))return false;
        globals.enemy_animation_files[1]=resources.load(8,last?last_names[globals.current_spell-205]:stage_names[globals.stage]);if(!globals.enemy_animation_files[1])return false;
        script=last?last_ecl[globals.current_spell-205]:(practice?practice_ecl:stage_ecl)[globals.stage];
    }
    for(auto& timeline:enemies.state.timelines)timeline=EclTimelineVm{};
    for(auto& value:globals.integer_arguments)value=0;for(auto& value:globals.float_arguments)value=0;globals.timeline_parameter=0;
    if(!context.keep_resources){const auto bytes=resources.ecl(script);if(!program.load(bytes.data(),bytes.size()))return false;}
    enemies.state.drops.enemies=random.bounded16(3);enemies.state.drops.item=random.bounded16(8);hide_markers();return true;
}
void EnemyFlow::release(){
    for(u32 i=0;i<480;i++)if(auto* enemy=enemies.population.at(i))cancel_enemy_async(*enemy);
    if(!context.keep_resources)resources.release(8);if(context.release_resources)resources.release(7);
    if(!context.keep_resources)program.release();hide_markers();
}
bool EnemyFlow::attach(Chain& owner){
    detach();enemies.reset();chain=&owner;
    calculation.set_callback([](void* p){return static_cast<EnemyFlow*>(p)->enemies.update();});calculation.argument=this;calculation.added=[](void* p){return static_cast<EnemyFlow*>(p)->setup()?0:-1;};calculation.deleted=[](void* p){static_cast<EnemyFlow*>(p)->release();return 0;};
    if(owner.add(&calculation,11))return false;
    high.set_callback([](void* p){return static_cast<EnemyFlow*>(p)->enemies.draw_high()?JobResult::Continue:JobResult::Error;});high.argument=this;owner.add(&high,8,true);
    low.set_callback([](void* p){return static_cast<EnemyFlow*>(p)->enemies.draw_low()?JobResult::Continue:JobResult::Error;});low.argument=this;owner.add(&low,11,true);return true;
}
void EnemyFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&high);chain->cut(&low);chain=nullptr;}}
}
