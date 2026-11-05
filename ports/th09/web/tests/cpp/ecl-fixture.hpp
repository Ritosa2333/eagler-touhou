#pragma once
#include "../../cpp/game/EclVariables.hpp"
#include "../../cpp/game/EclProgram.hpp"
#include "../../cpp/game/EclVm.hpp"
namespace ecl_test {
using namespace th09;
struct Fixture {
    EclVariables variables;EclLocals locals;EclWorldState world;EclPlayfieldState field,opponent;
    Fixture(){variables.world=&world;variables.field=&field;variables.opponent=&opponent;variables.locals=&locals;}
};
struct Field {u32 base,original,offset,size;};
#define EVF(base,original,field) {base,original,u32(offsetof(Fixture,variables)+offsetof(EclVariables,field)),sizeof(EclVariables::field)}
#define EGF(base,original,section,type,field) {base,original,u32(offsetof(Fixture,section)+offsetof(type,field)),sizeof(type::field)}
inline const Field fields[]={
    EVF(0,0x2ce8,shared_integer),EVF(0,0x2d08,shared_real),EVF(0,0x2d74,position),EVF(0,0x2dd4,resolved_position),
    EVF(0,0x2e1c,origin),EVF(0,0x2e10,target),EVF(0,0x2db0,last_delta),EVF(0,0x2de0,direction),EVF(0,0x2de4,angular_velocity),
    EVF(0,0x2df4,speed),EVF(0,0x2df8,acceleration),EVF(0,0x2dfc,orbit_radius),EVF(0,0x2de8,orbit_angle),EVF(0,0x2dec,orbit_velocity),
    {0,0x2e6c,offsetof(Fixture,variables)+offsetof(EclVariables,lifetime)+offsetof(Timer,current),4},EVF(0,0x2e48,life),EVF(0,0x33ac,last_damage),EVF(0,0x33b0,life_thresholds),EVF(0,0x335c,item_reward),EVF(0,0x2e54,score_reward),EVF(0,0x3380,flags),EVF(0,0x336b,boss_id),
    EVF(0,0x3360,drop_count),EVF(0,0x3364,drop_item),
    {1,0x1c,offsetof(Fixture,locals),sizeof(EclLocals)},
    EGF(5,0x4ace0c,world,EclWorldState,random),EGF(5,0x4a7eac,world,EclWorldState,difficulty),EGF(5,0x4a7e44,world,EclWorldState,rank),
    EGF(3,0x1b88,field,EclPlayfieldState,player),EGF(6,0x20,field,EclPlayfieldState,character),
    EGF(2,0x168,field,EclPlayfieldState,integer_arguments),EGF(2,0x178,field,EclPlayfieldState,float_arguments),
    EGF(4,0xa0,opponent,EclPlayfieldState,attack_levels)
};
struct VmFixture {
    struct Emissions:BulletEmissionActions,LaserEmissionActions,EnemyAnimationActions {
        std::vector<BulletEmission> events;
        std::vector<BulletEmission> laser_events;std::array<Laser,64> laser_pool;u32 next_laser=0;
        std::vector<std::array<i32,3>> animation_events;
        bool emit(const BulletEmission& p)override{events.push_back(p);return true;}
        void clear(i32)override{}
        Laser* emit_laser(const BulletEmission& p)override{
            if(next_laser>=laser_pool.size())return nullptr;laser_events.push_back(p);auto& l=laser_pool[next_laser++];
            l.position=p.position;l.direction=p.angle;l.width=p.laser.width;l.active_width=3.5f;l.active=1;return &l;
        }
        bool start_animation(EclVm& vm,u32 slot,bool alternate,i32 script)override{animation_events.push_back({i32(slot),i32(alternate),script});vm.animation.layers[slot].scriptIndex=i16(script);return true;}
    } emissions;
    EclVm vm;EclProgram program;EclWorldState world;EclPlayfieldState field,opponent;
    AnmLoadedSprite test_sprite{};
    VmFixture(){vm.values.world=&world;vm.values.field=&field;vm.values.opponent=&opponent;vm.bind_context();}
    u8* field_pointer(u32 index){
        if(index>=sizeof(fields)/sizeof(Field))return nullptr;const auto& f=fields[index];
        if(f.base==0)return reinterpret_cast<u8*>(&vm.values)+f.offset;
        if(f.base==1)return reinterpret_cast<u8*>(&vm.primary.locals);
        if(f.base==5)return reinterpret_cast<u8*>(&world)+f.offset-offsetof(Fixture,world);
        if(f.base==4)return reinterpret_cast<u8*>(&opponent)+f.offset-offsetof(Fixture,opponent);
        return reinterpret_cast<u8*>(&field)+f.offset-offsetof(Fixture,field);
    }
    EclContext* context(i32 slot){return slot<0?&vm.primary:slot<4?vm.asynchronous[slot].get():nullptr;}
};
struct ExtraField {u32 original,offset,size;};
#define EMF(original,field) {original,offsetof(EclVm,movement)+offsetof(EnemyMotion,field),sizeof(EnemyMotion::field)}
#define EBF(original,field) {original,offsetof(EclVm,emitter)+offsetof(EclEmitter,field),sizeof(EclEmitter::field)}
#define ELF(original,field) {original,offsetof(EclVm,lasers)+offsetof(EclLasers,field),sizeof(EclLasers::field)}
#define EAF(original,field) {original,offsetof(EclVm,animation)+offsetof(EnemyAnimation,field),sizeof(EnemyAnimation::field)}
#define ESF(original,field) {original,offsetof(EclVm,status)+offsetof(EnemyStatus,field),sizeof(EnemyStatus::field)}
#define ETF(original,field) {original,offsetof(EclVm,trail)+offsetof(EnemyTrail,field),sizeof(EnemyTrail::field)}
inline const ExtraField extra_fields[]={
    {0x337c,offsetof(EclVm,behavior_flags),4},EMF(0x2e28,time),EMF(0x2e34,duration),EMF(0x2e00,orbit_growth),EMF(0x3398,bounds),
    EMF(0x2dbc,hitbox),EMF(0x2dc8,low_damage_hitbox),EMF(0x33a8,player_protect_squared),
    EMF(0x2d98,capture_velocity),EMF(0x2da4,previous_position),EMF(0x5424,capture_time),
    EBF(0x2e74,parameters),EBF(0x2e04,offset),EBF(0x3088,repeated),EBF(0x30b4,period),EBF(0x30b8,time),ELF(0x30c4,parameters),ELF(0x3358,selected),
    EAF(0x338a,idle),EAF(0x3390,left),EAF(0x3392,right),EAF(0x338c,stop_left),EAF(0x338e,stop_right),EAF(0x3394,death),EAF(0x3386,pose),
    {0x222,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+offsetof(AnmVm,scriptIndex),2},
    {0x4c6,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)+offsetof(AnmVm,scriptIndex),2},
    {0x76a,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)*2+offsetof(AnmVm,scriptIndex),2},
    ESF(0x2e4c,initial_life),ESF(0x2e50,maximum_life),ESF(0x33c0,life_subroutines),ESF(0x33d0,timeout),ESF(0x33d4,timeout_subroutine),ESF(0x2d2e,death_subroutine),
    ESF(0x3387,draw_group),ESF(0x3368,effects),ESF(0x53a8,invulnerable),ESF(0x5414,attached_effect_count),ESF(0x5418,attached_effect_radius),ESF(0x2e38,motion_range),ESF(0x2e40,motion_modes),
    ESF(0x2e58,index),ESF(0x2e70,saved_color),
    ESF(0x2e5c,shot_damage),ESF(0x5420,remaining_seconds),ESF(0x336c,damage_flash),
    ETF(0x53a0,flags),ETF(0x53a2,length),ETF(0x53a4,collision_length),ETF(0x53a6,interval),
    {0x2e64,offsetof(EclVm,values)+offsetof(EclVariables,lifetime),8},
    {0x206,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+offsetof(AnmVm,pendingInterrupt),2},
    {0x4aa,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)+offsetof(AnmVm,pendingInterrupt),2},
    {0x74e,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)*2+offsetof(AnmVm,pendingInterrupt),2},
    {0x10,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+offsetof(AnmVm,rotation)+8,4}
};
#undef ESF
#undef ETF
#undef EAF
#undef ELF
#undef EBF
#undef EMF
#undef EVF
#undef EGF
}
