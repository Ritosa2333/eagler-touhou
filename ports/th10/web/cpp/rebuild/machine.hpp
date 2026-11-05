#pragma once
// Execution state retained by statically reconstructed TH10 control flow.
// The generated game calls these integer/stack helpers directly; no binary
// instruction decoding, JIT, PC device emulation or v86 is used here.
#include <stdint.h>
#include <stddef.h>
using u8=uint8_t; using u16=uint16_t; using u32=uint32_t; using u64=uint64_t;
using i8=int8_t; using i16=int16_t; using i32=int32_t; using i64=int64_t;
struct F80 { u64 significand; u16 exponent; u16 pad16=0; u32 pad32=0; };
static_assert(sizeof(F80)==16);
struct alignas(16) RebuildState {
  u32 r[8], pc, flags, fs_base, budget, count, reason, language, error;
  F80 fp[8];
  u32 fp_top, fp_empty, fp_control, fp_status, fp_ip, fp_dp, fp_opcode, mxcsr;
  u32 xmm[8][4];
};
extern RebuildState state;
enum {AX=0,CX=1,DX=2,BX=3,SP=4,BP=5,SI=6,DI=7};
constexpr u32 CF=1,PF=4,AF=16,ZF=64,SF=128,OF=2048,ARITH=0x8d5;
template<class T> inline T read(u32 p) {
  T v; __builtin_memcpy(&v,reinterpret_cast<const void*>(p),sizeof(v)); return v;
}
template<class T> inline void write(u32 p,T v) {
  __builtin_memcpy(reinterpret_cast<void*>(p),&v,sizeof(v));
}
template<unsigned Bits> constexpr u32 mask() {if constexpr(Bits==32)return 0xffffffffu;else return (u32(1)<<Bits)-1;}
template<unsigned Bits> inline i32 sign(u32 v) {if constexpr(Bits==8)return i8(v);else if constexpr(Bits==16)return i16(v);else return i32(v);}
template<unsigned Bits> inline u32 zsp(u32 v) {
  v &= mask<Bits>();return (!v?ZF:0)|((v>>(Bits-1))?SF:0)|((__builtin_popcount(v&255)&1)?0:PF);
}
template<unsigned Bits> inline u32 add(RebuildState &s,u32 a,u32 b,u32 carry=0) {
  a&=mask<Bits>();b&=mask<Bits>();const u64 n=u64(a)+b+carry;const u32 v=u32(n)&mask<Bits>();
  s.flags=(s.flags&~ARITH)|zsp<Bits>(v)|(n>mask<Bits>()?CF:0)|((a^b^v)&AF)|((~(a^b)&(a^v))>>(Bits-1)&1?OF:0);return v;
}
template<unsigned Bits> inline u32 sub(RebuildState &s,u32 a,u32 b,u32 borrow=0) {
  a&=mask<Bits>();b&=mask<Bits>();const u32 v=(a-b-borrow)&mask<Bits>();
  s.flags=(s.flags&~ARITH)|zsp<Bits>(v)|(u64(a)<u64(b)+borrow?CF:0)|((a^b^v)&AF)|(((a^b)&(a^v))>>(Bits-1)&1?OF:0);return v;
}
template<unsigned Bits> inline u32 logical(RebuildState &s,u32 v) {v&=mask<Bits>();s.flags=(s.flags&~ARITH)|zsp<Bits>(v);return v;}
template<unsigned Bits> inline u32 increment(RebuildState &s,u32 v,int delta) {const u32 cf=s.flags&CF;v=delta>0?add<Bits>(s,v,1):sub<Bits>(s,v,1);s.flags=(s.flags&~CF)|cf;return v;}
template<unsigned Bits> inline u32 shift(RebuildState &s,u32 a,u32 n,unsigned kind) {
  a&=mask<Bits>();n&=31;if(!n)return a;u32 v=0,c=0,o=0;
  if(kind==0){v=n<Bits?a<<n:0;c=n<=Bits?(a>>(Bits-n))&1:0;o=((v>>(Bits-1))^c)&1;}
  else if(kind==1){v=n<Bits?a>>n:0;c=n<=Bits?(a>>(n-1))&1:0;o=a>>(Bits-1);}
  else {v=u32(sign<Bits>(a)>>n);c=n<Bits?(a>>(n-1))&1:a>>(Bits-1);}
  s.flags=(s.flags&~ARITH)|zsp<Bits>(v)|(c?CF:0)|(o?OF:0);return v&mask<Bits>();
}
template<unsigned Bits> inline u32 rotate(RebuildState &s,u32 a,u32 n,unsigned kind) {
  a&=mask<Bits>();n&=31;if(kind<2)n%=Bits;else if(Bits<32)n%=Bits+1;if(!n)return a;
  u32 cf=s.flags&1;for(u32 i=0;i<n;i++){
    if(kind==0){cf=a>>(Bits-1);a=((a<<1)|cf)&mask<Bits>();}
    else if(kind==1){cf=a&1;a=(a>>1)|(cf<<(Bits-1));}
    else if(kind==2){const u32 next=a>>(Bits-1);a=((a<<1)|cf)&mask<Bits>();cf=next;}
    else {const u32 next=a&1;a=(a>>1)|(cf<<(Bits-1));cf=next;}
  }
  const u32 of=kind==0||kind==2?((a>>(Bits-1))^cf):((a>>(Bits-1))^(a>>(Bits-2)));
  s.flags=(s.flags&~(CF|OF))|(cf?CF:0)|(of&1?OF:0);return a;
}
inline void push(RebuildState &s,u32 v){s.r[SP]-=4;write<u32>(s.r[SP],v);}
inline u32 pop(RebuildState &s){const u32 v=read<u32>(s.r[SP]);s.r[SP]+=4;return v;}
inline void r8(RebuildState &s,unsigned r,u32 v,unsigned high=0){const u32 n=high?8:0;s.r[r]=(s.r[r]&~(255u<<n))|((v&255)<<n);}
inline void r16(RebuildState &s,unsigned r,u32 v){s.r[r]=(s.r[r]&0xffff0000)|(v&65535);}
inline bool condition(const RebuildState &s,unsigned cc){
  const bool c=s.flags&CF,z=s.flags&ZF,n=s.flags&SF,o=s.flags&OF,p=s.flags&PF;
  switch(cc){case 0:return o;case 1:return !o;case 2:return c;case 3:return !c;case 4:return z;case 5:return !z;case 6:return c||z;case 7:return !c&&!z;case 8:return n;case 9:return !n;case 10:return p;case 11:return !p;case 12:return n!=o;case 13:return n==o;case 14:return z||n!=o;default:return !z&&n==o;}
}
inline bool step(RebuildState &s,u32 pc,u32 count=1){s.pc=pc;if(s.budget<count){s.budget=0;return false;}s.count+=count;s.budget-=count;return true;}
inline void fault(RebuildState &s,u32 pc,u32 code){s.pc=pc;s.error=code;s.reason=3;s.budget=0;}
void multiply(RebuildState &s,u32 b,unsigned bits,bool is_signed);
void divide(RebuildState &s,u32 b,unsigned bits,bool is_signed);
u32 double_shift(RebuildState &s,u32 a,u32 b,u32 n,unsigned bits,bool right);
void string_op(RebuildState &s,unsigned op,unsigned size,unsigned repeat);

void fp_control(RebuildState &s,u32 control);
F80 fp_get(RebuildState &s,unsigned i=0);
void fp_set(RebuildState &s,unsigned i,F80 v);
void fp_push(RebuildState &s,F80 v);
void fp_pop(RebuildState &s);
F80 fp_load(RebuildState &s,u32 p,unsigned bytes,bool integer=false);
void fp_store(RebuildState &s,u32 p,unsigned bytes,F80 v,bool integer=false,bool truncate=false);
F80 fp_from_double(double value);
double fp_to_double(F80 value);
F80 fp_binary(RebuildState &s,unsigned op,F80 a,F80 b);
void fp_compare(RebuildState &s,F80 a,F80 b,bool flags=false,bool quiet=false);
void fp_special(RebuildState &s,unsigned op);
void fp_environment(RebuildState &s,u32 p,bool store,bool full=false);
u32 fp_tag(const RebuildState &s);
inline u32 fp_status(const RebuildState &s){return (s.fp_status&~0x3800)|((s.fp_top&7)<<11);}
void sse(RebuildState &s,unsigned op,unsigned dst,u32 src,unsigned memory,unsigned immediate=0);
