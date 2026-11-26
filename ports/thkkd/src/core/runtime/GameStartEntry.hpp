// 原scene99、Replay分配/加载/校验桥；失败保留原部分状态并返回精确转移或访问边界。
#pragma once
#include "core/runtime/GameStartEntryInitialization.hpp"
namespace kaikeidou {
namespace game_start_entry_detail {
inline void compare_flags(GameStartEntryRegisters& r,std::uint32_t a,std::uint32_t b){
 const auto value=a-b;unsigned byte=value&255u,ones=0;for(unsigned i=0;i<8;++i)ones+=(byte>>i)&1u;
 r.eflags=(r.eflags&~0x8d5u)|(a<b?1u:0u)|((ones&1u)?0u:4u)|((a^b^value)&16u)|(value==0?64u:0u)|((value>>24)&128u)|((((a^b)&(a^value))>>20)&0x800u);
}
struct EntryMachine {
 GameStartEntryMemory& memory;GameStartEntryReplayContext& replay;GameStartEntryMachineFrame& frame;GameStartEntryServices& api;
 std::vector<std::uint32_t> handles;
 EntryMachine(GameStartEntryMemory& m,GameStartEntryReplayContext& p,GameStartEntryMachineFrame& f,GameStartEntryServices& a):memory(m),replay(p),frame(f),api(a){}
 void push(std::uint32_t value){frame.registers.esp-=4u;memory.word(frame.registers.esp,value);}
 std::uint32_t pop(){const auto value=memory.word(frame.registers.esp);frame.registers.esp+=4u;return value;}
 void pushad(){const auto r=frame.registers;push(r.eax);push(r.ecx);push(r.edx);push(r.ebx);push(r.esp);push(r.ebp);push(r.esi);push(r.edi);}
 void popad(){auto& r=frame.registers;r.edi=pop();r.esi=pop();r.ebp=pop();frame.registers.esp+=4u;r.ebx=pop();r.edx=pop();r.ecx=pop();r.eax=pop();}
 void call(GameStartEntryPlatformApi which,std::initializer_list<std::uint32_t> arguments,std::uint32_t return_site){
  std::vector<std::uint32_t> args(arguments);for(auto i=args.rbegin();i!=args.rend();++i)push(*i);push(return_site);
  memory.reconcile();auto result=api.platform(which,args);memory.reconcile();
  if(which==GameStartEntryPlatformApi::virtual_alloc&&result.eax!=0){
   replay.allocations.push_back({result.eax,std::vector<std::uint8_t>(args.at(1),0)});auto& allocation=replay.allocations.back();memory.bind_runtime_bytes(allocation.address,allocation.bytes.data(),allocation.bytes.size());
  }
  for(const auto& write:result.writes)memory.write(write.address,write.bytes.data(),write.bytes.size());
  auto& r=frame.registers;r.eax=result.eax;r.ecx=result.ecx;r.edx=result.edx;
  const auto destination=pop();if(destination!=return_site)throw std::logic_error("Game start platform altered its return address");r.esp+=static_cast<std::uint32_t>(args.size()*4u);
  if(which==GameStartEntryPlatformApi::create_file&&result.eax!=0xffffffffu)handles.push_back(result.eax);
  if(which==GameStartEntryPlatformApi::close_handle&&result.eax!=0){const auto it=std::find(handles.begin(),handles.end(),args.at(0));if(it!=handles.end())handles.erase(it);}
 }
 void read(std::uint32_t destination,std::uint32_t count,std::uint32_t count_address,std::uint32_t return_site){call(GameStartEntryPlatformApi::read_file,{frame.registers.edi,destination,count,count_address,0},return_site);}
 void seek(std::uint32_t distance,std::uint32_t origin,std::uint32_t return_site){call(GameStartEntryPlatformApi::set_file_pointer,{frame.registers.edi,distance,0,origin},return_site);}
 GameStartEntryOutcome outcome(GameStartEntryOutcomeKind kind,std::uint32_t target){GameStartEntryOutcome out;out.kind=kind;out.target=target;out.registers=frame.registers;out.open_handles=handles;return out;}
};
}
inline GameStartEntryOutcome enter_game_start_scene99(GameStartEntryRefs& refs,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,bool replay_variant,unsigned precision,GameMath& math,GameStartEntryServices& services,GameStartEntryMachineFrame& frame,const GameStartEntryMemoryExtensions& extensions={}){
 using namespace game_start_entry_detail;GameStartEntryMemory memory(refs.random);bind_game_start_entry(memory,refs,gaps,replay,replay_variant);
 for(const auto& extra:extensions)memory.bind_bytes(extra.address,extra.data,extra.size);
 memory.bind_bytes(frame.stack_address,frame.stack.data(),frame.stack.size());memory.seal();EntryMachine m(memory,replay,frame,services);auto& reg=frame.registers;
 try{
  m.push(0);
  if(replay_variant){
   m.pushad();m.call(GameStartEntryPlatformApi::virtual_alloc,{0,0x100000,0x1000,0x40},0xb4201f);memory.word(0xb43100,reg.eax);
   reg.ecx=0xa74c0c;memory.word(0xb43818,reg.ecx);compare_flags(reg,memory.word(0xb4310c),0);
   if(memory.word(0xb4310c)==0){
    memory.word(0xb43a00,0);compare_flags(reg,memory.word(0xa87f48),21);memory.word(0xb43a04,memory.word(0xa87f48)==21?1u:0u);m.popad();
   }else{
    reg.eax=memory.word(0xb43940);const auto filename=memory.word(0xb4337fu+reg.eax*4u);
    m.call(GameStartEntryPlatformApi::create_file,{filename,0xc0000000,0,0,3,0x80,0},0xb426fc);reg.edi=reg.eax;m.seek(0xffffffc2u,2,0xb4270b);reg.eax=0;
    for(const auto& pair:std::array<std::array<std::uint32_t,2>,6>{{{{0xb43800,0xa7e610}},{{0xb43804,0xb43255}},{{0xb43808,0xa7e614}},{{0xb4380c,0xb3e2ec}},{{0xb43810,0xa7ea50}},{{0xb43814,0xb43a08}}}}){reg.ecx=pair[1];memory.word(pair[0],reg.ecx);}
    for(;;){memory.word(0xb43904,reg.eax);compare_flags(reg,reg.eax,7);if(reg.eax==7)break;
     const auto destination=memory.word(0xb43800u+reg.eax*4u);m.read(destination,4,0xb4390c,0xb4277b);reg.eax=memory.word(0xb43904)+1u;
    }
    m.seek(0,0,0xb427f3);m.read(memory.word(0xb43100),memory.word(0xb43a08),0xb43900,0xb4280d);
    m.seek(0xffffffdeu,2,0xb42d15);m.read(0xb43c08,4,0xb43900,0xb42d2a);m.read(0xb43c14,4,0xb43900,0xb42d3f);
    reg.eax=memory.word(0xb43c14)+memory.word(0xb43c08);reg.edx=reg.eax%119u;reg.eax/=119u;reg.ecx=119;memory.word(0xb43c0c,reg.edx);
    m.seek(0xfffffff8u,2,0xb42d66);m.read(0xb43c08,4,0xb43900,0xb42d7b);reg.eax=memory.word(0xb43c08);reg.edx=reg.eax%119u;reg.eax/=119u;reg.ecx=119;memory.word(0xb43c0c,memory.word(0xb43c0c)+reg.edx);
    m.read(0xb43c08,4,0xb43900,0xb42da4);reg.eax=memory.word(0xb43c08);memory.word(0xb43110,0);compare_flags(reg,memory.word(0xb43c0c),reg.eax);
    if(memory.word(0xb43c0c)!=reg.eax){
     compare_flags(reg,memory.word(0xb4310c),1);if(memory.word(0xb4310c)!=1)memory.word(reg.ecx+reg.edx*8u-0x4c4u,reg.eax);
     return m.outcome(GameStartEntryOutcomeKind::original_failure_transfer,0x64423d);
    }
    m.call(GameStartEntryPlatformApi::close_handle,{reg.edi},0xb4281e);m.popad();
    if(memory.word(0xb3e2ec)==7){
     memory.word(0xa8cd70,0x400);memory.word(0xa8cd80,0);memory.word(0xa8cd7c,0);
     // Omniverse 初始化读取菜单选关；在 Boss 字段清零前恢复录像保存的编号。
     const auto recorded_boss=memory.word(0xa7ea50);
     if(memory.word(0xa7e610)==5&&recorded_boss<8)memory.word(0xa87f30,recorded_boss);
    }
    else if(memory.word(0xb3e2ec)!=6&&memory.word(0xb43255)!=2)memory.word(0xb3e2ec,0);
    memory.word(0xb41215,0);memory.word(0xa87f40,0);
   }
  }
  initialize(refs,memory,memory.word(reg.esp),replay_variant,precision,math,services);reg.esp+=4u;memory.word(0xa8ce58,100);
  return m.outcome(GameStartEntryOutcomeKind::scene100,0x65345a);
 }catch(const GameStartEntryAccessFault& fault){auto out=m.outcome(GameStartEntryOutcomeKind::memory_fault,0);out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.write_fault=fault.writing;return out;}
}
}
