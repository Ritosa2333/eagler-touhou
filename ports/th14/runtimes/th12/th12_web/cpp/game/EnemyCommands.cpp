#include "EclTrap.hpp"
#include "EnemyCommands.hpp"
#include <cmath>
namespace th11 {
namespace {
constexpr float missing=-999999.f,pi=3.1415927410125732421875f,half_pi=1.57079637050628662109375f,tau=6.283185482025146484375f;
float keep(float value,float previous){return value<=missing?previous:value;}
Vec3 add(Vec3 a,Vec3 b){return {float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};}
float mirror(float angle){return normalize_angle(float(double(half_pi)-normalize_angle(float(double(angle)-half_pi))));}
template<u32 N>void setup(Interpolator<float,N>& p,i32 duration,i32 mode,const float* rate){
    p.duration=duration;p.mode=InterpolationMode(mode);
    for(u32 i=0;i<N;++i)p.initial_tangent[i]=p.final_tangent[i]=0;
    p.timer.set(0,rate);
}
void endpoints(Vec2Interpolator& p,Vec2 start,Vec2 end){p.start[0]=start.x;p.start[1]=start.y;p.end[0]=end.x;p.end[1]=end.y;}
void endpoints(Vec3Interpolator& p,Vec3 start,Vec3 end){p.start[0]=start.x;p.start[1]=start.y;p.start[2]=start.z;p.end[0]=end.x;p.end[1]=end.y;p.end[2]=end.z;}
}
// Hand reconstruction of movement cases in the original 0x412e30 dispatcher.
// Argument evaluation order is explicit because references can consume RNG.
#include "NoopOpcodes.hpp"
bool enemy_movement_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    // TH12 routes this opcode to FUN_004154d0's epilogue at 0x41962b (`xor eax,eax` then the SEH
    // unwind), so the original does nothing at all and reports success. The guard lives here as well
    // as in EnemyScriptServices::command because the game and the test exports reach the handlers
    // through different entry points and both have to see the original's behaviour.
    if(runtime_noop_opcode(c.instruction->opcode))return true;
    const u16 op=c.instruction->opcode;auto& world=g.environment;const float* rate=&world.rate;
    auto f=[&](u32 index){return float(c.float_argument(index,g));};
    auto i=[&](u32 index){return c.integer_argument(index,g);};
    switch(op){
    // 0x118 is deliberately absent. TH12's dispatcher sends it to switchD_00415546_caseD_100,
    // the 0x100 family body that EnemyManager.cpp already handles (its relative list has op==0x118),
    // and the routing chain sends it to the manager too. TH11's movement command claimed it as an
    // absolute-position set, which is what this gate did - a live shadow the pre-chain audit and the
    // routing gate both flagged.
    //
    // 0x11a is now unreachable from here: TH12 routes it, like 0x11b..0x12b and 188 other opcodes, to
    // FUN_004154d0's epilogue at 0x41962b, so th12_noop_opcode returns before the switch both in
    // EnemyScriptServices::command and at the top of this handler. The body is kept only as a record of
    // the TH11 shadow; it must not be re-armed by removing either guard.
    //
    // It is shaped like the real 0x12c/0x12e body below but it is not the same transcription: this one
    // clears only bit 0 of flags and picks its target from the opcode==0x118 test, while the original
    // 0x12c/0x12e body at 0x4165aa clears bits 0 and 1 (`and dword ptr [edi+0x30],0xfffffffc`) and picks
    // its target from `cmp ax,0x12c`. So it is a TH11 body that merely resembles the original, and it must
    // not be read as a mis-filed copy of 0x12c - the two differ in exactly the places that matter.
    case 0x11a:{
        auto& m=op==0x118?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>missing)m.position.x=x;if(y>missing)m.position.y=y;m.flags&=~1u;
        e.current.position=add(e.relative.position,e.absolute.position);e.combine_movement();break;
    }
    // 0x119's body at 0x415ce5 is entirely this:
    //     call FUN_00468e20(1, ctx)                 -> the value of argument 1
    //     mov  eax,[ebx+0x174c] / mov edx,[eax+4]   -> the context through script_owner
    //     call FUN_00468e20(0, ctx)                 -> the value of argument 0
    //     mov  [ebx+eax*4+0x1e0],esi                -> store argument 1 at EnemyState + argument0*4 + 0x1e0
    //     jmp  0x41962b
    // Argument 1 is read *before* argument 0. The test's shared-RNG-variable cases can see read order, so
    // the order of the two reads is part of the transcription. There is no bounds check on the index, and
    // the field at +0x1e0 is still unnamed, and in this port's layout it sits inside the interpolator block at
// 0x174..0x20c, so a store at index >= 11 lands on a different field entirely. The oracle proved that this
// is not a theoretical overlap: at 0x119 case 13 the store went to 0x20c, the port's absolute_angle.start[0],
// and wrote the example index 13 into it as a denormal. In the original there is no collision - its
// interpolator is at 0x324, which leaves 0x1e0..0x324, room for 80 floats, for this array. The port is
// faithful to the machine (the same base, the same indexed store) and the defect is the layout: the port's
// interpolators were placed where the original keeps this array. Fixing it means giving the array the
// original's extent. The store below now targets that field instead of a raw byte offset, so the collision
// is gone. What remains is that the field sits at its own offset rather than 0x1e0, so the oracle cannot yet
// translate this store and 0x119's array target is still uncovered.
    // but the original writes it unconditionally. The previous version of this case returned -2, which in
    // a bool function is true and is read upstream as "handled, success": it refused nothing and left the
    // fixture's value in place while th12 wrote the argument, which is exactly what the oracle reported at
    // state+0x1d8 with argument 0 equal to -2.
    case 0x117:{
        // Transcribed from the handler at 0x415c93, which is:
        //     call FUN_00468e20(0, ctx)             -> the value of argument 0, as an index
        //     call FUN_00468ec0(1, ctx)             -> the value of argument 1, as a float
        //     call FUN_00468ec0(2, ctx)             -> the value of argument 2, as a float
        //     fstp [ebx + index*12 + 0x120]        -> argument 1
        //     fstp [ebx + index*12 + 0x124]        -> argument 2
        //     fldz / fstp [ebx + index*12 + 0x128] -> a literal zero
        //     jmp 0x41962b
        // The reads are in that order - the index first, then the two floats - and the test's shared-RNG
        // arguments can see read order, so the order is part of the transcription. The third store is a
        // constant zero rather than a read, which is why this writes a whole record. 0x117 is not in
        // kTh12CollidingOpcodes, so this case is reachable; 0x119 is, so that one is not - yet.
        const i32 index=i(0);
        const float x=f(1),y=f(2);
        if(static_cast<u32>(index)<13u)e.indexed_vectors[index].x=x,e.indexed_vectors[index].y=y,e.indexed_vectors[index].z=0.0f;
        break;
    }
    case 0x119:{
        const i32 value=i(1);const i32 index=i(0);
        // TH12 writes [ebx+eax*4+0x1e0] with no bound of its own: the bound is its layout, which gives that
        // field 0x1e0..0x324. indexed_values keeps the same capacity, so an index inside that range stores
        // exactly as the machine does and one outside it is refused rather than written past it, which in
        // this port's layout would be a different field. The array no longer overlaps anything, so the
        // collision recorded above is gone; it still sits at its own offset, so the oracle cannot translate
        // this store yet and 0x119's array target remains uncovered.
        if(static_cast<u32>(index)<80u)e.indexed_values[index]=value;break;
    }
    case 0x11b:{
        auto& m=e.relative;auto& p=e.relative_position;
        const float x=f(2),y=f(3);const i32 duration=i(0),mode=i(1);
        endpoints(p,m.position,{keep(x,m.position.x),keep(y,m.position.y),0});setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    case 0x11c:case 0x11e:{
        auto& m=op==0x11c?e.absolute:e.relative;float angle=f(0);const float speed=f(1);
        if(angle>missing){if(e.flags&0x8000)angle=mirror(angle);m.angle=normalize_angle(angle);}
        if(speed>missing)m.speed=speed;m.flags&=~1u;break;
    }
    case 0x11d:case 0x11f:{
        auto& m=op==0x11d?e.absolute:e.relative;auto& p=op==0x11d?e.absolute_angle:e.relative_angle;
        const float angle=f(2),speed=f(3);const i32 mode=i(1);
        Vec2 start{m.angle,m.speed},end;
        if(mode==7)end={keep(angle,0),keep(speed,0)};
        else end={angle<=missing?m.angle:(e.flags&0x8000)?mirror(angle):angle,keep(speed,m.speed)};
        if(std::fabs(float(double(start.x)-end.x))>=pi){if(end.x<=start.x)end.x=float(double(end.x)+tau);else start.x=float(double(start.x)+tau);}
        const i32 duration=i(0);endpoints(p,start,end);setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    case 0x120:case 0x122:{
        auto& m=op==0x120?e.absolute:e.relative;const float angle=f(0),speed=f(1),radius=f(2),radial=f(3);
        if(!(m.flags&1))m.velocity=m.position;
        if(angle>missing)m.angle=normalize_angle(angle);if(speed>missing)m.speed=speed;
        if(radius>missing)m.radius=radius;if(radial>missing)m.radial_velocity=radial;
        m.flags|=1;m.refresh_position();e.combine_movement();break;
    }
    case 0x121:case 0x123:{
        auto& m=op==0x121?e.absolute:e.relative;auto& a=op==0x121?e.absolute_angle:e.relative_angle;auto& r=op==0x121?e.absolute_radius:e.relative_radius;
        const float speed=f(2),radius=f(3),radial=f(4);
        const Vec2 a0{m.angle,m.speed},a1{0,keep(speed,m.speed)},r0{m.radius,m.radial_velocity},r1{keep(radius,m.radius),keep(radial,m.radial_velocity)};
        const i32 duration=i(0),mode=i(1);endpoints(a,a0,a1);endpoints(r,r0,r1);setup(a,duration,mode,rate);setup(r,duration,mode,rate);
        m.flags|=1;m.refresh_position();e.combine_movement();break;
    }
    case 0x124:case 0x125:{
        auto& m=op==0x124?e.absolute:e.relative;auto& p=op==0x124?e.absolute_angle:e.relative_angle;
        const double random=float(double(world.rng().signed_unit())*pi);float angle;
        if(e.current.position.x<double(e.clamp_center.x)-double(e.clamp_size.x)*.25)angle=float(random/3);
        else if(e.current.position.x>double(e.clamp_center.x)+double(e.clamp_size.x)*.25)angle=normalize_angle(float(random/3+pi));
        else if(e.current.position.x<world.player_position.x)angle=float(random*.5);
        else angle=normalize_angle(float(random*.5+pi));
        if(e.current.position.y<double(e.clamp_center.y)-double(e.clamp_size.y)*.25)angle=std::fabs(angle);
        else if(e.current.position.y>double(e.clamp_center.y)+double(e.clamp_size.y)*.25)angle=-std::fabs(angle);
        const i32 mode=i(1);const float speed=f(2);const i32 duration=i(0);
        endpoints(p,{angle,speed},{angle,0});setup(p,duration,mode,rate);m.flags&=~1u;break;
    }
    // 320 (0x140) sets the absolute or relative Movement from six guarded floats. Confirmed against
    // the body at 0x417093; the full disassembly and the argument mapping are in EnemyScript.cpp.
    // Note the ellipse angle is NOT normalized here - only f(0), through FUN_00408910.
    // 0x142 is deliberately absent: it shares TH12's body with 320, but this file's movement pre-chain
    // runs first, so EnemyStateCommands.cpp's existing 0x142 case is what actually executes. Routing
    // 0x142 here would shadow it and never run, which tools/th12-handler-routing.mjs reports.
    case 0x140:{
        auto& m=op==0x140?e.absolute:e.relative;
        const float angle=f(0),speed=f(1),radius=f(2),radial=f(3),axis=f(4),ratio=f(5);
        if(!(m.flags&1))m.velocity=m.position;
        if(angle>missing)m.angle=normalize_angle(angle);if(speed>missing)m.speed=speed;
        if(radius>missing)m.radius=radius;if(radial>missing)m.radial_velocity=radial;
        if(axis>missing)m.ellipse_angle=axis;if(ratio>missing)m.ellipse_ratio=ratio;
        m.flags|=3;m.refresh_position();e.combine_movement();break;
    }
    case 0x126:case 0x127:{if(!world.boss)ecl_trap(__FILE__,__LINE__);(op==0x126?e.absolute:e.relative).position=world.boss->current.position;break;}
    case 0x128:case 0x129:{
        auto& m=op==0x128?e.absolute:e.relative;const float x=f(0),y=f(1),z=f(2);m.position=add(m.position,{x,y,z});e.combine_movement();break;
    }
    case 0x12a:case 0x12b:{
        auto& m=op==0x12a?e.absolute:e.relative;const float x=f(0),y=f(1);
        if(x>missing)m.velocity.x=x;if(y>missing)m.velocity.y=y;m.refresh_position();e.combine_movement();
        // The original falls through at 0x41444f and selects relative motion.
        [[fallthrough]];
    }
    case 0x12c:case 0x12e:{
        auto& m=op==0x12c?e.absolute:e.relative;
        const float x=f(0),y=f(1);
        if(x>missing)m.position.x=x;if(y>missing)m.position.y=y;
        // Two things the original does that an earlier version of this case omitted, both visible in the
        // oracle's report for 0x12c case 0 (velocity x was -271.99 against the original's 0):
        //
        // 1. 0x416644 writes current.position = relative.position + absolute.position *before* calling
        //    combine_movement, so combine_movement's `velocity = sum - current.position` comes out zero.
        //    Without this store the port computed a velocity of -271.99 from a stale current.position.
        // 2. There is no refresh_position() call anywhere in this body: 0x416613 goes straight from
        //    `and dword ptr [edi+0x30],0xfffffffc` into the coordinate sum. refresh_position() would write
        //    position = velocity + polar(angle,radius) and destroy the argument stores made just above.
        m.flags&=~3u;e.current.position=add(e.relative.position,e.absolute.position);e.combine_movement();break;
    }
    // 301/303 set up the position interpolator, shared body VA 0x4166d6. TH11's body built
    // angle, radius and ellipse interpolators from eight arguments instead; 301 was a collision
    // entry and stage 4 stopped at frame 1502 on it. 0x119 below is the same shape.
    case 0x12d:case 0x12f:{
        auto& m=op==0x12d?e.absolute:e.relative;auto& p=op==0x12d?e.absolute_position:e.relative_position;
        const i32 duration=i(0),mode=i(1);const float x=f(2),y=f(3);
        if(duration<1){p.duration=0;break;}
        endpoints(p,m.position,{keep(x,m.position.x),keep(y,m.position.y),0});
        setup(p,duration,mode,rate);m.flags&=~3u;break;
    }
    // 0x130/0x132/0x148/0x14a set a movement's angle and speed, shared body in TH12's
    // dispatcher. TH11 gave 0x130 a flag toggle, which is what the port had; the audit of the
    // pre-chain residue caught it at 241 uses. The two opcodes select absolute movement and
    // the other two relative. Pointing 0x130/0x132 also applies the aim flip when the enemy
    // carries flag 0x40000, and every path clears both low mode bits.
    case 0x130:case 0x132:case 0x148:case 0x14a:{
        Movement& m=(op==0x130||op==0x148)?e.absolute:e.relative;
        const float angle=f(0),speed=f(1);
        if(angle>-999999.f){
            float value=angle;
            if((e.flags&0x40000)&&(op==0x130||op==0x132))
                value=normalize_angle(float(1.5707963705062866-normalize_angle(float(double(value)-1.5707963705062866))));
            m.angle=normalize_angle(value);
        }
        m.flags&=~3u;
        if(speed>-999999.f)m.speed=speed;
        break;
    }
    // TH12's 0x131/0x149 is the absolute speed+angle interpolation. TH11's
    // 0x131/0x132 - the position curve that used to live here - has no TH12
    // counterpart at this number, so it is gone rather than kept: TH12's 0x131
    // reads i(0) duration, i(1) mode, f(2) target angle and f(3) target speed
    // and drives absolute_angle plus Movement.angle/speed. Its interpolator is
    // absolute_angle at EnemyState+0x324, which is TH11's 0x20c + 0x118; the
    // same +0x118 puts emitters at 0x48c, confirmed independently by the
    // dispatcher's own memset(param_1 + i*0x214 + 0x48c, 0, 0x214). Field
    // offsets line up exactly: start at +0x00, end at +0x08, both tangents at
    // +0x10/+0x18, duration at +0x34 and mode at +0x38, and TH12's special case
    // mode == 7 is InterpolationMode::Velocity.
    //
    // Three constants differ from TH11's 0x11d/0x11f twin and are taken from
    // TH12: the mirror flag is e.flags & 0x40000 (TH11 tested 0x8000), and the
    // movement flags are cleared with ~3 (TH11 used ~1). Everything else matches
    // TH11 exactly, including the mode==7 branch, the -999999.0 keep-current
    // sentinels, and the reference-angle wrap at pi. TH12's mirror is written as
    // cos/sin in the decompilation, but that is normalize_angle inlined:
    // normalize(pi/2 - normalize(angle - pi/2)) is pi - angle, i.e. mirror().
    // Dispatch gives 0x131, 0x133 and 0x149 the same case body at 0x416ab3. Inside it, 0x131 and 0x149
    // take the movement at +0x68 and the interpolator at +0x324, while 0x133 - being neither - takes the
    // movement at +0x9c and the interpolator at +0x360. 0x133 previously had its own body here, which
    // added relative into absolute and cleared relative: the original has no such body, and the oracle
    // showed it at 0x133 case 0 (absolute jumped to relative+absolute while relative went to zero).
    // The +0x360 mapping onto relative_angle is an explicit assumption for now: the fixture defers the
    // interpolator region, so the oracle cannot confirm it yet.
    case 0x131:case 0x133:case 0x149:{
        const bool absolute_op=(op==0x131||op==0x149);
        auto& m=absolute_op?e.absolute:e.relative;
        auto& p=absolute_op?e.absolute_angle:e.relative_angle;
        const float angle=f(2),speed=f(3);const i32 duration=i(0);
        // 0x416b22 reads i(0) here, before i(1) at 0x416b4d: the machine reads f(2), f(3), i(0), i(1) in
        // that order, and argument reads are test-visible when two of them resolve the same RNG variable.
        // 0x416b32 then tests duration and, when it is <= 0, stores zero into [edi+0x34] and jumps to the
        // epilogue without ever reaching the `and dword ptr [esi+0x30],0xfffffffc` at 0x416cfd. Clearing
        // the movement flags unconditionally was therefore wrong: it zeroed state+0x98, the absolute
        // movement's flags word, which is exactly the divergence the oracle reported for 0x131 case 1.
        if(duration<=0){p.duration=0;break;}
        const i32 mode=i(1);
        Vec2 start{m.angle,m.speed},end;
        if(mode==7)end={keep(angle,0),keep(speed,0)};
        else end={angle<=missing?m.angle:(e.flags&0x40000)?mirror(angle):angle,keep(speed,m.speed)};
        if(std::fabs(float(double(start.x)-end.x))>=pi){if(end.x<=start.x)end.x=float(double(end.x)+tau);else start.x=float(double(start.x)+tau);}
        endpoints(p,start,end);setup(p,duration,mode,rate);m.flags&=~3u;break;
    }
    // 0x133 used to have its own body here - add relative into absolute, clear relative, zero both speeds
    // and both velocities, clear two flag words and zero eight interpolator durations. TH12 has no such
    // body: its dispatch table routes 0x133 into the 0x131 body above. The oracle caught it at 0x133
    // case 0, where absolute became relative+absolute and relative became zero while the original left
    // both untouched on its early return. Removed rather than kept as a TH11 shadow.
    default:return false;
    }
    return true;
}
}
