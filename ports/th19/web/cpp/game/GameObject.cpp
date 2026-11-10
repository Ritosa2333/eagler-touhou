#include "EclVm.hpp"
#include "GameWorld.hpp"
#include "BossMovement.hpp"
#include "EnemyOrbit.hpp"
#include <cmath>
#include <cstdio>
#include <algorithm>
namespace th19 {
// Game-object instruction dispatch (opcode >=300). Verified th19 subset from
// wave01.ecl + enemy.anm (see docs/ecl-opcodes reference). Returns true if handled.
bool EclVm::game_object(const EclInstruction& ins){
    if(!world_)return false;
    switch(ins.id){
        case 609: case 610: case 611: case 612: {
            // 0x5027a3: explicit slot (609/610), append slot (611/612).
            if(!has_host)return false;
            const int emitter=param_i32(ins,0);if(emitter<0||emitter>=16)return false;
            const bool append=ins.id==611||ins.id==612,wide=ins.id==610||ins.id==612;
            for(auto&e:world_->enemies)if(e.id==host_id){
                const int slot=append?e.change_append[emitter]:param_i32(ins,1);
                if(slot<0||slot>=24)return false;
                auto&s=e.laser_shooters[emitter];auto&c=s.changes[slot];int arg=append?1:2;
                c.concurrent=param_i32(ins,arg++);c.flags=u32(param_i32(ins,arg++));
                c.integers[0]=param_i32(ins,arg++);c.integers[1]=param_i32(ins,arg++);
                if(wide){c.integers[2]=param_i32(ins,arg++);c.integers[3]=param_i32(ins,arg++);}
                c.floats[0]=param_f32(ins,arg++);c.floats[1]=param_f32(ins,arg++);
                if(wide){c.floats[2]=param_f32(ins,arg++);c.floats[3]=param_f32(ins,arg++);}
                e.change_append[emitter]=slot+1;
                s.has_unhandled_changes=false;
                for(const auto&entry:s.changes)if(!supportedBulletChange(entry))s.has_unhandled_changes=true;
                // Unsupported change types remain visible in the runtime audit.
                return supportedBulletChange(c);
            }
            return false;
        }
        case 614: { // 0x50213c: copy emitter 1 -> 0, including both origin vectors.
            if(!has_host)return false;
            const int dst=param_i32(ins,0),src=param_i32(ins,1);
            if(dst<0||dst>=16||src<0||src>=16)return false;
            for(auto&e:world_->enemies)if(e.id==host_id){
                // Copy first: insertion may rehash the emitter map.
                const LaserShooter copy=e.laser_shooters[src];e.laser_shooters[dst]=copy;return true;
            }
            return false;
        }
        // 613 is global bullet/laser cancellation (0x502dbd), not an emitter
        // change. Leave it unsupported until cancellation parameters are wired.
        case 613:return false;
        case 601: {
            if(!has_host)return false;const int index=param_i32(ins,0);
            for(auto&e:world_->enemies)if(e.id==host_id){auto it=e.laser_shooters.find(index);
                if(it==e.laser_shooters.end())return false;
                // EXE 0x4fbea0 updates the small Suika actor's angle before
                // ExAttackFunc emits its shooter. Preserve that angle in the
                // descriptor; leaving the initial value (0) fires right.
                auto shooter=it->second;
                if(e.native_update_callback==3)shooter.angle=e.angle;
                return world_->fire_bullets(shooter,e.x,e.y);}
            return false;
        }

        case 600: case 602: case 603: case 604: case 605: case 606: case 607: case 608:
        case 618: case 619: case 620:
        case 700: case 701: case 702: case 703: case 711: {
            if(!has_host)return false;
            const int index=param_i32(ins,0);if(index<0||index>=16)return false;
            for(auto&e:world_->enemies)if(e.id==host_id){
                auto&s=e.laser_shooters[index];
                switch(ins.id){
                case 600:s={};e.change_append[index]=0;break;
                case 602:s.type=param_i32(ins,1);s.color=param_i32(ins,2);break;
                case 603:s.x=param_f32(ins,1);s.y=param_f32(ins,2);break;
                case 604:s.angle=param_f32(ins,1);s.angle2=param_f32(ins,2);break;
                case 605:s.speed=param_f32(ins,1);s.speed2=param_f32(ins,2);break;
                // Native 0x502695 / 0x5026fc stores unsigned 16-bit fields.
                case 606:s.count1=static_cast<unsigned short>(param_i32(ins,1));s.count2=static_cast<unsigned short>(param_i32(ins,2));break;
                case 607:s.aim_mode=static_cast<unsigned short>(param_i32(ins,1));break;
                case 608:s.sound=param_i32(ins,1);s.end_sound=param_i32(ins,2);break;
                // Native +4e6c relative vector, +4f2c absolute vector, +24 muzzle radius.
                case 618:{float a=param_f32(ins,1),r=param_f32(ins,2);s.x=std::cos(a)*r;s.y=std::sin(a)*r;break;}
                case 619:s.muzzle_radius=param_f32(ins,1);break;
                case 620:s.absolute_x=param_f32(ins,1);s.absolute_y=param_f32(ins,2);s.absolute_origin=s.absolute_x>=-990.f;break;
                case 700:s.length=param_f32(ins,1);s.max_length=param_f32(ins,2);s.offset=param_f32(ins,3);s.width=param_f32(ins,4);break;
                case 701:s.warning=param_i32(ins,1);s.grow=param_i32(ins,2);s.hold=param_i32(ins,3);s.shrink=param_i32(ins,4);s.flags=param_i32(ins,5);break;
                case 702:{ // 0x5031af -> 0x530df0(kind0) -> 0x52fca0.
                    // 5031af copies shooter+8 to descriptor+14 (initial), +c to +10 (target).
                    if(s.type!=8||s.color<0||s.color>=16||s.speed<0||s.length<0||s.max_length<0||s.width<=0)return false;
                    for(const auto&c:s.changes)if(!supportedStraightLaserChange(c))return false; // verified laser-specific subset.
                    if(world_->lasers.size()>=512)return true;
                    InfiniteLaser l;l.id=world_->next_id++;l.source=s;l.straight=true;l.phase=2;l.angle=s.angle;
                    l.x=(s.absolute_origin?s.absolute_x:e.x)+s.x+std::cos(l.angle)*s.muzzle_radius;
                    l.y=(s.absolute_origin?s.absolute_y:e.y)+s.y+std::sin(l.angle)*s.muzzle_radius;
                    l.length=s.length;l.straight_target=s.max_length;l.travel_cap=s.offset;l.width=s.width;
                    l.travelled=s.max_length<s.length?.01f:0.f;world_->lasers.push_back(l);
                    if(s.sound>=0)world_->sound_events.push_back(s.sound);break;}
                case 711:{ // 50388c -> 530df0(kind2) -> 5286c0
                    if(s.type<0||s.type>2||s.width<=0)return false;
                    InfiniteLaser l;l.id=world_->next_id++;l.curved=true;l.phase=2;l.source=s;l.width=s.width;
                    l.x=(s.absolute_origin?s.absolute_x:e.x)+s.x+std::cos(s.angle)*s.muzzle_radius;
                    l.y=(s.absolute_origin?s.absolute_y:e.y)+s.y+std::sin(s.angle)*s.muzzle_radius;l.angle=s.angle;
                    if(s.type==2){if(e.animation_files.size()<2)return false;l.curve_file=e.animation_files[1];}
                    if(!l.curve.initialize(l.x,l.y,s.angle,s.speed,s.warning,s.changes))return false;
                    if(world_->lasers.size()<512)world_->lasers.push_back(std::move(l));
                    if(s.sound>=0)world_->sound_events.push_back(s.sound);break;}
                case 703:{
                    // Only this descriptor has been verified against the EXE.
                    // Other laser types remain in the unsupported-opcode audit.
                    if(s.type!=38||s.color<0||s.color>=16||s.flags!=0)return false;
                    InfiniteLaser l;l.id=world_->next_id++;l.source=s;l.angle=s.angle;
                    // 50345a selects the origin. 52b8e0 adds the ECL619 muzzle radius.
                    l.x=(s.absolute_origin?s.absolute_x:e.x)+s.x+std::cos(l.angle)*s.muzzle_radius;
                    l.y=(s.absolute_origin?s.absolute_y:e.y)+s.y+std::sin(l.angle)*s.muzzle_radius;
                    l.length=s.length;world_->lasers.push_back(l);
                    if(s.sound>=0)world_->sound_events.push_back(s.sound);break;}
                }
                return true;
            }
            return false;
        }
        case 569: { // 4fecc0 case 6d -> 4d3140: forward intersection with 296x448 field.
            const float x=param_f32(ins,2),y=param_f32(ins,3),a=param_f32(ins,4);
            const float dx=std::cos(a),dy=std::sin(a);float t=1e30f;
            auto candidate=[&](float v,float other,float lo,float hi){if(v>=0&&other>=lo-.001f&&other<=hi+.001f)t=std::min(t,v);};
            if(std::abs(dx)>1e-7f){for(float edge:{-148.f,148.f}){float v=(edge-x)/dx;candidate(v,y+v*dy,0,448);}}
            if(std::abs(dy)>1e-7f){for(float edge:{0.f,448.f}){float v=(edge-y)/dy;candidate(v,x+v*dx,-148,148);}}
            if(t==1e30f)t=0;
            if(param_stack_ref(ins,0))set_var_float(i32(read_f32(param_off(0))),x+dx*t);
            if(param_stack_ref(ins,1))set_var_float(i32(read_f32(param_off(1))),y+dy*t);
            return true;
        }
        case 504: { // 0x500f6a: center x/y and full width/height, not corner bounds.
            float x=param_f32(ins,0),y=param_f32(ins,1),w=param_f32(ins,2),h=param_f32(ins,3);
            if(w<0||h<0)return false;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.bound_x=x;e.bound_y=y;e.bound_w=w;e.bound_h=h;e.status_flags|=2u;break;}
            return true;
        }
        case 505: // 0x500fcb: disable bounding rectangle.
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.status_flags&=~2u;break;}return true;
        case 615: // Native dispatch table 0x505560 maps both 615 and 616 to 0x503d9e.
        case 616: { // 0x503d9e: 4ee880 ignores bullet lock; laser call uses respect-lock=1.
            const float radius=param_f32(ins,0);if(radius<0)return false;
            if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){
                world_->cancel_bullets_circle(e.x,e.y,radius,99999,false);
                world_->cancel_lasers_circle(e.x,e.y,radius,true);break;}
            return true;
        }
        case 515: { // 0x501b6f -> 0x402650: replace, not extend, the invulnerability timer.
            const int frames=param_i32(ins,0);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.invulnerability_time=float(frames);return true;}
            return false;
        }
        case 512: {
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id)
                return world_->register_boss(e,param_i32(ins,0));
            return false;
        }
        case 522: {
            if(ins.param_bytes<16)return false;
            const int len=read_i32(param_off(3));if(len<0||u32(len)>ins.param_bytes-16)return false;
            auto&sp=world_->spell;sp.host_id=has_host?host_id:0;sp.active=true;sp.number=param_i32(ins,0);sp.timeout=param_i32(ins,1);sp.flags=param_i32(ins,2);++sp.generation;
            sp.name_bytes.clear();u8 key=0x77,step=7;
            for(int i=0;i<len;++i){u8 value=stream()[param_off(4)+i]^key;key+=step;step+=0x10;if(value)sp.name_bytes.push_back(char(value));}
            for(const auto&e:world_->enemies)if(has_host&&e.id==host_id&&e.animation_files.size()>1)sp.animation_file=e.animation_files[1];
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                // ECL522: HP record +0x18 bit0; preserve sub-HP damage across hits.
                e.spell_damage_scaled=true;e.life_reset_c=i32(u32(e.life)*7u);
            }
            world_->sound_events.push_back(33);return true;
        }
        case 1020: world_->boss_escape_result=param_i32(ins,0);return true;
        case 525: { // 0x50ca70: scripted enemy cleanup, not a rewarded kill.
            for(auto&e:world_->enemies){
                const bool remove=(!(e.flags&0xa0)&&!(e.status_flags&0xc0)&&!(e.flags&0x400))||(e.flags&0x100);
                if(!e.active||!remove)continue;
                e.active=false;e.status_flags|=0x200;
                for(auto&animation:e.animations)animation.script=-1;
            }
            return true;
        }
        case 523:
            world_->spell.active=false;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id)e.spell_damage_scaled=false;
            return true;
        case 513: // 0x502ce3 resets the separate phase timer, not enemy age.
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.phases.clock=0;break;}return true;
        case 514: { // 0x502d06 -> 0x50d080: slot, HP threshold, timeout, string.
            const int slot=param_i32(ins,0),hp=param_i32(ins,1),timeout=param_i32(ins,2);
            if(slot<0||slot>=8||ins.param_bytes<16)return false;
            const int len=read_i32(param_off(3));if(len<0||u32(len)>ins.param_bytes-16)return false;
            std::string name(reinterpret_cast<const char*>(stream()+param_off(4)),size_t(len));
            while(!name.empty()&&name.back()==0)name.pop_back();
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){auto&t=e.phases.entries[slot];t.hp=hp;
                if(hp>=0){t.timeout=timeout;t.defeated=t.timed_out=name;e.callbacks[slot]=name;}break;}
            return true;
        }
        case 521: { // 0x50d0f0: eight named callbacks; slot0 is aura conversion.
            const int slot=param_i32(ins,0),len=read_i32(param_off(1));
            if(slot<0||slot>=8||len<0||u32(len)>ins.param_bytes-8)return false;
            std::string name(reinterpret_cast<const char*>(stream()+param_off(2)),size_t(len));
            while(!name.empty()&&name.back()==0)name.pop_back();
            for(auto&e:world_->enemies)if(has_host&&e.id==host_id){e.callbacks[slot]=name;e.phases.entries[slot].timed_out=name;break;}return true;
        }
        case 624: { // 0x501458 -> table 0x5994f8; 1 = Nazrin recoil 0x4fc120.
            const int callback=param_i32(ins,0);
            if(callback<0||callback>3)return false;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.native_update_callback=callback;e.recoil_pending=false;break;}
            return true;
        }
        case 547: { // 0x4fedee: EnemyInf+0x510c, per-enemy hit sound override.
            int sound=param_i32(ins,0);if(has_host)for(auto&e:world_->enemies)if(e.id==host_id)e.hit_sound=sound;
            return true;
        }
        case 517: { // 502e35: global shake, not host-local sprite movement.
            world_->screen_shake_events.push_back({param_i32(ins,0),param_i32(ins,1),param_i32(ins,2)});return true;
        }
        case 516: { // Native 0x502df4 -> 0x4bb8a0: game sound ID at enemy position.
            world_->sound_events.push_back(param_i32(ins,0));return true;
        }
        case 300: case 301: case 304: { // createEnemy; 301 uses absolute position (0x4feb00).
            // th13+ string param is length-prefixed: int32 length + that many bytes.
            if(ins.param_bytes<24){error_=true;return false;}
            const i32 name_len=read_i32(ip_+16);
            if(name_len<=0 || u32(name_len)>ins.param_bytes-24 ||
               ip_>stream_len() || ins.size>stream_len()-ip_){
                error_=true;return false;
            }
            std::string name(reinterpret_cast<const char*>(stream()+ip_+20),u32(name_len));
            while(!name.empty()&&name.back()=='\0')name.pop_back();
            const u32 p=ip_+20+u32(name_len);
            // x/y follow a variable-length string, but masks still address
            // logical parameters 1/2 (float variable addresses are float encoded).
            auto farg=[&](u32 off,u32 arg){
                float value=read_f32(off);
                return param_stack_ref(ins,arg)?var_float(i32(value)):value;
            };
            auto iarg=[&](u32 off,u32 arg){
                i32 value=read_i32(off);
                return param_stack_ref(ins,arg)?var_int(value):value;
            };
            float x=farg(p,1),y=farg(p+4,2);
            i32 life=iarg(p+8,3),score=iarg(p+12,4),item=iarg(p+16,5);
            bool mirror=ins.id==304;
            std::array<i32,4> inherited_ints{};std::array<float,4> inherited_floats{};
            if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){
                if(ins.id!=301){x+=e.x;y+=e.y;}
                // Native adds the parent's position BEFORE applying reflection.
                if(e.mirrored){x=-x;mirror=!mirror;}
                inherited_ints=e.ecl_ints;inherited_floats=e.ecl_floats;break;
            }
            // Native 0x4fec6a copies EnemyInf+0x29c/+0x2ac into the spawn
            // record. Children receive a snapshot, including wave orbit phase.
            auto&child=world_->spawn_enemy(name,x,y,life,item,score,&programs_);
            child.mirrored=mirror;if(mirror)child.flags|=8;
            child.ecl_ints=inherited_ints;child.ecl_floats=inherited_floats;
            // 0x50d410 -> 0x50be30 -> 0x508f20 executes initial ECL
            // before the creator resumes (BossStartInit registers ECL512).
            child.script_initialized_this_tick=true;
            EclVm* initial=child.vm.get();if(initial)initial->step();
            return true;
        }
        case 307: { // 0x4ff14c -> 0x40ff60: independent ANM at host position.
            const int file=param_i32(ins,0),script=param_i32(ins,1);
            // Shared effect bank. Keep other banks reported as unsupported.
            if(file!=1)return false;
            if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){
                world_->deaths.push_back({e.x,e.y,0,script,-1,file});break;
            }
            return true;
        }
        case 323: { // 0x4fef43: death ANM file, script (negative disables visual).
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.death_file=param_i32(ins,0);e.death_script=param_i32(ins,1);break;}
            return true;
        }
        case 302: { // select ANM file slot (1-based; 2+ = user-loaded via anim statement)
            i32 slot=param_i32(ins,0);
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.anm_slot=slot;break;}
            return true;
        }
        case 303: case 306: { // Native 0x4fe980: direct animation slot, retaining its selected file.
            i32 slot=param_i32(ins,0),sid=param_i32(ins,1);
            if(slot<0||slot>=16)return false;
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){
                auto&a=e.animations[slot];a.script=sid;a.file=e.anm_slot;a.rotation=0;a.rotation_set=false;++a.generation;a.interrupt_generation=0;
                if(slot==0){e.sprite=sid;e.directional_file=e.anm_slot;
                    // 4fe980 (303) writes current pose +27c and file +278,
                    // preserving base +280, direction +284 and status bit10.
                    // Dispatcher 306 additionally initializes those three fields.
                    if(ins.id==306){e.directional=true;e.directional_base=sid;e.animation_direction=0;}}
                break;
            }
            return true;
        }
        case 318: { // 0x4ff549: restart base animation and leave directional mode.
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                auto&a=e.animations[0];a.script=e.directional_base;a.file=e.anm_slot;a.rotation=0;a.rotation_set=false;
                e.sprite=e.directional_base;e.directional_file=e.anm_slot;e.animation_direction=0;
                ++a.generation;a.interrupt_generation=0;e.directional=false;e.status_flags&=~0x10u;break;
            }return true;
        }
        case 511: { // 0x501a22: reset all four HP record fields (+0x5008).
            const i32 hp=param_i32(ins,0);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                e.life=hp;e.life_reset_4=hp;e.life_reset_8=hp;e.life_reset_c=i32(u32(hp)*7u);
                if(e.status_flags&0x80)e.status_flags|=0x4000;break;
            }return true;
        }
        case 313: case 316: { // 4ff5e5: 313 selects base+5. 316 accepts an explicit pose.
            const int slot=param_i32(ins,0),pose=ins.id==313?0:param_i32(ins,1);
            if(slot<0||slot>=16)return false;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                auto&a=e.animations[slot];a.script=e.directional_base+(pose<0?0:5+pose);
                // Native dispatcher case10 resolves Enemy+274 (current ANM
                // selection), not the file retained by animation slot zero.
                a.file=e.anm_slot;a.rotation=0;a.rotation_set=false;
                ++a.generation;a.interrupt_generation=0;break;
            }return true;
        }
        case 317: { // 0x4cb520 -> 0x4cb830: interrupt the slot and its children.
            const int slot=param_i32(ins,0);if(slot<0||slot>=16)return false;
            const i32 label=i16(param_i32(ins,1));
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){auto&a=e.animations[slot];a.interrupt_label=label;++a.interrupt_generation;break;}
            return true;
        }
        case 319: { // Native 0x4fe350, case 0x13f: animation Z rotation.
            i32 slot=param_i32(ins,0);
            if(slot<0||slot>=16)return false;
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){
                e.animations[slot].rotation=param_f32(ins,1);e.animations[slot].rotation_set=true;
                break;
            }
            return true;
        }
        case 329: case 330: { // main ANM scale / scale interpolation (slot 0)
            if(param_i32(ins,0)!=0)return false;
            const int mode=ins.id==330?param_i32(ins,2):0;
            if(ins.id==330&&mode!=0&&mode!=1&&mode!=9&&mode!=28)return false;
            for(auto&e:world_->enemies)if(has_host&&e.id==host_id){
                if(ins.id==329){e.sx=param_f32(ins,1);e.sy=param_f32(ins,2);e.scale_x={};e.scale_y={};}
                else {const int duration=param_i32(ins,1);
                    e.scale_x={duration,0,mode,e.sx,param_f32(ins,3)};
                    e.scale_y={duration,0,mode,e.sy,param_f32(ins,4)};}
                break;
            }return true;
        }
        case 400: case 401: {
            const int mode=ins.id==401?param_i32(ins,1):0;
            if(ins.id==401&&mode!=0&&mode!=1&&mode!=4&&mode!=9)return false;
            float x=param_f32(ins,ins.id==400?0:2),y=param_f32(ins,ins.id==400?1:3);
            for(auto&e:world_->enemies)if(has_host&&e.id==host_id){
                // Native 4ffaa7 -> 40ece0 only clears interpolator duration.
                // Zero-time ECL401 is a stop, not ECL400 position assignment.
                if(ins.id==401&&param_i32(ins,0)<=0){e.move_x={};e.move_y={};break;}
                if(x<=-999999.f)x=e.x;if(y<=-999999.f)y=e.y;
                // Native ECL400 writes coordinates without cancelling velocity.
                // Mamizou uses it to place a reflected animal back at the edge.
                if(ins.id==401){e.polar=e.orbit=e.ellipse=false;e.vx=e.vy=0;e.turn={};e.accelerate={};}
                const int duration=ins.id==401?param_i32(ins,0):0;
                if(duration>0){e.move_x={duration,0,mode,e.x,x};e.move_y={duration,0,mode,e.y,y};}
                else{e.x=x;e.y=y;e.move_x={};e.move_y={};}break;
            }return true;
        }
        case 404: { // set polar velocity (angle, speed)
            float ang=param_f32(ins,0),spd=param_f32(ins,1);
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.angle=e.mirrored?3.14159265358979323846f-ang:ang;e.speed=spd;e.polar=true;e.orbit=false;e.vx=std::cos(e.angle)*spd;e.vy=std::sin(e.angle)*spd;break;}
            return true;
        }
        case 405: { // native 0x4ff...: angle/speed interpolation; shipped waves use mode 0.
            const int duration=param_i32(ins,0),mode=param_i32(ins,1);
            if(mode!=0&&mode!=3)return false; // EXE 0x4d66ec -> 0x4d5e9d: mode 3 = t^4
            float angle=param_f32(ins,2),speed=param_f32(ins,3);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                if(duration<=0){e.turn={};e.accelerate={};break;}
                if(angle<=-999999.f)angle=e.angle;
                else if(e.mirrored)angle=3.14159265358979323846f-angle;
                if(speed<=-999999.f)speed=e.speed;
                const float delta=std::remainder(angle-e.angle,6.28318530717958647692f);
                e.turn={duration,0,mode,e.angle,e.angle+delta};
                e.accelerate={duration,0,mode,e.speed,speed};e.polar=true;e.orbit=false;break;
            }return true;
        }
        case 408: { // 0x5005db; native movement mode 2, NOT curved linear velocity.
            const float phase=param_f32(ins,0),omega=param_f32(ins,1);
            const float radius=param_f32(ins,2),radial=param_f32(ins,3);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                // Reissuing this instruction retains the existing center.
                if(!e.orbit){e.orbit_x=e.x;e.orbit_y=e.y;}
                if(phase>-999999.f)e.angle=std::remainder(phase,6.28318530717958647692f);
                if(omega>-999999.f)e.speed=omega;
                if(radius>-999999.f)e.radius=radius;
                if(radial>-999999.f)e.radial_speed=radial;
                e.orbit=true;e.ellipse=false;e.polar=false;
                // The dispatcher resolves the initial position immediately;
                // the frame update then advances radius and phase once.
                e.x=e.orbit_x+std::cos(e.angle)*e.radius;
                e.y=e.orbit_y+std::sin(e.angle)*e.radius;
                break;
            }return true;
        }
        case 412: { // 0x501013: bounded random direction, initial speed -> zero.
            const int duration=param_i32(ins,0),mode=param_i32(ins,1);
            const float speed=param_f32(ins,2);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                const float randomAngle=world_->bullet_rng.signedUnit()*3.14159274101257324219f;
                const bool middle=e.x>=e.bound_x-e.bound_w*.25f&&e.x<=e.bound_x+e.bound_w*.25f;
                const unsigned random16=middle?(world_->bullet_rng.next()&65535u):0;
                const float angle=nativeBossMoveAngle(e.x,e.y,world_->player_x,e.bound_x,e.bound_y,e.bound_w,e.bound_h,randomAngle,random16);
                e.angle=angle;e.speed=speed;e.polar=true;e.orbit=e.ellipse=false;
                e.move_x={};e.move_y={};
                e.turn={duration,0,mode,angle,angle};
                // 0x50138a writes the angle interpolator's mode again. The
                // speed interpolator retains its previous mode (normally 0).
                e.accelerate={duration,0,e.accelerate.mode,speed,0};
                break;
            }return true;
        }
        case 422: { // 0x5009cd: opcode420 selects +90; 422 selects independent +d8 motion.
            const float phase=param_f32(ins,0),omega=param_f32(ins,1),radius=param_f32(ins,2),radial=param_f32(ins,3),rotation=param_f32(ins,4),ratio=param_f32(ins,5);
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                e.relative_center_x=e.relative_x;e.relative_center_y=e.relative_y;
                if(phase>-999999)e.relative_phase=phase;if(omega>-999999)e.relative_omega=omega;
                if(radius>-999999)e.relative_radius=radius;if(radial>-999999)e.relative_radial=radial;
                if(rotation>-999999)e.relative_rotation=rotation;if(ratio>-999999)e.relative_ratio=ratio;
                e.relative_ellipse=true;
                const auto offset=enemyEllipseOffset(e.relative_phase,e.relative_rotation,e.relative_radius,e.relative_ratio);
                e.x-=e.relative_x;e.y-=e.relative_y;
                e.relative_x=e.relative_center_x+offset[0];
                e.relative_y=e.relative_center_y+offset[1];
                e.x+=e.relative_x;e.y+=e.relative_y;break;
            }return true;
        }
        case 424: { // 0x4fed7d: replace bit 3 of EnemyInf+0x5170 (spawn mirror).
            const bool mirror=(param_i32(ins,0)&1)!=0;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                e.flags=(e.flags&~8u)|(mirror?8u:0u);e.mirrored=mirror;break;
            }return true;
        }
        case 427: { // native 0x500e..: clear movement while preserving absolute position
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                e.vx=e.vy=e.speed=e.angle=0;e.polar=false;e.orbit=false;
                e.relative_ellipse=false;e.relative_x=e.relative_y=e.relative_center_x=e.relative_center_y=0;
                e.radius=e.radial_speed=0;
                e.move_x={};e.move_y={};
                e.turn={};e.accelerate={};break;
            }return true;
        }
        case 440: { // native 0x40fdf0: change heading, preserve speed and motion.
            float angle=param_f32(ins,0);
            for(auto&e:world_->enemies)if(has_host&&e.id==host_id){
                e.angle=std::remainder(e.mirrored?3.14159265358979323846f-angle:angle,6.28318530717958647692f);break;
            }return true;
        }
        case 441: case 445: { // native angleTime / speedTime
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){
                auto& m=ins.id==441?e.turn:e.accelerate;
                m.duration=std::max(0,param_i32(ins,0));m.elapsed=0;m.mode=param_i32(ins,1);
                m.from=ins.id==441?e.angle:e.speed;m.to=param_f32(ins,2);
                if(ins.id==441&&e.mirrored)m.to=3.14159265358979323846f-m.to;
                e.polar=true;break;
            }return true;
        }
        case 520: return true; // 0x502f22: retry handled before operand consumption.
        case 519: return true; // MSG wait; this world only advances after pre-battle MSG ends.
        case 500: case 501: { // damage hitbox versus player-contact hitbox
            float w=param_f32(ins,0),h=param_f32(ins,1);
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){if(ins.id==500){e.hitbox_w=w;e.hitbox_h=h;}else{e.contact_w=w;e.contact_h=h;}break;}
            return true;
        }
        case 557: // 5016b7: replace flag bit12, not OR-only.
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.flags=(e.flags&~0x1000u)|((param_i32(ins,0)&1)?0x1000u:0u);break;}
            return true;
        case 558: // 5016ee: independent contact rectangle angle (+0x130).
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.contact_angle=param_f32(ins,0);break;}
            return true;
        case 502: { // set enemy flags (bitmask) — e.g. 32 = invincible/invisible/undeletable
            u32 mask=u32(param_i32(ins,0));
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.flags|=mask;break;}
            return true;
        }
        case 503: { // clear enemy flags (bitmask)
            u32 mask=u32(param_i32(ins,0));
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.flags&=~mask;break;}
            return true;
        }
        case 1010:
            world_->wave_schedule.clear_pool();return true;
        case 1011: { // append wave ID 'weight' times, native 0x537aa0
            world_->wave_schedule.append(param_i32(ins,0),param_i32(ins,1));
            return true;
        }
        case 1012: { // native 0x5048ad: choose ONE wave and create its controller
            i32 wave;
            if(!world_->wave_schedule.next(wave))return false;
            // Native name pointer table 0x5a0f50, verified IDs 1..40.
            if(wave<1||wave>40)return false;
            char name[32];std::snprintf(name,sizeof(name),"Wave%02d%s",(wave+1)/2,wave%2?"":"f");
            float x=0,y=0;
            if(has_host)for(const auto& e:world_->enemies)if(e.id==host_id){x=e.x;y=e.y;break;}
            world_->spawn_enemy(name,x,y,0,0,0);
            world_->current_wave=wave;++world_->wave_counts[wave];
            return true;
        }
        case 1013: { // native 0x504a07: set a row of eight integer parameters
            i32 type=param_i32(ins,0);
            if(type<0||type>=32)return false;
            auto& arr=world_->difficulty_values[type];
            for(i32 k=0;k<8;++k)arr[k]=param_i32(ins,1+k);
            return true;
        }
        case 1015: { // native dispatcher case 0xaa: row[difficulty] -> output
            const i32 addr=read_i32(param_off(0)),type=param_i32(ins,1);
            if(type<0||type>=32||world_->difficulty<0||world_->difficulty>=8)return false;
            set_var_int(addr,world_->difficulty_values[type][world_->difficulty]);
            return true;
        }
        case 1023: // native 0x40f1e0: item-generation percentage (0..500)
            world_->pickup_rate=std::clamp(param_i32(ins,0),0,500);return true;
        case 1027: case 1040: // native +0x5170 kind bits; 1040 additionally marks purified
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){world_->set_spirit_kind(e,param_i32(ins,0),ins.id==1040);break;}
            return true;
        case 1028: { // death-chain radius: EnemyInf+0x5110, consumed at 0x50c1f0
            float t=param_f32(ins,0);
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.spirit_type=t;break;}
            return true;
        }
        case 1032: // native 0x504ea8: extension of the chain timer on this enemy's death
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.combo_frames=param_i32(ins,0);break;}
            return true;
        case 1029: case 1030: case 1031: case 1033: { // chain damage / secondary / duration / remaining parameter
            i32 v=param_i32(ins,0);
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){e.spirit_params[ins.id-1029]=v;break;}
            return true;
        }
        case 529: case 530: // 0x501c19 / 0x501cdf: four-way selector, same enemy+5168 rank.
        case 570: case 571: { // 0x501dd5 / 0x501e36: eight-way attack-rank parameter.
            int rank=world_->difficulty;
            if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){if(e.attack_rank>=0)rank=e.attack_rank;break;}
            const int last=ins.id==529||ins.id==530?4:8;
            const int arg=rank>=0&&rank<last-1?rank+1:last;
            if(ins.id==529||ins.id==570)set_var_int(read_i32(param_off(0)),param_i32(ins,arg));
            else set_var_float(i32(read_f32(param_off(0))),param_f32(ins,arg));
            return true;
        }
        case 1007: // 0x5047b3: explicit enemy rank + VM instruction difficulty mask.
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.attack_rank=param_i32(ins,0);return true;}
            return false;
        case 1034: case 1035: // 0x504ef2 / 0x504f15: opposing attack manager, not spirit stock.
            if(has_host)for(auto& e:world_->enemies)if(e.id==host_id){
                e.attack_rank=std::clamp(ins.id==1034?world_->incoming_extra_level:world_->incoming_boss_level,0,7);break;
            }
            return true;
        case 1036: // 0x50509a: opposite side lives=0, then 0x549830 death.
            world_->opponent_defeat_requested=true;
            return true;
        case 1037: {
            // The ECL host already lives in the receiving field. Keep its
            // sender's program set across the delayed native ExAttack spawn.
            int character=-1;
            for(const auto*p:programs_)for(const auto&file:p->anims)
                if(file.size()==9&&file.substr(0,2)=="pl"&&file.substr(4)=="b.anm")character=(file[2]-'0')*10+file[3]-'0';
            int variant=0;if(has_host)for(const auto&e:world_->enemies)if(e.id==host_id){variant=e.ecl_ints[0];break;}
            const float x=param_f32(ins,0),y=param_f32(ins,1);
            return world_->queue_extra_attack(character,x,y,variant,programs_);
        }
        case 1038: case 1039: case 1041: case 1042: {
            // Native dispatch cases bf/c0/c2/c3 replace one bit, including
            // clearing it for argument zero. 1041 excludes the chain reward.
            const u32 mask=ins.id==1038?0x08000000u:ins.id==1039?0x10000000u:ins.id==1041?0x20000000u:0x00800000u;
            const bool enabled=(param_i32(ins,0)&1)!=0;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.status_flags=(e.status_flags&~mask)|(enabled?mask:0u);break;}
            return true;
        }
        case 550: { // 0x502d92: copy the death ECL name into EnemyInf+0x55e8.
            if(ins.param_bytes<4)return false;
            const int len=read_i32(param_off(0));
            if(len<0||u32(len)>ins.param_bytes-4)return false;
            std::string name(reinterpret_cast<const char*>(stream()+param_off(1)),size_t(len));
            const auto zero=name.find('\0');if(zero!=std::string::npos)name.resize(zero);
            if(name.size()>63)return false;
            if(has_host)for(auto&e:world_->enemies)if(e.id==host_id){e.death_callback=name;break;}
            return true;
        }
        case 549: { // enemyExists(out, id), NOT a wave-spawn command.
            // Native 0x5043c4 -> 0x40f3a0: reject ID zero, scan the enemy
            // list for matching +0x6858 ID, store a boolean through argument 0.
            // Actual wave selection/spawning is opcode 1012 (0x5048ad).
            const i32 result_addr=read_i32(param_off(0));
            const u32 id=u32(param_i32(ins,1));
            bool found=false;
            if(id)for(const auto& e:world_->enemies)if(e.active&&e.id==id){found=true;break;}
            set_var_int(result_addr,found?1:0);
            return true;
        }
        default:
            return false;
    }
}
}
