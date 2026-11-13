#include "Dialogue.hpp"
#include "EnemyManager.hpp"
#include "EnemyCommands.hpp"
#include "LaserManager.hpp"
#include "BulletManager.hpp"
namespace th11 {
#include "NoopOpcodes.hpp"
i32 enemy_laser_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    // TH12 routes a block of the extended range to FUN_004154d0's epilogue at 0x41962b, which is
    // `xor eax,eax` and the SEH unwind: those opcodes do nothing and report success. This handler is
    // called directly by tests as well as by the routing chain, so it guards its own entry rather than
    // relying on EnemyScript.cpp::command having done it first.
    if(runtime_noop_opcode(c.instruction->opcode))return 0;
    const u16 op=c.instruction->opcode;
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    const auto origin=[&](){const auto& offset=e.emitter_offset[0];const auto& absolute=e.emitter_origin[0];
        if(absolute.z<=.9)return Vec3{float(double(e.current.position.x)+offset.x),float(double(e.current.position.y)+offset.y),float(double(e.current.position.z)+offset.z)};
        return Vec3{float(double(absolute.x)+offset.x),float(double(absolute.y)+offset.y),0};
    };
    // 415 is a plain Timer::set on the enemy and needs no laser manager, so it is handled ahead of
    // the guard below: FUN_004067e0 is Timer::set, the same function 413 calls.
    if(op==0x19f){e.timer_1690.set(i(0),&g.environment.rate);return 0;}
    if(!w.lasers)return -2;auto& lasers=*w.lasers;
    switch(op){
    // 0x16d is absent on purpose. The dispatch formula resolves it to 0x41962b, the shared `xor eax,eax`
    // epilogue, so the original does nothing with it, and EnemyScript.cpp returns before routing it here.
    // The body that used to sit here called lasers.mark_all(), which would have acted on an opcode the
    // original ignores - a divergence that stayed invisible only because the earlier guard kept it from
    // running, and that the routing gate surfaced as an unreachable case.
    // 0x19a is absent for the opposite reason: it is not a no-op, and its handler at 0x417a38 is
    // `xor ecx,ecx ; mov eax,ebx ; call 0x41a900 ; mov [ebx+0x1620],eax`, which is drops.primary = i(0)
    // and belongs to the state dispatcher, where EnemyStateCommands.cpp already carries it. The cancel
    // body that used to sit here was TH11's laser command under the same number.
    case 0x1a4:case 0x1be:case 0x1bf:{
        if(!w.bullets)return -2;const float radius=f(0);const bool reward=op==0x1a4||op==0x1be;
        if(!w.bullets->cancel_circle(e.current.position,radius,reward,op>=0x1be,w.cancellation?*w.cancellation:BulletCancelContext{}))return -2;
        // All four commands preserve protected lasers, independently of the bullet delay flag.
        return lasers.cancel_circle(e.current.position,radius,reward,true)<0?-2:0;
    }
    // TH12's 0x19c is not a laser spawn. TH11's was, which is why this opcode sat in the
    // collision list grouped with 0x1ac/0x1af/0x1b1. The real body is:
    //   index = i(0); FUN_004124e0(0) clears bit 0 of manager+0x3c;
    //   index < 0: clear manager[0x1c + e[0x16c4]*4], then flags &= ~0x400000;
    //   index >= 0: flags |= 0x400000, manager[0x1c + index*4] = e.script_owner, e[0x16c4] = index.
    // Flag 0x400000 is the world-coordinate projection bit, and the port already models it
    // throughout: EnemyManager sets it from the spawn parameter's world_coordinates bit 22, and
    // the projection, collision, animation and state paths all branch on it per enemy. The port
    // tests that bit per enemy rather than iterating the original's 8-slot manager array, so
    // setting and clearing it here reproduces exactly what array membership produces there. The
    // array slots and the saved index at e[0x16c4] have no port consumer, so they are recorded
    // here rather than modelled. manager+0x3c bit 0 does have one: the port calls it
    // w.manager_flags (case 0x1ba sets its bit 3, case 0x14c clears bit 0), so FUN_004124e0(0) is
    // modelled exactly.
    // 0x1ac is absent: TH12 gives 428 the shared 422 body, which EnemyStateCommands.cpp handles.
    case 0x1af:case 0x1b1:{
        LaserLineParameters p{};if(op==0x1af||op==0x1b1)std::memcpy(p.transforms,e.emitters[0].transforms,sizeof p.transforms);
        p.position=origin();p.sprite=i16(i(0));p.color=i16(i(1));p.angle=f(2);
        if(op==0x1af)p.angle=normalize_angle(p.angle);
        p.speed=f(3);p.initial_length=f(4);p.growth_limit=f(5);p.end_distance=f(6);p.width=f(7);
        p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;p.flags=(op==0x1af)?1:0;
        lasers.spawn(p);return lasers.last_error<0?-2:0;
    }
    // 413 is a single Timer::set - see the disassembly note in tools/tmp/patch-413.mjs. TH11's 0x19d
    // was an infinite laser and shared this body, so it is split out here.
    case 0x19d:
        e.lifetime.set(0,&g.environment.rate);
        return 0;
    case 0x1ad:case 0x1b0:case 0x1b2:{
        const bool additive=op==0x1b0;LaserInfiniteParameters p{};p.speed=8;
        i32* destination=additive?nullptr:c.integer_reference(0,g);
        if(op==0x1b0||op==0x1b2)std::memcpy(p.transforms,e.emitters[0].transforms,sizeof p.transforms);
        p.position=origin();p.sprite=i16(i(1));p.color=i16(i(2));p.angle=f(3);if(additive)p.angle=normalize_angle(p.angle);
        p.initial_length=f(4);p.max_length=f(5);p.warning_frames=i(6);p.expand_frames=i(7);p.active_frames=i(8);p.shrink_frames=i(9);p.width=f(10);p.flags=u32(i(11));
        if(additive){p.flags|=2;p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;if(p.flags&4)p.id=i(0);else destination=c.integer_reference(0,g);}
        else{p.flags&=~2u;p.start_sound=e.emitters[0].fire_sound;p.transform_sound=e.emitters[0].transform_sound;}
        const i32 id=lasers.spawn(p);if(destination)*destination=id;return lasers.last_error<0?-2:0;
    }
    // 0x1a2 does NOT belong to this dispatcher. Disassembling FUN_00418813 shows it is the dialogue
    // starter: it loads i(0) and calls FUN_0041fbe0, which frees any live dialogue and creates the one
    // for that id, then calls FUN_0040cf60(0) to clear bullets and FUN_00428750 to cancel lasers. The
    // port previously read FUN_0041fbe0 as a close and ran the clears as a teardown, and then fell
    // through into FUN_00414c60, which is the START of 0x1a9's handler, not part of this one. The case
    // is removed from here and 0x1a2 is routed to the state dispatcher, where the starter belongs.
    // 0x19f used to be the first label of the group below. TH12's 0x19f is a plain timer reset
    // (FUN_004067e0 = Timer::set on enemy+0x1690), so it is handled ahead of the laser guard
    // because it needs no laser manager. The 0x19f arm of the compound condition further down is now
    // unreachable - left in place rather than tidied, because its 0x19e arm belongs to 414 and 414's
    // TH12 semantics are not established yet.
    //
    // 0x1a1 and 0x1a3 used to be stubs here and are gone in round 188. Neither spawns a laser, so
    // neither belongs to this dispatcher, and the routing no longer sends them here - they are the
    // same failure 0x19c had, a state opcode sitting inside the laser range and never reaching its own
    // body. 0x1a1 is the screen shake: its body at 0x4187df builds the 0x44-byte shake object through
    // FUN_004529a0(1, i(0), i(1), i(2), 0, 0x46), and EnemyStateCommands.cpp already calls
    // w.shakes->start(duration, start, end, rate) with the arguments in the disassembly's order.
    // 0x1a3 is not the no-op this file used to call it. Its body at 0x41883d returns 0 only when the
    // dialogue object is absent or FUN_00412720 reports it finished, and otherwise does
    // `or eax,-1 ; jmp 0x41962d`, landing one byte past the `xor eax,eax` epilogue at 0x41962b so the
    // -1 survives: that is a wait. EnemyStateCommands.cpp carries the matching dialogue_waiting() body.
    // Reading the decompiler's one-statement summary of FUN_00412720 had hidden the -1 arm, and the
    // stub here returned 0 instead, so the wait was lost for the 75 uses of this opcode in th12.dat.
    // TH11's 0x1c0 was a laser-cancel loop. TH12's 0x1c0 is not a laser op at all:
    // it subtracts a difficulty-selected integer argument from the script context's
    // time field (see EnemyStateCommands.cpp), so this case is removed rather than
    // left as a th11-numbered body that would silently cancel lasers. The dispatch
    // gate's routing check is what surfaced it: 0x1c0 stopped being routed here and
    // the gate reported the orphan case.
    // The 0x25c..0x262 family addresses one laser object found by id. Three of the seven have field
    // meanings the original's own update loop at 0x4287c0 settles, so they are transcribed here; the
    // other four wait on the port's LaserState carrying TH12's meanings (see below).
    //
    // 0x25c at 0x418f0c is `id = i(0); o = find(id); if (!o) return; o[0x50..0x58] = {f(1), f(2), 0.0f}`.
    // The update loop rebuilds +0x50 from the spawn position at +0x454 and then advances it by a delta
    // produced from +0x460 and +0x47c, which is the live position the port keeps in LaserState::position
    // and advances by LaserState::velocity. The stores copy the fetched float words unchanged, so the
    // port stores the floats rather than converting through double.
    case 0x25c:
        {auto* laser=lasers.find(i(0));if(laser)laser->position={f(1),f(2),0.0f};return 0;}
    // 0x25d at 0x418f66 reads the same id and calls 0x412900 with ecx = the object and eax = a
    // three-float block built from f(1), f(2) and a zero. 0x412900 copies three dwords to +0x460
    // through +0x468, the block the update loop takes +0x460 from. That block is where the port keeps
    // the three velocity words, so the transcription writes velocity and keeps the port's
    // LaserInfiniteParameters word in step: the original has one block for both laser types, while the
    // port's infinite update reads its velocity from the parameter block instead.
    case 0x25d:
        {auto* laser=lasers.find(i(0));if(laser){laser->velocity={f(1),f(2),0.0f};
         if(laser->type==1)reinterpret_cast<LaserInfinite*>(laser)->parameters.velocity=laser->velocity;}return 0;}
    // 0x262 at 0x418ebd is a loop that re-reads i(0) every iteration - the argument fetch is inside the
    // loop - finds the object by id, calls its vtable slot +0x14 with two zero arguments, clears the id
    // at +0x80, and repeats until find returns null. The two zero pushes are the reason this cannot
    // reuse cancel_id: that loop passes reward = true, and paying a graze reward the original does not
    // pay is exactly the kind of divergence that stays invisible until a score is compared.
    case 0x262:
        while(auto* laser=lasers.find(i(0))){if(!lasers.cancel_object(*laser,false,false))return -2;laser->id=0;}
        return 0;
    default:return -2;
    }
}
}
