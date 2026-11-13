#include "AnmVm.hpp"
#include "AnmGeometry.hpp"
#include "PortTitle.hpp"
#include <cmath>
namespace th11 {
void AnmVm::interrupt(i32 label) noexcept{
#if PORT_TITLE == 14
    if(th13_effect==1&&geometry){auto& transition=*static_cast<TransitionType0Th14*>(geometry);
        if(!transition.environment||!transition.resource_owner||!transition.interrupt(*this,label,*transition.resource_owner,*transition.environment))transition.failed=true;
    }
    if(th13_effect==7&&label==1&&geometry)static_cast<AnmTh14Particles*>(geometry)->timer.advance(300);
#endif
    pending_interrupt=label;
}
float AnmVm::inherited_slowdown() const noexcept{
#if PORT_TITLE == 14
    const AnmVm* vm=this;while(vm->parent&&!(vm->flags2&0x8000))vm=vm->parent;
    return vm->slowdown_factor;
#elif PORT_TITLE == 16
    const AnmVm* vm=this;while(vm->th16_transform_parent&&!(vm->flags2&0x10000))vm=vm->th16_transform_parent;
    return vm->slowdown_factor;
#else
    return 0;
#endif
}
Vec3 AnmVm::render_position(const AnmRenderSpace& space) const noexcept {
    // TH13 466a80 rounds each local sum before following the live parent.
    Vec3 result{float(double(float(double(position.x)+script_position.x))+child_position.x),float(double(float(double(position.y)+script_position.y))+child_position.y),float(double(float(double(position.z)+script_position.z))+child_position.z)};
    return transform_position(result,space);
}
Vec3 AnmVm::transform_position(Vec3 result,const AnmRenderSpace& space) const noexcept {

#if PORT_TITLE == 16
    {
        // Complete406a70: resolution, live transform parent, then root origin.
        const u32 resolution=(flags2>>20)&7;
        if(resolution>=1&&resolution<=4){
            const float scale=resolution==2||resolution==4?float(space.scale*.5f):space.scale;
            result={float(result.x*scale),float(result.y*scale),float(result.z*scale)};
        }
        if(th16_transform_parent&&!(flags2&0x10000)){
            if(flags2&0x800000){
                const float c=float(std::cos(double(th16_transform_parent->rotation.z))),s=float(std::sin(double(th16_transform_parent->rotation.z)));
                const float x=float(float(result.x*c)-float(result.y*s)),y=float(float(result.x*s)+float(result.y*c));result.x=x;result.y=y;
            }
            const auto p=th16_transform_parent->render_position(space);
            result={float(result.x+p.x),float(result.y+p.y),float(result.z+p.z)};
        }else if(const u32 origin=(flags2>>18)&3){
            const auto offset=origin==1?space.play_origin:space.scaled_origin;
            result.x=float(result.x+offset.x);result.y=float(result.y+offset.y);
        }
    }
#else
    if constexpr(kPortTitle==13){
        if(parent&&!(flags2&0x10000)){const auto p=parent->render_position();result={float(double(result.x)+p.x),float(double(result.y)+p.y),float(double(result.z)+p.z)};}
        else if(flags2&0x40000){result.x=float(double(result.x)+320);result.y=float(double(result.y)+16);}
    }else if constexpr(kPortTitle==14){
        // 435490 -> 4352b0: scale the local vector before parent rotation,
        // then recurse. Modes 3/4 scale position without scaling sprite size.
        const u32 resolution=(flags2>>19)&7;
        if(resolution>=1&&resolution<=4){
            const float scale=resolution==2||resolution==4?float(space.scale*.5f):space.scale;
            result={float(result.x*scale),float(result.y*scale),float(result.z*scale)};
        }
        if(parent&&!(flags2&0x8000)){
            if(flags2&0x400000){
                const float c=float(std::cos(double(parent->rotation.z))),s=float(std::sin(double(parent->rotation.z)));
                const float x=float(float(result.x*c)-float(result.y*s)),y=float(float(result.y*c)+float(result.x*s));
                result.x=x;result.y=y;
            }
            const auto p=parent->render_position(space);
            result={float(result.x+p.x),float(result.y+p.y),float(result.z+p.z)};
        }else if(const u32 origin=(flags2>>17)&3){
            const auto offset=origin==1?space.play_origin:space.scaled_origin;
            result.x=float(result.x+offset.x);result.y=float(result.y+offset.y);
        }
    }
#endif
    return result;
}
void AnmVm::initialize() noexcept {
    const auto keep_position=position;const auto keep_layer=layer;
    std::memset(this,0,sizeof(*this));position=keep_position;layer=keep_layer;
    scale={1,1};color=0xffffffff;sprite_matrix.identity();flags=7;
#if PORT_TITLE == 13 || PORT_TITLE == 14
    extra_scale=texture_scale={1,1};
#endif
#if PORT_TITLE == 16
    extra_scale=texture_scale={1,1};
#endif
#if PORT_TITLE == 14
    random_scale={1,3.1415927410125732421875f};random_bound=65536;
#endif
    timer.previous=-999999;registry.value=child.value=this;
}
bool AnmVm::bind_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept {
    if(script<0||u32(script)>=file.scripts.size())return false;
    initialize();script_index=i16(script);file_index=file_id;resource=&file;
    script_begin=instruction=reinterpret_cast<AnmInstruction*>(file.scripts[script].bytes.data());
    timer.set(0,rate);flags&=~1u;return true;
}
#if PORT_TITLE == 14
bool AnmVm::copy_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept {
    if(script<0||u32(script)>=file.scripts.size())return false;
    if(file.templates&&u32(script)<file.templates->scripts.size()){
        // 40af00 copies the initialized 0x540-byte script prefix. Its live
        // suffix is reset by40ae30 and both clocks restart at zero.
        const auto origin=position;
        *this=file.templates->scripts[u32(script)];
        position=origin;id=0;registry={this,nullptr,nullptr};child={this,nullptr,nullptr};draw_next=nullptr;
        geometry=nullptr;parent=nullptr;slowdown_factor=0;inherited_color=0;
        before_update=nullptr;draw_callback=nullptr;after_update=nullptr;sprite_callback=nullptr;th13_effect=0;
        timer={};timer.set(0,rate);age={};age.set(0,rate);
        file_index=file_id;resource=&file;trace_count=0;
        return true;
    }
    return bind_script(file,script,file_id,rate);
}
#endif
bool AnmVm::bind_sprite(i32 index) noexcept {
    return resource&&bind_sprite_from(*resource,index);
}
bool AnmVm::bind_sprite_from(AnmResource& file,i32 index) noexcept {
    if(index<0||u32(index)>=file.sprites.size())return false;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    sprite_resource=&file;
#endif
    sprite_index=i16(index);sprite=&file.sprites[index];sprite_size={sprite->width,sprite->height};
    uv[0]={sprite->u0,sprite->v0};uv[1]={sprite->u1,sprite->v0};uv[2]={sprite->u0,sprite->v1};uv[3]={sprite->u1,sprite->v1};
    sprite_matrix.identity();uv_matrix.identity();
    sprite_matrix.m[0]=float(double(sprite_size.x)/256);sprite_matrix.m[5]=float(double(sprite_size.y)/256);transform_matrix=sprite_matrix;
    const auto& texture=file.textures[sprite->texture];
    uv_matrix.m[0]=float(double(sprite_size.x)/texture.width);uv_matrix.m[5]=float(double(sprite_size.y)/texture.height);return true;
}
i32 AnmVm::integer_value(i32 value,AnmEnvironment& env){
#if PORT_TITLE == 14
    if(value==10022)return random_bound?signed_bits(env.script_rng.next32()%u32(random_bound)):0;
    if(value==10027)return truncate_int(random_scale.x);
    if(value==10028)return truncate_int(random_scale.y);
    if(value==10029)return random_bound;
    if(value>=10033&&value<=10035){const float v[]={extra_floats.x,extra_floats.y,extra_floats.z};return truncate_int(v[value-10033]);}
#endif
    if(value>=10000&&value<=10003)return integers[value-10000];
    if(value>=10004&&value<=10007)return truncate_int(floats[value-10004]);
    if(value==10008||value==10009)return extra_integers[value-10008];
    if(value==10022)return signed_bits(env.random(*this).next32());return value;
}
double AnmVm::float_value(float value,AnmEnvironment& env){
    const i32 id=truncate_int(value);
#if PORT_TITLE == 14
    if((id>=10010&&id<=10012)||(id>=10030&&id<=10032))env.th14_execution_precision()=true;
    if(id==10010||id==10030)return float((id==10010?env.script_rng:env.visual_rng).signed_unit()*random_scale.y);
    if(id==10011||id==10031)return float((id==10011?env.script_rng:env.visual_rng).unit()*random_scale.x);
    if(id==10012||id==10032)return float((id==10012?env.script_rng:env.visual_rng).signed_unit()*random_scale.x);
    if(id==10022)return float(env.script_rng.next32());
    if(id>=10023&&id<=10025){const float v[]={rotation.x,rotation.y,rotation.z};return v[id-10023];}
    if(id==10026){
        float v=rotation.z+(parent?parent->rotation.z:0.f);u32 loops=0;
        while(v>3.1415927410125732421875f){v-=6.283185482025146484375f;if(loops++>32)break;}
        while(v<-3.1415927410125732421875f){v+=6.283185482025146484375f;if(loops++>32)break;}
        return v;
    }
    if(id==10027)return random_scale.x;
    if(id==10028)return random_scale.y;
    if(id==10029)return float(random_bound);
    if(id>=10033&&id<=10035){const float v[]={extra_floats.x,extra_floats.y,extra_floats.z};return v[id-10033];}
#endif
    if(id>=10000&&id<=10003)return integers[id-10000];
    if(id>=10004&&id<=10007)return floats[id-10004];
    if(id==10008||id==10009)return extra_integers[id-10008];
    if(id==10010)return float(double(env.random(*this).signed_unit())*3.1415927410125732421875f);
    if(id==10011)return env.random(*this).unit();
    if(id==10012)return env.random(*this).signed_unit();
    if(id>=10013&&id<=10015){const float v[]={script_position.x,script_position.y,script_position.z};return v[id-10013];}
    if(id>=10016&&id<=10021){const auto& p=env.reference_positions[(id-10016)/3];const float v[]={p.x,p.y,p.z};return v[(id-10016)%3];}
    if(id==10022)return env.random(*this).next32();return value;
}
i32* AnmVm::integer_destination(i32* argument) noexcept {
#if PORT_TITLE == 14
    if(*argument==10029)return &random_bound;
#endif
    if(*argument>=10000&&*argument<=10003)return integers+(*argument-10000);
    if(*argument==10008||*argument==10009)return extra_integers+(*argument-10008);return argument;
}
float* AnmVm::float_destination(float* argument) noexcept {
#if PORT_TITLE == 14
    const i32 value=truncate_int(*argument);
    if(value==10023)return &rotation.x;if(value==10024)return &rotation.y;if(value==10025)return &rotation.z;
    if(value==10027)return &random_scale.x;if(value==10028)return &random_scale.y;
    if(value==10033)return &extra_floats.x;if(value==10034)return &extra_floats.y;if(value==10035)return &extra_floats.z;
#endif
    const i32 id=truncate_int(*argument);if(id>=10004&&id<=10007)return floats+(id-10004);
    if(id==10013)return &script_position.x;if(id==10014)return &script_position.y;if(id==10015)return &script_position.z;return argument;
}
}
