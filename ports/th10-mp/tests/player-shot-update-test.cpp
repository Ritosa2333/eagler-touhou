// Exact original-update oracle. Uses real movement, arithmetic, timer and registry.
// Compare full raw state at the same addresses, including callback mutations.
#include "PlayerShooting.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <initializer_list>
using namespace th10;
extern "C" {extern u8 softfloat_exceptionFlags,softfloat_detectTininess,softfloat_roundingMode,extF80_roundingPrecision;}
i32 original_update_shots(Player& player,PlayerShootingEnvironment& env);
namespace {
constexpr unsigned N=300;
struct Trace {u32 slot,animation,collided,first_collision,state,flags;};
struct State {Player player;AnmVm vms[N];AnmSprite sprite;PlayerShotDefinition defs[128];Trace traces[128];unsigned calls;};
static State state,initial,expected;
static AnmManager manager;
static AnmRegistry initial_registry,expected_registry;
static float rate;
static unsigned scenario;
u32 rng=1;
u32 random32(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
float from_bits(u32 bits){float v;std::memcpy(&v,&bits,4);return v;}
void link(bool ui,unsigned offset){
 unsigned first=ui?200:0,count=ui?100:200;
 auto*& head=ui?manager.registry.ui_head:manager.registry.world_head;
 auto*& tail=ui?manager.registry.ui_tail:manager.registry.world_tail;
 head=tail=nullptr;
 for(unsigned i=0;i<count;++i){auto& node=state.vms[first+(i+offset)%count].registry_node;node.value=&state.vms[first+(i+offset)%count];node.previous=tail;node.next=nullptr;if(tail)tail->next=&node;else head=&node;tail=&node;}
}
struct Env final:PlayerShootingEnvironment {
 void initialize_shot(Player&,PlayerShot&,i32) override {std::abort();}
 void play_shot_sound(i32,float) override {std::abort();}
 void update_shot(Player& player,PlayerShot& shot) override {
  const unsigned index=&shot-player.shots;
  state.traces[state.calls++]={index,shot.animation,static_cast<u32>(shot.collided),static_cast<u32>(shot.first_collision),static_cast<u32>(shot.state),softfloat_exceptionFlags};
  switch((scenario+index)%8){
   case 0:link(false,(index+scenario)%200);break;
   case 1:state.vms[(index*7)%N].id=shot.animation;break;
   case 2:shot.animation=state.vms[(index*11)%N].id;shot.secondary_animation=state.vms[(index*13)%N].id;break;
   case 3:if(index+1<128){player.shots[index+1].state=1;state.defs[index+1].on_update=1;player.shots[index+1].animation=state.vms[(index*17)%N].id;}break;
   case 4:if(index+2<128){player.shots[index+2].state=0;state.defs[index+2].on_update=0;}break;
   case 5:link(true,(index+scenario)%100);state.vms[(index*3)%N].id=0;break;
   case 6:if(index+4<128){player.shots[index+4].animation=0xffffffffu;state.defs[index+4].type=3;state.defs[index+4].option=2;}break;
   case 7:shot.motion.flags^=1;shot.motion.radius=.5f;shot.motion.radial_velocity=.01f;break;
  }
 }
} env;
void setup(unsigned id){
 scenario=id;rng=id*16777619u+1;std::memset(&state,0,sizeof(state));manager.registry={};rate=(id%3==0)?.75f:1.f;
 state.sprite.width=8;state.sprite.height=16;
 for(unsigned i=0;i<N;++i){auto& vm=state.vms[i];
  auto* bytes=reinterpret_cast<u8*>(&vm);for(unsigned j=0;j<sizeof(vm);++j)bytes[j]=random32();
  vm.id=(id%7==0)?((i%47)+1):((i+1)*512u+17u);vm.registry_node={};vm.child_node={&vm,nullptr,nullptr};vm.sprite=&state.sprite;vm.scale={1,1};
  vm.flags=(i%3==0)?0x8000000u:0u;
 }
 link(false,id%200);link(true,id%100);
 for(unsigned i=0;i+2<N;i+=8){auto& r=state.vms[i];auto& a=state.vms[i+1];auto& b=state.vms[i+2];r.child_node.next=&a.child_node;a.child_node.previous=&r.child_node;a.child_node.next=&b.child_node;b.child_node.previous=&a.child_node;}
 auto& player=state.player;player.option_count=2;player.fire_timer.current=(id%5)?12:-1;
 for(unsigned i=0;i<128;++i){auto& shot=player.shots[i];auto& def=state.defs[i];
  shot.state=(id%4==0&&i%7==0)?0:((i%5==0)?2:1);shot.definition=&def;
  def.type=(id%3==0&&i%9==0)?3:1;def.option=i%5;
  switch(id%8){case 0:def.on_update=0;break;case 1:def.on_update=i%2;break;case 2:def.on_update=i==0;break;case 3:def.on_update=i==127;break;case 4:def.on_update=(i==3||i==4||i==63);break;default:def.on_update=i%13==0;break;}
  shot.animation=(i%17==0)?0:((i%19==0)?0xffffffffu:state.vms[(i*7)%N].id);
  shot.secondary_animation=(id%2==0)?state.vms[(i*11+137)%N].id:0;
  shot.collided=i%2;shot.first_collision=i%3;shot.timer={0,static_cast<i32>(i%20),static_cast<float>(i%20),&rate};shot.timer_flags=1;
  shot.motion.position={static_cast<float>(int(i%11)-5)*40.f,static_cast<float>(i%17)*35.f,0};
  shot.motion.speed=(i%5)*.5f;shot.motion.angle=(i%7)*.3f;shot.motion.velocity={.5f,-.5f,0};shot.motion.radius=1;shot.motion.radial_velocity=.1f;shot.motion.flags=i%11==0?1:0;
  if(id%11==0&&i%17==1)shot.motion.position.x=from_bits(0x7fc12345);
  if(id%13==0&&i%19==2)shot.motion.position.y=from_bits(0x7f800001);
  if(id%5==0)shot.timer.rate=&shot.motion.speed;
  if(id%17==0&&i<127)shot.timer.rate=&player.shots[i+1].motion.position.x;
 }
 env.manager=&manager;env.dialogue_active=id%7==0;env.enemy_manager_present=id%9!=0;env.default_rate=&rate;
}
}
int main(){
 unsigned tests=0;
 for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})for(unsigned rounding=0;rounding<4;++rounding)for(unsigned tiny=0;tiny<2;++tiny)for(unsigned id=0;id<96;++id){
  setup(id);std::memcpy(&initial,&state,sizeof(state));initial_registry=manager.registry;
  arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;softfloat_exceptionFlags=id%32;errno=EDOM;
  const auto before_result=original_update_shots(state.player,env);std::memcpy(&expected,&state,sizeof(state));expected_registry=manager.registry;
  const unsigned flags=softfloat_exceptionFlags;const int expected_errno=errno;
  std::memcpy(&state,&initial,sizeof(state));manager.registry=initial_registry;
  arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;softfloat_exceptionFlags=id%32;errno=EDOM;
  const auto after_result=state.player.update_shots(env);
  if(before_result!=after_result||std::memcmp(&state,&expected,sizeof(state))||std::memcmp(&manager.registry,&expected_registry,sizeof(expected_registry))||flags!=softfloat_exceptionFlags||expected_errno!=errno||softfloat_detectTininess!=tiny||softfloat_roundingMode!=rounding||extF80_roundingPrecision!=static_cast<u8>(precision)){
   std::printf("FAIL case=%u precision=%u rounding=%u tiny=%u calls=%u/%u flags=%u/%u errno=%d/%d\n",id,unsigned(precision),rounding,tiny,state.calls,expected.calls,unsigned(softfloat_exceptionFlags),flags,errno,expected_errno);
   const auto* a=reinterpret_cast<const u8*>(&state);const auto* b=reinterpret_cast<const u8*>(&expected);for(unsigned i=0;i<sizeof(state);++i)if(a[i]!=b[i]){std::printf("first differing raw state byte=%u %u/%u\n",i,a[i],b[i]);break;}
   return 1;
  }
  ++tests;
 }
 std::printf("PASS %u full Player::update_shots raw-byte/registry/callback/flags/errno/mode comparisons\n",tests);
}

// Original ordered implementation, retained independently as the oracle.
#include "PlayerShooting.hpp"
using namespace th10;
namespace { void set_sprite_position(AnmVm& vm,const Vec3& point){vm.position={Scalar::add(point.x,224.f),Scalar::add(point.y,16.f),point.z};}}
i32 original_update_shots(Player& player,PlayerShootingEnvironment& env){
    auto& registry=env.manager->registry;
    for(auto& shot:player.shots)if(shot.state){
        if(shot.definition->type==3&&shot.state==1&&(player.fire_timer.current<0||shot.definition->option-1>=player.option_count)){
            registry.interrupt(shot.animation,1);registry.interrupt(shot.secondary_animation,1);shot.state=2;player.active_lasers[shot.definition->option]=0;
        }
        if(shot.definition->type==3&&shot.state==1&&(env.dialogue_active||!env.enemy_manager_present)){
            shot.state=2;registry.interrupt(shot.animation,1);registry.interrupt(shot.secondary_animation,1);player.active_lasers[shot.definition->option]=0;
        }
        if(shot.definition->type==3&&!shot.collided&&shot.state==1&&shot.first_collision==1){registry.interrupt(shot.animation,3);shot.first_collision=0;}
        shot.collided=0;if(shot.definition->on_update)env.update_shot(player,shot);
        auto& motion=shot.motion;motion.update_velocity();motion.update();
        auto* sprite=registry.find(shot.animation);
        if(!sprite){shot.state=0;registry.request_delete(shot.secondary_animation);shot.animation=shot.secondary_animation=0;continue;}
        if(shot.definition->type!=3&&shot.timer.current>=10&&outside_playfield(motion.position,
                Scalar::mul(sprite->sprite->width,sprite->scale.x),Scalar::mul(sprite->sprite->height,sprite->scale.y))){
            registry.request_delete(shot.animation);shot.animation=0;shot.state=0;continue;
        }
        set_sprite_position(*sprite,motion.position);
        if(auto* secondary=registry.find_and_clear(shot.secondary_animation))set_sprite_position(*secondary,motion.position);
        if(sprite->flags&0x8000000){sprite->rotation.z=motion.angle;sprite->flags|=4;}
        shot.timer.tick();
    }
    return 0;
}