#include "binding.hpp"
#include <cstring>
namespace th20::source::sprite {
namespace {
float sub(float a,float b) {return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b) {return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float mul(float a,float b) {return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a),_mm_set_ss(b)));}
constexpr Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
}
int initialize_animation_script(AnimationFile& file,Animation& animation,std::int32_t index) {
    if(!file.scripts[index]){std::memset(&animation,0,sizeof(animation));return -1;}
    reset_animation_state(animation);clear_animation_suffix(animation);
    animation.base.field_440=static_cast<std::uint16_t>(index);
    animation.base.fields_10_28[2]=file.id;animation.base.fields_10_28[3]=file.id;
    auto* bytes=reinterpret_cast<std::uint8_t*>(&animation);
    bytes[0x4a0]=(bytes[0x4a0]&0x3c)|2;
    animation.base.fields_10_28[5]=index;animation.base.fields_10_28[6]=0;
    th20::recovered::timer_set(animation.timer_4d8,0);th20::recovered::timer_set(animation.timer_4c8,0);
    bytes[0x49a]&=0xfe;return 0;
}
void store_sprite_descriptor(AnimationFile& file,std::int32_t index,const SpriteData& input) {
    auto& sprite=file.sprites[index];std::memmove(&sprite,&input,sizeof(sprite));
    sprite.u0=div(sprite.left,sprite.texture_extent_20);
    sprite.u1=div(sprite.right,sprite.texture_extent_20);
    sprite.v0=div(sprite.top,sprite.texture_extent_1c);
    sprite.v1=div(sprite.bottom,sprite.texture_extent_1c);
    sprite.extent_4c=div(sub(sprite.right,sprite.left),input.scale_50);
    sprite.extent_48=div(sub(sprite.bottom,sprite.top),input.scale_54);
}
void copy_animation_base(Animation& destination,const Animation& source) noexcept {
    std::memmove(&destination.base,&source.base,sizeof(AnimationBase));
}
void select_animation_template(AnimationFile& file,Animation& animation,std::int32_t index) {
    animation.fields_550[8]=0;clear_animation_suffix(animation);
    copy_animation_base(animation,file.templates[index]);
    th20::recovered::timer_set(animation.timer_4d8,0);
    th20::recovered::timer_set(animation.timer_4c8,0);
}
void attach_animation_parent(Animation& animation,Animation* parent) noexcept {
    if(!parent) animation.fields_550[3]=animation.fields_550[2]=0;
    else {
        animation.base.flags[1]=(animation.base.flags[1]&~0x1000000u)|(parent->base.flags[1]&0x1000000u);
        // Original 0x40e5e0 is a verified no-op; parent assignment follows it.
        animation.fields_550[3]=reinterpret_cast<std::uintptr_t>(parent);
        if(parent->fields_550[2]) parent=reinterpret_cast<Animation*>(parent->fields_550[2]);
        animation.field_4e8=parent->field_4e8;
        animation.fields_550[2]=reinterpret_cast<std::uintptr_t>(parent);
    }
}
void set_animation_layer(Animation& animation,std::int32_t layer) noexcept {
    animation.base.fields_10_28[1]=static_cast<std::uint32_t>(layer);
    const auto mode=layer>=3&&layer<=19?1u:layer>=20&&layer<=23?2u:0u;
    animation.base.flags[2]=(animation.base.flags[2]&~0x03000000u)|(mode<<24);
    if(((layer>19&&layer<37)||(layer>44&&layer<54))&&!(animation.base.flags[1]&0x800000u))
        animation.base.flags[3]=(animation.base.flags[3]&~0xffu)|1u;
}
int assign_animation_sprite(AnimationFile& file,Animation& animation,std::int32_t index) {
    if(!file.bytes) return -1;
    auto& b=animation.base;const auto& sprite=file.sprites[index];
    b.fields_10_28[4]=static_cast<std::uint32_t>(index);
    b.vectors_378[0]={sprite.u0,sprite.v0};b.vectors_378[1]={sprite.u1,sprite.v0};
    b.vectors_378[2]={sprite.u0,sprite.v1};b.vectors_378[3]={sprite.u1,sprite.v1};
    b.vector_398={sub(sprite.u1,sprite.u0),sub(sprite.v1,sprite.v0)};
    b.vector_70={sprite.extent_4c,sprite.extent_48};
    b.matrix_3b8=identity;b.matrix_3f8=identity;
    // Original 0x56cda0 is 256.0f; this scale precedes the texture scale.
    b.matrix_3b8.elements[0]=div(b.vector_70.x,256.0f);
    b.matrix_3b8.elements[5]=div(b.vector_70.y,256.0f);
    b.matrix_3f8.elements[0]=mul(div(b.vector_70.x,sprite.texture_extent_20),sprite.scale_50);
    b.matrix_3f8.elements[5]=mul(div(b.vector_70.y,sprite.texture_extent_1c),sprite.scale_54);
    animation.matrix_57c=b.matrix_3b8;
    b.fields_3a0[2]=static_cast<std::uint32_t>(th20::recovered::truncate32(sprite.left));
    b.fields_3a0[3]=static_cast<std::uint32_t>(th20::recovered::truncate32(sprite.top));
    b.fields_3a0[4]=static_cast<std::uint32_t>(th20::recovered::truncate32(sprite.right));
    b.fields_3a0[5]=static_cast<std::uint32_t>(th20::recovered::truncate32(sprite.bottom));
    return 0;
}
AnimationFile& script_file(Controller& c,const Animation& animation) {return *c.files[animation.base.fields_10_28[2]];}
AnimationFile& sprite_file(Controller& c,const Animation& animation) {return *c.files[animation.base.fields_10_28[3]];}
SpriteData& current_sprite(Controller& c,const Animation& animation) {return sprite_file(c,animation).sprites[animation.base.fields_10_28[4]];}
AnmInstruction* script_start(Controller& c,const Animation& animation) {return script_file(c,animation).scripts[animation.base.fields_10_28[5]];}
DeviceTexture* texture(Controller& c,std::uint32_t packed) {return c.files[th20::recovered::signed_bits(packed)>>8]->textures[packed&0xffu].texture;}
}
