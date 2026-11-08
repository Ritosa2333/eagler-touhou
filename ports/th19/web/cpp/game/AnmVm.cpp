#include "AnmVm.hpp"
#include "Interpolation.hpp"
#include "NativeRandom.hpp"
#include <cmath>
#include <algorithm>
namespace th19 {
u32 AnmVm::next_u16(){if(ip+2>stream_len)return 0;u32 v=u16(stream[ip])|(u16(stream[ip+1])<<8);ip+=2;return v;}
i32 AnmVm::next_i16(){return i32(i16(next_u16()));}
u32 AnmVm::next_u32(){if(ip+4>stream_len)return 0;u32 v=u32(stream[ip])|(u32(stream[ip+1])<<8)|(u32(stream[ip+2])<<16)|(u32(stream[ip+3])<<24);ip+=4;return v;}
float AnmVm::next_f32(){u32 b=next_u32();float v;std::memcpy(&v,&b,4);return v;}

void AnmVm::jump_to(i32 offset,i32 new_time){
    if(offset>=0&&u32(offset)<stream_len){ip=u32(offset);time=new_time;}
    else halted=true;
}

// ---- ANM variable system (opcodes 100-121) ----
// Variable ids (from FUN_004c14c0 / FUN_004c1400 / FUN_004c1760):
//   10000..10003 -> int scratch ivar[0..3]
//   10004..10007 -> float scratch fvar[4..7]
//   10010 -> signed angle, 10011 -> unsigned scalar, 10012 -> signed scalar
//   10013,10014,10015 -> x / y / z
// A param whose mask bit is set is a variable reference; otherwise immediate.
namespace { NativeRandom animation_random{0x12345678}, primary_animation_random{0x12345678}; }
u32 AnmVm::random_checkpoint(bool primary){return (primary?primary_animation_random:animation_random).state;}
void AnmVm::restore_random(u32 state,bool primary){(primary?primary_animation_random:animation_random).state=state;}
// The web match still supplies one seed; native replay import must provide both
// original object states separately and align other gameplay consumers.
void AnmVm::seed_random(u32 seed){animation_random.state=primary_animation_random.state=seed;}
u32 AnmVm::rand_integer(){return animation_random.next();}
float AnmVm::rand_unit(bool primary){
    return (primary?primary_animation_random:animation_random).unit();
}
float AnmVm::rand_signed_unit(float scale,bool primary){
    return (primary?primary_animation_random:animation_random).signedUnit(scale);
}
float AnmVm::read_var_f(i32 id) const{
    switch(id){
        case 10023: return rot_x;
        case 10024: return rot_y;
        case 10025: return rot_z;
        case 10026: return rot_z+(!ignore_parent_transform&&execution_parent_?execution_parent_->read_var_f(10026):0.f);
        case 10033: return child_var_x;
        case 10034: return child_var_y;
        case 10035: return effect_angle;
        case 10004: return fvar[4];
        case 10005: return fvar[5];
        case 10006: return fvar[6];
        case 10007: return fvar[7];
        // 0x4c1760 -> 0x404bd0 (signed) / 0x404c90 (unsigned).
        case 10010: return animation_random.signedUnit(random_angle_scale);
        case 10011: return animation_random.unit(random_scale);
        case 10012: return animation_random.signedUnit(random_scale);
        // Native jump table 0x4c1930: these use object 0x5c6620,
        // whereas 10010..10012 use object 0x5c6638.
        case 10030: return primary_animation_random.signedUnit(random_angle_scale);
        case 10031: return primary_animation_random.unit(random_scale);
        case 10032: return primary_animation_random.signedUnit(random_scale);
        case 10022: return float(animation_random.next());
        case 10027: return random_scale;
        case 10028: return random_angle_scale;
        // Outside native variable range 10000..10035: preserve literal.
        case 10038: return 10038.f;
        case 10013: return x;
        case 10014: return y;
        case 10015: return z;
        default:
            if(id>=10000&&id<=10007) return fvar[id-10000];
            if(id>=10000&&id<=10003) return float(ivar[id-10000]);
            return 0.0f;
    }
}
void AnmVm::write_var_f(i32 id, float v){
    if(id==10027){random_scale=v;return;}
    if(id==10028){random_angle_scale=v;return;}
    switch(id){
        case 10033: child_var_x=v;return;
        case 10034: child_var_y=v;return;
        case 10035: effect_angle=v;return;
        case 10004: fvar[4]=v; return;
        case 10005: fvar[5]=v; return;
        case 10006: fvar[6]=v; return;
        case 10007: fvar[7]=v; return;
        case 10013: x=v; return;
        case 10014: y=v; return;
        case 10015: z=v; return;
        default:
            if(id>=10000&&id<=10007) fvar[id-10000]=v;
            return;
    }
}
i32 AnmVm::read_var_i(i32 id) const{
    switch(id){
        case 10008: return extra_ivar[0];
        case 10009: return extra_ivar[1];
        case 10029: return extra_ivar[2];
        case 10022: return i32(animation_random.bounded(u32(extra_ivar[2]))); // 0x4c16d5 -> 0x402630
        case 10004: return i32(fvar[4]);
        case 10005: return i32(fvar[5]);
        case 10006: return i32(fvar[6]);
        case 10007: return i32(fvar[7]);
        // 0x4c1738 integer dispatch routes these to literal passthrough.
        // Float-only random variables must not advance RNG in integer reads.
        case 10010: case 10011: case 10012:
        case 10030: case 10031: case 10032: case 10038: return id;
        case 10013: return i32(x);
        case 10014: return i32(y);
        case 10015: return i32(z);
        default:
            if(id>=10000&&id<=10003) return ivar[id-10000];
            return 0;
    }
}
void AnmVm::write_var_i(i32 id, i32 v){
    if(id==10008||id==10009){extra_ivar[id-10008]=v;return;}
    if(id==10029){extra_ivar[2]=v;return;}
    if(id>=10000&&id<=10003){ivar[id-10000]=v;return;}
    if(id>=10004&&id<=10007){fvar[id-10000]=float(v);return;}
}

// Shared native interpolation table (0x4d5e10), also used by STD and ECL.
static float ease(u32 mode,float t){
    return nativeEase(mode,t);
}
static void lerp3(float* v,const float* s,const float* t,float e){v[0]=s[0]+(t[0]-s[0])*e;v[1]=s[1]+(t[1]-s[1])*e;v[2]=s[2]+(t[2]-s[2])*e;}

void AnmVm::advance_interp(){
    if(ip_secondary_scale.active){auto&i=ip_secondary_scale;++i.elapsed;if(i.elapsed>=i.dur){i.elapsed=i.dur;i.active=false;}float e=ease(i.mode,i.dur?float(i.elapsed)/i.dur:1.f);secondary_sx=i.v0+(i.t0-i.v0)*e;secondary_sy=i.v1+(i.t1-i.v1)*e;}
    if(ip_rgb2.active){auto&i=ip_rgb2;i.elapsed++;if(i.elapsed>=i.dur){i.elapsed=i.dur;i.active=false;}float e=ease(i.mode,i.dur?float(i.elapsed)/i.dur:1.f);r2=u8(i.v0+(i.t0-i.v0)*e);g2=u8(i.v1+(i.t1-i.v1)*e);b2=u8(i.v2+(i.t2-i.v2)*e);}
    if(ip_alpha2.active){auto&i=ip_alpha2;i.elapsed++;if(i.elapsed>=i.dur){i.elapsed=i.dur;i.active=false;}float e=ease(i.mode,i.dur?float(i.elapsed)/i.dur:1.f);alpha2=i32(i.v0+(i.t0-i.v0)*e);}
    if(ip_pos.active){
        ip_pos.elapsed++;
        if(ip_pos.elapsed>=ip_pos.dur){ip_pos.elapsed=ip_pos.dur;ip_pos.active=false;}
        float e=ease(ip_pos.mode, ip_pos.dur?(float)ip_pos.elapsed/ip_pos.dur:1.0f);
        float s[3]={ip_pos.v0,ip_pos.v1,ip_pos.v2},t[3]={ip_pos.t0,ip_pos.t1,ip_pos.t2},o[3];
        lerp3(o,s,t,e);
        if(ip_pos.mode==8){float q=ip_pos.dur?float(ip_pos.elapsed)/ip_pos.dur:1.f,u=1-q;
            // 0x4037e0: cubic Hermite, tangents are not multiplied by duration.
            for(int k=0;k<3;++k)o[k]=u*u*(2*q+1)*s[k]+(3-2*q)*q*q*t[k]+u*u*q*ip_pos.tangent0[k]+(q-1)*q*q*ip_pos.tangent1[k];}
        x=o[0];y=o[1];z=o[2];
    }
    if(ip_rgb.active){
        ip_rgb.elapsed++;
        if(ip_rgb.elapsed>=ip_rgb.dur){ip_rgb.elapsed=ip_rgb.dur;ip_rgb.active=false;}
        float e=ease(ip_rgb.mode, ip_rgb.dur?(float)ip_rgb.elapsed/ip_rgb.dur:1.0f);
        float s[3]={ip_rgb.v0,ip_rgb.v1,ip_rgb.v2},t[3]={ip_rgb.t0,ip_rgb.t1,ip_rgb.t2},o[3];
        lerp3(o,s,t,e); r=u8(o[0]);g=u8(o[1]);b=u8(o[2]);
    }
    if(ip_alpha.active){
        ip_alpha.elapsed++;
        if(ip_alpha.elapsed>=ip_alpha.dur){ip_alpha.elapsed=ip_alpha.dur;ip_alpha.active=false;}
        float e=ease(ip_alpha.mode, ip_alpha.dur?(float)ip_alpha.elapsed/ip_alpha.dur:1.0f);
        alpha=i32(ip_alpha.v0+(ip_alpha.t0-ip_alpha.v0)*e);
    }
    if(ip_scale.active){
        ip_scale.elapsed++;
        if(ip_scale.elapsed>=ip_scale.dur){ip_scale.elapsed=ip_scale.dur;ip_scale.active=false;}
        float e=ease(ip_scale.mode, ip_scale.dur?(float)ip_scale.elapsed/ip_scale.dur:1.0f);
        sx=ip_scale.v0+(ip_scale.t0-ip_scale.v0)*e;
        sy=ip_scale.v1+(ip_scale.t1-ip_scale.v1)*e;
    }
    if(ip_rot.active){
        ip_rot.elapsed++;
        // Native vector interpolator 0x4037e0, mode 7: targets are per-frame
        // increments; duration -1 keeps accumulating indefinitely (enemy 54/55).
        if(ip_rot.mode==7){
            rot_x+=ip_rot.t0;rot_y+=ip_rot.t1;rot_z+=ip_rot.t2;
            if(ip_rot.dur!=u32(-1)&&ip_rot.elapsed>=ip_rot.dur)ip_rot.active=false;
            return;
        }
        if(ip_rot.elapsed>=ip_rot.dur){ip_rot.elapsed=ip_rot.dur;ip_rot.active=false;}
        float e=ease(ip_rot.mode, ip_rot.dur?(float)ip_rot.elapsed/ip_rot.dur:1.0f);
        rot_x=ip_rot.v0+(ip_rot.t0-ip_rot.v0)*e;
        rot_y=ip_rot.v1+(ip_rot.t1-ip_rot.v1)*e;
        rot_z=ip_rot.v2+(ip_rot.t2-ip_rot.v2)*e;
    }
}

void AnmVm::tick(const AnmVm* parent){
    execution_parent_=parent;
    if(!deleted){
    ++time;
    advance_interp();                       // interp advances even while halted
    rot_x += angle_vel_x;                    // opcode 415 angle velocity (per frame)
    rot_y += angle_vel_y;
    rot_z += angle_vel_z;
    u_offset=std::fmod(u_offset+u_speed,1.f);v_offset=std::fmod(v_offset+v_speed,1.f);
    if(!halted){
        for(;;){
            if(ip+8>stream_len){halted=true;break;}
            const u8* h=stream+ip;
            const u16 type=u16(h[0])|(u16(h[1])<<8);
            const u16 length=u16(h[2])|(u16(h[3])<<8);
            const i16 itime=i16(u16(h[4])|(u16(h[5])<<8));
            if(type==0xffff){if(itime<=time){visible=false;halted=true;deleted=true;}break;}
            if(length<8){halted=true;break;}
            if(itime>time)break;
            cur_mask=u16(h[6])|(u16(h[7])<<8);   // param variable-reference mask
            const u32 instr_start=ip;
            ip+=8;
            const u32 ip_start=ip;
            run_instruction(type,instr_start);
            if(ip==ip_start)ip+=length-8;
            if(halted)break;
        }
    }
    }
    // Child animations have independent native lifetimes (including children
    // of an emitter that has already executed delete).
    for(auto& c:children)c.tick(this);
    children.erase(std::remove_if(children.begin(),children.end(),[](const AnmVm& c){return c.deleted&&c.children.empty();}),children.end());
    creation_registers_=false;
    execution_parent_=nullptr;
}

void AnmVm::run_instruction(u16 op,u32 instr_start){
    // read a float param (index i); if mask bit i set, the float encodes a
    // variable id (read from the variable store), else it is an immediate.
    auto read_f = [&](u32 idx)->float{
        float v=next_f32();
        if(cur_mask & (1u<<idx)) return read_var_f(i32(v));
        return v;
    };
    auto read_i = [&](u32 idx)->i32{
        i32 v=i32(next_u32());
        if(cur_mask & (1u<<idx)) return read_var_i(v);
        return v;
    };
    switch(op){
        case 0: break;
        case 1: visible=false;halted=true;deleted=true;break; // delete
        case 3: case 4: halted=true;break;              // stop / stop2
        case 5: {u32 l=next_u32();(void)l;break;}       // case label
        case 6: {i32 w=read_i(0);time-=w;break;}  // wait
        case 7: if(has_return){ip=return_ip;time=return_time;halted=return_halted;has_return=false;}else halted=true;break;                                  // caseReturn
        case 200: {i32 o=i32(next_u32());i32 t=i32(next_u32());jump_to(o,t);break;} // jmp
        case 201: { // 0x4bd843: decrement the counter, branch only while positive
            i32 c=i32(next_u32()),o=i32(next_u32()),t=i32(next_u32());
            i32 remaining=(cur_mask&1)?read_var_i(c)-1:c-1;
            if(cur_mask&1)write_var_i(c,remaining);
            if(remaining>0)jump_to(o,t);break;
        }
        case 204: { // 0x4be8ee: integer not-equal branch, absolute script offset/time.
            i32 a=read_i(0),b=read_i(1),offset=read_i(2),target=read_i(3);
            if(a!=b)jump_to(offset,target);break;
        }
        case 206: { // EXE 4be991..4be9bf: signed integer less-than jump.
            i32 a=read_i(0),b=read_i(1),offset=read_i(2),target=read_i(3);
            if(a<b)jump_to(offset,target);break;
        }
        case 300: {u32 n=next_u32();sprite=i32(n);break;} // sprite
        case 319: { // 0x4bf052: select one of four sprites by Z-rotation quadrant
            const i32 front=read_i(0),back=read_i(1),negative=read_i(2),positive=read_i(3);
            const float a=std::remainder(rot_z,6.28318530718f),quarter=.785398163397f;
            sprite=a>-quarter&&a<quarter?front:
                   a>=quarter&&a<3*quarter?positive:
                   a<=-quarter&&a>-3*quarter?negative:back;
            break;
        }
        case 302: {render_mode=i32(next_u32());break;}       // render geometry, independent of blending
        case 303: {blend_mode=i32(next_u32());break;}        // blendMode
        case 304: {layer=i32(next_u32());break;}          // layer (FUN_004d0b30 -> +0x18)
        case 305: depth_write=read_i(0)==0;break;
        case 307: {next_u32();break;}                     // render flag (+0x4bc bit0x20)
        case 308: sx=-sx;break;
        case 309: sy=-sy;break;
        case 310: visible=read_i(0)!=0;break;
        case 312: address_u=read_i(0);address_v=read_i(1);break;
        case 313: {coordinate_mode=read_i(0);break;}                     // render flag (+0x4bc|=0x80000, +0x4ce)
        case 421: { // UV anchor: 1 u32 = lo16 anchor_x, hi16 anchor_y (+0x4c7/+0x4c8)
            u32 v = next_u32();
            anchor_x = u8(v & 0xffff);
            anchor_y = u8((v >> 16) & 0xffff);
            break;
        }
        case 431: {next_u32();break;}                     // render flag (+0x4bc bit3)
        case 437: rotation_order=read_i(0);break;
        case 400: {x=read_f(0);y=read_f(1);z=read_f(2);break;} // pos
        case 401: {rot_x=read_f(0);rot_y=read_f(1);rot_z=read_f(2);break;} // rotate
        case 402: {sx=read_f(0);sy=read_f(1);break;}      // scale
        case 403: {alpha=i32(read_i(0));break;}           // alpha
        case 404: {r=u8(read_i(0));g=u8(read_i(1));b=u8(read_i(2));break;} // rgb
        case 405: {alpha2=i32(read_i(0));break;}
        case 406: { // effect color (SSS): r,g,b for procedural glow
            r2=u8(read_i(0));g2=u8(read_i(1));b2=u8(read_i(2));
            break;
        }
        case 410: { // rotateTime (SSfff): dur, mode, target rot x/y/z
            u32 dur=u32(read_i(0)),mode=u32(read_i(1));
            float tx=read_f(2),ty=read_f(3),tz=read_f(4);
            ip_rot.active=true;ip_rot.dur=dur;ip_rot.elapsed=0;ip_rot.mode=mode;
            ip_rot.v0=rot_x;ip_rot.v1=rot_y;ip_rot.v2=rot_z;
            ip_rot.t0=tx;ip_rot.t1=ty;ip_rot.t2=tz;
            break;
        }
        case 608: { // Native mode 0x16: paired horizontal-gradient rectangles
            effect_type=22;
            effect_w=read_f(0);effect_h=read_f(1);
            geometry=608;geometry_width=effect_w;geometry_height=effect_h;
            break;
        }
        case 600: {geometry=600;segments=read_i(0);break;} // 4c06f0 mode9 textured circular strip
        case 604: case 617: case 628: {geometry=op;geometry_radius=read_f(0);segments=read_i(1);if(op==628)segments*=2;break;}
        case 611: {geometry=op;geometry_radius=read_f(0);geometry_width=read_f(1);segments=read_i(2);break;}
        case 626: case 627: {geometry=op;geometry_width=read_f(0);geometry_height=read_f(1);geometry_corner=read_f(2);geometry_stroke=read_f(3);segments=read_i(4);break;}
        case 623: {geometry=op;geometry_width=read_f(0);geometry_height=read_f(1);geometry_corner=read_f(2);segments=read_i(3);break;}
        case 434: secondary_sx=read_f(0);secondary_sy=read_f(1);break;
        case 435: {auto&i=ip_secondary_scale;i.active=true;i.dur=u32(read_i(0));i.mode=u32(read_i(1));i.elapsed=0;i.v0=secondary_sx;i.v1=secondary_sy;i.t0=read_f(2);i.t1=read_f(3);break;}
        case 605: {geometry=605;geometry_radius=read_f(0);segments=read_i(1);break;} // native mode 0x12: line strip
        case 618: {geometry=618;break;} // EXE switch 0x59 -> render mode 0x20 (point list)
        case 603: case 606: case 614: {geometry=op;geometry_width=read_f(0);geometry_height=read_f(1);break;}
        case 621: case 622: case 631: case 632: case 615: case 616: {geometry=op;geometry_radius=read_f(0);geometry_width=read_f(1);segments=read_i(2);if(op==631||op==632)segments*=2;break;}
        case 507: ignore_parent_transform=read_i(0)!=0;break;
        case 433: { // 0x4bfc3f: vector interpolation to polar endpoint (not an offset).
            auto&i=ip_pos;i={};i.active=true;i.dur=u32(read_i(0));i.mode=u32(read_i(1));
            float angle=read_f(2),radius=read_f(3);
            i.v0=x;i.v1=y;i.v2=z;i.t0=std::cos(angle)*radius;i.t1=std::sin(angle)*radius;i.t2=0;
            break;
        }
        case 407: { // posTime: dur, mode, x, y, z
            u32 dur=u32(read_i(0)),mode=u32(read_i(1));
            float tx=read_f(2),ty=read_f(3),tz=read_f(4);
            ip_pos.active=true;ip_pos.dur=dur;ip_pos.elapsed=0;ip_pos.mode=mode;
            ip_pos.v0=x;ip_pos.v1=y;ip_pos.v2=z;ip_pos.t0=tx;ip_pos.t1=ty;ip_pos.t2=tz;
            break;
        }
        case 420: {auto&i=ip_pos;i={};i.active=true;i.dur=u32(read_i(0));i.mode=8;
            for(int k=0;k<3;++k)i.tangent0[k]=read_f(k+1);
            i.v0=x;i.v1=y;i.v2=z;i.t0=read_f(4);i.t1=read_f(5);i.t2=read_f(6);
            for(int k=0;k<3;++k)i.tangent1[k]=read_f(k+7);break;}
        case 124: case 125: {i32 dest=i32(next_f32());float a=read_f(1);write_var_f(dest,op==124?std::sin(a):std::cos(a));break;}
        case 408: { // rgbTime: dur, mode, r, g, b
            u32 dur=next_u32(),mode=next_u32();
            u8 tr=u8(next_u32()),tg=u8(next_u32()),tb=u8(next_u32());
            ip_rgb.active=true;ip_rgb.dur=dur;ip_rgb.elapsed=0;ip_rgb.mode=mode;
            ip_rgb.v0=r;ip_rgb.v1=g;ip_rgb.v2=b;ip_rgb.t0=tr;ip_rgb.t1=tg;ip_rgb.t2=tb;
            break;
        }
        case 409: { // alphaTime: dur, mode, alpha
            u32 dur=next_u32(),mode=next_u32();
            i32 ta=i32(next_u32());
            ip_alpha.active=true;ip_alpha.dur=dur;ip_alpha.elapsed=0;ip_alpha.mode=mode;
            ip_alpha.v0=alpha;ip_alpha.t0=ta;
            break;
        }
        case 412: { // scaleTime: dur, mode, sx, sy
            u32 dur=next_u32(),mode=next_u32();
            float tsx=read_f(2),tsy=read_f(3);
            ip_scale.active=true;ip_scale.dur=dur;ip_scale.elapsed=0;ip_scale.mode=mode;
            ip_scale.v0=sx;ip_scale.v1=sy;ip_scale.t0=tsx;ip_scale.t1=tsy;
            break;
        }
        case 413: {auto&i=ip_rgb2;i.active=true;i.dur=u32(read_i(0));i.mode=u32(read_i(1));i.elapsed=0;i.v0=r2;i.v1=g2;i.v2=b2;i.t0=read_i(2);i.t1=read_i(3);i.t2=read_i(4);break;}
        case 414: {auto&i=ip_alpha2;i.active=true;i.dur=u32(read_i(0));i.mode=u32(read_i(1));i.elapsed=0;i.v0=alpha2;i.t0=read_i(2);if(color_mode==0)color_mode=1;break;}
        case 423: {color_mode=read_i(0);break;}
        case 424: {rotation_mode=read_i(0);break;}
        case 425: {u_speed=read_f(0);break;} // native VM +0x3cc
        case 426: {v_speed=read_f(0);break;} // native VM +0x3d0
        case 415: { // angleVel: per-frame rotation velocity (opcode 415)
            angle_vel_x=read_f(0);angle_vel_y=read_f(1);angle_vel_z=read_f(2);
            break;
        }
        case 416: {sx=next_f32();sy=next_f32();break;}      // scaleGrowth
        case 417: {alpha=i32(next_u32());break;}            // alphaTimeLinear (basic)
        // ---- ANM variable arithmetic (opcodes 100-121) ----
        // 2-param int: 100= 102+= 104-= 106*= 108/= 110%=
        case 100: case 102: case 104: case 106: case 108: case 110: {
            i32 dest=i32(next_u32());   // param0 = dest var id (int)
            i32 val=read_i(1);
            i32 cur=read_var_i(dest);
            switch(op){
                case 100: cur=val; break;
                case 102: cur+=val; break;
                case 104: cur-=val; break;
                case 106: cur*=val; break;
                case 108: cur= val? cur/val : 0; break;
                case 110: cur= val? cur%val : 0; break;
            }
            write_var_i(dest,cur);
            break;
        }
        // 2-param float: 101= 103+= 105-= 107*= 109/= 111%=
        case 101: case 103: case 105: case 107: case 109: case 111: {
            i32 dest=i32(next_f32());   // param0 = dest var id (float-encoded)
            float val=read_f(1);
            float cur=read_var_f(dest);
            switch(op){
                case 101: cur=val; break;
                case 103: cur+=val; break;
                case 105: cur-=val; break;
                case 107: cur*=val; break;
                case 109: cur= val? cur/val : 0.0f; break;
                case 111: cur= val? std::fmod(cur,val) : 0.0f; break;
            }
            write_var_f(dest,cur);
            break;
        }
        // 3-param int: 112=+ 114=- 116=* 118=/ 120=%
        case 112: case 114: case 116: case 118: case 120: {
            i32 dest=i32(next_u32());
            i32 a=read_i(1), b=read_i(2);
            i32 r=0;
            switch(op){
                case 112: r=a+b; break;
                case 114: r=a-b; break;
                case 116: r=a*b; break;
                case 118: r= b? a/b : 0; break;
                case 120: r= b? a%b : 0; break;
            }
            write_var_i(dest,r);
            break;
        }
        // 3-param float: 113=+ 115=- 117=* 119=/ 121=%
        case 113: case 115: case 117: case 119: case 121: {
            i32 dest=i32(next_f32());
            float a=read_f(1), b=read_f(2);
            float r=0;
            switch(op){
                case 113: r=a+b; break;
                case 115: r=a-b; break;
                case 117: r=a*b; break;
                case 119: r= b? a/b : 0.0f; break;
                case 121: r= b? std::fmod(a,b) : 0.0f; break;
            }
            write_var_f(dest,r);
            break;
        }
        case 508: special_effect=read_i(0);break;
        case 509: { // 0x4beef2: copy the parent's scratch registers, not its transform/tint
            if(execution_parent_&&!creation_registers_){
                const auto&p=*execution_parent_;
                std::copy(std::begin(p.ivar),std::end(p.ivar),std::begin(ivar));
                std::copy(std::begin(p.fvar),std::end(p.fvar),std::begin(fvar));
                std::copy(std::begin(p.extra_ivar),std::end(p.extra_ivar),std::begin(extra_ivar));
                child_var_x=p.child_var_x;child_var_y=p.child_var_y;effect_angle=p.effect_angle;
            }
            break;
        }
        case 500: case 501: case 502: case 504: case 505: case 510: {
            u32 sid=u32(read_i(0));
            // 0x4bedd1: positioned child (same creation flags as 502).
            // 0x4bed7b writes +4a0/+4a4, separate from animated position.
            float cx=0,cy=0;if(op==505||op==510){cx=read_f(1);cy=read_f(2);}
            if(global_scripts){
                auto it=global_scripts->find(i32(sid));
                if(it!=global_scripts->end() && it->second.stream && it->second.len){
                    AnmVm child;
                    child.global_scripts=global_scripts;
                    child.bind(it->second.stream, it->second.len);
                    // A native child's initial instructions run at creation.
                    // Our children tick after the parent's instruction batch;
                    // preserve the creation-time registers for their first 509.
                    std::copy(std::begin(fvar),std::end(fvar),std::begin(child.fvar));
                    std::copy(std::begin(ivar),std::end(ivar),std::begin(child.ivar));
                    std::copy(std::begin(extra_ivar),std::end(extra_ivar),std::begin(child.extra_ivar));
                    child.child_var_x=child_var_x;child.child_var_y=child_var_y;child.effect_angle=effect_angle;
                    child.creation_registers_=true;
                    if(op==504){child.detached_spawn=true;child.spawn_x=x+child_x;child.spawn_y=y+child_y;
                        child.spawn_rot_x=rot_x;child.spawn_rot_y=rot_y;child.spawn_rot_z=rot_z;}
                    if(op==505||op==510){child.child_x=cx;child.child_y=cy;}
                    if(op==502||op==510) children.insert(children.begin(), std::move(child));
                    else children.push_back(std::move(child));
                }
            }
            break;
        }
        default: break;
    }
}

void AnmVm::interrupt(i32 case_id){
    if(deleted)return;
    u32 q=0;
    while(q+8<=stream_len){
        const u8* h=stream+q;
        const u16 type=u16(h[0])|(u16(h[1])<<8);
        const u16 length=u16(h[2])|(u16(h[3])<<8);
        if(type==0xffff||length<8)break;
        if(type==5){   // case 标签
            const u32 label=u32(h[8])|(u32(h[9])<<8)|(u32(h[10])<<16)|(u32(h[11])<<24);
            if(i32(label)==case_id){return_ip=ip;return_time=time;return_halted=halted;has_return=true;ip=q;time=i16(u16(h[4])|(u16(h[5])<<8))-1;halted=false;return;}
        }
        q+=length;
    }
}
void AnmVm::interrupt_all(i32 case_id){
    interrupt(case_id);
    for(auto& c:children)c.interrupt_all(case_id);
}
// Expression interrupts contain sprite changes followed by caseReturn. They
// must not replace the ongoing entry/speaking/exit movement and its clock.
void AnmVm::interrupt_returning(i32 case_id){
    const auto old_ip=ip;const auto old_time=time;const auto old_halted=halted;const auto old_mask=cur_mask;
    const auto old_return_ip=return_ip;const auto old_return_time=return_time;const auto old_return_halted=return_halted;const auto old_has_return=has_return;
    // A stopped script may already point at this case label. Successful dispatch
    // must not be inferred from a change of instruction pointer.
    has_return=false;
    interrupt(case_id);
    if(has_return)for(int n=0;n<64&&ip+8<=stream_len;++n){
        const u8*h=stream+ip;u16 op=u16(h[0])|(u16(h[1])<<8),len=u16(h[2])|(u16(h[3])<<8);
        if(len<8||ip+len>stream_len||op==7||op==1||op==3||op==4||op==0xffff)break;
        cur_mask=u16(h[6])|(u16(h[7])<<8);u32 start=ip;ip+=8;run_instruction(op,start);ip=start+len;
    }
    ip=old_ip;time=old_time;halted=old_halted;cur_mask=old_mask;
    return_ip=old_return_ip;return_time=old_return_time;return_halted=old_return_halted;has_return=old_has_return;
}
}
