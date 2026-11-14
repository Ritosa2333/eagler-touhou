#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f){u32 w;std::memcpy(&w,&f,4);return w;}
float value(u32 w){float f;std::memcpy(&f,&w,4);return f;}

// Ring/half-step angle synthesis (etAim modes 4/5, plus mode 3 negative control).
//
// Original 0x40e840 dispatches etAim through the jump table at 0x40efd8:
//   mode2 -> 0x40e950, mode3 -> 0x40e95c, mode4 -> 0x40e98d, mode5 -> 0x40e999.
// mode5 skips the aim store and retains the per-bullet zero base.
//
// mode4/5 body (mode4 entry 0x40e98d, mode5 entry 0x40e999, block ends
// 0x40e9d3 jmp 0x40eb13; EDI=et pointer, EBP+0x10=idx, +0x14=way,
// +0x18=aim float, et+0x14=angle1, et+0x18=angle2, et+0x364=count i16):
//   40e98d flds [ebp+0x18]        ; aim
//   40e990 faddl [0x4ae878](0.0)  ; \
//   40e996 fstps [ebp-4]          ; / base = f32(aim+0)   (mode5 skips: base=0)
//   40e9a3 fildl count
//   40e9a6 fldl [0x4ae858]        ; pi  = 3.1415927410125732421875
//   40e9ac fdiv st,st1            ; pi/count
//   40e9ae fadds [ebp-4]          ; + base
//   40e9b1 fstps [ebp+0x18]       ; mid = f32(pi/count + base)      <-- f32 drop 1
//   40e9b4 fildl idx
//   40e9b7 fmull [0x4ae850]       ; two_pi = 6.283185482025146484375
//   40e9bd fdivp st1,st0          ; idx*two_pi/count (80-bit)
//   40e9bf fadds [ebp+0x18]       ; + mid
//   40e9c2 fstps [ebp+0x10]       ; spoke = f32(... + mid)          <-- f32 drop 2
//   40e9c5 flds [ebp+0x10]        ; reload spoke
//   40e9c8 flds [edi+0x18]        ; angle2
//   40e9cb fimull [ebp+0x14]      ; angle2*way
//   40e9ce fadds [edi+0x14]       ; + angle1
//   40e9d0 faddp st1,st0          ; spoke + (angle1+angle2*way), 80-bit
//   40e9d3 jmp 0x40eb13 tail      ; fstps local_8 f32 drop, then 0x472e90 ->
//                                 ; 0x472f30 double wrap writes node+0xb8c.
// All three tests keep every raw angle strictly inside (-pi,pi), so the wrap
// tail is an identity and the expected bits below isolate the synthesis.
//
// All 84 expected words were independently checked by executing these
// original EXE blocks in Unicorn with control word023f, stopping at40eb16.
// Inputs are count28,way0 and the exact angle1/angle2/aim values below.
// Stores40e9b1/40e9c2 and the tail write remain original instructions.
// The pre-fix
// all-double form (single drop of angle1+angle2*way+idx*2pi/count+pi/count
// +base) differs at 1ULP for 15 of 28 mode5 indices (4,9,10,12,13,14,15,17,
// 18,19,20,21,24,25,26) and 17 of 28 mode4 indices (2,7,8,10,11,13,14,15,16,
// 17,19,20,21,22,25,26,27) — the count=28/way0 shape reported for EX
// sprite32/3 (angle birth 1ULP, 0.01 after e2347 quantization).
static const u32 kMode4[28]={
  3225368607u,3224427407u,3223486208u,3222545008u,3221603808u,3220099745u,
  3218217346u,3216334946u,3214452548u,3212303432u,3208538632u,3204773836u,
  3197569816u,3180370528u,1042119616u,1052877408u,1058685808u,1062450604u,
  1065784310u,1067666710u,1069549110u,1071431506u,1073313906u,1074469065u,
  1075410265u,1076351465u,1077292663u,1078233863u};
static const u32 kMode5[28]={
  3225788037u,3224846838u,3223905638u,3222964438u,3222023238u,3220938606u,
  3219056207u,3217173807u,3215291408u,3213409008u,3210216356u,3206451556u,
  3200925256u,3190731680u,1029461376u,1049521960u,1057008084u,1060772884u,
  1064537684u,1066827850u,1068710246u,1070592646u,1072475046u,1074049635u,
  1074990835u,1075932033u,1076873233u,1077814433u};
// Negative control: mode3 body is the separate 0x40e950..0x40e988 block with
// its own single f32 drop at 40e977 (idx*2pi/count + [ebp-4] base, base=fldz
// 0 for mode3) — no pi/count half-step term, untouched by the mode4/5 fix.
// angle1 differs here because mode3's window (no mid offset) cannot share
// mode4/5's angle1 while keeping every raw angle inside (-pi,pi).
static const u32 kMode3[28]={
  3225419776u,3224478576u,3223537377u,3222596177u,3221654978u,3220202084u,
  3218319684u,3216437285u,3214554886u,3212508108u,3208743312u,3204978512u,
  3197979168u,3182007968u,1041300912u,1052468056u,1058481128u,1062245928u,
  1065681972u,1067564372u,1069446768u,1071329168u,1073211568u,1074417896u,
  1075359096u,1076300296u,1077241494u,1078182694u};

TEST(mode4_ring_mid_and_spoke_f32_drops) {
  StageSim sim;sim.interactive=true;
  StageEt et;et.ways=1;et.count=28;et.aim=4;
  et.angle1=-3.2f;et.angle2=0.11f;
  const float origin[]={0,200,0};
  sim.emit_et(et,0,0,origin,0.1f);
  CHECK_EQ(sim.bullets.size(),size_t(28));
  if(sim.bullets.size()!=28)return;
  for(int idx=0;idx<28;++idx)
    CHECK_EQ(word(sim.bullets[idx].angle),kMode4[idx]);
}
TEST(mode5_ring_base_zero_mid_and_spoke_f32_drops) {
  StageSim sim;sim.interactive=true;
  StageEt et;et.ways=1;et.count=28;et.aim=5;
  et.angle1=-3.2f;et.angle2=0.11f;
  const float origin[]={0,200,0};
  sim.emit_et(et,0,0,origin,0.0f);
  CHECK_EQ(sim.bullets.size(),size_t(28));
  if(sim.bullets.size()!=28)return;
  for(int idx=0;idx<28;++idx)
    CHECK_EQ(word(sim.bullets[idx].angle),kMode5[idx]);
}
TEST(mode3_fan_unaffected_by_halfstep_fix) {
  StageSim sim;sim.interactive=true;
  StageEt et;et.ways=1;et.count=28;et.aim=3;
  et.angle1=-3.0f;et.angle2=0.11f;
  const float origin[]={0,200,0};
  sim.emit_et(et,0,0,origin,0.0f);
  CHECK_EQ(sim.bullets.size(),size_t(28));
  if(sim.bullets.size()!=28)return;
  for(int idx=0;idx<28;++idx)
    CHECK_EQ(word(sim.bullets[idx].angle),kMode3[idx]);
}
int main(){return th13test::run_all("halfstep ring precision");}
