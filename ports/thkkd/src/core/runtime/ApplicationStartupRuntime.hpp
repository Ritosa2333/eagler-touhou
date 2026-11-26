// 原配置和WinMain前置链的有界执行，文件与SDK返回均由显式宿主服务提供。
#pragma once
#include "core/runtime/ApplicationStartupContext.hpp"
#include "core/runtime/ApplicationStartupProgram.hpp"
#include <algorithm>
namespace kaikeidou {
namespace application_startup_detail {
using U=std::uint32_t;using Q=std::uint64_t;
inline U& reg(GameStartEntryRegisters& r,int i){switch(i){case 0:return r.eax;case 1:return r.ecx;case 2:return r.edx;case 3:return r.ebx;case 4:return r.esp;case 5:return r.ebp;case 6:return r.esi;case 7:return r.edi;default:throw std::logic_error("Startup register");}}
inline U parity(U n){unsigned bits=0;for(unsigned i=0;i<8;++i)bits+=(n>>i)&1u;return(bits&1u)?0u:4u;}
inline void flags(GameStartEntryRegisters&r,U a,U b,U value,bool subtract,bool logical){
 U f=parity(value)|(value==0?64u:0u)|((value>>24)&128u);
 if(!logical){f|=(a^b^value)&16u;f|=subtract?(a<b?1u:0u):static_cast<U>((Q(a)+b)>>32);f|=subtract?(((a^b)&(a^value))>>20)&0x800u:((~(a^b)&(a^value))>>20)&0x800u;}
 r.eflags=(r.eflags&~0x8d5u)|f;
}
inline Q quiet(Q q){return(q&0x7ff0000000000000ull)==0x7ff0000000000000ull&&(q&0xfffffffffffffull)?q|0x8000000000000ull:q;}
template<class Memory>class Engine {
 Memory& memory;GameStartEntryMachineFrame& frame;ApplicationStartupServices& services;const ApplicationStartupPolicy& policy;
 GameStartEntryRegisters& r;ApplicationStartupOutcome out;U pc;std::vector<Q> fpu;
 U address(const Operand&o){return o.value+(o.base<0?0:reg(r,o.base))+(o.index<0?0:reg(r,o.index)*o.scale);}
 std::uint8_t byte(U a){for(const auto& c:constants)if(a>=c.address&&a-c.address<c.size)return c.bytes[a-c.address];return memory.byte(a);}
 Q read(U a,U n){for(const auto& c:constants)if(a>=c.address&&Q(a)-c.address+n<=c.size){Q value=0;for(U i=0;i<n;++i)value|=Q(c.bytes[a-c.address+i])<<(i*8);return value;}try{auto bytes=memory.read(a,n);Q value=0;for(U i=0;i<n;++i)value|=Q(bytes[i])<<(i*8);return value;}catch(const GameStartEntryAccessFault&){throw GameStartEntryAccessFault{a,n,false};}}
 void write(U a,U n,Q value){std::uint8_t b[8]{};for(U i=0;i<n;++i)b[i]=static_cast<std::uint8_t>(value>>(i*8));try{memory.write(a,b,n);}catch(const GameStartEntryAccessFault&){throw GameStartEntryAccessFault{a,n,true};}}
 Q load(const Operand&o){if(o.kind==1)return reg(r,o.base);if(o.kind==2)return o.value;if(o.kind==3)return read(address(o),o.width);throw std::logic_error("Startup load operand");}
 void store(const Operand&o,Q value){if(o.kind==1){reg(r,o.base)=static_cast<U>(value);out.exact_registers|=1u<<o.base;}else if(o.kind==3)write(address(o),o.width,value);else throw std::logic_error("Startup store operand");}
 void push(U value){const U next=r.esp-4u;write(next,4,value);r.esp=next;}
 U pop(){const U value=static_cast<U>(read(r.esp,4));r.esp+=4;return value;}
 std::string text(U a){std::string s;for(U i=0;i<0x100000;++i){const auto c=byte(a+i);if(!c)return s;s.push_back(static_cast<char>(c));}throw GameStartEntryAccessFault{a+0x100000u,1,false};}
 bool inside(U target)const{return std::any_of(program.begin(),program.end(),[&](const auto&i){return i.pc==target;});}
 void api(U target){
  const auto sp=r.esp;ApplicationStartupRequest request;request.api=target;request.source=pc;
  for(U i=0;i<arity(target);++i)request.words.push_back(static_cast<U>(read(sp+4u+i*4u,4)));
  if(target==0x4f4572){request.texts.push_back(text(request.words[0]));request.texts.push_back(text(request.words[1]));}
  else if(target==0x4f4257||target==0x444500)request.texts.push_back(text(request.words[0]));
  memory.reconcile();const auto result=services.invoke(request);memory.reconcile();
  for(const auto&w:result.writes)memory.write(w.address,w.bytes);
  r.eax=result.eax;r.ecx=result.ecx;r.edx=result.edx;out.exact_registers|=7;out.known_eflags=0;
  if(target==0x4f4572&&r.eax)out.open_config_handles.push_back(r.eax);
  if(target==0x4f4600){auto at=std::find(out.open_config_handles.begin(),out.open_config_handles.end(),request.words[0]);if(at!=out.open_config_handles.end())out.open_config_handles.erase(at);}
  pc=pop();
 }
 // 固定00100复制按原对齐源的首dword与末word写入，源来自冻结只读文字。
 void default_copy(){
  pc=0x4f3f40;push(r.edi);pc=0x4f3f41;r.edi=static_cast<U>(read(r.esp+8u,4));const U dest=r.edi;
  pc=0x4f3fb5;const U source=static_cast<U>(read(r.esp+12u,4));pc=0x4f3fe3;const auto first=read(source,4);
  pc=0x4f3fd9;write(dest,4,first);r.edi+=4;pc=0x4f3fe3;const auto second=read(source+4u,2);
  pc=0x4f4027;write(r.edi,2,second);pc=0x4f402a;r.eax=static_cast<U>(read(r.esp+8u,4));pc=0x4f402e;r.edi=pop();
  out.exact_registers&=~6u;out.known_eflags=0;pc=0x4f402f;pc=pop();
 }
public:
 Engine(Memory&m,GameStartEntryMachineFrame&f,ApplicationStartupServices&s,const ApplicationStartupPolicy&p,U entry):memory(m),frame(f),services(s),policy(p),r(f.registers),pc(entry){}
 ApplicationStartupOutcome run(){memory.reconcile();try{for(;out.steps<policy.maximum_instructions;++out.steps){
  out.source=pc;
  if(pc==0x653290){out.kind=ApplicationStartupKind::entered_outer;out.target=pc;memory.reconcile();return out;}
  if(pc==0x4f81df){out.kind=ApplicationStartupKind::security_cookie_failure;out.source=0x4f369a;out.target=pc;memory.reconcile();return out;}
  auto found=std::find_if(program.begin(),program.end(),[&](const auto&i){return i.pc==pc;});
  if(found==program.end()){out.target=pc;out.kind=std::find(policy.registered_return_targets.begin(),policy.registered_return_targets.end(),pc)!=policy.registered_return_targets.end()?ApplicationStartupKind::returned_target:ApplicationStartupKind::fetch_transfer;memory.reconcile();return out;}
  const auto i=*found;const auto&a=i.operands[0];const auto&b=i.operands[1];U next=i.next;
  switch(i.op){
  case Op::op_mov:store(a,load(b));break;
  case Op::op_movsx:{const U v=static_cast<U>(load(b));store(a,b.width==1&&v&128u?v|0xffffff00u:v);break;}
  case Op::op_lea:store(a,address(b));break;
  case Op::op_push:push(static_cast<U>(load(a)));break;
  case Op::op_pop:{const U v=pop();store(a,v);break;}
  case Op::op_add:case Op::op_sub:case Op::op_xor:case Op::op_or:case Op::op_cmp:case Op::op_test:{
   const U left=static_cast<U>(load(a)),right=static_cast<U>(load(b));const bool sub=i.op==Op::op_sub||i.op==Op::op_cmp;const bool logical=i.op==Op::op_xor||i.op==Op::op_or||i.op==Op::op_test;
   const U value=sub?left-right:i.op==Op::op_xor?left^right:i.op==Op::op_or?left|right:i.op==Op::op_test?left&right:left+right;
   flags(r,left,right,value,sub,logical);out.known_eflags=logical?0x8c5u:0x8d5u;if(i.op!=Op::op_cmp&&i.op!=Op::op_test)store(a,value);break;}
  case Op::op_je:if(r.eflags&64u)next=static_cast<U>(load(a));break;
  case Op::op_jne:if(!(r.eflags&64u))next=static_cast<U>(load(a));break;
  case Op::op_jmp:next=static_cast<U>(load(a));break;
  case Op::op_call:{const U target=static_cast<U>(load(a));push(next);if(arity(target)!=0xffffffffu){api(target);continue;}if(target==0x4f3f40){default_copy();continue;}next=target;break;}
  case Op::op_ret:case Op::op_repz_ret:next=pop();if(a.kind)r.esp+=static_cast<U>(load(a));break;
  case Op::op_fld1:fpu.push_back(0x3ff0000000000000ull);break;
  case Op::op_fld:fpu.push_back(quiet(load(a)));break;
  case Op::op_fstp:if(fpu.empty())throw std::logic_error("Startup x87 underflow");store(a,fpu.back());fpu.pop_back();break;
  }
  pc=next;
 }out.kind=ApplicationStartupKind::execution_limit;out.target=pc;
 }catch(const ApplicationStartupPlatformTransfer&transfer){out.kind=ApplicationStartupKind::platform_transfer;out.source=transfer.source;out.target=transfer.target;out.exact_registers=transfer.exact_registers;out.known_eflags=transfer.known_eflags;}catch(const GameStartEntryAccessFault&fault){out.kind=ApplicationStartupKind::memory_fault;out.source=pc;out.target=pc;out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.writing=fault.writing;}memory.reconcile();return out;}
};
}
template<class Memory>ApplicationStartupOutcome configure_application(Memory&memory,GameStartEntryMachineFrame&frame,ApplicationStartupContext&,ApplicationStartupServices&api,const ApplicationStartupPolicy&policy={}){return application_startup_detail::Engine<Memory>(memory,frame,api,policy,0x652f80).run();}
template<class Memory>ApplicationStartupOutcome begin_application(Memory&memory,GameStartEntryMachineFrame&frame,ApplicationStartupContext&,ApplicationStartupServices&api,const ApplicationStartupPolicy&policy={}){return application_startup_detail::Engine<Memory>(memory,frame,api,policy,0x653230).run();}
}
