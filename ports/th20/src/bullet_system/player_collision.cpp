#include "bullet.hpp"
#include "../player_entity/player.hpp"
#include "../player_entity/events.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../effect_system/effect.hpp"
#include "../program_entry/program_entry.hpp"
#include <cstring>
namespace th20::source::bullet {
namespace n=recovered;
namespace {
void graze_visual(sprite::Animation& a,std::uint32_t mode){a.base.flags[2]=(a.base.flags[2]&~0x1c00u)|((mode&7u)<<10);}
int clamp_graze_counter(game_session::Player& p){auto& value=p.fields_b4[0];int count=n::signed_bits(value);if(count<0)count=0;if(count>100)count=100;value=count;return count;}
void hit_animation(Bullet& b){
    b.state=3;sprite::set_animation_interrupt(*b.animation,1);auto& sprites=*program_entry::sprite_controller;
    if(sprite::resolve_animation_handle(sprites,b.animation_handle))sprite::interrupt_animation_children(sprites,b.animation_handle,1);
    if(b.cancel_script>=0){
        auto& owner=*static_cast<Controller*>(b.context->primary_owner);std::uint32_t handle;
        sprite::spawn_named_animation(sprites,*owner.file,handle,"bullet",b.cancel_script,&b.position,0,-1,0);
        auto& a=*sprite::resolve_animation_handle(sprites,handle);
        if(((b.flags>>11)&3u)==1)sprite::set_animation_interrupt(a,3);
        const sprite::Vec3 endpoint{n::mul32(n::mul32(b.velocity.x,state::clock_scale),10),n::mul32(n::mul32(b.velocity.y,state::clock_scale),10),n::mul32(n::mul32(b.velocity.z,state::clock_scale),10)};
        auto& p=a.base.interpolation_8c;p.duration=30;p.mode=6;p.start={0,0,0};p.end=endpoint;p.current=p.start;n::timer_set(p.timer,0); //4614a0/4395d0, tangents preserved
        if(((b.flags>>11)&3u)==2){effects::Parameters params;effects::construct_parameters(params);params.vector_00=b.position;params.value_20=b.handle;static_cast<effects::Controller*>(b.context->objects_04[7])->spawn(4,&params);}
    }
}
}
namespace unrecovered {
int player_hit_test_00484a20(Bullet& b,std::int32_t preview){
    auto& a=*b.animation;graze_visual(a,0);a.base.vector_2c={0,0,0};
    if(!(b.flags&2u)||!(b.size.x>0))return 0;
    assign_timer_float(b.timer_4d8,1);
    void* player=b.context->objects_04[0];sprite::Vec2 size=b.size;
    if(b.flags&64u){size.x=n::mul32(size.x,b.scale);size.y=n::mul32(size.y,b.scale);}
    const int collision=(b.flags&16u)?player_entity::collide_circle(player,b.position,size.x,preview):player_entity::collide_axis_aligned(player,b.position,size,preview);
    if(collision==0){n::timer_tick(b.timer_4c0,state::timer_rate);if(b.timer_4c0.current>=60&&b.field_4d4==0){b.field_4d4=3;n::timer_set(b.timer_4a0,0);}}
    if(collision==1&&b.field_18==0){hit_animation(b);return collision;}
    auto& sprites=*program_entry::sprite_controller;
    if(collision!=2){if(b.timer_4b0.current!=0)sprite::interrupt_animation_children(sprites,b.animation_handle,3);n::timer_set(b.timer_4b0,0);return collision;}
    if(b.field_4d4&&b.timer_4a0.current!=b.timer_4a0.previous&&b.timer_4a0.current%n::signed_bits(b.field_4d0)==0){
        player_entity::graze(player,b.position,b.handle);n::timer_set(b.timer_4a0,0);--b.field_4d4;
        auto& record=*game_session::context(0).current_player;
        if(record.byte_b0){
            ++record.fields_b4[0];clamp_graze_counter(record);
            if(clamp_graze_counter(record)>22&&b.field_18==0){
                void* visual;std::memcpy(&visual,reinterpret_cast<const std::uint8_t*>(game_session::overlay_owner(0))+0x34,sizeof(visual));
                static_cast<std::uint8_t*>(visual)[0x35]=1; //4859d0 ->4865b0, actual overlay-owned object
                const auto random=state::next(state::random_streams[0])%10;record.fields_b4[0]=random+(record.bytes_2c[3]?7u:0u);clamp_graze_counter(record);
                hit_animation(b);return collision;
            }
        }
    }
    n::timer_tick(b.timer_4a0,state::timer_rate);n::timer_tick(b.timer_4b0,state::timer_rate);n::timer_set(b.timer_4c0,0);
    if(!(b.field_90&0x200000000ull)){
        if(b.timer_4b0.current<=1)sprite::interrupt_animation_children(sprites,b.animation_handle,2);
        int color=n::signed_bits(208u-static_cast<std::uint32_t>(b.timer_4b0.current)*2u);if(color>255)color=255;if(color<96)color=96;
        graze_visual(a,1);a.base.field_494=0xffff0080u|(static_cast<std::uint32_t>(color)<<8);
        const float y=state::signed_unit(state::random_streams[1]);const float x=state::signed_unit(state::random_streams[1]);a.base.vector_2c={x,y,0};
    }
    return collision;
}
}
}
