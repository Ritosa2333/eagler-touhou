#include "EclTrap.hpp"
#include "EnemyCommands.hpp"
#include "BulletManager.hpp"
#include "PortTitle.hpp"
namespace th11 {
void BulletEmitter::initialize()noexcept{std::memset(this,0,sizeof(*this));count=layers=1;speed=2;fire_sound=7;transform_sound=24;flags=0x83;}
namespace {
u32 rank_pair(i32 rank,bool five){return five?(rank>=600?9:rank>=200?7:rank>=-200?5:rank>=-600?3:1):(rank>=512?5:rank>=-512?3:1);}
i16 interpolate_count(i32 a,i32 b,i32 rank){return i16(wrapping_add(signed_bits(u32(wrapping_sub(b,a))*u32(wrapping_add(rank,1024)))/2048,a));}
// The modern generation's 0x20c writes four places and no slot: the value it reads from its one
// argument goes to the global at 0x4b59e4, to (*[0x4b76b0]).+0x24, and to the enemy manager's
// pair +0x4548/+0x454c (the second of which is zeroed) - measured in TH16/TH17/TH18's own code,
// cited at the row below. The port keeps one mirror per destination rather than folding them, since
// the original has not been measured to read two of them through one name.
i32 g_emitter_command_value=0;      // the original's global at 0x4b59e4
i32 g_emitter_command_singleton=0; // the original's (*[0x4b76b0]).+0x24
i32 g_emitter_command_pair=0;       // the original's +0x4548
i32 g_emitter_command_pair_aux=0;   // the original's +0x454c, written as zero
}
// The four mirrors are written by the row and read here, so a test can ask what the original's own
// routine left behind without reaching into the file's namespace.
void emitter_command_20c_probe(i32& a,i32& b,i32& c,i32& d)noexcept{a=g_emitter_command_value;b=g_emitter_command_singleton;c=g_emitter_command_pair;d=g_emitter_command_pair_aux;}
#include "NoopOpcodes.hpp"
bool enemy_emitter_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    // TH12 routes this opcode to FUN_004154d0's epilogue at 0x41962b (`xor eax,eax` then the SEH
    // unwind), so the original does nothing at all and reports success. The guard lives here as well
    // as in EnemyScriptServices::command because the game and the test exports reach the handlers
    // through different entry points and both have to see the original's behaviour.
    if(runtime_noop_opcode(c.instruction->opcode))return true;
    const u16 op=c.instruction->opcode;auto& world=g.environment;
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    // 0x192/0x193 are deliberately absent: TH12 gives those numbers to the
    // flags/visibility pair (EnemyStateCommands.cpp, th12.exe 0x417934/0x417974),
    // while TH11's emitter command of the same number read i(0) as an emitter
    // index. Stage 1's `402 0x20` therefore reached `if(u32(index)>=8)
    // __builtin_trap()` below and killed the stage on its first frame.
    // TH12's 0x196/0x197/0x198 are the drop family (clear counts, index the drop
    // table, set spread), not TH11's emitter count/aim/sound. 0x197 was the worst
    // of the three: it was not in kTh12CollidingOpcodes, so TH11's `b.aim=i16(i(1))`
    // ran silently where TH12 stores into the enemy's drop table.
    // TH12's emitter command family sits at 0x1f6..0x1fc, not at TH11's
    // 0x194..0x19b. The emitter struct is 0x214 bytes in both (0x374 + 8*0x214
    // = 0x1414 = emitter_offset in TH11, and TH12's dispatcher uses base 0x48c
    // with the same 0x214 stride), so field offsets carry over one for one:
    // sprite +0x00, color +0x02, angle +0x10, spread +0x14, speed +0x18,
    // slow_speed +0x1c, count +0x1f8, layers +0x1fa, aim +0x1fc, flags +0x200,
    // fire_sound +0x204, transform_sound +0x208. Each case below is read
    // straight off that map: 0x1f6 writes base+0x00/+0x02, 0x1f7 writes
    // emitter_offset[index] (0x152c + index*0xc), 0x1f9 writes +0x18/+0x1c,
    // 0x1fb writes +0x1fc and 0x1fc writes +0x204/+0x208.
    switch(op){case 0x1aa:case 0x1ab:case 0x1f6:case 0x1f7:case 0x1f8:case 0x1f9:case 0x1fa:case 0x1fb:case 0x1fc:case 0x1fd:case 0x1ff:case 0x209:case 0x20a:case 0x20b:case 0x20c:case 0x20d:case 0x259:break;default:return false;}
    // 0x199 (409) is gone from the claim too: TH12's 0x199 despawns slot 0 of the effect-id
    // array, and the body here was a TH11-mapped bullet transform that really belongs at 0x1fd,
    // which still carries it.
    // 0x1a6 (422) and 0x1a7 (423) are gone too. They are a load/unload pair around the
    // game-side loader global DAT_004b43cc, not emitter commands; EnemyStateCommands.cpp keeps the
    // enemy's flag bit and the lifetime reset, and the old body here was a TH11 emitter speed.
    // 0x1a8 (424) is gone from the claim entirely. Its disassembly loads EAX with 0x4b0c40,
    // the base of a global struct, not the enemy: FUN_0041c550 writes g[0x78] = i(0) and clears
    // g[0x80] when the value changed. DAT_004b0c40 is a running maximum and belongs to a
    // game-side manager the port does not model, like the table 411 and 427 touch, so the state
    // handler no-ops it. The old body here was a TH11 emitter speed and was a residue.
    // 0x1ab (427) is claimed on purpose and has no inner case. TH12's 0x1ab writes one entry of
    // the game-side table FUN_004126f0 owns - table[i(0)] = (i(2), f(1) / ring_sides) - and
    // nothing else. The port owns no such table, so falling through to the no-op return below is
    // the faithful behaviour, which is why 427 could leave kTh12CollidingOpcodes.
    // 0x1a9 is absent below as well: the dispatcher gives that number a single call,
    // FUN_00414c60 (erase the stage enemies), and grepping the decompiled tree for
    // `case 0x1a6:` or `case 0x1a9:` returns only FUN_004154d0, the dispatcher itself.
    // TH12 has no separate emitter command function, so this pre-chain gate is TH11
    // architecture and its TH11 claim on 0x1a9 has to give way to the dispatcher's body.
    // TH12's emitter copy is 511 (0x1ff), and reading it exposes a gap in the
    // TH11 body above: FUN_004154d0's case 0x1ff copies 0x85 dwords (0x214 bytes)
    // from emitter[arg1] to emitter[arg0], and then also copies the three floats
    // at 0x152c (emitter_offset) and the three at 0x158c (emitter_origin) through
    // the LAB_004174d7 tail, which is exactly  = b; a[1] = b[1]; a[2] = b[2];.
    // 0x19b is no longer handled here. TH12's dispatcher gives it the ring descriptor, which
// EnemyStateCommands.cpp implements, so the emitter gate's claim was a TH11 residue and the TH11
// body that copied only the emitter itself (silently keeping the old offset and origin) is gone.
    if(op==0x1ff){const i32 source=i(1),target=i(0);if(u32(source)>=8||u32(target)>=8)ecl_trap(__FILE__,__LINE__);
        e.emitters[target]=e.emitters[source];
        e.emitter_offset[target]=e.emitter_offset[source];
        e.emitter_origin[target]=e.emitter_origin[source];
        return true;}
    // TH13 gives 0x1f6 a record TH12's body was not written for: one argument, where TH12's carries
    // three. Printed from the shipped resources (`tools/th-ecl-instructions.py`): TH13's st05.ecl holds
    // `0x1f6` twice, both length 20, params 1, arg 0x20, while TH12's stage05.ecl holds it six times,
    // length 28, params 3, args 0/0/0xd and 1/0/2. Reading i(0) as the emitter index therefore asked
    // for emitters[0x20] and stopped TH13's stage five on its first frame (the handler's own report:
    // `command=0x1f6`, EnemyEmitterCommands.cpp:72).
    // TH16, TH17 and TH18 answer the same way, each from its own stage files: in all 22 of
    // TH16's stage scripts every 0x1f6 record is length 20 with one parameter (138 records, the
    // arguments 0x20, 0xc, 0x10, 0x4c, 0x120 and 0x20d among them, past the eight emitters
    // TH12's body indexes), TH17's are 99 records and TH18's 110, all the same shape. The
    // generation's body is each of their executables' to read, and until it is, this one is a
    // documented no-op.
    if(kPortTitle!=12&&op==0x1f6)return true;
    // The modern generation's 0x20c is not TH12's emitter radius. Its group holds exactly one
    // opcode (tools/th-ecl-dispatch.py over 0x12c..0x2bc: TH16 0x20c -> group 69 -> 0x4222bf,
    // TH17 -> 0x425d00, TH18 -> 0x435b23, each of those groups holding only 0x20c), so the
    // routine the resolver returns is its case: it reads the record's single argument, hands it to
    // a routine that copies it to the global at 0x4b59e4 and to (*[0x4b76b0]).+0x24 (TH17
    // 0x29b10), and stores it at +0x4548 with a zero at +0x454c (TH17 0x25d17). The argument is
    // a value, not a slot: the records reach 81, past any emitter table the titles have - TH17's
    // own clear loop walks 16 of stride 0x380 at +0x598 (`mov ebx, 0x10`, RVA 0x1ea4d) where
    // TH12's walks 8 of stride 0x214 at +0x48c (RVA 0x123fe). So it is handled before the slot
    // index below is read, which the 20-byte record would not survive.
    if(kPortTitle!=12&&op==0x20c){
        const i32 value=i(0);
        g_emitter_command_value=value;
        g_emitter_command_singleton=value;
        g_emitter_command_pair=value;
        g_emitter_command_pair_aux=0;
        return true;}
    const i32 index=i(0);if(u32(index)>=8)ecl_trap(__FILE__,__LINE__);auto& b=e.emitters[index];
    switch(op){
    // 0x194 (404) was removed from the claim above: TH12's 0x194 sets flags bit 0x10000 and stores
    // four floats, whereas the body that used to sit here was TH11's angle/spread pair, which the
    // comment below already flagged as a different instruction. 0x195 (405) stays.
    // 0x195 (405) was removed from the claim above: TH12's 0x195 clears flags bit 0x10000, whereas
    // the body that used to sit here was TH11's speed/slow_speed pair. 0x194 left the same way.
    // 0x1a9 is gone from this inner switch as well: its outer gate no longer claims it,
    // so the case could never be released. The sibling 0x1aa distinguishes itself with op==0x1aa.
    case 0x1aa:{const u32 arg=rank_pair(world.rank,op==0x1aa);b.count=i16(i(arg));b.layers=i16(i(arg+1));break;}
    case 0x1ab:{const i32 a=i(1),v=i(2),d=i(3),z=i(4);b.count=interpolate_count(a,d,world.rank);b.layers=interpolate_count(v,z,world.rank);break;}
    // TH11's 0x1b3/0x1b4 used to sit here and shadowed TH12's real ones. They were
    // kept under the belief that both opcodes were still in kTh12CollidingOpcodes,
    // but that entry was removed when TH12's own 0x1b3/0x1b4 were released, which
    // made this a live shadow rather than dead code. TH12's 0x1b4 is not this body:
    // it is the float twin in EnemyStateCommands.cpp. TH11's version reads
    // count/layers through the integer path with a difficulty selector, so when
    // TH12 data supplied a float argument there, the raw bits reached
    // stack.local<i32> as an offset - measured as a clamped position of 0x40800000,
    // the bit pattern of 4.0f, and as a wasm out-of-bounds trap before the clamp.
    // TH11's 0x1b5/0x1b6/0x1b7 were emitter offset/radius/origin writes. TH12 gives those
    // numbers 422's shared body, which EnemyStateCommands.cpp now handles, so the labels are gone
    // from this file rather than left to shadow the real handler.
    case 0x1f6:b.sprite=i16(i(1));b.color=i16(i(2));break;
    case 0x1f7:e.emitter_offset[index].x=f(1);e.emitter_offset[index].y=f(2);break;
    // TH12's 0x20d is the origin twin of 0x1f7 and its body is read straight off th12.exe 0x417fab:
    // `call 0x41a900` with ecx 0 gives the emitter index, `lea edx,[esi+esi*2]` and `lea edi,[ebx+edx*4]`
    // stride that index by 0xc, then f(1) lands at edi+0x158c and f(2) at (3*esi+0x564)*4, which is
    // ebx+12*esi+0x1590 - the x and y of emitter_origin[index]. The z slot at +0x1594 is then written from
    // an x87 comparison of origin.x against the double at 0x4a3e70, which is -990.0: `fcomp` sets C0 for
    // x < -990, `fnstsw ax` / `test ah,5` / `jp` reaches `fldz` exactly when one of C0 or C2 is set, so
    // x < -990 stores 0.0 and everything else - including -990 itself and NaN, where both flags are set -
    // stores 1.0. The handler then tail-jumps to 0x41962b, so it returns 0 and touches nothing else.
    case 0x20d:{const float x=f(1),y=f(2);e.emitter_origin[index].x=x;e.emitter_origin[index].y=y;
        e.emitter_origin[index].z=(double(x)<-990.0)?0.0f:1.0f;break;}
    // TH12's 0x259 stores five integers into the emitter named by i(0). The handler at 0x4186cd calls the integer
    // fetcher six times, uses the first result as an index strided by 0x214 and writes the other five at +0x670
    // through +0x680 from the enemy, which is emitter+0x1e4 through +0x1f4 once the dispatcher's 0x48c base is
    // subtracted. Measured against the original rather than deduced: with i(0) at 0, 1, 3 and 7 the writes land at
    // +0x670, +0x884, +0xcac and +0x14fc, which is 0x670 + index*0x214 exactly, and each block receives i(1)..i(5)
    // in order. The handler returns through 0x41962b, so the return value is 0 and nothing else is touched.
    case 0x259:for(u32 k=0;k<5;++k)b.values_1e4[k]=u32(i(k+1));break;
    // TH12's 0x1f8 writes emitter+0x10 (angle) and then - note, not TH11's
    // spread at +0x14 - the expression (i*3 + 0x54c)*4, which is i*0xc + 0x1530
    // and therefore emitter_offset[i].y. The disassembly is unambiguous because
    // the stride is 0xc, not the emitter's 0x214: this case reaches out of the
    // emitter array into the offset array beside it. TH11's 0x194, which set
    // angle and spread, is a different instruction and stays where it is.
    case 0x1f8:b.angle=f(1);e.emitter_offset[index].y=f(2);break;
    case 0x1f9:b.speed=f(1);b.slow_speed=f(2);break;
    // TH12's 0x1fd is TH11's 0x199 field for field: it resolves emitter[arg0],
    // strides by arg1 * 0x18 into the transform array and writes concurrent from
    // arg2, opcode from arg3, c from arg4, d from arg5, a from f(6) and b from
    // f(7). The decompiled offsets are 0x4c4, 0x4c0, 0x4b8, 0x4bc, 0x4b4 and
    // (slot*3 + 0x96)*8, which after subtracting the 0x48c emitter base are 0x38,
    // 0x34, 0x2c, 0x30, 0x28 and 0x24 - i.e. transforms[slot] + 0x14, +0x10,
    // +0x08, +0x0c, +0x04 and +0x00, exactly the port's BulletTransform layout.
    case 0x1fd:{const i32 slot=i(1);if(u32(slot)>=18)ecl_trap(__FILE__,__LINE__);auto& t=b.transforms[slot];t.concurrent=i(2);t.opcode=u32(i(3));t.c=i(4);t.d=i(5);t.a=f(6);t.b=f(7);break;}
    // TH12's 0x1fa is TH11's 0x196 count/layers pair: the shared tail
    // LAB_004180d4 writes i16(arg1) to emitter+0x684 (count, +0x1f8) and
    // i16(arg[2]) to emitter+0x686 (layers, +0x1fa). 0x1fa reaches that tail with
    // the argument index fixed at 2; 0x205 and 0x206 reach the same tail
    // conditionally on rank and are not implemented yet.
    case 0x1fa:b.count=i16(i(1));b.layers=i16(i(2));break;
    case 0x1fb:b.aim=i16(i(1));break;
    case 0x1fc:b.fire_sound=i(1);b.transform_sound=i(2);break;
    // TH12's 0x209/0x20a are TH11's 0x1b3/0x1b4 shifted by +0x56. The bodies
    // match the TH11 twins above exactly: the same difficulty selector over arg
    // indices 1..4 and 5..8, writing speed (+0x18) and slow_speed (+0x1c),
    // or count (+0x1f8) and layers (+0x1fa). TH12's data reaches these numbers
    // through FUN_0041a900 with the selector computed as 0->1, 1->2, 2->3,
    // otherwise 4 - the same table the port already uses for world.difficulty.
    // TH11's own 0x1b3/0x1b4 are gone from this switch entirely: treating them as
    // unreachable is what let them shadow TH12's real handlers.
    case 0x209:{const u32 arg=world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4;b.speed=f(arg);b.slow_speed=f(arg+4);break;}
    // TH12's 0x20c writes f(1) to emitter+0x4ac, which is +0x20 over the 0x48c
    // emitter base - the radius field. Its siblings 0x200 and 0x201 both call
    // FUN_0040caa0 with a flag then FUN_004286f0, i.e. a bullet-cancel pair, and
    // 0x202/0x203/0x204/0x205/0x206/0x207/0x208 are rank- or global-gated and
    // need their tails read before they can be transcribed.
    case 0x20c:b.radius=f(1);break;
    // 523 (0x20b) @ 0x417f06, signature "Sff", 22 uses in th12.dat. The handler reads i(0)
    // into esi and f(1)/f(2), calls 0x41c580 with the two floats, and stores the two results
    // at ebx + 12*esi + 0x152c and ebx + 12*esi + 0x1530 - the same 0x152c emitter_offset
    // array, stride 0xc, that 0x1f7 and 0x1f8 already reach into. 0x41c580 is the polar
    // helper: it is fsincos over its first argument then a multiply by its second, storing
    // the two results through the pointer it is handed, so the call is polar(f(1), f(2)).
    // It writes x and y only, leaving z as it was. That is why 523 sat between the two
    // claimed neighbours 0x20a and 0x20c while itself being unclaimed.
    case 0x20b:{const Vec2 offset=polar(f(1),f(2));e.emitter_offset[index].x=offset.x;e.emitter_offset[index].y=offset.y;break;}
    case 0x20a:{const u32 arg=world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4;b.count=i16(i(arg));b.layers=i16(i(arg+4));break;}
    }
    return true;
}
// TH12's fire opcode is 501 (0x1f5), not TH11's 0x191. FUN_004154d0's case 0x1f5
// is this function verbatim: it tests emitter_origin[index].z <= 0.9 and then
// either adds current.position to emitter_offset[index] (all three components) or
// adds emitter_origin[index] to emitter_offset[index] and forces z to 0 - the
// same two branches, with the same z=0. It then stores param_1+0x16c8 into the
// bullet manager's exclusion slot before firing and zeroes it after, and 0x16c8
// is TH11's 0x1590 plus the 0x138 flags-region shift, i.e. exclusion_distance.
//
// One deliberate difference from TH11's body: TH12's case ends in an
// unconditional break, so the result of the fire call is discarded. TH11's
// version aborts the script when firing fails, which would stop an enemy that
// the original simply lets continue. The faithful behaviour is kept here and the
// failure is not reported.
i32 enemy_fire_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    if(c.instruction->opcode!=0x1f5||!w.bullets)return -2;
    const i32 index=c.integer_argument(0,g);if(u32(index)>=8)return -2;
    auto& emitter=e.emitters[index];const auto& offset=e.emitter_offset[index];const auto& origin=e.emitter_origin[index];
    if(origin.z<=.9){const auto& p=e.current.position;emitter.position={float(double(offset.x)+p.x),float(double(offset.y)+p.y),float(double(offset.z)+p.z)};}
    else emitter.position={float(double(origin.x)+offset.x),float(double(origin.y)+offset.y),0};
    w.bullets->exclusion_squared=e.exclusion_distance;w.bullets->fire(emitter);w.bullets->exclusion_squared=0;return 0;
}
}
