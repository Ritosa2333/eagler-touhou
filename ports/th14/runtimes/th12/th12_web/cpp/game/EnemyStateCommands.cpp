#include "EclTrap.hpp"
#include "EnemyManager.hpp"
#include "EnemyCommands.hpp"
#include "EclRoutes.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
#include "SpellController.hpp"
#include "ScreenShake.hpp"
#include "Dialogue.hpp"
#include "ScreenDeformation.hpp"
#include "EnemyCallbacks.hpp"
#include "Stage.hpp"
namespace th11 {
namespace {
template<class T>T& destination(T* p){if(!p)ecl_trap(__FILE__,__LINE__);return *p;}
i32 drop_type(i32 type,u32 flags){if(flags&0x80000){if(type==1)return 10;if(type==4)return 11;}return type;}
u32 rank_index(i32 rank){return rank>=600?4:rank>=200?3:rank>=-200?2:rank>=-400?1:0;}
}
#include "NoopOpcodes.hpp"
i32 enemy_state_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    // TH12 routes a block of the extended range to FUN_004154d0's epilogue at 0x41962b, which is
    // `xor eax,eax` and the SEH unwind: those opcodes do nothing and report success. This handler is
    // called directly by tests as well as by the routing chain, so it guards its own entry rather than
    // relying on EnemyScript.cpp::command having done it first.
    if(runtime_noop_opcode(c.instruction->opcode))return 0;
    // The titles renumber this block; EclRoutes.hpp carries the measured rows (TH13's
    // 0x206/0x207 are TH12's 0x1a2/0x1a3). The no-op claim above stays on the raw
    // number, because that table is TH12's own case table.
    const u16 op=ecl_state_opcode(c.instruction->opcode);auto& world=g.environment;const float* rate=&world.rate;
    auto f=[&](u32 n){return float(c.float_argument(n,g));};auto i=[&](u32 n){return c.integer_argument(n,g);};
    switch(op){
    // Shipped scripts contain these labels; the original dispatcher explicitly
    // falls through to success without evaluating arguments.
    // 0x116 is not a no-op: it resolves &Enemy[0xe0 + i(0)*4] through FUN_00461c50 and, when that
    // returns non-null, writes the same float to +0x40 and +0x44 and sets flag bit 8. The object is a
    // class the port does not own, so this refuses explicitly rather than acting on the wrong fields.
    // kTh12CollidingOpcodes already refuses 0x116, so this is the second line of defence.
    case 0x116:return -2;
    // 0x208 IS a fall-through-to-success in the original: the dispatcher evaluates no arguments.
    case 0x1f4:break;
    // 0x208 at 0x4193ef writes the angle from a point to the player into the float variable the instruction's
    // argument zero names. The body: `mov eax,[0x4b4514]` loads the player, `fld [eax+0x97c]` and
    // `fld [eax+0x980]` are its x and y as floats, `fsubr` subtracts f(1) and f(2) from them with the
    // double-wide loads on the memory side, and each difference is `fstp dword` then `fld dword`, i.e. rounded
    // to f32 before it is used. `call 0x4088c0` is atan2: its own body loads [ebp+8] into st0 and then
    // [ebp+0xc], so the first pushed word becomes st1, and the first pushed word is player.x - f(1). The tail
    // at 0x419629 is `fstp dword ptr [eax]`, with eax holding what 0x41a960 resolved for argument zero - the
    // port's EclContext::float_reference and EnemyGlobals::float_reference pair is the transcription of that
    // resolver and of the table it looks negative ids up in. The original does not check that - with a literal
    // argument its resolver returns 0 and it faults - so the port keeps this file's own convention, which is
    // case 0x1b4's: an unresolved reference is reported with -2 instead of being written through.
    case 0x208:{float* ref=c.float_reference(0,g);if(!ref)return -2;*ref=aim_angle(Vec3{f(1),f(2),0},world.player_position);return 0;}
    // TH12 moved TH11's 0x140/0x141 body (the two Vec2 argument readers) up to
    // 0x190/0x191, and gave 0x140/0x141 a different family over the Vec2s at
    // ebx+0x68 and ebx+0x9c (absolute/relative position). Evidence: th12.exe
    // 0x4178d8 and 0x417906 both do `mov eax,[esi+4]` with the dispatcher
    // prologue's `mov esi,[ebx+0x174c]` / `mov eax,[esi+4]`, so esi is
    // script_owner and [esi+4] is script_owner->script.active - the same
    // EclContext the port passes as `c`. The signature agrees: 400/401 are `ff`
    // in TH12 while TH11's 0x190/0x191 were `S` and the fire command.
    // 0xd0 and 0xd8 are hitbox and collision_box because Movement is 0x34, so
    // four Movements plus two Vec2s put hitbox at 0xd0 and animations at 0xe0.
    // TH12's own 0x140/0x141 (320/321) is still refused by kTh12CollidingOpcodes.
    case 0x190:e.hitbox.x=f(0);e.hitbox.y=f(1);break;
    case 0x191:e.collision_box.x=f(0);e.collision_box.y=f(1);break;
    // TH12 renumbered TH11's flags/visibility pair (0x142/0x143) to 402/403 and
    // widened it from `animations` to the sixteen dwords at state+0xe0, which the
    // original walks as ids (th12.exe 0x417934 sets, 0x417974 clears):
    //   flags  = op==402 ? flags|bits : flags&~bits
    //   update = op==402 ? !!(flags & 0x20) : !(flags & 0x20)
    //   per id: clear (0x461d80) or set (0x461d40) bit 1 of whatever the manager
    //   lookup at 0x461920 finds. That lookup returns null for id 0, so a slot
    //   range of zeros is a no-op in the original, not an unimplemented path.
    // TH12's own 0x140..0x143 is a different four-opcode family operating on the
    // two Vec2 boxes at ebx+0x68 and ebx+0x9c, so it must not reuse this body.
    case 0x192:case 0x193:{
        const u32 bits=u32(i(0)),flags=op==0x192?e.flags|bits:e.flags&~bits;
        const bool visible=!(flags&0x20),update=op==0x192?!visible:visible;
        e.flags=flags;
        if(update){
            const u32 slots[16]={e.animations[0],e.animations[1],e.animations[2],e.animations[3],
                e.animations[4],e.animations[5],e.animations[6],e.animations[7],e.animations[8],
                e.animations[9],u32(e.animation_file),u32(e.bound_animation_file),
                u32(e.animation_script),u32(e.base_animation),u32(e.direction),e.animation_options};
            // FUN_00461d80 (case 0x192) clears bit 1 of +0x47c on the manager
            // object 0x461920 finds, and FUN_00461d40 (case 0x193) sets it, and
            // each also touches [0x11f] and walks the linked list at [5] when
            // [6] is zero. Bit 1 of +0x47c is the same visibility bit the TH11
            // body below drives through w.animation_visibility, and the `visible`
            // computed above is already the right argument: 0x192 clears the bit
            // and passes visible == false, 0x193 sets it and passes true.
            //
            // The sixteen ids are the sixteen consecutive dwords starting at
            // animations[0], and the loop count in the original is literally 0x10.
            // This previously refused whenever any id was non-zero, on the
            // strength of stage 1 only, where all sixteen happen to be zero - so
            // the refusal was invisible there and killed stage 4 the moment it
            // set an animation. Doing nothing was exact for stage 1 and wrong in
            // general; the callback is the correct general path.
            if(!w.animation_visibility)return -2;
            for(u32 id:slots)if(id)w.animation_visibility(id,visible,w.animation_user);
        }
        break;
    }
    case 0x142:case 0x143:{
        const u32 bits=u32(i(0)),flags=op==0x142?e.flags|bits:e.flags&~bits;const bool visible=!(flags&0x20);
        const bool update=op==0x142?!visible:visible;
        if(update&&!w.animation_visibility)return -2;e.flags=flags;
        if(update)for(u32 id:e.animations)w.animation_visibility(id,visible,w.animation_user);break;
    }
    case 0x144:e.flags|=0x2000;e.clamp_center.x=f(0);e.clamp_center.y=f(1);e.clamp_size.x=f(2);e.clamp_size.y=f(3);break;
    // 325 (0x145) @ 0x416831, signature "Sffffff", 397 bytes, with 326 as its fallthrough
    // (the machine code compares against 0x145 and takes the other branch for anything else,
    // and the dispatch map marks 326 used zero times in th12.dat). The handler picks one of
    // the two Movements and the matching position interpolator:
    //   ebx+0x68 -> absolute        ebx+0x9c -> relative
    //   ebx+0x28c -> absolute_position  ebx+0x2d8 -> relative_position
    // The two Movement offsets are 0x34 apart, which is sizeof(Movement); the two interpolator
    // offsets are 0x4c apart, which is sizeof(Vec3Interpolator) (4 arrays of 3 floats, then a
    // Timer). That is what identifies the object this command writes - the port already has it.
    // Field for field against 0x416904..0x4169b5:
    //   +0x44 <- i(0)                duration
    //   +0x48 <- 8                   mode = Hermite
    //   +0x18,+0x1c,+0x20 <- f(1),f(2),0     initial_tangent
    //   +0x24,+0x28,+0x2c <- f(5),f(6),0     final_tangent
    //   +0x00,+0x04,+0x08 <- Movement.position   start
    //   +0x0c,+0x10,+0x14 <- end, where each coordinate that equals the -999999.0f sentinel
    //                        at 0x4a4440 falls back to the Movement's position, and z is 0
    //   call 0x406340 clears the interpolator's Timer
    //   and [movement+0x30],~3 clears the low two Movement flag bits
    case 0x145:case 0x146:{
        const bool relative=op==0x146;
        Movement& movement=relative?e.relative:e.absolute;
        Vec3Interpolator& interpolation=relative?e.relative_position:e.absolute_position;
        constexpr float kPositionSentinel=-999999.0f;
        interpolation.start[0]=movement.position.x;interpolation.start[1]=movement.position.y;interpolation.start[2]=movement.position.z;
        const float end_x=f(3),end_y=f(4);
        interpolation.end[0]=end_x==kPositionSentinel?movement.position.x:end_x;
        interpolation.end[1]=end_y==kPositionSentinel?movement.position.y:end_y;
        interpolation.end[2]=0.0f;
        interpolation.initial_tangent[0]=f(1);interpolation.initial_tangent[1]=f(2);interpolation.initial_tangent[2]=0.0f;
        interpolation.final_tangent[0]=f(5);interpolation.final_tangent[1]=f(6);interpolation.final_tangent[2]=0.0f;
        interpolation.duration=i(0);interpolation.mode=InterpolationMode::Hermite;
        interpolation.timer.set(0,rate);
        movement.flags&=~3u;
        break;
    }
    // TH12 moved TH11's drop family (0x146/0x147/0x148/0x14a) up by 0x50 AND
    // dropped the boss-type remap. FUN_004154d0's 0x196/0x197/0x198/0x19a call
    // FUN_0041a900 directly with no drop_type() step, where TH11's same bodies
    // called it through the flags&0x80000 remap. The table also grew from 12
    // counts to 19: 0x196 clears 0x4c bytes from 0x1624 (= counts[0..18]) and
    // 0x197 stores into ((i32*)(&drops))[arg0], so index 0 is primary and 1..19
    // are counts. 0x197 has no bound check in the original; refusing an index
    // past the table is a deliberate deviation, because the original's response
    // to one is to corrupt the adjacent spread floats.
    case 0x196:for(auto& count:e.drops.counts)count=0;break;
    case 0x197:{const i32 index=i(0),count=i(1);if(u32(index)>=20)return -2;reinterpret_cast<i32*>(&e.drops)[index]=count;break;}
    case 0x198:{const float y=f(1);e.drops.spread.x=f(0);e.drops.spread.y=y;break;}
    case 0x19a:e.drops.primary=i(0);break;
    case 0x14b:{const i32 health=i(0);e.health=e.max_health=e.interrupt_health=health;e.scaled_health=signed_bits(u32(health)*7);
        if(e.flags&0x80000){for(auto& segment:w.health_segments)segment={0,0};e.flags|=0x4000000;}break;}
    case 0x19c:{const i32 slot=i(0);if(slot>=8)return -2;w.manager_flags&=~1u;
        if(slot<0){if(e.flags&0x80000){if(e.boss_slot<0||e.boss_slot>=8)return -2;w.bosses[e.boss_slot]=nullptr;}e.flags&=~0x80000u;}
        else {w.bosses[slot]=&e;e.boss_slot=slot;e.flags|=0x80000;}
        world.boss=w.bosses[0];break;}
    case 0x14e:{const i32 time=i(2),health=i(1),slot=i(0);if(slot<0||slot>=8)return -2;auto& interrupt=e.interrupts[slot];interrupt.health=health;
        if(health>=0){interrupt.time=time;interrupt.health_script=interrupt.timeout_script=reinterpret_cast<const char*>(c.instruction)+32;}break;}
    case 0x19f:e.damage_immunity.set(i(0),rate);break;
    case 0x19d:e.lifetime.set(0,rate);break;
    case 0x1a1:{if(!w.shakes)return -2;const i32 end=i(2),start=i(1),duration=i(0);w.shakes->start(duration,start,end,rate);break;}
    // 0x1a2 starts the section dialogue. Its handler FUN_00418813 loads i(0) and calls FUN_0041fbe0,
    // and disassembling that callee shows it is a STARTER, not a close: it frees any live dialogue
    // object, allocates a fresh 0xac-byte DialogueState, resolves the message entry for the id and
    // calls FUN_0041f900 to initialise it. The port previously read it as a close, which made this
    // opcode look like a teardown and left the real starter unreachable, because 0x1a2 sat inside the
    // laser range in EnemyScript.cpp. The bullet clear and laser cancel that follow in the handler are
    // performed by GameBattle::start_dialogue, the DialogueControl implementation. The stage-enemy
    // erase at FUN_00414c60 begins at 0x418833, which is 0x1a9's handler, so it does not belong here.
    case 0x1a2:{const i32 id=i(0);if(!w.dialogue||!w.dialogue->start_dialogue(id))return -2;break;}
    // FUN_0041883d: returns 0 when no dialogue object exists, 0 when the dialogue reports itself
    // finished, and -1 (wait) otherwise. The original's first branch is a plain `je` to the
    // success epilogue, so a missing dialogue lets the script continue; returning -2 here turned a
    // normal no-dialogue pass into a script error.
    case 0x1a3:return w.dialogue&&w.dialogue->dialogue_waiting()?-1:0;
    case 0x1a4:return w.bosses[0]?-1:0;
    case 0x155:{const i32 slot=i(0);if(slot<0||slot>=8)return -2;e.interrupts[slot].timeout_script=reinterpret_cast<const char*>(c.instruction)+24;break;}
    // TH12's 0x19e and 0x1a5 are the interrupt registration pair - TH11's 0x14e
    // and 0x155 moved up by 0x50 - and the bodies below are deliberately the same
    // code as those TH11 twins, because the decompiled TH12 bodies match them
    // exactly: 0x19e stores arg1 in the slot's first dword and, only when
    // arg1 >= 0, fills in time and both script pointers from instruction+0x20;
    // 0x1a5 writes instruction+0x18 into the slot's timeout script.
    //
    // These are added rather than obtained by renaming the TH11 cases because
    // both numbers existed already and the wrong one was winning. 0x19e used to
    // fall through to the laser property family, whose body begins
    // `lasers.find(i(0)); if(!b) return 0;` - so an enemy interrupt referencing
    // no live laser returned success and silently registered nothing. 0x1a5 was
    // in the laser group too. Both have now been removed from that group and
    // from the laser routing range in EnemyScript.cpp's command().
    // 0x1c6 is the one opcode TH12 uses whose whole body is a call into the
    // engine's effect pool. FUN_004154d0's case 0x1c6 is just
    // FUN_00421740(); break;, and that function guards on DAT_004cee40 != 8 and
    // (DAT_004b0ce0 & 0x20) == 0 before calling
    // FUN_004615a0(NULL, *(void **)(DAT_004b43e4 + 0x6ce4), &local_4, 0, 0).
    // FUN_004615a0 has 117 callers; it bumps a counter, spawns from a pool via
    // FUN_004621c0, and sets an id at +0x20, a flag at +0x480 and a position at
    // +0x430/+0x434/+0x438 - a visual effect instance. This port has no effect
    // system to spawn into: there is no Effect or Particle type anywhere under
    // cpp/game, and FUN_00421740 has no other caller in the whole executable.
    //
    // So rather than keep refusing 0x1c6 and stopping all four stages at the same
    // instruction, it is accepted as a named no-op. The enemy script continues
    // and the only thing lost is the engine-side visual. This is a recorded gap
    // with its evidence, not a silent success: it is the single remaining
    // blocker across stages 3, 4, 5 and 6, and it is called out in DELIVERY 17.
    case 0x1c6:return 0;
    // TH12's 0x1b0..0x1b3 store a rank- or difficulty-selected integer into an
    // ECL integer reference. FUN_0041a910 resolves the reference out of the
    // current instruction (instruction->[8] bit 0 marks it, and the value at
    // instruction->[0x10] indexes the context's data segment), which is the same
    // accessor the port already exposes as integer_reference. The bodies are
    // FUN_004154d0 lines 1448-1518: 0x1b0 stores arg2 when rank > 0x1ff and arg0
    // otherwise; 0x1b1 is a five-step ladder over args 4,3,2,1,0 with thresholds
    // 599, 199, -201 and -401; 0x1b3 selects by difficulty. 0x1b1 and 0x1b3 share
    // the labels 00417bad/004192a9/00417ba4_caseD_1/_caseD_3, but 0x1b3's jumps
    // hit a break before 0x1b1's rank ladder, so 0x1b3 really is a plain select.
    // 0x1b3 is the one that was blocking stages 3 and 4. Its twin 0x1b2
    // interpolates arg1..arg2 by rank with a signed divide-by-2048 rounding, and
    // is deliberately left out until that rounding is transcribed exactly.
    case 0x1b0:{i32* ref=c.integer_reference(0,g);if(!ref)return -2;*ref=i(world.rank>0x1ff?2:0);break;}
    case 0x1b1:{i32* ref=c.integer_reference(0,g);if(!ref)return -2;
        *ref=i(world.rank>599?4:world.rank>199?3:world.rank>-201?2:world.rank>-401?1:0);break;}
    // TH12's 0x1b4 is the float twin of 0x1b3: the same difficulty selector, but
    // over float arguments, written through FUN_004690b0 - the float reference
    // getter, whose shape matches the integer one (mask test, then
    // ctx+0x10+index*4) - and stored by the shared tail LAB_00419629, which is
    // *pfVar11 = (float)pfVar18;. The port already exposes float_reference
    // beside integer_reference, so the body maps directly.
    // TH12's 0x1c0 subtracts a difficulty-selected integer argument, converted to
    // float, from the script context's time field. The decompilation takes
    // pfVar11 = *(float **)(*(int *)(param_1 + 0x174c) + 4), which is the active
    // EclContext, and stores *pfVar11 = *pfVar11 - (float)value through the shared
    // float tail LAB_00419629. EclContext's first member is exactly loat time
    // (EclContext.hpp line 27, with owner asserted at +0x1014), so the store lands
    // on time and this is a script-timer adjustment.
    //
    // The argument index is 0/1/2/3 chosen by difficulty - note zero-based, unlike
    // 0x1b3 and 0x1b4 whose selectors are one-based - and the argument is an
    // integer that is converted, not read as a float.
        // TH12's 0x201 cancels bullets around the enemy. Its body calls
    // FUN_0040caa0(position, radius = f(0), 0, 0), and that function is exactly the
    // port's BulletManager::cancel_circle, field for field: skip bullets whose state
    // is 0 or 3, hit when dx*dx + dy*dy <= (hitbox.x*0.5 + radius)^2, cancel, award
    // through the outside(position,2,2,0) test, and finish with the enemy pass.
    // cancel_circle's hitbox.x*0.5 term is where TH12's bullet[+0x4dc] lands: the
    // port's BulletState is TH11's layout and TH12's bullet fields sit exactly 0x80
    // further in, which position (0x43c vs 0x4bc), velocity (0x448 vs 0x4c8), speed
    // (0x454 vs 0x4d4) and angle (0x458 vs 0x4d8) all confirm, so +0x4dc is the
    // hitbox at port offset 0x45c. The protected-spell radius/3 in cancel_circle
    // matches the decompiled 0x5f < timer < 100 guard as well.
    //
    // param_4 is 0 in this call, so the effect-9 spawn FUN_0040caa0 would otherwise
    // perform is off. The separate call to FUN_004286f0, which writes the enemy
    // position into a manager and runs a bulk pass over an effect list, is not
    // modelled.

    // 512 (0x200) is the same call as 0x201 with one flag flipped. TH12 passes
    // FUN_0040caa0(position, radius=f(0), 0, param_4, param_5) with param_4 = 1 for 512
    // and 0 for 513, and param_4 is the effect-9 toggle, not the reward: FUN_00414f40
    // (the enemy pass inside it) applies its damage and award unconditionally and only
    // branches on param_5 to spawn FUN_004273f0(...,9,position,-pi/2,0.6). So 512 asks for
    // a per-bullet effect 9 that 513 does not. cancel_circle has no per-bullet effect hook,
    // so that spawn is a recorded deviation here; the cancel and the award, which are the
    // gameplay-relevant parts, are exact, and reward=true is right for both because TH12
    // awards unconditionally. The separate FUN_004286f0 bulk pass is not modelled either.
    case 0x200:
    case 0x201:
        if(!w.bullets)return -2;
        return w.bullets->cancel_circle(e.current.position,f(0),true,false,w.cancellation?*w.cancellation:BulletCancelContext{})?0:-2;
    // TH12's 0x134 and 0x136 share one body, and the leading
    // `mov edx,0x134 / cmp ax,dx / je` is what picks the struct: 0x134 drives
    // EnemyState::absolute and 0x136 drives EnemyState::relative. This is the
    // Movement switch into polar mode, and every field lines up with the port:
    //
    //   FUN_00408910(edi, arg0) sets edi+0x1c  -> Movement::angle
    //   fstp [edi+0x18] = arg1                 -> Movement::speed
    //   fstp [edi+0x20] = arg2                 -> Movement::radius
    //   fstp [edi+0x24] = arg3                 -> Movement::radial_velocity
    //   flags |= 1                             -> leave velocity mode
    //   FUN_00464eb0                           -> Movement::update_velocity (same flags&1 test)
    //   FUN_00413700                           -> EnemyState::combine_movement
    //
    // FUN_004646e0 is normalize_angle: a +/-pi wrap with the same 32-iteration cap as
    // the port, applied twice (idempotent) by FUN_00408910. The threshold at 0x4a4440
    // is the double -999999, so arg1..arg3 are written for any realistic value.
    //
    // The copy at edi+0x0c looks odd until Movement's two modes are read together: in
    // velocity mode Movement::update does position += velocity, while in polar mode
    // refresh_position does position = velocity + polar(angle,radius). So velocity is
    // the orbit centre in polar mode, and copying position into it before switching is
    // exactly "orbit around wherever the enemy is now". It is skipped when already in
    // polar mode so that re-parameterising the orbit keeps its centre.
    // 309 (0x135) and 311 (0x137) are the interpolator siblings of 308/310: same struct
    // selector, but they also drive the angle and radius interpolators. Read from the
    // decompiled dispatcher (FUN_004154d0, case 0x135/0x137), every field maps onto a
    // construct the port already has:
    //   local_314 = 0x68/0x9c        -> EnemyState::absolute/relative (Movement)
    //   local_2f0 = 0x324/0x360      -> absolute_angle/relative_angle
    //   pfVar11   = 0x39c/0x3d8      -> absolute_radius/relative_radius
    //   f(2)/f(3)/f(4)               -> the three values; each falls back to the Movement's
    //                                   own speed/radius/radial_velocity when it is not
    //                                   greater than -999999, which also covers NaN
    //   FUN_00468e20(...,0) / (...,1) -> duration and mode, read as integer references
    //   piVar9[4..7]                 -> both tangents, from DAT_004ce8dc/DAT_004ce8e0.
    //                                   Those globals are only ever read anywhere in the
    //                                   decompiled tree and sit in .data's zero-filled tail,
    //                                   so both tangents are zero.
    //   FUN_0041c350                 -> Timer::set(0, rate): it leaves previous at -1,
    //                                   current and fractional at 0, and installs the
    //                                   default rate pointer, exactly like the port's set().
    // A duration below 1 skips the interpolation entirely and only zeroes both durations,
    // including skipping the flags OR and the two Movement calls.
    case 0x135:case 0x137:{
        Movement& m=op==0x135?e.absolute:e.relative;
        Vec2Interpolator& angle=op==0x135?e.absolute_angle:e.relative_angle;
        Vec2Interpolator& radius=op==0x135?e.absolute_radius:e.relative_radius;
        const auto keep=[](float value,float current){return value>-999999.f?value:current;};
        const float angle_end=keep(f(2),m.speed);
        const float radius_end=keep(f(3),m.radius);
        const float radial_end=keep(f(4),m.radial_velocity);
        const float start_speed=m.speed,start_radius=m.radius,start_radial=m.radial_velocity;
        const i32 duration=i(0);
        if(duration<1){angle.duration=0;radius.duration=0;break;}
        const auto mode=static_cast<InterpolationMode>(i(1));
        angle.start[0]=0.f;angle.start[1]=start_speed;angle.end[0]=0.f;angle.end[1]=angle_end;
        angle.initial_tangent[0]=0.f;angle.initial_tangent[1]=0.f;angle.final_tangent[0]=0.f;angle.final_tangent[1]=0.f;
        angle.duration=duration;angle.mode=mode;angle.timer.set(0,rate);
        radius.start[0]=start_radius;radius.start[1]=start_radial;radius.end[0]=radius_end;radius.end[1]=radial_end;
        radius.initial_tangent[0]=0.f;radius.initial_tangent[1]=0.f;radius.final_tangent[0]=0.f;radius.final_tangent[1]=0.f;
        radius.duration=duration;radius.mode=mode;radius.timer.set(0,rate);
        m.flags|=1;
        m.update_velocity();
        e.combine_movement();
        break;
    }
    case 0x134:case 0x136:{
        Movement& m=op==0x134?e.absolute:e.relative;
        if(!(m.flags&1))m.velocity=m.position;
        if(f(0)>-999999.f)m.angle=normalize_angle(f(0));
        if(f(1)>-999999.f)m.speed=f(1);
        if(f(2)>-999999.f)m.radius=f(2);
        if(f(3)>-999999.f)m.radial_velocity=f(3);
        m.flags|=1;
        m.update_velocity();
        e.combine_movement();
        break;
    }
    case 0x1c0:c.time-=float(i(world.difficulty==0?0:world.difficulty==1?1:world.difficulty==2?2:3));break;
    case 0x1b4:{float* ref=c.float_reference(0,g);if(!ref)return -2;
        *ref=float(f(world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4));break;}
    case 0x1b3:{i32* ref=c.integer_reference(0,g);if(!ref)return -2;
        *ref=i(world.difficulty==0?1:world.difficulty==1?2:world.difficulty==2?3:4);break;}
    case 0x19e:{const i32 time=i(2),health=i(1),slot=i(0);if(slot<0||slot>=8)return -2;auto& interrupt=e.interrupts[slot];interrupt.health=health;
        if(health>=0){interrupt.time=time;interrupt.health_script=interrupt.timeout_script=reinterpret_cast<const char*>(c.instruction)+32;}break;}
    case 0x1a5:{const i32 slot=i(0);if(slot<0||slot>=8)return -2;e.interrupts[slot].timeout_script=reinterpret_cast<const char*>(c.instruction)+24;break;}
    case 0x156:case 0x15c:case 0x165:case 0x166:case 0x167:{
        if(!w.spells||!w.bosses[0]||c.instruction->length<32)return -2;
        const i32 count=c.instruction->argument<i32>(3);
        if(count<=0||count>64||u32(count)>u32(c.instruction->length)-32)return -2;
        char name[65]{};u8 key=0x77,step=7;const auto* encoded=reinterpret_cast<const u8*>(c.instruction)+32;
        for(i32 j=0;j<count;++j){name[j]=char(encoded[j]^key);key=u8(key+step);step=u8(step+0x10);}
        if(!std::memchr(name,0,count))return -2;
        i32 id=i(0);if(op>=0x165)id=wrapping_add(id,wrapping_sub(world.difficulty,op-0x165));
        (void)i(2);const i32 timeout=i(1);
        if(!w.spells->begin(id,timeout,name,w.bosses[0]->current.position,w.spells->bomb_active))return -2;
        e.health_flags|=1;e.scaled_health=signed_bits(u32(e.health)*7);e.lifetime.set(0,rate);break;
    }
    case 0x157:if(!w.spells||!w.spells->end())return -2;e.health_flags&=~1u;break;
    case 0x16a:if(!w.spells)return -2;w.spells->survival();break;
    case 0x16b:if(!w.spells)return -2;w.spells->hide_circle();break;
    case 0x158:{const i32 section=i(0);if(w.stage_section!=section)w.section_frame=0;w.stage_section=section;break;}
    case 0x15a:{const double radius=c.float_argument(0,g);e.exclusion_distance=float(radius*radius);break;}
    case 0x15b:{const i32 type=i(2);const float value=f(1);const i32 health=e.max_health,slot=i(0);if(slot<0||slot>=4)return -2;w.health_segments[slot]={float(double(value)/health),type};break;}
    case 0x1ad:case 0x1ae:{const u32 index=op==0x1ad?(world.rank<512?0:2):rank_index(world.rank);auto* out=c.float_reference(0,g);destination(out)=f(index);break;}
    case 0x1af:{const double start=f(1),end=f(2);const float value=float((double(world.rank)+1024)*(end-start)*0.00048828125+start);destination(c.float_reference(0,g))=value;break;}
    case 0x160:case 0x161:{const u32 index=op==0x160?(world.rank<512?0:2):rank_index(world.rank);auto* out=c.integer_reference(0,g);destination(out)=i(index);break;}
    case 0x1b2:{const i32 start=i(1),end=i(2);const i32 product=signed_bits(u32(wrapping_sub(end,start))*u32(wrapping_add(world.rank,1024)));destination(c.integer_reference(0,g))=wrapping_add(product/2048,start);break;}
    case 0x163:{if(u32(world.difficulty)>4)break;const i32 value=i(world.difficulty>=3?4:world.difficulty+1);destination(c.integer_reference(0,g))=value;break;}
    case 0x164:{if(u32(world.difficulty)>4)break;const float value=f(world.difficulty>=3?4:world.difficulty+1);destination(c.float_reference(0,g))=value;break;}
    case 0x168:w.remaining_phases=i(0);break;
    case 0x169:e.collision_immunity.set(i(0),rate);break;
    case 0x16c:e.flags=(e.flags&~0x800000u)|((u32(i(0))&1)<<23);break;
    // 0x25a (602, signature S, 98 uses in th12.dat) is not a bullet fire. Its handler builds a
    // 0x1e8-byte descriptor on the stack, copies emitter i(0)'s whole transform table into it, fills it
    // with the emitter's sprite, colour, normalised angle and position, and calls 0x428450 with mode 0.
    // That function is the effect-entity allocator, not the bullet spawner: mode 0 allocates a 0x1e8
    // effect from the pool at [0x4b44f4] (full at 0x100) and mode 1 allocates a 0x208 one, which the
    // neighbour 0x25b uses. The position derivation inside is byte for byte the one 0x1f5 uses, but
    // 0x25a stores it into the descriptor rather than back into the emitter, so the emitter is left
    // alone and the port - which owns no effect-entity pool - has nothing observable to keep beyond the
    // argument read. Only i(0) is read: the disassembly has a single argument fetch at 0x4189d7.
    case 0x25a:{
        const i32 index=i(0);                                    // 0x4189d7
        if(u32(index)>=8)return -2;
        break;
    }
    // 0x1fe (510, empty signature, 8 uses in th12.dat) is the bare cancel pair at 0x4187af:
    //   push 1 ; call 0x40cf60                          ; clear bullets, third argument 1
    //   xor edi,edi ; lea ebx,[edi+1] ; call 0x428750   ; cancel lasers
    // It reads no arguments at all, which is why its signature is empty. FUN_0040cf60 walks the 2000
    // bullet slots and marks every on-screen bullet with flag bit 8 - the cancel flag - which is
    // BulletManager::cancel_all; FUN_00428750 walks the laser list at DAT_004b44f4+0x18 and calls each
    // object's vtable+0x14, which is LaserManager::cancel_all. The neighbour 0x1a2 calls the same two
    // with third argument 0, so its bullets are not filtered by delay; here the third argument is 1,
    // which is the skip_delayed arm. 0x1a2 also closes the dialogue and erases the stage enemies;
    // 0x1fe does neither, so only these two clears belong here.
    case 0x1fe:{
        const BulletCancelContext context=w.cancellation?*w.cancellation:BulletCancelContext{};
        if(!w.bullets||!w.lasers)return -2;
        if(!w.bullets->cancel_all(true,context))return -2;
        if(!w.lasers->cancel_all(false,false))return -2;
        break;
    }
    // 0x1c5 (453, signature S, 3 uses in th12.dat) is a one-liner at 0x415acd:
    //   mov edx,[esi+4] ; call 0x468e20 (i(0)) ; mov [ebx+0x168c],eax
    // It reads no other argument and returns 0. The field is named after its TH12 offset because no
    // other body reads it and its consumer is not established - see EnemyState::value_168c.
    case 0x1c5:e.value_168c=i(0);break;
    // 0x258 (0x41866e, signature Sffff, 70 uses in th12.dat): the emitter position setter. i(0) picks
    // the emitter, f(1), f(2) and f(3) go straight into its position and f(4) into the word at +0x1d4.
    // The original indexes with i(0)*0x214 and no bound check; the port keeps the emitter index guard
    // the other emitter commands use, which is the same thing for every script in the archive.
    case 0x258:{
        const i32 index=i(0);                                    // 0x41866e
        if(u32(index)>=8)return -2;
        auto& b=e.emitters[index];                               // 0x41867d
        b.position.x=f(1);                                       // 0x418693
        b.position.y=f(2);                                       // 0x41869e
        b.position.z=f(3);                                       // 0x4186b0
        b.value_1d4=f(4);                                        // 0x4186c2
        break;
    }
    // 0x138 (0x41758d) and 0x139 share one 652-byte body. It is the 0x131 family's shape with a single
    // float argument: it derives an angle from the enemy's four quad_15f4 floats against its own current
    // position and the player's, snaps it at the playfield edge, then writes an interpolation from
    // {angle, speed f(2)} down to {angle, 0}. Original addresses are on each line because the branch chain
    // is dense. The argument order is i(1), f(2), i(0) - not 0, 1, 2 - which is visible when two
    // arguments resolve the same RNG variable.
    case 0x138:case 0x139:{
        const bool absolute_op=(op==0x138);
        auto& m=absolute_op?e.absolute:e.relative;
        auto& p=absolute_op?e.absolute_angle:e.relative_angle;
        const Vec3& cur=e.current.position;
        const float q0=e.quad_15f4[0],q1=e.quad_15f4[1],q2=e.quad_15f4[2],q3=e.quad_15f4[3];
        // 0x409310 is ((f64)(u32)rng * 2^-31 - 1.0) * pi: Rng::signed_unit() scaled by pi, a random angle
        // in [-pi, pi). Its two scaling constants are 0x4a3ea8 = 2^-31 and 0x4a3ce0 = 1.0, and the
        // negative-int correction 0x4a3e10 is 2^32.
        const auto random_angle=[&](){return float(double(world.rng().signed_unit())*3.1415927410125732421875);};
        float a;
        if(q0>cur.x){                                                                     // 0x4175db
            a=float(double(random_angle())/3.0);                                           // 0x4175e7 fdiv 3.0
        }else if(cur.x>q2*0.25f){                                                         // 0x417604
            a=normalize_angle(float(double(random_angle())/3.0+3.1415927410125732421875)); // 0x417612 fadd pi
        }else{                                                                            // 0x41762a
            a=random_angle()*0.25f;                                                       // 0x417646 fmul 0.25
            const u32 roll=world.rng().next32()%3u;                                       // 0x417655 idiv 3
            // The two arms of this branch differ only in which side of zero the roll lands on: a non-zero
            // roll (0x417667) or a zero one (0x417691) adds the quarter turn, and the comparison at
            // 0x417638 decides which. 0x4b4514+0x97c is the player position x: it is the same global the
            // ECL property -9991 reads through the getter at 0x41a0b2, and +0x980 is its y.
            const bool player_above=world.player_position.x>cur.x;                        // 0x41763f
            if((roll!=0)!=player_above)a=float(double(a)+3.1415927410125732421875);       // 0x417693
        }
        // 0x4176a1: the vertical stage, with the same quarter-velocity margins as the horizontal one.
        if(q1>cur.y)a=(a<0?-a:a);                                                         // 0x4176c7 fabs
        else if(cur.y>q3*0.25f)a=-(a<0?-a:a);                                             // 0x4176f5 fchs
        // 0x417719: snap when the angle is already within 0.05 of a quarter turn away from vertical. The
        // unordered arm at 0x417751 leaves the angle untouched, so only this finite test changes it, and
        // the two snapping values are -(pi/2+0.05) and -(pi/2-0.05) taken as f32 from 0x4a4434/0x4a4430.
        const float quarter=float(double(a)+1.57079637050628662109375);
        if((quarter<0?-quarter:quarter)<=0.05f)a=(a<=-1.57079637050628662109375f)?-1.6207963f:-1.5207964f;
        // Tail. 0x41c300 and 0x41c320 copy the global tangent pair 0x4ce8dc/0x4ce8e0 into both tangents;
        // nothing in the image ever writes those two words, so they stay zero and zeroing here is the same.
        const i32 mode=i(1);p.mode=InterpolationMode(mode);                               // 0x4177a0, 0x4177ad
        const float speed=f(2);                                                           // 0x4177c8
        const i32 duration=i(0);p.duration=duration;                                      // 0x4177d5, 0x4177da
        p.initial_tangent[0]=p.initial_tangent[1]=p.final_tangent[0]=p.final_tangent[1]=0;
        p.start[0]=a;p.start[1]=speed;                                                    // 0x4177f7/0x4177fd
        p.end[0]=a;p.end[1]=0;                                                            // 0x417802/0x417805
        p.timer.set(0,rate);                                                              // 0x41c350
        m.flags&=~3u;                                                                     // 0x417811
        break;
    }
    case 0x1be:e.flags=(e.flags&~0x1000000u)|((u32(i(0))&1)<<24);e.alternate_animation=i(1);e.flags&=~0x2000000u;e.saved_animation=e.base_animation;break;
    case 0x1bf:world.rate=f(0);break;
    case 0x170:{const i32 difficulty=world.difficulty;const float frames=float(i(difficulty==0?0:difficulty==1?1:difficulty==2?2:3));c.time=float(double(c.time)-frames);break;}
    case 0x171:e.flags=(e.flags&~0x8000000u)|((u32(i(0))&1)<<27);break;
    case 0x1c2:e.animation_options=u32(i(0));break;
    case 0x1c4:e.facing=i(0);break;
    case 0x1b9:if(!w.stage||!w.stage->interrupt(i(0)))return -2;break;
    // 425 (0x1a9) is one call, FUN_00414c60, walking DAT_004b43dc+0x68 - the same enemy list
    // FUN_00414f40 walks - and erasing the stage enemies. The port already has that as
    // EnemyManager::erase_stage_enemies, the call 0x159 and 0x173 make. Round 32 reverted this
    // because the pre-chain emitter gate claimed 0x1a9 and swallowed it; that claim is a TH11
    // residue and is now removed, so this case is reachable.
    case 0x1a9:
        if(!w.manager)return -2;
        return w.manager->erase_stage_enemies()?0:-2;
    // 411 sets the ring descriptor. FUN_0041aba0 reads ring_scale and ring_sides to draw a
    // closed 31-point ring; the game-side four-entry table FUN_004126f0 clears is not modelled
    // here because the port owns no such table. See EnemyState.hpp for the evidence.
    case 0x19b:{
        const i32 scale=i(0);
        e.ring_scale=scale;e.ring_sides=scale;e.ring_saved=scale;e.ring_step=i32(u32(scale)*7u);
        if(e.flags&0x400000)e.flags|=0x20000000;
        break;
    }
    // 526 destroys any previous effect instance, then creates one with duration f(0), a fixed
    // 16.0 scale, id i(1) and a zeroed position, but only when the duration is above zero. The
    // instance itself is not spawned: see the field comment in EnemyState.hpp.
    case 0x20e:{
        e.effect_duration=f(0);e.effect_scale=16.0f;e.effect_id=i(1);e.effect_position={};
        e.effect_active=e.effect_duration>0.0f;
        break;
    }
    // 424's only effect is on the game-side global DAT_004b0c40, which the port does not
    // model: its disassembly loads that base into EAX rather than the enemy. Nothing is left for
    // the port to do, but the case must exist or the chain's fallback refuses the opcode.
    case 0x1a8:break;
    // 428 (0x1ac), 437 (0x1b5), 438 (0x1b6) and 439 (0x1b7) share this body exactly; the only
    // per-opcode difference in the disassembly adjusts the pointer handed to the loader
    // (esi += DAT_004b0ca8, -1 or -2 by opcode), and that loader is the game-side one the port does
    // not model. So all five reach this same case, and 437/438/439 leave the collision list.
    // 422 begins by decoding an XOR-obfuscated string out of the instruction and handing it to
    // FUN_0040e060, a 1359-byte loader that installs it into the game-side global DAT_004b43cc;
    // 428, 437, 438 and 439 share that body and pick a per-opcode offset into DAT_004b0ca8. The
    // port loads resources up front and owns no such global, so the loader is recorded, not
    // reimplemented. What is enemy state is kept, including the fall into 413's lifetime reset -
    // 422's body ends exactly at 413's case address.
    case 0x1a6:case 0x1ac:case 0x1b5:case 0x1b6:case 0x1b7:{
        e.ring_flags|=1u;
        e.ring_step=i32(u32(e.ring_scale)*7u);
        e.lifetime.set(0,rate);
        break;
    }
    // 423 is the unload half: FUN_0040e5e0 tears the same game-side global down, which has no port
    // counterpart, and clears the enemy's bit 0.
    case 0x1a7:{
        e.ring_flags&=~1u;
        break;
    }
    // 529/530/531 select a callback from one of the three tables and store it in a slot. The port
    // keeps the index; see the callbacks field in EnemyState.hpp for the table contents and for why
    // the pointers themselves are not kept.
    case 0x211:{const i32 n=i(0);if(u32(n)>6)ecl_trap(__FILE__,__LINE__);e.callbacks[0]=n;break;}
    case 0x212:{const i32 n=i(0);if(u32(n)>1)ecl_trap(__FILE__,__LINE__);e.callbacks[1]=n;break;}
    case 0x213:{const i32 n=i(0);if(u32(n)>1)ecl_trap(__FILE__,__LINE__);e.callbacks[2]=n;break;}
    // 534 invokes DAT_004af0fc[i(0)] with the enemy as its argument. Those callbacks draw custom
    // geometry the port cannot reproduce (FUN_0041aba0 draws a closed ring of 31 points), so this is
    // a documented no-op rather than a guess at what they render.
    case 0x216:break;
    // 409 spawns the drop described by the enemy's drop descriptor and clears its primary field:
    // FUN_00412100 reads *(e+0x1620) - drops.primary, not an effect slot - and passes it to
    // FUN_004273f0, which is 1798 bytes dispatching on that value (9, 10..15, 0x10..0x12) into the
    // DAT_004b44f0 item-manager family, with a fixed -pi/2 and 2.2, then detaches through
    // FUN_00412140 and writes *(e+0x1620) = 0. The item manager is host-side and unmodelled, so only
    // the clear is kept. Rounds 43/44 had misread +0x1620 as a four-entry effect-id array; the drop
    // layout disproves that, and 406/407/408/410 - which write the same block - are all correct.
    case 0x199:{
        e.drops.primary=0;
        break;
    }
    // Verified against TH12's offsets, not self-declared: 406 clears counts (0x4c = 19 words at
    // +0x1624, so primary survives), 407 indexes the block as words, 408 writes spread (0x1620 +
    // 0x50 = 0x1670) and 410 writes primary.
    // 404 sets flags bit 0x10000 and stores four floats from arguments 0..3 at +0x15f4..+0x1600.
    case 0x194:{
        e.flags|=0x10000u;
        for(i32 k=0;k<4;++k)e.quad_15f4[k]=f(k);
        break;
    }
    // 405 clears flags bit 0x10000, the matching half of 404: the body is
    // `and dword[ebx+0x16b8], 0xfffeffff` and nothing else.
    case 0x195:{
        e.flags&=~0x10000u;
        break;
    }
    // 455 stores FUN_00412500(i(1)) - whether an enemy with that instance id exists - into the
    // variable named by argument 0. Comparing call sites shows FUN_0041a910 targets argument 0, and
    // its two branches match EclContext::integer_reference, whose local branch indexes stack.data
    // from frame_base and whose negative branch delegates to the globals.
    case 0x1c7:{
        i32* target=c.integer_reference(0,g);
        if(!target||!w.manager)return -2;
        *target=w.manager->enemy_exists(i(1))?1:0;
        break;
    }
    // 0x1ba sets bit 3 of the loader global with no argument: mov eax,[0x4b43cc]; or [eax+0x7c],8.
    // The port read i(0) and wrote bit 0, which was TH11's meaning for this number.
    case 0x1ba:w.manager_flags|=8u;break;
    // 0x1bb is a bare call to FUN_004127b0, which sets bit 4 of DAT_004b43cc+0x7c and then tears down
    // the object that global holds at +0x20 (FUN_00461a70 with it, then zeroing the slot). The
    // teardown belongs to a host-side object the port does not own, so only the flag is kept.
    case 0x1bb:w.manager_flags|=0x10u;break;
    case 0x1bc:{const i32 id=i(0);if(u32(id)>u32(EnemyDamage::Arc))return -2;e.damage_callback=EnemyDamage(id);break;}
    // 445 calls FUN_00428670, which sweeps the global list at DAT_004b44f4+0x18, resetting each
    // object's effect instance (words 0x10e..0x113 = byte offsets 0x438..0x44c, the same instance
    // 0x1c6 names) and notifying it through virtual slot 5 with (1,0). The port has no enemy
    // effect-instance system to reset, so this is a documented no-op.
    // This replaced a body that set e.collision_callback from i(0). TH12's 0x1bd reads no argument
    // - the decomp calls FUN_00428670 with none and the disassembly is a bare call - so that body
    // carried a TH11 opcode number. Where TH12 sets the collision callback is now an open question,
    // recorded rather than guessed at; 0x1bc (444) may be it.
    case 0x1bd:break;
    case 0x1c1:{const i32 id=i(0);i32 result=0;if(!id||u32(id)>u32(EnemyTick::TransformAngles)||!w.callbacks||!w.callbacks->invoke_tick(e,EnemyTick(id),result))return -2;break;}
    case 0x1b8:{
        if(!w.deformations)return -2;
        if(e.deformation&&!w.deformations->release_deformation(e))return -2;
        e.deformation=nullptr;e.deformation_target=f(0);e.deformation_radius=16;e.deformation_color=u32(i(1));
        e.deformation_phase_x=e.deformation_phase_y=normalize_angle(0);
        if(e.deformation_target>0&&!w.deformations->create_deformation(e))return -2;break;
    }
    default:return -2;
    }
    return 0;
}
}
