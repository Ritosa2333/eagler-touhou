#include "EclRoutes.hpp"
#include "EnemyScript.hpp"
#include "EnemyAnimations.hpp"
#include "EnemyManager.hpp"
#include "NoopOpcodes.hpp"
#include <cstdlib>
namespace th11 {
namespace {
// TH12 keeps TH11's core opcode numbering (0..89) but renumbers much of the
// extended range. For these ids TH12's instruction signature differs from the
// TH11 opcode of the same number, so routing them into a TH11 handler runs a
// different instruction with the wrong argument layout - silently, because the
// handler still returns success. That is what made stage 1 die on a divide trap
// instead of reporting an error.
//
// Until each of these is transcribed from FUN_004154d0 (@0x4154d0, th12-decomp)
// the engine refuses them and reports `unavailable`, which the session surfaces
// by opcode. The list is checked against docs/th12/ecl-extended-opcodes.json by
// tests/cpp/th12-ecl-coverage.test.mjs, so it cannot drift from the evidence.
// 320 (0x140) and 322 (0x142) set the absolute or relative Movement from six guarded floats. The body
// is at 0x417093: lea esi,[ebx+0x68] or [ebx+0x9c]; f(0); six FLOAT reads; if(!(flags&1)) [+0x0c]=[+0x00];
// then FUN_00408910(m,f(0)) writes m[+0x1c] = FUN_004646e0(FUN_004646e0(f(0))) - the angle normalizer,
// applied twice - and guarded stores follow at +0x18, +0x20 and +0x24.
//
// The port's Movement is position +0x00, velocity +0x0c, speed +0x18, angle +0x1c, radius +0x20,
// radial_velocity +0x24, ellipse_angle +0x28, ellipse_ratio +0x2c, flags +0x30. Read against that, 320
// is argument-for-argument the body this file used to carry at 0x12c before round 56 replaced it:
// angle = f(0) through the normalizer, speed = f(1), radius = f(2), radial_velocity = f(3), and
// if(!(flags&1)) velocity = position. In other words the port already had TH12's 0x140 body, parked at
// the wrong opcode number. Round 57 then identified FUN_004646e0 as the angle normalizer, read the
// flags tail, landed it as case 0x140 and removed it from the list below.
//
// 277 (0x115) and 278 (0x116) resolve &Enemy[0xe0 + i(0)*4] through FUN_00461c50, a pointer table the
// port does not model, and write fields of the object it returns: 277 sets +0x2c and flag bit 4, 278
// sets +0x40 and +0x44 and flag bit 8. They stay here until that object class exists.
//
// 325 (0x145) @ 0x416831, 397 bytes, shared with 326. It selects the absolute or relative Movement,
// selects a second object at ebx+0x28c or ebx+0x2d8 by opcode, reads seven floats and one integer,
// then writes that object's +0x18, +0x1c, +0x20, +0x24, +0x28 and +0x2c, an integer into +0x44, the
// constant 8 into +0x48, and copies the Movement's position into +0x00..+0x08. The 0x4c gap between
// 0x28c and 0x2d8 matches that field range exactly. The port does not identify that object, and
// guessing an 0x4c field layout is the invented-field mistake this project has already paid for twice,
// so it refuses. It is the same reason 321 (0x141, 715 bytes, shared with 323) remains here: its body
// is the largest of the set and is not decoded.
// 320 (0x140) left this list in round 57: its body was transcribed and verified, so it is the first
// entry to come off it by being implemented rather than by being recorded as unmodellable.
// 277 (0x115) left this list in round 186. It resolves &Enemy[0xe0 + i(0)*4] through
// FUN_00461c50, which is not an unmodelled class after all: the slot holds an id, FUN_00461c50 looks
// the id up through FUN_00461920 - the same two-list, id-at-+0x00 lookup the 0x192/0x193 body already
// drives through w.animation_visibility - and caches the entry pointer back into the slot. The entry
// is the animation object the port models as AnmVm, and it agrees with the port from +0x00 to +0x2c,
// so 277's [entry+0x2c] IS AnmVm::rotation.z. 277 then sets bit 2 of the entry's flags word at
// +0x47c, and bit 2 is the port's own "rotation changed" marker, set by AnmInterpreter for a rotation
// velocity or rotation interpolation and cleared by AnmProjection once the rotations are applied.
// So 277 is exactly "set rotation.z and mark the rotation dirty", which is what its body in
// EnemyAnimations.cpp already does. The port's flags word sits at its own offset (0x404) rather than
// the original's +0x47c because the port shares algorithms, not object layouts; that is not a
// divergence in behaviour and is why 277 no longer needs to be refused.
//
// 278 (0x116) stays here: it writes +0x40 and +0x44 of the same entry and flag bit 8, and the port has
// no body for it.
// 417 (0x1a1) left this list in round 188, and it was never an unmodelled class. Its body at 0x4187df
// pushes 0x46, then 0, then i(2), i(1) and i(0), then the constant 1, and calls FUN_004529a0, whose
// `ret 0x18` confirms six stack arguments: FUN_004529a0(1, i(0), i(1), i(2), 0, 0x46). FUN_004529a0 is
// operator_new(0x44) + memset(0x44) + `or [obj],2` + `and [obj+0x40],~1` + FUN_00452a60, and
// FUN_00452a60 dispatches on a small mode through a jump table at 0x452ed0. That is the screen-shake
// object: the port already had case 0x1a1 as w.shakes->start(duration, start, end, rate) with the
// arguments read in the disassembly's order i(2), i(1), i(0), which is exactly what the pushes deliver,
// and 0x46 is the shake id the constructor takes. The guard was hiding a correct body behind a refusal,
// which is why the script stopped with -1001 on an opcode the port could already run.
constexpr u16 kTh12CollidingOpcodes[]={278,281,321};
// 419 (0x1a3) is NOT a no-op, which this note used to claim on the strength of a decompiler summary of
// FUN_00412720. Its handler at 0x41883d has a third exit: after two returns of zero it does
// `or eax,-1 ; jmp 0x41962d`, landing one byte past the `xor eax,eax` at 0x41962b, so the -1 survives. It is
// a dialogue wait, and EnemyStateCommands.cpp carries the matching body. The routing had also been sending
// it into the laser range, so its body could never run.
// The collide list is a list of TH12 numbers whose bodies are not decoded, and a title that gives one
// of those numbers a different command must not inherit the refusal. TH13's 0x141 is exactly that: its
// record is TH12's named spawn byte for byte, and the manager route serves it through
// ecl_named_spawn_relative (EclRoutes.hpp). The list stays as the TH12 table; the row decides whether
// the number is still undecoded for the title at hand.
bool colliding(u16 opcode)noexcept{
    if(ecl_named_spawn_relative(opcode))return false;
    for(u16 id:kTh12CollidingOpcodes)if(id==opcode)return true;
    return false;}
// Execution-order ring of extended commands, for the question trace_point cannot answer: the interpreter's
// own trace covers call/jump/spawn/return, not the 0x1xx commands, so a script that ought to trigger
// something and does not leaves nothing to read.
i32 g_ecl_trace[kEclTraceCapacity]{};
u32 g_ecl_trace_total=0;
u32 g_ecl_update_total=0;
u32 g_ecl_error_opcode=0xffffffffu;
float g_ecl_error_time=0;
// A ring only remembers the tail, and the question here is about the whole run: whether an opcode that ends a
// stage section is ever executed at all, and whether it is executed once or thousands of times. A monotonically
// counted histogram answers that where the ring cannot. 0x100..0x2ff covers every extended id the tables assign.
constexpr u32 kEclCountBase=0x100,kEclCountSize=0x200;
u32 g_ecl_counts[kEclCountSize]{};
}
u32 ecl_opcode_count(u32 opcode)noexcept{return opcode<kEclCountBase||opcode>=kEclCountBase+kEclCountSize?0u:g_ecl_counts[opcode-kEclCountBase];}
u32 ecl_trace_total()noexcept{return g_ecl_trace_total;}
u32 ecl_update_total()noexcept{return g_ecl_update_total;}
u32 ecl_error_opcode()noexcept{return g_ecl_error_opcode;}
float ecl_error_time()noexcept{return g_ecl_error_time;}
i32 ecl_trace_at(u32 index)noexcept{
    if(index>=g_ecl_trace_total)return -1;
    const u32 oldest=g_ecl_trace_total>kEclTraceCapacity?g_ecl_trace_total-kEclTraceCapacity:0;
    if(index<oldest)return -1;
    return g_ecl_trace[index%kEclTraceCapacity];
}
i32 EnemyScriptServices::update(float elapsed){
    ++g_ecl_update_total;
    if(error.instruction){g_ecl_error_opcode=error.opcode;g_ecl_error_time=error.time;return -2;}
    auto& owner=globals.enemy.script_owner->script;const i32 result=owner.update_threads(elapsed,*this);
    if(result==-2&&!error.instruction&&owner.active&&owner.active->instruction)error={owner.active->instruction,owner.active->thread_id,owner.active->time,owner.active->instruction->opcode};
    return result;
}
// Set by the interpreter's argument pops when they run dry, and read together with
// `g_ecl_last_opcode` (set here) so the failing command can be named in one line.
u32 g_ecl_last_opcode=0;
// The id the generation's 0x276 resolved, kept where the port's script selection can read it; the
// original stores the found record's offset in its owner's +0x20/+0x1c pair, and this is the part
// of that effect the port has a name for until the selection is consumed.
i32 g_ecl_selected_sub_id=-1;
i32 g_th14_fire_stage=0,g_th14_fire_bullet_error=0;
i32 g_th14_bullet_sprite=-1,g_th14_bullet_transform_index=-1,g_th14_bullet_transform_opcode=-1;
i32 g_th14_bullet_program[36];
u32 g_th14_bullet_unhandled=0;
u32 g_th14_bullet_unsupported_native=0;
i32 EnemyScriptServices::command(EclContext& context){
    auto& enemy=globals.enemy;
    g_ecl_last_opcode=context.instruction->opcode;
    g_ecl_trace[g_ecl_trace_total%kEclTraceCapacity]=i32(context.instruction->opcode);
    ++g_ecl_trace_total;
    if(u32 op=context.instruction->opcode;op>=kEclCountBase&&op<kEclCountBase+kEclCountSize)++g_ecl_counts[op-kEclCountBase];
    if constexpr(kPortTitle==14){
        const u16 op=context.instruction->opcode;
        // Native TH14 extended numbers are a separate instruction set. A TH12
        // handler accepting the same number does not establish compatibility.
        i32 result=-2;
        if(runtime_noop_opcode(op))result=0;
        else if((op==300||op==301||op==304||op==305||(op>=309&&op<=312)||op==321||op==432||op==433||op==509||op==515||op==516||op==517||op==518||op==523||op==525||op==545||op==613||op==615||op==616||op==802)&&commands.manager)
            result=commands.manager->command(enemy,context,globals);
        else if((op==302||op==303||op==306||op==307||op==308||op==313||op==315||op==316||op==318||op==319||op==320||op==325||op==326||op==327||op==328||op==329||op==330||op==331||op==332||op==335||op==336||op==337)&&commands.animations)result=commands.animations->command(enemy,context,globals);
#if PORT_TITLE == 14
        else if(op==554&&commands.animations)result=commands.animations->th14_stage_logo(commands.th14_runtime_mode,commands.th14_run_flags)?0:-2;
#endif
        else if((op>=400&&op<=447))result=enemy_th14_movement_command(enemy,context,globals);
        else if(op==601)result=enemy_th14_fire_command(enemy,context,globals,commands);
        else if(op==615||op==616||op==629||op==630||op==632||op==633||op==637)result=enemy_th14_state_command(enemy,context,globals,commands);
        else if(op>=600&&op<=641)result=enemy_th14_emitter_command(enemy,context,globals);
        else result=enemy_th14_state_command(enemy,context,globals,commands);
        if(result==-2)error={context.instruction,context.thread_id,context.time,op};
        return result;
    }
    if constexpr(kPortTitle==16){
        const u16 op=context.instruction->opcode;
        if(op==522||op==523||op==537||op==538||op==539||op==542||op==543||op==567||op==568){const i32 result=enemy_th16_spell_command(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==339||op==307||op==308||op==316||op==338){const i32 result=commands.animations?commands.animations->command(enemy,context,globals):-2;if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==556){const i32 result=enemy_th16_death_script_command(enemy,context);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==632){const i32 result=enemy_th16_callback_command(enemy,context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==319||op==325||op==327||op==329||op==331||op==335||op==337){const i32 result=commands.animations?commands.animations->command(enemy,context,globals):-2;if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op>=400&&op<=447){const i32 result=enemy_th16_movement_command(enemy,context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
#if PORT_TITLE == 16
        if(op==513){enemy.th16_phase_timer.set(0,&globals.environment.rate);return 0;} // 421860.
        if(op==515){enemy.damage_immunity.set(context.integer_argument(0,globals),&globals.environment.rate);return 0;} // Own4206bf / 406490.
        if(op==540){commands.remaining_phases=context.integer_argument(0,globals);return 0;} // Own422536, HUD+188.
#endif
        if(op==1000||op==1001){const i32 result=enemy_th16_time_drop_command(enemy,context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==500||op==501){const i32 result=enemy_th16_hitbox_command(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==535||op==536){const i32 result=enemy_th16_difficulty_command(context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==511){const i32 result=enemy_th16_health_command(enemy,context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==507||op==508||op==510){const i32 result=enemy_th16_drop_command(enemy,context,globals);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==527){const i32 result=enemy_th16_health_marker(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==514||op==521){const i32 result=enemy_th16_interrupt_command(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==629){const i32 result=enemy_th16_deformation_command(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if(op==524||op==569||op==570){const i32 result=enemy_th16_chapter_command(enemy,context,globals,commands);if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;}
        if((op>=600&&op<=641)||op==700||op==701||op==702){
            // Own TH16 setters run ahead of any legacy emitter body. Emission
            // and unmeasured commands remain explicit gaps until ported.
            const i32 result=op==613||op==615||op==616?enemy_th16_cancel_command(enemy,context,globals,commands):(op==601||op==702)?enemy_th16_fire_command(enemy,context,globals,commands):enemy_th16_emitter_command(enemy,context,globals);
            if(result==-2)error={context.instruction,context.thread_id,context.time,op};return result;
        }
#if PORT_TITLE == 16
        if(op==554){
            const i32 result=commands.animations&&commands.animations->th16_stage_logo(commands.th16_runtime_mode,commands.th16_run_flags)?0:-2;
            if(result==-2)error={context.instruction,context.thread_id,context.time,op};
            return result;
        }
#endif
        // These measured TH16 bodies must precede the legacy movement/emitter
        // gates: TH12's emitter gate consumes 502 as a no-op on newer titles.
        if(op==300||op==301||op==304||op==305||(op>=309&&op<=312)||op==321||op==502||op==503||op==509||op==512||op==516||op==517||op==518||op==520||op==525||op==545||op==800){
            const i32 result=commands.manager?commands.manager->command(enemy,context,globals):-2;
            if(result==-2)error={context.instruction,context.thread_id,context.time,op};
            return result;
        }
    }
    // The generation's selection of a script block by a numeric id. TH12 has no such command
    // (`--words 630` counts 0 in its whole ECL set against 4/28/35/34/33 in the other five), and
    // the number's group holds only itself in each of TH16/TH17/TH18 (tools/th-ecl-dispatch.py
    // over 0x12c..0x2bc), so the case is the routine the resolver returns. TH17's own (RVA
    // 0x2627d) reads the record's one argument and calls a walk over the file's records - the ones
    // tagged 0x10, which is the tag of every ECLH header in these files - matching the argument
    // against the record's dword at +8 and storing the found offset into the owner's +0x20/+0x1c
    // (RVA 0xc580). The instruction stream continues in place afterwards, so it selects what the
    // next spawn runs rather than jumping. The port keeps the id it resolved.
    if(kPortTitle!=12&&context.instruction->opcode==0x276){
        g_ecl_selected_sub_id=context.integer_argument(0,globals);
        return 0;}
    if(colliding(context.instruction->opcode)){
        error={context.instruction,context.thread_id,context.time,context.instruction->opcode};
        return -2;
    }
    // TH12 routes 193 opcodes in 0x100..0x263 straight to FUN_004154d0's epilogue at 0x41962b, whose
    // body is `xor eax,eax` followed by the SEH unwind and `ret`: they do nothing and report success.
    // TH12's byte table at 0x419890 gives some of those numbers to real TH11 handlers, which is the same
    // "live shadow" that the 0x1a4 check below is written for and that made 0x118 leave the movement
    // switch - except the artifact lists 193 of them rather than one. This has to be checked before the
    // handlers: a handler that acts on a no-op still returns success, so the divergence would only show
    // up in the enemy's state, which is exactly how the oracle found 0x11a writing position.x.
    // Table: docs/th12/ecl-case-bodies.json, generated by tools/th12-noop-opcodes.mjs.
    if(runtime_noop_opcode(context.instruction->opcode))return 0;
    if(enemy_movement_command(enemy,context,globals)||enemy_emitter_command(enemy,context,globals))return 0;
    // 0x1a4 is a literal no-op in TH12: FUN_004154d0's case 0x1a4 body is `break;`.
    // TH11 routed this number into the laser handler, which would act on it.
    if(context.instruction->opcode==0x1a4)return 0;
    i32 result=-2;
    const u16 op=context.instruction->opcode;
    // TH11 routed 0x191 into enemy_fire_command. TH12 gives 0x191 to the
    // collision box setter (case body 0x417906) and moves the fire command to
    // 501, whose body matches that function line for line, so the route is now
    // 0x1f5. Two rounds ago this route was removed outright rather than left at
    // 0x191, because a wrong route silently runs the wrong handler; now that the
    // right number is known it is wired back at the right number.
    if(commands.manager&&ecl_manager_route(op))result=commands.manager->command(enemy,context,globals);
    else if(op==0x1f5)result=enemy_fire_command(enemy,context,globals,commands);
    // 0x19c is the boss registration command and belongs to the state dispatcher. It sat inside the laser
    // range because TH11's 0x19c spawned a laser, so registration was routed away from its own case and
    // could never run. The range starts at 0x19d, where the laser commands actually begin. TH12 uses 0x19c
    // thirty seven times in the archive and every one of them was being sent to the wrong dispatcher.
    // 0x1a1 and 0x1a3 are the same failure 0x19c had, and they were still inside the range. 0x1a1 is the
    // screen shake (FUN_004529a0(1, i(0), i(1), i(2), 0, 0x46) allocates the 0x44-byte shake object) and
    // 0x1a3 is the dialogue wait (FUN_004154d0's body returns -1 while the dialogue is running and 0 when
    // it is not). Neither spawns a laser, so neither belongs to the laser dispatcher; the port's correct
    // bodies for both live in EnemyStateCommands.cpp and could never run while the range claimed them.
    // 0x1a2 is excluded for the same reason 0x19c, 0x1a1 and 0x1a3 were: it is the dialogue starter
    // (FUN_00418813 calls FUN_0041fbe0, which creates the dialogue for i(0)), not a laser op. It sat
    // inside this range, so the port's real starter body in EnemyStateCommands.cpp could never run and
    // the engine never started a section dialogue at all.
    // 0x19a (410) was in this range explicitly rather than by falling inside it, and it is the same mistake
    // the note above describes for 0x19c, 0x1a1, 0x1a2 and 0x1a3. The dispatch formula puts its handler at
    // 0x417a38, inside the contiguous block the drop family occupies - 0x196 at 0x4179b4, 0x197 at
    // 0x4179cc, 0x198 at 0x4179ef, 0x199 at 0x417a25, 0x19a at 0x417a38, 0x19b at 0x417a4c - while the
    // laser block does not begin until 0x19d, whose handler is 0x418963. The body at 0x417a38 is
    // `xor ecx,ecx ; mov eax,ebx ; call 0x41a900 ; mov [ebx+0x1620],eax`, which is drops.primary = i(0),
    // exactly the case EnemyStateCommands.cpp already carries. It reaches that case by falling through to
    // enemy_state_command below, the same way its siblings 0x196, 0x197 and 0x198 do - routing it to
    // commands.manager instead puts it in front of EnemyManager's per-enemy state machine, which has no
    // case for it. 0x16d is out for a different reason: the
    // formula resolves it to 0x41962b, the `xor eax,eax` epilogue that 193 opcodes share, so it is a no-op
    // and its body can only be reached by falling through to enemy_state_command at the end.
    else if((op>=0x19d&&op<=0x1a5&&op!=0x19e&&op!=0x1a5&&op!=0x1a0&&op!=0x1a1&&op!=0x1a2&&op!=0x1a3)||(op>=0x1ac&&op<=0x1ad)||(op>=0x1af&&op<=0x1b2)||(op>=0x1be&&op<=0x1bf)||op==0x25c||op==0x25d||op==0x262)result=enemy_laser_command(enemy,context,globals,commands);
    else if(context.instruction->opcode>=0x102&&context.instruction->opcode<=0x115&&commands.animations)result=commands.animations->command(enemy,context,globals);
    else result=enemy_state_command(enemy,context,globals,commands);
    if(result==-2)error={context.instruction,context.thread_id,context.time,context.instruction->opcode};
    return result;
}
void* EnemyScriptServices::allocate(u32 bytes){return std::calloc(1,bytes);}
void EnemyScriptServices::release(void* p){std::free(p);}
}

