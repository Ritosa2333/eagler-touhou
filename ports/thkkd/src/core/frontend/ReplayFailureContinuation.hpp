// 原坏校验JMP入口的有界续接，复用生产池和数学，保留已有PUSHAD与outer EBP。
#pragma once
#include "core/frontend/ReplayFailureContinuationContext.hpp"
#include "core/runtime/GameStartEntryBindings.hpp"
#include "core/runtime/GameStartEntryMemory.hpp"
#include "core/entities/BossPhase.hpp"
namespace kaikeidou {
namespace replay_failure_detail {
using U=std::uint32_t;
struct Transfer {U target;};
inline std::uint64_t loaded_real(std::uint64_t x){return (x&0x7ff0000000000000ull)==0x7ff0000000000000ull&&(x&0xfffffffffffffull)?x|0x8000000000000ull:x;}
class Continuation {
 GameStartEntryRefs r_;GameStartEntryMemory memory_;GameStartEntryMachineFrame& frame_;BossPhaseServices& host_;SaveIO& save_;GameMath& math_;RandomPrecision precision_;const ReplayFailureContinuationPolicy& policy_;
 U pc_=0x64423d,known_=failure_ebx|failure_esp|failure_ebp|failure_esi|failure_edi;
 GameStartEntryRegisters& reg(){return frame_.registers;}
 U read(U pc,U a){pc_=pc;return memory_.word(a);}void write(U pc,U a,U v){pc_=pc;memory_.word(a,v);}
 std::uint64_t real(U pc,U a){pc_=pc;try{return loaded_real(memory_.quad(a));}catch(const GameStartEntryAccessFault&){throw GameStartEntryAccessFault{a,8,false};}}
 void push(U pc,U value){reg().esp-=4;write(pc,reg().esp,value);}
 U pop(U pc){const auto value=read(pc,reg().esp);reg().esp+=4;return value;}
 void real_push(U subtract_pc,U write_pc,std::uint64_t value){pc_=subtract_pc;reg().esp-=8;pc_=write_pc;memory_.quad(reg().esp,value);}
 void call(U pc,U target_return){push(pc,target_return);}
 void returned(U expected){const auto actual=pop(pc_);if(actual!=expected)throw Transfer{actual};}
 template<class F>auto synced(F f){memory_.reconcile();if constexpr(std::is_void_v<decltype(f())>){f();memory_.reconcile();}else{auto result=f();memory_.reconcile();return result;}}
 struct Math final:GameMath {Continuation& c;explicit Math(Continuation& x):c(x){}
  binary_float::Number sine(std::uint64_t x)override{return c.synced([&]{return c.math_.sine(x);});}
  binary_float::Number cosine(std::uint64_t x)override{return c.synced([&]{return c.math_.cosine(x);});}
  binary_float::Number atan2(std::uint64_t y,std::uint64_t x)override{return c.synced([&]{return c.math_.atan2(y,x);});}
 } math_api_;
 struct IO final:SaveIO {Continuation& c;explicit IO(Continuation& x):c(x){}
  bool open(std::string_view p,bool w)override{return c.synced([&]{return c.save_.open(p,w);});}
  std::size_t read(std::uint8_t* p,std::size_t n)override{return c.synced([&]{return c.save_.read(p,n);});}
  void write(const std::uint8_t* p,std::size_t n)override{c.synced([&]{c.save_.write(p,n);});}
  void close()override{c.synced([&]{c.save_.close();});}
 } io_;
 struct DeathServices final:EnemyDeathServices {Continuation& c;explicit DeathServices(Continuation& x):c(x){}
  U color(U r,U g,U b)override{return c.synced([&]{return c.host_.color(r,g,b);});}
  void advance_phase(U mode)override{c.synced([&]{c.host_.advance_phase(mode);});}
 } death_api_;
 U enemy_address(U index,U offset)const{return 0xa87fe8u+index*0xf8u+offset;}
 void scatter_boss(U count,U item_kind,std::uint64_t x,std::uint64_t y){
  using namespace binary_float;using namespace enemy_death_detail;const auto bits=static_cast<unsigned>(precision_);
  for(U i=0;i<count;++i){const auto slot=find_free_item(r_.items);if(slot==-1)continue;auto& item=r_.items.items[static_cast<std::size_t>(slot)];set_item_base(item,0,item_kind);item.quad(0x18,x);if(enemy_death_detail::real(x)<0.)item.quad(0x18,0);else if(enemy_death_detail::real(x)>384.)item.quad(0x18,0x4078000000000000ull);item.quad(0x20,y);
   const auto angle=degrees(r_.random.real_number(0x4076700000000000ull,precision_),bits);const auto velocity=add(r_.random.real_number(0x4040000000000000ull,precision_),integer(48),bits);const auto speed=to_binary64(divide(velocity,integer(20),bits));item.quad(0x28,to_binary64(multiply(math_api_.cosine(angle),binary64(speed),bits)));item.quad(0x30,to_binary64(multiply(math_api_.sine(angle),binary64(speed),bits)));item.quad(0x38,0xc008000000000000ull);
  }
 }
 void bounded_drop(U index){
  using namespace enemy_death_detail;
  reg().eax=index*0xf8u;known_|=failure_eax;
  const auto kind=read(0x6596cf,enemy_address(index,4));
  known_&=~failure_eax;
  const bool other_alias=signed_word(kind)>=10&&r_.death.stage==7&&r_.death.part==7&&r_.death.character==1&&r_.patterns.integers[0]>=80;
  bool clamp_detail=false;
  if(index<80&&!other_alias&&(kind<=4||signed_word(kind)>=10)){auto x=r_.enemies.enemies[index].quad(0x38);if(signed_word(kind)>=10){if(r_.death.stage==6&&r_.death.part==3&&(r_.death.subphase==17||r_.death.subphase==18))x=0x4068000000000000ull;else if(r_.death.stage==7&&r_.death.part==7)x=r_.death.character==1?r_.enemies.enemies[r_.patterns.integers[0]].quad(0x38):0x4068000000000000ull;}clamp_detail=enemy_death_detail::real(x)<0.||enemy_death_detail::real(x)>384.;}
  if(index<80&&!other_alias&&!clamp_detail){synced([&]{drop_enemy_items(r_.items,r_.enemies,index,r_.player,r_.patterns,r_.death,r_.random,math_api_,precision_);});known_&=~failure_eax;return;}
  const auto bits=static_cast<unsigned>(precision_);
  if(signed_word(kind)>=10){
   std::uint64_t x,y;
   if(r_.death.stage==6&&r_.death.part==3&&(r_.death.subphase==17||r_.death.subphase==18)){x=0x4068000000000000ull;y=0x4059000000000000ull;}
   else if(r_.death.stage==7&&r_.death.part==7){if(r_.death.character==1){const auto other=r_.patterns.integers[0];x=real(0x65975b,enemy_address(other,0x38));y=real(0x659770,enemy_address(other,0x40));}else{x=0x4068000000000000ull;y=r_.death.character==0?0x4054000000000000ull:0x406c000000000000ull;}}
   else{x=real(0x659798,enemy_address(index,0x38));y=real(0x6597aa,enemy_address(index,0x40));}
   synced([&]{fixed_three(r_.items,x,y,0,bits);scatter_boss(20,3,x,y);scatter_boss(10,0,x,y);});
  }
  const auto following_kind=read(0x659be9,enemy_address(index,4));
  if(following_kind==5){if(signed_word(read(0x659bff,enemy_address(index,0x1c)))>0){for(U i=0;i<3;++i){const auto slot=find_free_item(r_.items);if(slot==-1)continue;auto& item=r_.items.items[static_cast<std::size_t>(slot)];item.word(0,1);item.word(0x10,special_five(r_.death)?1u:0u);item.word(4,0);item.word(0xc,0);item.word(8,i?5u:7u);
    if(i){const auto x=real(0x659cd2,enemy_address(index,0x38));const auto shifted=binary_float::add(binary_float::integer(i*48u),binary_float::binary64(x),bits);item.quad(0x18,binary_float::to_binary64(binary_float::add(shifted,binary_float::integer(0xffffffb8u),bits)));const auto y=real(0x659cf3,enemy_address(index,0x40));item.quad(0x20,sum(y,0x4034000000000000ull,bits));}
    else{item.quad(0x18,real(0x659d2c,enemy_address(index,0x38)));const auto y=real(0x659d41,enemy_address(index,0x40));item.quad(0x20,sum(y,0xc010000000000000ull,bits));}
    item.quad(0x28,0);item.quad(0x30,0);item.quad(0x38,0xc008000000000000ull);
   }}known_&=~failure_eax;return;}
  U item_kind;
  switch(following_kind){case 0:item_kind=special_five(r_.death)||signed_word(r_.player.word(8))<400?0u:4u;break;case 1:item_kind=3;break;case 2:item_kind=4;break;case 3:item_kind=5;break;case 4:item_kind=7;break;default:known_&=~failure_eax;return;}
  for(U i=0;signed_word(i)<signed_word(read(0x659e3e,enemy_address(index,0x1c)));++i){const auto slot=find_free_item(r_.items);if(slot==-1)continue;auto& item=r_.items.items[static_cast<std::size_t>(slot)];set_item_base(item,special_five(r_.death)?1u:0u,item_kind);const auto x=real(0x659ee7,enemy_address(index,0x38));item.quad(0x18,x);if(enemy_death_detail::real(x)<0.)item.quad(0x18,0);else if(enemy_death_detail::real(x)>384.)item.quad(0x18,0x4078000000000000ull);item.quad(0x20,real(0x659f52,enemy_address(index,0x40)));
   std::uint64_t angle=0,speed=0;if(read(0x659f67,enemy_address(index,0x1c))!=1){angle=degrees(r_.random.real_number(0x4076700000000000ull,precision_),bits);speed=binary_float::to_binary64(binary_float::divide(r_.random.real_number(0x404e000000000000ull,precision_),binary_float::integer(20),bits));}
   item.quad(0x28,binary_float::to_binary64(binary_float::multiply(math_api_.cosine(angle),binary_float::binary64(speed),bits)));item.quad(0x30,binary_float::to_binary64(binary_float::multiply(math_api_.sine(angle),binary_float::binary64(speed),bits)));item.quad(0x38,0xc008000000000000ull);
  }
  known_&=~failure_eax;
 }
 void drop(U index,U return_address){
  push(0x6442b6,index);call(0x6442b7,return_address);push(0x6596c0,reg().ebp);reg().ebp=reg().esp;reg().esp-=0x54;
  bounded_drop(read(0x6596c6,reg().ebp+8));reg().esp=reg().ebp;reg().ebp=pop(0x65a011);returned(return_address);reg().esp+=4;
 }
 void clear(U mode,U push_pc,U call_pc,U return_pc){push(push_pc,mode);call(call_pc,return_pc);reg().eax=static_cast<U>(synced([&]{return clear_projectiles(r_.projectiles,static_cast<std::int32_t>(mode));}));known_|=failure_eax;returned(return_pc);reg().esp+=4;}
 void bounded_death(U index){
  known_&=~failure_eax;
  if(index<80){synced([&]{resolve_enemy_death(r_.enemies,index,r_.items,r_.effects,r_.shake,r_.projectiles,r_.player,r_.patterns,r_.death,r_.random,math_api_,precision_,death_api_);});known_&=~failure_eax;return;}
  // 进入该调用的原筛选仅允许普通敌人与100以上效果敌人，boss类型由调用前分支排除。
  const auto address=enemy_address(index,0);if(enemy_death_detail::signed_word(read(0x65a960,address+0x14))>=1)return;
  const auto kind=read(0x65a976,address+4);const bool upper=enemy_death_detail::signed_word(kind)>=100;write(upper?0x65a98c:0x65b283,address,0);
  const auto color_index=upper?kind-100u:kind;
  if(color_index<6){constexpr std::array<std::array<U,3>,6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,128,0}},{{255,0,255}}}};constexpr std::array<U,6> upper_y{{0x65a9cb,0x65aa16,0x65aa61,0x65aaac,0x65aafa,0x65ab45}},lower_y{{0x65b2bf,0x65b30a,0x65b355,0x65b3a0,0x65b3ee,0x65b439}};const auto ypc=upper?upper_y[color_index]:lower_y[color_index];const auto saved_sp=reg().esp;reg().esp-=8;const auto y=real(ypc,address+0x40);memory_.quad(reg().esp,y);reg().esp-=8;const auto x=real(ypc+0x15,address+0x38);memory_.quad(reg().esp,x);const auto& rgb=colors[color_index];const auto ink=death_api_.color(rgb[0],rgb[1],rgb[2]);synced([&]{allocate_effect(r_.effects,r_.projectiles.sound_triggers,{0,ink,x,y});});reg().esp=saved_sp;}
  if(!upper){
   if(enemy_death_detail::special_five(r_.death)&&read(0x65b4a6,address+0x1c)){
    const auto live_kind=read(0x65b4bc,address+4);
    if(live_kind<=5){constexpr std::array<std::array<U,3>,6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,255,0}},{{255,0,255}}}};constexpr std::array<std::uint64_t,6> spacing{{0x3f947ae147ae147bull,0x3fb1eb851eb851ecull,0x3fbeb851eb851eb8ull,0x3fa999999999999aull,0x3fa999999999999aull,0x3fe8000000000000ull}};ProjectileEmission emission;emission.allocation={live_kind<3?4u:live_kind-3u,0,colors[live_kind],live_kind==5?2u:0u,255,1,41};emission.x=memory_.quad(address+0x38);emission.y=memory_.quad(address+0x40);emission.count=live_kind<3?r_.patterns.integers[2]:(live_kind==3?r_.patterns.integers[3]:(live_kind==4?r_.patterns.integers[4]:r_.death.pattern_integer_5));emission.spacing=spacing[live_kind];emission.speed=r_.patterns.reals[0];synced([&]{emit_projectiles(r_.projectiles,emission,r_.player.quad(0x90),r_.player.quad(0x98),r_.patterns,r_.random,precision_,math_api_);});}
    if(!r_.player.word(0x2c))bounded_drop(index);
   }else bounded_drop(index);
  }
  known_&=~failure_eax;
 }
 void run(){
  reg().ecx=read(0x644290,0xa8cd78);reg().ecx+=read(0x644296,0xa7eb98);write(0x64429c,0xa8cd78,reg().ecx);
  call(0x6442a2,0x6442a7);synced([&]{store_spell_records(r_.savedata,io_);});known_&=~failure_eax;returned(0x6442a7);
  if(read(0x6442a7,0xa7ea9c)==0)drop(read(0x6442b0,0xa7ea40),0x6442bc);
  reg().eax=read(0x6442bf,0xa7ea40)*0xf8u;known_|=failure_eax;reg().esp-=8;const auto y=real(0x6442cd,reg().eax+0xa88028u);pc_=0x6442d3;memory_.quad(reg().esp,y);
  reg().ecx=read(0x6442d6,0xa7ea40)*0xf8u;reg().esp-=8;const auto x=real(0x6442e5,reg().ecx+0xa88020u);pc_=0x6442eb;memory_.quad(reg().esp,x);
  push(0x6442ee,255);push(0x6442f3,255);push(0x6442f8,255);call(0x6442fd,0x644302);reg().eax=synced([&]{return host_.color(255,255,255);});known_|=failure_eax;returned(0x644302);reg().esp+=12;
  push(0x644305,reg().eax);push(0x644306,2);call(0x644308,0x64430d);
  const auto kind=read(0x65ca60,reg().esp+4),ink=read(0x65ca60,reg().esp+8);const auto fx=real(0x65ca60,reg().esp+12),fy=real(0x65ca60,reg().esp+20);
  reg().eax=static_cast<U>(synced([&]{return allocate_effect(r_.effects,r_.projectiles.sound_triggers,{kind,ink,fx,fy});}));returned(0x64430d);reg().esp+=24;
  if(read(0x644310,0xb3e2ec)==6&&read(0x644319,0xa77390)==1&&read(0x644322,0xa733c0)==2){
   clear(0,0x64432b,0x64432d,0x644332);write(0x644335,reg().ebp-4,0);
   while(enemy_death_detail::signed_word(read(0x644347,reg().ebp-4))<3000){reg().eax=read(0x644350,reg().ebp-4)*0xf0u;
    if(read(0x644359,reg().eax+0xa8e664u)==18){reg().ecx=read(0x644362,reg().ebp-4)*0xf0u;write(0x64436b,reg().ecx+0xa8e668u,25);break;}
    reg().edx=read(0x64433e,reg().ebp-4)+1u;write(0x644344,reg().ebp-4,reg().edx);
   }
  }else{clear(100,0x64437b,0x64437d,0x644382);clear(101,0x644385,0x644387,0x64438c);}
  write(0x64461c,0xa7ea98,0);write(0x644626,0xa7eb3c,0);write(0x644630,reg().ebp-12,0);
  while(enemy_death_detail::signed_word(read(0x644642,reg().ebp-12))<80){
   reg().ecx=read(0x644648,reg().ebp-12)*0xf8u;
   if(read(0x644651,reg().ecx+0xa87fe8u)==1){reg().edx=read(0x64465a,reg().ebp-12)*0xf8u;const auto kind0=enemy_death_detail::signed_word(read(0x644663,reg().edx+0xa87fecu));bool remove=kind0<=9;
    if(!remove){reg().eax=read(0x64466c,reg().ebp-12)*0xf8u;remove=enemy_death_detail::signed_word(read(0x644675,reg().eax+0xa87fecu))>=100;}
    if(remove){reg().ecx=read(0x64467e,reg().ebp-12)*0xf8u;write(0x644687,reg().ecx+0xa87ffcu,0);reg().edx=read(0x644691,reg().ebp-12)*0xf8u;write(0x64469a,reg().edx+0xa88004u,0);reg().eax=read(0x6446a4,reg().ebp-12);push(0x6446a7,reg().eax);call(0x6446a8,0x6446ad);push(0x65a950,reg().ebp);reg().ebp=reg().esp;reg().esp-=0x38;push(0x65a956,reg().esi);bounded_death(read(0x65a957,reg().ebp+8));reg().esi=pop(0x65b7d9);reg().esp=reg().ebp;reg().ebp=pop(0x65b7dc);returned(0x6446ad);reg().esp+=4;}
   }
   reg().eax=read(0x644639,reg().ebp-12)+1u;known_|=failure_eax;write(0x64463f,reg().ebp-12,reg().eax);
  }
  reg().esp=reg().ebp;reg().ebp=pop(0x6446b4);const auto target=pop(0x6446b5);throw Transfer{target};
 }
public:
 Continuation(GameStartEntryRefs refs,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,GameStartEntryMachineFrame& frame,unsigned precision,GameMath& math,BossPhaseServices& services,SaveIO& save,const ReplayFailureContinuationPolicy& policy,const GameStartEntryMemoryExtensions& extras):r_(refs),memory_(refs.random),frame_(frame),host_(services),save_(save),math_(math),precision_(static_cast<RandomPrecision>(precision)),policy_(policy),math_api_(*this),io_(*this),death_api_(*this){bind_game_start_entry(memory_,r_,gaps,replay,true);memory_.bind_bytes(frame.stack_address,frame.stack.data(),frame.stack.size());for(auto x:extras)memory_.bind_bytes(x.address,x.data,x.size);memory_.seal();}
 ReplayFailureContinuationOutcome execute(){ReplayFailureContinuationOutcome out;memory_.reconcile();try{run();}catch(const Transfer& transfer){out.target=transfer.target;out.address=transfer.target;out.pc=transfer.target;out.bytes=1;out.kind=std::find(policy_.registered_return_targets.begin(),policy_.registered_return_targets.end(),transfer.target)!=policy_.registered_return_targets.end()?ReplayFailureContinuationKind::returned_target:ReplayFailureContinuationKind::fetch_transfer;}catch(const GameStartEntryAccessFault& fault){out.kind=fault.writing?ReplayFailureContinuationKind::write_fault:ReplayFailureContinuationKind::read_fault;out.address=fault.address;out.bytes=fault.bytes;out.pc=pc_;}out.registers=reg();out.exact_registers=known_;out.open_replay_handles=policy_.open_replay_handles;return out;}
};
}
inline ReplayFailureContinuationOutcome continue_replay_failure(GameStartEntryRefs refs,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,GameStartEntryMachineFrame& frame,unsigned precision,GameMath& math,BossPhaseServices& services,SaveIO& save,const ReplayFailureContinuationPolicy& policy,const GameStartEntryMemoryExtensions& extensions={}){replay_failure_detail::Continuation c(refs,gaps,replay,frame,precision,math,services,save,policy,extensions);return c.execute();}
}
