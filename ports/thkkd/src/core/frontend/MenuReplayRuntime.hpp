// Win32保存短链和共用调用约定；所有原指针通过模板Memory访问。
#pragma once
#include "core/frontend/MenuReplayServices.hpp"
#include "core/shared/replayfileruntime.hpp"
#include <array>
namespace kaikeidou {
struct MenuReplaySaveOutcome {bool file_opened=false,pushad_restored=false;MenuReplayRegisters registers;};
template<class Memory>class MenuReplayRuntime {
public:
 Memory& memory;MenuReplayServices& services;MenuReplayRegisters registers;
 MenuReplayRuntime(Memory& m,MenuReplayServices& s,MenuReplayRegisters r={}):memory(m),services(s),registers(r){}
 std::uint32_t accept(const MenuReplayPlatformResult& result){
  memory.reconcile();
  for(const auto& write:result.writes)memory.write(write.address,write.bytes);memory.reconcile();
  registers.eax=result.eax;registers.ecx=result.ecx;registers.edx=result.edx;return result.eax;
 }
 std::uint32_t call(MenuReplayApi api,const std::vector<std::uint32_t>& args,MenuReplayFormatMemory* stack=nullptr){return accept(services.call(api,args,stack));}
 std::uint32_t input(std::uint32_t binding_address){const auto binding=memory.word(binding_address);return call(MenuReplayApi::input_count,{binding});}
 void volume(){const auto music=memory.word(0xa7ebfc);call(MenuReplayApi::music_volume,{music});const auto sound=memory.word(0xa7ec00);call(MenuReplayApi::sound_volume,{sound});}
 void reset_name(){memory.word(0xb41215,0);memory.word(0xb43015,0x2d2d2d2d);memory.word(0xb43046,0);memory.word(0xb4304a,0);memory.word(0xb43019,0x2d2d2d2d);}
 void free_buffer(){const auto buffer=memory.word(0xb43100);call(MenuReplayApi::virtual_free,{buffer,0x100000,0x8000});}
 void scan(){
  struct Adapter:ReplayFileServices {
   MenuReplayRuntime& owner;explicit Adapter(MenuReplayRuntime& r):owner(r){}
   std::uint32_t replay_file_call(ReplayFileOperation op,const std::vector<std::uint32_t>& args)override{
    MenuReplayApi api=MenuReplayApi::create_file;switch(op){case ReplayFileOperation::create_file:break;case ReplayFileOperation::seek_file:api=MenuReplayApi::set_file_pointer;break;case ReplayFileOperation::read_file:api=MenuReplayApi::read_file;break;case ReplayFileOperation::close_file:api=MenuReplayApi::close_handle;break;}return owner.call(api,args);
   }
  }adapter(*this);
  const auto saved=registers;scan_replay_files(memory,adapter);registers=saved;
 }
 MenuReplaySaveOutcome save(){
  const auto saved=registers;registers.esp-=32u;
  registers.eax=memory.word(0xb43042);const auto path=memory.word(0xb4337f+registers.eax*4u);
  call(MenuReplayApi::create_file,{path,0xc0000000,0,0,2,0x80,0});registers.edi=registers.eax;
  if(registers.edi==0xffffffffu)return {false,false,registers};
  const auto size=memory.word(0xb43110);const auto buffer=memory.word(0xb43100);
  call(MenuReplayApi::write_file,{registers.edi,buffer,size,0xb43900,0});
  registers.eax=0;
  for(const auto field:std::array<std::array<std::uint32_t,2>,6>{{{{0xb43800,0xa7e610}},{{0xb43804,0xb43255}},{{0xb43808,0xa7e614}},{{0xb4380c,0xb3e2ec}},{{0xb43810,0xa7ea50}},{{0xb43814,0xb43110}}}}){registers.ecx=field[1];memory.word(field[0],registers.ecx);}
  for(;;){memory.word(0xb43904,registers.eax);if(registers.eax==7)break;const auto source=memory.word(0xb43800+registers.eax*4u);call(MenuReplayApi::write_file,{registers.edi,source,4,0xb43900,0});registers.eax=memory.word(0xb43904);++registers.eax;}
  call(MenuReplayApi::write_file,{registers.edi,0xb43015,8,0xb43900,0});
  auto source=memory.word(0xb43259);call(MenuReplayApi::write_file,{registers.edi,source,8,0xb43900,0});
  source=memory.word(0xb4325d);call(MenuReplayApi::write_file,{registers.edi,source,10,0xb43900,0});
  registers.eax=memory.word(0xb43c04);registers.eax-=memory.word(0xb43015);registers.eax-=memory.word(0xb43019);memory.word(0xb43c04,registers.eax);
  call(MenuReplayApi::write_file,{registers.edi,0xb43c04,4,0xb43900,0});
  registers.eax=memory.word(0xb43c04);registers.edx=registers.eax%119u;registers.eax/=119u;registers.ecx=119;memory.word(0xb43c04,registers.edx);
  registers.eax=0;registers.eax+=memory.word(0xb43015);registers.eax+=memory.word(0xb43019);registers.edx=registers.eax%119u;registers.eax/=119u;registers.ecx=119;
  memory.word(0xb43c04,memory.word(0xb43c04)+registers.edx);call(MenuReplayApi::write_file,{registers.edi,0xb43c04,4,0xb43900,0});
  call(MenuReplayApi::close_handle,{registers.edi});registers=saved;
  memory.word(0xb43056,0xf1cfbcc2);registers.eax=0;memory.word(0xb4305a,0);memory.word(0xb4303e,0);memory.word(0xb41215,0);memory.word(0xa87f40,0);
  free_buffer();call(MenuReplayApi::return_title,{});memory.word(0xb43255,0);reset_name();return {true,true,registers};
 }
};
}
