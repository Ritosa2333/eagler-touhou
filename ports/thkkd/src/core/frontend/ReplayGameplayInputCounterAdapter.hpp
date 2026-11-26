// 已恢复Fire/Bomb主体使用的物理计数适配；在实际查询点同步并区分原读取器。
#pragma once
#include "core/frontend/ReplayGameplayInput.hpp"
namespace kaikeidou {
template<class Memory>class ReplayGameplayInputCounterAdapter:public PlayerShotFireInput {
 enum class Scope{fire,bomb,sanae_a,sanae_c};
 Memory& memory_;PlayerShotFireContext& fire_;PlayerShotFireInput& physical_;Scope scope_=Scope::fire;std::uint32_t calls_=0;
 void begin(Scope scope){memory_.reconcile();scope_=scope;calls_=0;}
public:
 ReplayGameplayInputCounterAdapter(Memory& memory,PlayerShotFireContext& fire,PlayerShotFireInput& physical):memory_(memory),fire_(fire),physical_(physical){}
 void begin_fire(){begin(Scope::fire);}void begin_bomb(){begin(Scope::bomb);}void begin_sanae_a(){begin(Scope::sanae_a);}void begin_sanae_c(){begin(Scope::sanae_c);}
 std::uint32_t read_counter(std::uint32_t binding)override{
  memory_.reconcile();++calls_;const bool raw_replay=scope_==Scope::sanae_c||(scope_==Scope::fire&&calls_>1&&fire_.options.character==2&&fire_.options.weapon==2);
  const auto caller=scope_==Scope::bomb?0xb420e9u:raw_replay?0x5c3d21u:scope_==Scope::sanae_a||(scope_==Scope::fire&&calls_>1)?0x5c3f61u:0x5c4aa8u;
  if(fire_.replay_variant&&binding>=16)throw ReplayGameplayInputTransfer{raw_replay?0xb4241du:0xb42382u,raw_replay?0x924726u:0x924682u,raw_replay?0xb42448u:0xb423a4u,binding,caller,binding};
  const auto value=physical_.read_counter(binding);memory_.reconcile();return binding<16?value:0xffffffffu;
 }
};
}
