#include "text.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../ecl_vm/math.hpp"
#include <cstring>
#include <emmintrin.h>
namespace th20::source::text {
namespace s=sprite;namespace pe=program_entry;namespace n=th20::recovered;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float neg(float a){return _mm_cvtss_f32(_mm_xor_ps(_mm_set_ss(a),_mm_castsi128_ps(_mm_set_epi32(0,0,0,static_cast<int>(0x80000000u)))));}
void filter(s::Animation& a,bool enabled){a.base.flags[2]=(a.base.flags[2]&~12u)|(enabled?4u:0u);}
void apply_rotation(s::Animation& a,float angle){a.base.vector_38.z=angle;a.base.flags[1]|=2;}
void draw(s::Animation& a,float angle){if(angle==0.f)s::draw_axis_aligned_sprite(*pe::sprite_controller,a,false);else s::draw_rotated_sprite(*pe::sprite_controller,a);}
bool variable_width(int font){return (font>=6 && font<=9)||font==12||font==13;}
}
void set_sprite(s::Controller& c,s::Animation& a,std::int32_t index) {
    const auto& sprite=s::sprite_file(c,a).sprites[index];a.base.fields_10_28[4]=index;
    a.base.vectors_378[2].x=sprite.u0;a.base.vectors_378[0].x=a.base.vectors_378[2].x;
    a.base.vectors_378[3].x=sprite.u1;a.base.vectors_378[1].x=a.base.vectors_378[3].x;
    a.base.vectors_378[1].y=sprite.v0;a.base.vectors_378[0].y=a.base.vectors_378[1].y;
    a.base.vectors_378[3].y=sprite.v1;a.base.vectors_378[2].y=a.base.vectors_378[3].y;
    a.base.vector_398={sub(a.base.vectors_378[1].x,a.base.vectors_378[0].x),sub(a.base.vectors_378[2].y,a.base.vectors_378[0].y)};
}
void Renderer::draw_line(const Line& line) {
    auto& a=animations[0];auto& c=*pe::sprite_controller;const auto scale=pe::window_state.scale;
    const auto length=std::strlen(line.text);const auto* text=reinterpret_cast<const unsigned char*>(line.text);
    s::Vec3 offset{};a.base.flags[0]|=0x10000u;a.base.flags[3]&=~0xffu;a.base.flags[4]=a.base.flags[5]=1;
    a.base.vector_2c=line.position;a.base.flags[0]=(a.base.flags[0]&~0xff00u)|((line.blend&0xffu)<<8);
    a.base.vector_50={line.scale_x,line.scale_y};a.base.flags[1]|=4;
    float width,height;
    switch(line.font) {
    case 1:filter(a,false);width=n::mul32(6.f,line.scale_x);height=n::mul32(9.f,line.scale_y);break;
    case 2:case 3:filter(a,line.font==3);width=n::mul32(7.f,line.scale_x);height=n::mul32(10.f,line.scale_y);break;
    case 4:case 5:case 10:case 11:filter(a,line.font==5);width=n::mul32(12.f,line.scale_x);height=n::mul32(16.f,line.scale_y);break;
    case 6:case 8:filter(a,line.scale_x!=1.f);width=n::mul32(div(24.f,2.f),line.scale_x);height=n::mul32(div(32.f,2.f),line.scale_y);break;
    case 7:case 9:filter(a,line.scale_x!=1.f);width=n::mul32(div(35.f,2.f),line.scale_x);height=n::mul32(div(47.f,2.f),line.scale_y);break;
    case 12:case 13:filter(a,line.scale_x!=1.f);width=n::mul32(div(14.f,2.f),line.scale_x);height=n::mul32(div(23.f,2.f),line.scale_y);break;
    default:filter(a,line.scale_x!=1.f);width=n::mul32(static_cast<float>(font_width),line.scale_x);height=n::mul32(14.f,line.scale_y);break;
    }
    if(line.align_x==0 || line.align_x==2) {
        const bool centered=line.align_x==0;
        if(line.font>=2 && line.font<=5 || line.font==10 || line.font==11) {
            float distance=0;for(auto* character=text;*character;++character) {
                const auto punctuation=(line.font==2||line.font==3)?'.':',';
                const float advance=*character==punctuation?n::mul32(-4.f,line.scale_x):neg(width);
                const float adjusted=centered?div(advance,2.f):advance;
                distance=n::add32(n::mul32(adjusted,scale),distance);
            }offset.x=distance;
        } else if(variable_width(line.font)) {
            float distance=0;for(auto* character=text;*character;++character) {
                int index;if(line.font==12||line.font==13)index=*character<32?0x349-*character:*character+0x2a8;
                else index=int(*character)-0x20+((line.font==6||line.font==8)?0x140:0x204);
                set_sprite(c,a,index);distance=n::add32(div(s::current_sprite(c,a).extent_4c,sub(3.f,scale)),distance);
            }
            offset.x=n::mul32(neg(distance),line.scale_x);if(centered)offset.x=div(offset.x,2.f);
        } else {
            offset.x=n::mul32(neg(static_cast<float>(static_cast<std::int32_t>(length))),width);
            if(centered)offset.x=div(offset.x,2.f);offset.x=n::mul32(offset.x,scale);
        }
    }
    if(line.align_y==0)offset.y=n::mul32(div(neg(height),2.f),scale);
    else if(line.align_y==2)offset.y=n::mul32(neg(height),scale);
    const auto beginning=offset; s::Vec3 rotated{};
    for(auto* character=text;*character;++character) {
        s::Vec3 glyph_offset{};
        if(*character==10){offset.x=beginning.x;offset.y=n::add32(n::mul32(n::mul32(height,line.scale_y),scale),offset.y);continue;}
        if(*character==32 && !(line.font>=6 && line.font<=9)){offset.x=n::add32(n::mul32(width,scale),offset.x);continue;}
        int index=-1;
        switch(line.font) {
        case 0:index=*character-0x20;break;
        case 1:index=*character+0x42;break;
        case 2:case 3:
            width=n::mul32(7.f,line.scale_x);
            if(*character>='a' && *character<='z')index=*character+0x74;
            else if(*character>='A' && *character<='Z')index=*character+0x94;
            else switch(*character) {
                case '/':index=0xce;break;case ':':index=0xcf;break;case '-':index=0xd0;break;case '*':index=0xd1;break;
                case '%':index=0xd2;break;case '$':index=0x11f;break;case '.':index=0xd3;width=n::mul32(4.f,line.scale_x);break;
                case '+':index=0xd4;break;default:index=*character+0x94;break;
            }break;
        case 4:case 5:case 10:case 11: {
            const int base=line.font==10?0x10d:(line.font==11?0xfe:0xef);width=n::mul32(12.f,line.scale_x);
            switch(*character){case '/':index=base+10;break;case '.':index=base+11;break;case 's':index=base+12;break;case '*':index=base+13;break;
            case ',':index=base+14;width=n::mul32(4.f,line.scale_x);glyph_offset.y=n::mul32(3.f,scale);break;default:index=*character-0x30+base;break;}break;}
        case 6:index=*character+0x120;break;case 7:index=*character+0x1e4;break;case 8:index=*character+0x182;break;case 9:index=*character+0x246;break;
        case 12:index=*character<32?0x349-*character:*character+0x2a8;break;
        case 13:index=*character<32?0x3ad-*character:*character+0x30c;break;
        }
        if(index!=-1)set_sprite(c,a,index);
        const auto& sprite=s::current_sprite(c,a);
        if(variable_width(line.font)) {
            width=n::mul32(div(div(sprite.extent_4c,sub(3.f,scale)),scale),line.scale_x);
            a.base.vector_70={n::mul32(div(sprite.extent_4c,2.f),scale),n::mul32(div(sprite.extent_48,2.f),scale)};
        }else a.base.vector_70={sprite.extent_4c,sprite.extent_48};
        a.base.flags[1]|=4;
        glyph_offset.x=n::add32(glyph_offset.x,offset.x);glyph_offset.y=n::add32(glyph_offset.y,offset.y);glyph_offset.z=n::add32(glyph_offset.z,offset.z);
        // Original458fa0 writes x/y and intentionally preserves output z.
        rotated.x=glyph_offset.x;rotated.y=glyph_offset.y;ecl::math::rotate(rotated.x,rotated.y,line.rotation);
        rotated.x=n::add32(rotated.x,line.position.x);rotated.y=n::add32(rotated.y,line.position.y);rotated.z=n::add32(rotated.z,line.position.z);
        apply_rotation(a,line.rotation);
        if(line.shadow) {
            s::set_animation_color(a,line.color&0xff000000u);a.base.field_490=(a.base.field_490&0xffffffu)|((line.color>>25)<<24);
            a.base.vector_2c.x=n::add32(n::mul32(2.f,scale),rotated.x);a.base.vector_2c.y=n::add32(n::mul32(2.f,scale),rotated.y);
            filter(a,a.base.vector_50.x==1.f);draw(a,line.rotation);
        }
        a.base.vector_2c.x=rotated.x;a.base.vector_2c.y=rotated.y;s::set_animation_color(a,line.color);draw(a,line.rotation);
        offset.x=n::add32(n::mul32(width,scale),offset.x);
    }
}
}
