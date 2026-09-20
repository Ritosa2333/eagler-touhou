#include "BackgroundScript.hpp"
#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include "Presentation.hpp"
namespace th08 {
namespace {
Vec3 subtract(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
float eased(float value,i32 mode,bool vector){
    auto t=number(value);if(mode>=1&&mode<=3){const auto inverse=number(1)-t;const auto rounded=number(inverse.to_float());auto result=inverse*rounded;
        for(i32 i=1;i<mode;++i)result=result*rounded;return (number(1)-result).to_float();}
    if(mode>=4&&mode<=6){auto result=t*t;for(i32 i=4;i<mode;++i)result=result*t;return result.to_float();}return value;
}
float hermite(float initial,float final,float first,float last,float t){
    const auto v=number(t),one=number(1),two=number(2);
    const float a=((v-one)*(v-one)*(two*v+one)).to_float();
    const float b=(v*v*(number(3)-two*v)).to_float();
    const float c=((one-v)*(one-v)*v).to_float();
    const float d=((v-one)*v*v).to_float();
    return (number(a)*number(initial)+number(b)*number(final)+number(c)*number(first)+number(d)*number(last)).to_float();
}
}
void BackgroundScript::start(AnmLoaded* file,AnmVm& vm,i32 index,bool base){
    if(!file||index<0||u32(index)>=file->scriptCount){invalid=true;return;}
    if(base){vm.pos={};vm.pos2={};vm.fontWidth=vm.fontHeight=15;}
    vm.scriptIndex=i16(index);anm.start(*file,vm,file->scripts[index]);
}
bool BackgroundScript::load(const u8* bytes,u32 size,AnmLoaded& background,AnmLoaded& text){
    release();invalid=false;if(!program.load(bytes,size))return false;
    auto& s=state;s.stage_data=program.header();s.object_count=s.stage_data->object_count;s.quad_count=s.stage_data->quad_count;
    s.objects=program.objects();s.instances=program.instances();s.instructions=program.instructions();s.animation=&background;
    quads.resize(s.quad_count);if(!quads.empty())std::memset(quads.data(),0,quads.size()*sizeof(AnmVm));s.quad_vms=quads.data();i32 index=0;
    for(i32 i=0;i<s.object_count;++i){auto& object=*s.objects[i];object.flags=1;
        for(auto* q=StageProgram::first(object);q->type>=0;q=StageProgram::next(*q)){q->vm=i16(index);start(&background,quads[index++],q->script,true);}
    }
    start(&text,s.tint_vm,33,false);s.tint_vm.SetInterrupt(2);s.youkai_tint=0;s.tint_time.set(0);return !invalid;
}
void BackgroundScript::release(bool keep_program){
    quads.clear();state.quad_vms=nullptr;presentation_camera_valid=presentation_camera_rebased=false;
    if(!keep_program){program.clear();state.stage_data=nullptr;state.objects=nullptr;state.instances=nullptr;state.instructions=nullptr;}
}
void BackgroundScript::reset_camera(){
    auto& s=state;s.time.set(0);s.instruction_index=0;s.position={};s.spell_state=0;s.fog_duration=0;
    s.fog={200,500,{i32(0xff000000)}};s.camera.position={0,0,1000};s.camera.target_offset={};s.camera.eye_offset={};s.camera.up={0,1,0};s.camera.field_of_view=.5235987901687622f;
    s.camera_final=s.camera_initial=s.camera;s.camera_effect=0;presentation_previous_camera=s.camera;presentation_camera_valid=presentation_camera_rebased=false;
    for(u32 i=0;i<4;++i){s.camera_durations[i]=0;s.camera_timers[i].set(0);}s.pending_interrupt=0;
    s.distance_limit=context.stage==5?1822500.f:context.stage==6||context.stage==7?3240000.f:1322500.f;
}
float BackgroundScript::progress(u32 index,bool vector){
    auto& s=state;auto& timer=s.camera_timers[index];auto& duration=s.camera_durations[index];float t;
    if(timer.current<duration){timer.tick(anm.timing);t=(timer.value()/Extended::from_int(duration)).to_float();}
    else{timer.set(duration);duration=0;t=1;}
    return eased(t,s.camera_modes[index],vector);
}
void BackgroundScript::interpolate(u32 index,Vec3& output,const Vec3& initial,const Vec3& final,const Vec3& d0,const Vec3& d1){
    if(index>=5)return;const float t=progress(index,true);
    if(state.camera_modes[index]==7){output={hermite(initial.x,final.x,d0.x,d1.x,t),hermite(initial.y,final.y,d0.y,d1.y,t),hermite(initial.z,final.z,d0.z,d1.z,t)};}
    else {const auto delta=subtract(final,initial);output={(number(Scalar::mul(delta.x,t))+number(initial.x)).to_float(),(number(Scalar::mul(delta.y,t))+number(initial.y)).to_float(),(number(Scalar::mul(delta.z,t))+number(initial.z)).to_float()};}
}
void BackgroundScript::tint(u32 color){
    auto& c=state.tint_color;if(!c.a){c.d3dColor=signed_bits(color);return;}c.b=(u32(c.b)+(color&255))>>1;c.g=(u32(c.g)+((color>>8)&255))>>1;c.r=(u32(c.r)+((color>>16)&255))>>1;c.a=(u32(c.a)+(color>>24))>>1;
}
void BackgroundScript::update_objects(){
    auto& s=state;if(bool(s.youkai_tint)!=context.youkai){s.youkai_tint=context.youkai;s.tint_time.set(0);s.tint_vm.SetInterrupt(context.youkai?1:2);}
    s.tint_time.tick(anm.timing);anm.execute(s.tint_vm);
    for(i32 i=0;i<s.object_count;++i){auto& object=*s.objects[i];if(!(object.flags&1))continue;u32 active=0;AnmVm* last=nullptr;
        for(auto* q=StageProgram::first(object);q->type>=0;q=StageProgram::next(*q)){last=s.quad_vms+q->vm;if(q->type==0||q->type==1)anm.execute(*last);active+=last->currentInstruction!=nullptr;}
        // The original applies the tint to the final VM of each object.
        if(last&&last->type==1){last->flag17=true;auto& c=last->color2;const auto& a=last->color1;const auto& b=s.tint_vm.color1;c.b=(u32(a.b)*b.b)>>8;c.g=(u32(a.g)*b.g)>>8;c.r=(u32(a.r)*b.r)>>8;c.a=(u32(a.a)*b.a)>>8;}
        if(!active)object.flags&=~1;
    }
}
void BackgroundScript::finish_frame(){
    auto& s=state;
    if(s.camera_durations[0])interpolate(0,s.camera.position,s.camera_initial.position,s.camera_final.position,s.camera_initial_derivative.position,s.camera_final_derivative.position);
    if(s.camera_durations[1])interpolate(1,s.camera.target_offset,s.camera_initial.target_offset,s.camera_final.target_offset,s.camera_initial_derivative.target_offset,s.camera_final_derivative.target_offset);
    if(s.camera_durations[2])interpolate(2,s.camera.up,s.camera_initial.up,s.camera_final.up,s.camera_initial_derivative.up,s.camera_final_derivative.up);
    if(s.camera_durations[3]){const float t=progress(3,false);s.camera.field_of_view=((number(s.camera_final.field_of_view)-number(s.camera_initial.field_of_view))*number(t)+number(s.camera_initial.field_of_view)).to_float();}
    GraphicsMath::normalize(s.camera.unused24,s.camera.target_offset);
    if(s.camera_effect>=1&&s.camera_effect<=3){auto& timer=s.camera_timers[4];const bool rotate=s.camera_effect==3;const auto phase=timer.value()*number(3.1415927410125732f);const float angle=((phase+phase)/number(rotate?4800.f:480.f)-number(3.1415927410125732f)).to_float();
        if(s.camera_effect==1)s.camera.eye_offset.x=(sine(angle)*number(40)).to_float();
        if(s.camera_effect==2){s.camera.eye_offset.x=(sine(angle)*number(70)).to_float();s.camera.up.x=(-sine(angle)*number(.1f)).to_float();}
        if(rotate){s.camera.up.x=sine(angle).to_float();s.camera.up.z=cosine(angle).to_float();}timer.tick(anm.timing);if(timer.current>=(rotate?4800:480))timer.set(0);
    }
    if(s.fog_duration){s.fog_time.tick(anm.timing);float t=(s.fog_time.value()/Extended::from_int(s.fog_duration)).to_float();if(t>=1)t=1;
        for(u32 i=0;i<4;++i){auto* dest=reinterpret_cast<u8*>(&s.fog.color);const auto* a=reinterpret_cast<const u8*>(&s.fog_initial.color);const auto* b=reinterpret_cast<const u8*>(&s.fog_final.color);dest[i]=u8((Extended::from_int(i32(b[i])-a[i])*number(t)+Extended::from_int(a[i])).truncate_int());}
        s.fog.near_plane=((number(s.fog_final.near_plane)-number(s.fog_initial.near_plane))*number(t)+number(s.fog_initial.near_plane)).to_float();s.fog.far_plane=((number(s.fog_final.far_plane)-number(s.fog_initial.far_plane))*number(t)+number(s.fog_initial.far_plane)).to_float();if(s.fog_time.current>=s.fog_duration)s.fog_duration=0;
    }
}
SceneCamera BackgroundScript::presentation_camera()const{
    SceneCamera result=state.camera;if(!presentation::render_only||!presentation::active||!presentation_camera_valid)return result;
    // Opcode 4 followed by opcode 5 rebases the stage coordinate system and
    // moves its companion effects by the same offset. The two endpoints may be
    // numerically close (stage 1 wraps by 511.5 after its ordinary 0.5 step),
    // but every in-between camera is a view that never exists in the script.
    if(presentation_camera_rebased)return result;
    const auto close=[](const Vec3& a,const Vec3& b){const float dx=a.x-b.x,dy=a.y-b.y,dz=a.z-b.z;return dx*dx+dy*dy+dz*dz<262144.0f;};
    if(!close(presentation_previous_camera.position,state.camera.position)||!close(presentation_previous_camera.target_offset,state.camera.target_offset)||!close(presentation_previous_camera.eye_offset,state.camera.eye_offset))return result;
    const auto mix=[](const Vec3& a,const Vec3& b){return Vec3{presentation::lerp_world(a.x,b.x),presentation::lerp_world(a.y,b.y),presentation::lerp_world(a.z,b.z)};};
    result.position=mix(presentation_previous_camera.position,state.camera.position);result.target_offset=mix(presentation_previous_camera.target_offset,state.camera.target_offset);result.eye_offset=mix(presentation_previous_camera.eye_offset,state.camera.eye_offset);result.up=mix(presentation_previous_camera.up,state.camera.up);result.field_of_view=presentation::lerp_world(presentation_previous_camera.field_of_view,state.camera.field_of_view);GraphicsMath::normalize(result.unused24,result.target_offset);return result;
}
#if defined(TH_PRESENTATION_AUDIT)
bool BackgroundScript::audit_cameras(SceneCamera& previous,SceneCamera& current,bool& rebased)const{
    previous=presentation_previous_camera;current=state.camera;rebased=presentation_camera_rebased;return presentation_camera_valid;
}
#endif
JobResult BackgroundScript::update(){
    auto& s=state;if(!s.stage_data||context.paused)return JobResult::Continue;
    if(presentation_marker.capture()){presentation_previous_camera=s.camera;presentation_camera_valid=true;presentation_camera_rebased=false;}
    if(context.stage==7){
        if(!s.moon_effect){s.moon_effect=actions.moon();if(s.moon_effect)start(s.animation,*s.moon_effect,11,false);}
        else if(s.pending_interrupt==1)start(s.animation,*s.moon_effect,11,false);
        else if(s.pending_interrupt>=2&&s.pending_interrupt<=4){auto& vm=*s.moon_effect;const AnmVm saved=vm;if(s.pending_interrupt==2)start(s.animation,vm,12,false);vm.SetInterrupt(i16(s.pending_interrupt));vm.posFinal=saved.posFinal;vm.posInitial=saved.posInitial;vm.interpCurrentTimers[0]=saved.interpCurrentTimers[0];vm.interpEndTimers[0]=saved.interpEndTimers[0];vm.interpModes[0]=saved.interpModes[0];vm.color1=saved.color1;}
    }
    const u32 count=program.instructions_size();
    if(s.pending_interrupt){s.instruction_index=0;for(u32 i=0;i<count;++i){const auto& instruction=s.instructions[i];if(instruction.frame==-1)break;if(instruction.opcode==31&&instruction.integer(0)==s.pending_interrupt){s.instruction_index=i+1;s.time.set(instruction.frame);s.pending_interrupt=0;break;}}}
    u32 executed=0;const StageInstruction* current=nullptr;bool stop=false;
    while(++executed<=65536){if(s.instruction_index<0||u32(s.instruction_index)>=count){invalid=true;return JobResult::Error;}current=s.instructions+s.instruction_index;const auto& ins=*current;if(ins.frame==-1||s.time.current<ins.frame)break;
        const auto vector=ins.vector();const i32 arg=ins.integer(0);
        switch(ins.opcode){
        case 0:s.position=s.previous_position=vector;s.previous_time=ins.frame;if(u32(s.instruction_index+1)<count){s.next_time=current[1].frame;s.next_position=current[1].vector();}break;
        case 1:s.fog={ins.real(1),ins.real(2),{arg}};s.fog_final=s.fog;break;
        case 2:s.fog_initial=s.fog;s.fog_duration=arg;s.fog_time.set(0);break;
        case 3:if(!s.pending_interrupt)stop=true;else s.pending_interrupt=0;break;
        case 4:s.instruction_index=arg;s.time.set(ins.integer(1));s.camera_durations[0]=0;s.jumped=1;continue;
        case 5:if(s.jumped){actions.move_effects(subtract(vector,s.camera_final.position));s.jumped=0;presentation_camera_rebased=true;}s.camera_initial.position=s.camera_final.position;s.camera_final.position=vector;if(!s.camera_durations[0])s.camera.position=vector;break;
        case 6:case 8:case 10:case 12:{const u32 i=(ins.opcode-6)/2;s.camera_durations[i]=arg;s.camera_timers[i].set(0);s.camera_modes[i]=ins.integer(1);break;}
        case 7:s.camera_initial.target_offset=s.camera_final.target_offset;s.camera_final.target_offset=vector;if(!s.camera_durations[1])s.camera.target_offset=vector;break;
        case 9:s.camera_initial.up=s.camera_final.up;s.camera_final.up=vector;if(!s.camera_durations[2])s.camera.up=vector;break;
        case 11:s.camera_initial.field_of_view=s.camera_final.field_of_view;s.camera_final.field_of_view=ins.real(0);if(!s.camera_durations[3])s.camera.field_of_view=ins.real(0);break;
        case 13:s.clear_color=ins.args[0];break;
        case 14:s.camera_initial.position=vector;break;case 15:s.camera_final.position=vector;break;case 16:s.camera_initial_derivative.position=vector;break;case 17:s.camera_final_derivative.position=vector;break;
        case 18:case 23:case 28:{const u32 i=(ins.opcode-18)/5;s.camera_durations[i]=arg;s.camera_timers[i].set(0);s.camera_modes[i]=7;break;}
        case 19:s.camera_initial.target_offset=vector;break;case 20:s.camera_final.target_offset=vector;break;case 21:s.camera_initial_derivative.target_offset=vector;break;case 22:s.camera_final_derivative.target_offset=vector;break;
        case 24:s.camera_initial.up=vector;break;case 25:s.camera_final.up=vector;break;case 26:s.camera_initial_derivative.up=vector;break;case 27:s.camera_final_derivative.up=vector;break;
        case 29:case 30:case 34:{const u32 i=ins.opcode==34?2:ins.opcode-29;if(arg<0)s.layers[i==1?0:i].activeSpriteIndex=-1;else start(s.animation,s.layers[i],arg,true);break;}
        case 32:s.camera.eye_offset=vector;break;
        case 33:s.camera_effect=u8(arg);s.camera_durations[4]=0;s.camera_timers[4].set(0);s.camera_modes[4]=0;break;
        }
        if(stop)break;s.instruction_index=wrapping_add(s.instruction_index,1);
    }
    if(executed>65536){invalid=true;return JobResult::Error;}
    finish_frame();if(current->opcode!=3)s.time.tick(anm.timing);update_objects();
    if(s.spell_state>0){if(s.spell_frames==60)s.spell_state=wrapping_add(s.spell_state,1);s.spell_frames=wrapping_add(s.spell_frames,1);for(i32 i=0;i<s.spell_vm_count&&i<32;++i)anm.execute(s.spell_vms[i]);}
    for(u32 i=0;i<3;++i)if(s.layers[i].activeSpriteIndex>0){anm.execute(s.layers[i]);if(i==2)s.clear_color=u32(s.layers[i].color1.d3dColor);}
    if(s.frames%3==0&&(s.frames>699||context.practice)&&s.spell_state<2)for(u32 i=0;i<12;++i)actions.sparkle(s.effect_positions[i]);
    s.effect_visible=1;if(s.spell_state>1)s.effect_flags=0;s.frames=wrapping_add(s.frames,1);
    return s.frames%500==250&&actions.integrity_failed()?JobResult::Exit:JobResult::Continue;
}
}
