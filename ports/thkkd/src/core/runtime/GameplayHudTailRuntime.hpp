// 游戏HUD与白闪显示尾的有界原序执行；正常ECX经GS或实际SDK返回取得。
#pragma once
#include "core/runtime/GameplayHudTailContext.hpp"
#include "core/runtime/GameplayHudTailProgram.hpp"
#include <algorithm>
#include "core/frontend/MenuReplayFormat.hpp"
#include "core/shared/MusicPresentationArithmetic.hpp"
#include <cmath>
#include <limits>
namespace kaikeidou {
namespace gameplay_hud_tail_detail {
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
 Memory& memory;GameStartEntryMachineFrame& frame;MenuScenePlatform& services;const GameplayHudTailPolicy& policy;
 GameStartEntryRegisters& r;GameplayHudTailOutcome out;U pc;std::vector<Q> fpu;unsigned precision;bool wrapper;U float_status=0;
 U address(const Operand&o){return o.value+(o.base<0?0:reg(r,o.base))+(o.index<0?0:reg(r,o.index)*o.scale);}
 std::uint8_t byte(U a){for(const auto& c:constants)if(a>=c.address&&a-c.address<c.size)return c.bytes[a-c.address];return memory.byte(a);}
 Q read(U a,U n){for(const auto& c:constants)if(a>=c.address&&Q(a)-c.address+n<=c.size){Q value=0;for(U i=0;i<n;++i)value|=Q(c.bytes[a-c.address+i])<<(i*8);return value;}try{auto bytes=memory.read(a,n);Q value=0;for(U i=0;i<n;++i)value|=Q(bytes[i])<<(i*8);return value;}catch(const GameStartEntryAccessFault&){throw GameStartEntryAccessFault{a,n,false};}}
 void write(U a,U n,Q value){std::uint8_t b[8]{};for(U i=0;i<n;++i)b[i]=static_cast<std::uint8_t>(value>>(i*8));try{memory.write(a,b,n);}catch(const GameStartEntryAccessFault&){throw GameStartEntryAccessFault{a,n,true};}}
 Q load(const Operand&o){if(o.kind==1){if(o.base==8)return r.eax&0xffffu;if(o.base==9)return r.eax&255u;if(o.base==10)return(r.eax>>8)&255u;return reg(r,o.base);}if(o.kind==2)return o.value;if(o.kind==3)return read(address(o),o.width);throw std::logic_error("Startup load operand");}
 void store(const Operand&o,Q value){if(o.kind==1){if(o.base==8)r.eax=(r.eax&0xffff0000u)|(static_cast<U>(value)&0xffffu);else if(o.base==9)r.eax=(r.eax&0xffffff00u)|(static_cast<U>(value)&255u);else if(o.base==10)r.eax=(r.eax&0xffff00ffu)|((static_cast<U>(value)&255u)<<8);else{reg(r,o.base)=static_cast<U>(value);out.exact_registers|=1u<<o.base;}}else if(o.kind==3)write(address(o),o.width,value);else throw std::logic_error("Startup store operand");}
 void push(U value){const U next=r.esp-4u;write(next,4,value);r.esp=next;}
 U pop(){const U value=static_cast<U>(read(r.esp,4));r.esp+=4;return value;}
 std::string text(U a){std::string s;for(U i=0;i<0x100000;++i){const auto c=byte(a+i);if(!c)return s;s.push_back(static_cast<char>(c));}throw GameStartEntryAccessFault{a+0x100000u,1,false};}
 bool inside(U target)const{return std::any_of(program.begin(),program.end(),[&](const auto&i){return i.pc==target;});}
 void api(U target){
  std::vector<U> args;for(U i=0;i<arity(target);++i)args.push_back(static_cast<U>(read(r.esp+4u+i*4u,4)));
  std::string contents;std::vector<std::string_view> texts;if(target==0x437c90){contents=text(args[2]);texts.push_back(contents);args.erase(args.begin()+2);}
  memory.reconcile();auto result=services.invoke({static_cast<MenuSceneApi>(target),std::move(args),std::move(texts),{}});memory.reconcile();for(const auto&w:result.writes)memory.write(w.address,w.bytes);
  r.eax=result.eax;r.ecx=result.ecx;r.edx=result.edx;out.exact_registers|=7;out.known_eflags=0;pc=pop();
 }
 struct FormatMemory:MenuReplayFormatMemory {Engine& owner;explicit FormatMemory(Engine&e):owner(e){}std::uint8_t byte(U a)override{return owner.byte(a);}U argument(U a)override{return static_cast<U>(owner.read(a,4));}void byte(U a,std::uint8_t v)override{owner.write(a,1,v);}};
 void format(){FormatMemory m(*this);const U target=static_cast<U>(read(r.esp+4u,4)),fmt=static_cast<U>(read(r.esp+8u,4));r.eax=format_menu_replay_integer(m,{fmt,r.esp+12u,target,0xffffffffu});out.exact_registers=(out.exact_registers|1u)&~6u;out.known_eflags=0;pc=pop();}
 void copy(){const U saved_pc=pc;pc=0x4f3f40;push(r.edi);pc=0x4f3f41;const U target=static_cast<U>(read(r.esp+8u,4));U at=static_cast<U>(read(r.esp+12u,4));r.edi=target;
  for(;;){const U value=static_cast<U>(read(at,4));unsigned n=0;while(n<4&&((value>>(8*n))&255u)!=0)++n;
   if(n==4){pc=0x4f3fd9;write(r.edi,4,value);r.edi+=4;at+=4;continue;}
   if(n==3){pc=0x4f4012;write(r.edi,4,value);}else if(n==2){pc=0x4f401a;write(r.edi,2,value);pc=0x4f4021;write(r.edi+2u,1,0);}else if(n==1){pc=0x4f4027;write(r.edi,2,value);}else{pc=0x4f4030;write(r.edi,1,0);}break;
  }r.eax=target;r.edi=pop();out.exact_registers=(out.exact_registers|1u)&~6u;out.known_eflags=0;pc=saved_pc;pc=pop();
 }
 void convert(){if(fpu.empty())throw std::logic_error("HUD x87 conversion underflow");const auto q=fpu.back();pc=0x6968c9;push(r.ebp);r.ebp=r.esp;r.esp=(r.esp-8u)&~7u;pc=0x6968d2;write(r.esp,8,q);fpu.pop_back();
  r.eax=(q&0x7ff0000000000000ull)==0x7ff0000000000000ull?0x80000000u:music_presentation_detail::truncate_sse(binary_float::binary64(q));r.esp=r.ebp;r.ebp=pop();out.exact_registers|=1;out.known_eflags=0;pc=pop();
 }
 static double real(Q q){double d;std::memcpy(&d,&q,8);return d;}
public:
 Engine(Memory&m,GameStartEntryMachineFrame&f,MenuScenePlatform&s,const GameplayHudTailPolicy&p,U entry,unsigned bits,bool whole):memory(m),frame(f),services(s),policy(p),r(f.registers),pc(entry),precision(bits),wrapper(whole){}
 GameplayHudTailOutcome run(){memory.reconcile();try{for(;out.steps<policy.maximum_instructions;++out.steps){
  out.source=pc;
  if(wrapper){if(pc==0x65343a){push(0x65343f);pc=0x61d690;continue;}if(pc==0x61d693){pc=0x61d838;push(0x61d83d);pc=0x61d180;continue;}if(pc==0x61d83d){push(0x61d842);pc=0x611ff0;continue;}if(pc==0x61d842){pc=0x61d8bb;continue;}if(pc==0x65343f){out.kind=GameplayHudTailKind::completed;out.target=pc;memory.reconcile();return out;}}
  if(pc==0x4f81df){out.kind=GameplayHudTailKind::security_cookie_failure;out.source=0x4f369a;out.target=pc;memory.reconcile();return out;}
  auto found=std::lower_bound(program.begin(),program.end(),pc,[](const auto&i,U value){return i.pc<value;});
  if(found==program.end()||found->pc!=pc){out.target=pc;out.kind=std::find(policy.registered_return_targets.begin(),policy.registered_return_targets.end(),pc)!=policy.registered_return_targets.end()?GameplayHudTailKind::returned_target:GameplayHudTailKind::fetch_transfer;memory.reconcile();return out;}
  const auto i=*found;const auto&a=i.operands[0];const auto&b=i.operands[1];const auto&c=i.operands[2];U next=i.next;
  switch(i.op){
  case Op::op_mov:store(a,load(b));break;
  case Op::op_lea:store(a,address(b));break;
  case Op::op_push:push(static_cast<U>(load(a)));break;
  case Op::op_pop:{const U v=pop();store(a,v);break;}
  case Op::op_add:case Op::op_sub:case Op::op_xor:case Op::op_and:case Op::op_cmp:case Op::op_test:{
   const U left=static_cast<U>(load(a)),right=static_cast<U>(load(b));const bool sub=i.op==Op::op_sub||i.op==Op::op_cmp;const bool logical=i.op==Op::op_xor||i.op==Op::op_and||i.op==Op::op_test;
   const U value=sub?left-right:i.op==Op::op_xor?left^right:i.op==Op::op_and?left&right:i.op==Op::op_test?left&right:left+right;
   flags(r,left,right,value,sub,logical);out.known_eflags=logical?0x8c5u:0x8d5u;if(i.op!=Op::op_cmp&&i.op!=Op::op_test)store(a,value);break;}
  case Op::op_imul:store(a,Q(load(b))*Q(load(c)));out.known_eflags=0;break;
  case Op::op_cdq:r.edx=(r.eax>>31)?0xffffffffu:0u;out.exact_registers|=4;break;
  case Op::op_idiv:{const auto divisor=static_cast<std::int32_t>(static_cast<U>(load(a)));const Q bits=(Q(r.edx)<<32)|r.eax;std::int64_t dividend;std::memcpy(&dividend,&bits,8);if(!divisor||(dividend==std::numeric_limits<std::int64_t>::min()&&divisor==-1)){out.kind=GameplayHudTailKind::arithmetic_fault;out.target=pc;return out;}const auto quotient=dividend/divisor;if(quotient<-2147483648ll||quotient>2147483647ll){out.kind=GameplayHudTailKind::arithmetic_fault;out.target=pc;return out;}r.eax=static_cast<U>(quotient);r.edx=static_cast<U>(dividend%divisor);out.exact_registers|=5;out.known_eflags=0;break;}
  case Op::op_neg:{const U v=static_cast<U>(load(a));store(a,0u-v);flags(r,0,v,0u-v,true,false);out.known_eflags=0x8d5;break;}
  case Op::op_shr:{const U v=static_cast<U>(load(a)),count=static_cast<U>(load(b))&31u,result=v>>count;store(a,result);r.eflags=(r.eflags&~64u)|(result==0?64u:0u);out.known_eflags=64;break;}
  case Op::op_ja:if(!(r.eflags&65u))next=static_cast<U>(load(a));break;
  case Op::op_jge:if(bool(r.eflags&128u)==bool(r.eflags&2048u))next=static_cast<U>(load(a));break;
  case Op::op_jl:if(bool(r.eflags&128u)!=bool(r.eflags&2048u))next=static_cast<U>(load(a));break;
  case Op::op_jle:if((r.eflags&64u)||bool(r.eflags&128u)!=bool(r.eflags&2048u))next=static_cast<U>(load(a));break;
  case Op::op_je:if(r.eflags&64u)next=static_cast<U>(load(a));break;
  case Op::op_jne:if(!(r.eflags&64u))next=static_cast<U>(load(a));break;
  case Op::op_jmp:next=static_cast<U>(load(a));break;
  case Op::op_call:{const U target=static_cast<U>(load(a));push(next);if(arity(target)!=0xffffffffu){api(target);continue;}if(target==0x4f3f40){copy();continue;}if(target==0x4f41da){format();continue;}if(target==0x6968c0){convert();continue;}next=target;break;}
  case Op::op_ret:case Op::op_repz_ret:next=pop();if(a.kind)r.esp+=static_cast<U>(load(a));break;
  case Op::op_fldz:fpu.push_back(0);break;
  case Op::op_fcomp:{if(fpu.empty())throw std::logic_error("HUD compare underflow");const auto left=real(fpu.back()),right=real(quiet(load(a)));fpu.pop_back();float_status=std::isnan(left)||std::isnan(right)?0x4500u:left<right?0x100u:left==right?0x4000u:0u;break;}
  case Op::op_fnstsw:r.eax=(r.eax&0xffff0000u)|float_status;out.exact_registers&=~1u;break;
  case Op::op_fmul:case Op::op_fdiv:{if(fpu.empty())throw std::logic_error("HUD arithmetic underflow");auto left=binary_float::binary64(fpu.back()),right=binary_float::binary64(load(a));fpu.back()=binary_float::to_binary64(i.op==Op::op_fmul?binary_float::multiply(left,right,precision):binary_float::divide(left,right,precision));break;}
  case Op::op_fld1:fpu.push_back(0x3ff0000000000000ull);break;
  case Op::op_fld:fpu.push_back(quiet(load(a)));break;
  case Op::op_fstp:if(fpu.empty())throw std::logic_error("Startup x87 underflow");store(a,fpu.back());fpu.pop_back();break;
  }
  pc=next;
 }out.kind=GameplayHudTailKind::execution_limit;out.target=pc;
 }catch(const GameStartEntryAccessFault&fault){out.kind=GameplayHudTailKind::memory_fault;out.source=pc;out.target=pc;out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.writing=fault.writing;}memory.reconcile();return out;}
};
}
template<class Memory>GameplayHudTailOutcome execute_gameplay_hud_tail_entry(Memory& memory,GameStartEntryMachineFrame& frame,MenuScenePlatform& platform,std::uint32_t entry,unsigned precision,const GameplayHudTailPolicy& policy={}){return gameplay_hud_tail_detail::Engine<Memory>(memory,frame,platform,policy,entry,precision,false).run();}
template<class Memory>GameplayHudTailOutcome present_gameplay_hud_tail(Memory& memory,GameStartEntryMachineFrame& frame,MenuScenePlatform& platform,unsigned precision,const GameplayHudTailPolicy& policy={}){return gameplay_hud_tail_detail::Engine<Memory>(memory,frame,platform,policy,0x65343a,precision,true).run();}
}
