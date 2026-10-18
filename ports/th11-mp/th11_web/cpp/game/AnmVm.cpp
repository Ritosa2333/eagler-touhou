#include "AnmVm.hpp"
namespace th11 {
void AnmVm::initialize() noexcept {
    const auto keep_position=position;const auto keep_layer=layer;
    std::memset(this,0,sizeof(*this));position=keep_position;layer=keep_layer;
    scale={1,1};color=0xffffffff;sprite_matrix.identity();flags=7;
    timer.previous=-999999;registry.value=child.value=this;
}
bool AnmVm::bind_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept {
    if(script<0||u32(script)>=file.scripts.size())return false;
    initialize();script_index=i16(script);file_index=file_id;resource=&file;
    script_begin=instruction=reinterpret_cast<AnmInstruction*>(file.scripts[script].bytes.data());
    timer.set(0,rate);flags&=~1u;return true;
}
bool AnmVm::bind_sprite(i32 index) noexcept {
    if(!resource||index<0||u32(index)>=resource->sprites.size())return false;
    sprite_index=i16(index);sprite=&resource->sprites[index];sprite_size={sprite->width,sprite->height};
    uv[0]={sprite->u0,sprite->v0};uv[1]={sprite->u1,sprite->v0};uv[2]={sprite->u0,sprite->v1};uv[3]={sprite->u1,sprite->v1};
    sprite_matrix.identity();uv_matrix.identity();
    sprite_matrix.m[0]=float(double(sprite_size.x)/256);sprite_matrix.m[5]=float(double(sprite_size.y)/256);transform_matrix=sprite_matrix;
    const auto& texture=resource->textures[sprite->texture];
    uv_matrix.m[0]=float(double(sprite_size.x)/texture.width);uv_matrix.m[5]=float(double(sprite_size.y)/texture.height);return true;
}
i32 AnmVm::integer_value(i32 value,AnmEnvironment& env){
    if(value>=10000&&value<=10003)return integers[value-10000];
    if(value>=10004&&value<=10007)return truncate_int(floats[value-10004]);
    if(value==10008||value==10009)return extra_integers[value-10008];
    if(value==10022)return signed_bits(env.random(*this).next32());return value;
}
double AnmVm::float_value(float value,AnmEnvironment& env){
    const i32 id=truncate_int(value);
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
    if(*argument>=10000&&*argument<=10003)return integers+*argument-10000;
    if(*argument==10008||*argument==10009)return extra_integers+*argument-10008;return argument;
}
float* AnmVm::float_destination(float* argument) noexcept {
    const i32 id=truncate_int(*argument);if(id>=10004&&id<=10007)return floats+id-10004;
    if(id==10013)return &script_position.x;if(id==10014)return &script_position.y;if(id==10015)return &script_position.z;return argument;
}
}
