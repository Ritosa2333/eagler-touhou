#include "update.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
namespace th20::source::stone_menu {
namespace {
constexpr unsigned stone_colors[]{0xffff0000,0xffff0000,0xff0000ff,0xff0000ff,0xffffff00,0xffffff00,0xff00c000,0xff00c000,0xff808080};
constexpr unsigned meter_colors[]{0xffff8080,0xffff4040,0xffc0c0ff,0xff8080ff,0xffd0d040,0xffd0d000,0xff80ff80,0xff40ff40,0xffa0a0a0};
constexpr unsigned particle_colors[]{0xf0ff1010,0xf04040ff,0xf0d0d000,0xf020ff20};
template<class T>T read(const game_session::Player& p,unsigned offset){T value;std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(&p)+offset,sizeof(value));return value;}
template<class T>void write(game_session::Player& p,unsigned offset,T value){std::memcpy(reinterpret_cast<std::uint8_t*>(&p)+offset,&value,sizeof(value));}
int clamped(game_session::Player& p,unsigned offset,int low,int high){const int value=std::clamp(read<int>(p,offset),low,high);write(p,offset,value);return value;}
void set_slot(game_session::Player& p,int slot,int value,bool committed){constexpr unsigned pending[]{0x1c,0x24,0x20,0x28},active[]{0xc,0x14,0x10,0x18};const unsigned index=slot>=1&&slot<=3?unsigned(slot):0;write(p,(committed?active:pending)[index],value);} //50a870/51c8b0
template<class T>void interpolate(sprite::Interpolation<T>& tween,int duration,int mode,const T& start,const T& end){tween.duration=duration;tween.mode=mode;tween.start=start;tween.end=end;tween.current=start;recovered::timer_set(tween.timer,0);}
void color_transition(sprite::Animation& animation,unsigned target){const auto source=animation.base.field_490;const sprite::Vec3i start{int(source&255),int(source>>8&255),int(source>>16&255)},end{int(target&255),int(target>>8&255),int(target>>16&255)};interpolate(animation.base.interpolation_e0,20,0,start,end);} //51c6f0 /4291e0 stores B,G,R
void set_handle_color(UpdateEnvironment& host,unsigned& handle,unsigned color){if(auto* animation=host.animation(handle,false))animation->base.field_490=color;} //44f9e0
void rebuild_inventory(UpdateEnvironment& host){for(unsigned i=0;i<9;++i)host.set_used_stones(i,0);for(int slot=1;slot<4;++slot)host.consume_stone(host.choices().selected_profile(slot));}
void refresh_category(StoneMenuInf& owner,UpdateEnvironment& host,bool include_panel){
    auto& animations=environment();const int category=owner.category.current;animations.interrupt(owner.animation_handles[0],category+7);animations.interrupt(owner.animation_handles[2],1);animations.interrupt(owner.animation_handles[3],1);
    if(include_panel)animations.interrupt(owner.animation_handles[4],1);
    if(category>0){owner.animation_handles[2]=host.spawn_named(*owner.file,category+15,nullptr,0);owner.animation_handles[3]=host.spawn_named(*owner.file,category+19,nullptr,0);if(include_panel)owner.animation_handles[4]=host.spawn_named(*owner.file,30,nullptr,0);}
}
void begin_selection(StoneMenuInf& owner,UpdateEnvironment& host){
    effects::Parameters parameters;effects::construct_parameters(parameters);host.immediate_interrupt(owner.animation_handles[6],owner.category.current+6);
    owner.selection_position=host.animation(owner.animation_handles[6],true)->base.vector_484;parameters.vector_00=host.animation(owner.animation_handles[6],true)->base.vector_2c;parameters.vector_0c={1,1,0};parameters.value_18=parameters.value_1c=0;parameters.value_20=0x80202020;parameters.value_24=std::bit_cast<unsigned>(192.f);
    owner.animation_handles[1]=host.spawn_effect(13,parameters,false);const sprite::Vec2 start{1,1},end{500.f,owner.category.current==1?400.f:480.f};interpolate(host.animation(owner.animation_handles[1],false)->base.interpolation_1e0,20,1,start,end);
    const auto destination=host.animation(owner.animation_handles[6],true)->base.vector_484,origin=host.animation(owner.animation_handles[6],true)->base.vector_2c;interpolate(host.animation(owner.animation_handles[1],false)->base.interpolation_8c,20,1,origin,destination);
    auto position=host.animation(owner.animation_handles[6],true)->base.vector_484;if(owner.category.current==1)position.y+=20;
    owner.animation_handles[5]=host.spawn_named(*owner.file,26,&position,0);owner.selection.count=owner.category.current==1?8:9;owner.selection.wrapping=1;owner.saved_selection=host.choices().selected_profile(owner.category.current-1);owner.selection.select(owner.saved_selection);environment().interrupt(owner.animation_handles[5],owner.selection.current+7);owner.state=4;recovered::timer_set(owner.age,0);environment().interrupt(owner.animation_handles[4],1);
}
void configure_meter_pair(StoneMenuInf& owner,UpdateEnvironment& host,int first_script,int second_script,float fraction){auto* first=host.child_animation(owner.animation_handles[7],first_script);auto* second=host.child_animation(owner.animation_handles[7],second_script);host.texture_rectangle(*first,480.f-480.f*fraction,0,480.f*fraction,16);host.texture_rectangle(*second,480.f-480.f*fraction,0,480.f*fraction,16);first->base.field_490=meter_colors[host.player().fields_00[3]];}
void begin_meters(StoneMenuInf& owner,UpdateEnvironment& host){
    owner.visible=2;effects::Parameters parameters;effects::construct_parameters(parameters);parameters.vector_00={528,350,0};parameters.value_18=parameters.value_1c=0;parameters.value_20=0xffff0000;parameters.value_24=std::bit_cast<unsigned>(96.f);owner.animation_handles[0]=host.spawn_effect(12,parameters,true);
    environment().interrupt(owner.animation_handles[0],17);owner.category.count=5;owner.category.select(0);environment().interrupt(owner.animation_handles[0],owner.category.current+7);set_handle_color(host,owner.animation_handles[0],stone_colors[host.player().fields_00[3]]);color_transition(*host.animation(owner.animation_handles[0],false),stone_colors[host.choices().selected_profile(0)]);
    owner.animation_handles[6]=host.spawn_named(*owner.file,27,nullptr,0);owner.animation_handles[7]=host.spawn_named(*owner.file,31,nullptr,0);
    configure_meter_pair(owner,host,33,35,float(clamped(host.player(),0x54,1,100))/200.f);configure_meter_pair(owner,host,34,36,float(clamped(host.player(),0x60,0,5000))/5000.f);host.child_animation(owner.animation_handles[7],32)->base.field_490=meter_colors[host.player().fields_00[3]];
    owner.state=7;recovered::timer_set(owner.age,0);
}
void update_meters(StoneMenuInf& owner,UpdateEnvironment& host){
    auto* animation=host.child_animation(owner.animation_handles[7],33);const float numerator=float(clamped(host.player(),0x4c,0,500));float fraction=numerator/float(clamped(host.player(),0x50,100,500));animation->base.vector_68.x=fraction;animation->base.flags[1]|=8;animation->base.vector_50.x=fraction;animation->base.flags[1]|=4;
    if(host.overlay_filling()&&owner.age.current%4==0){effects::Parameters parameters;effects::construct_parameters(parameters);fraction=(float(clamped(host.player(),0x54,1,100))/200.f)*fraction;parameters.vector_00.x=fraction*480.f+80.f;parameters.vector_00.y=920;parameters.vector_2c.x=(-fraction)*480.f;parameters.vector_2c.y=0;parameters.value_20=particle_colors[host.player().fields_00[3]/2];host.spawn_effect(14,parameters,false);}
    animation=host.child_animation(owner.animation_handles[7],34);const float amount=float(clamped(host.player(),0x5c,0,10000));fraction=amount/float(clamped(host.player(),0x60,0,5000));animation->base.vector_68.x=fraction;animation->base.flags[1]|=8;animation->base.vector_50.x=fraction;animation->base.flags[1]|=4;
    animation->base.field_490=(animation->base.field_490&0xff000000)|(meter_colors[host.special_color()*2]&0xffffff);
    auto* first=host.child_animation(owner.animation_handles[7],34);auto* second=host.child_animation(owner.animation_handles[7],36);const float maximum=float(clamped(host.player(),0x60,0,5000))/5000.f;host.texture_rectangle(*first,480.f-480.f*maximum,0,480.f*maximum,16);host.texture_rectangle(*second,480.f-480.f*maximum,0,480.f*maximum,16);
    if(host.special_active()&&owner.age.current%4==0){effects::Parameters parameters;effects::construct_parameters(parameters);parameters.vector_00.x=80;parameters.vector_00.y=912.f-((fraction*480.f)*float(clamped(host.player(),0x60,0,5000)))/5000.f;parameters.vector_2c.x=0;parameters.vector_2c.y=((fraction*480.f)*float(clamped(host.player(),0x60,0,5000)))/5000.f;parameters.value_20=particle_colors[host.special_color()];host.spawn_effect(14,parameters,false);}
    const auto position=host.player_position();if(!owner.field_210f8){if(position.x < -168.f&&position.y>320.f){owner.field_210f8=1;environment().interrupt(owner.animation_handles[7],5);}}
    else if(!(position.x < -160.f&&position.y>304.f)){owner.field_210f8=0;environment().interrupt(owner.animation_handles[7],4);}
}
}
int update(StoneMenuInf& owner,UpdateEnvironment& host){
    recovered::timer_tick(owner.age,state::timer_rate);
    switch(owner.state){
    case 1:case 2:
        if(owner.age.current<20)break;
        {const bool returning=owner.state==2;owner.state=3;recovered::timer_set(owner.age,0);if(returning)refresh_category(owner,host,false);}
        [[fallthrough]];
    case 3:
        owner.category.snapshot();if(host.pressed(0x10))owner.category.select(owner.category.current==4?0:1);if(host.pressed(0x20))owner.category.select(owner.category.current==1?0:4);if(host.pressed(0x40))owner.category.select(owner.category.current==3?0:2);if(host.pressed(0x80))owner.category.select(owner.category.current==2?0:3);
        if(host.pressed(0x106)){if(owner.category.current==0){hide(owner);break;}owner.category.select(0);}
        if(owner.category.changed()){host.sound(10);refresh_category(owner,host,true);recovered::timer_set(owner.age,0);}
        if(host.pressed(0x80001)){host.sound(7);if(owner.category.current==0){owner.state=5;recovered::timer_set(owner.age,0);environment().interrupt(owner.animation_handles[0],27);}else begin_selection(owner,host);}break;
    case 4:{
        owner.selection.snapshot();if(host.repeated(0x10))owner.selection.move(-1);if(host.repeated(0x20))owner.selection.move(1);
        if(owner.selection.changed()){
            host.sound(10);environment().interrupt(owner.animation_handles[5],owner.selection.current+7);
            if(!(host.choices().difficulty()==4&&owner.category.current==1&&!host.choices().extra_unlocked(owner.selection.current))){host.select_profile(owner.category.current-1,owner.selection.current);set_slot(host.player(),owner.category.current-1,owner.selection.current,false);host.refresh_overlay(host.player().fields_00[2]);environment().interrupt(owner.animation_handles[0],owner.category.current+12);color_transition(*host.animation(owner.animation_handles[0],false),stone_colors[host.player().fields_00[3]]);}
        }
        bool accepted=false;
        if(host.pressed(0x80001)){const auto used=host.choices().used_stone_count(owner.selection.current);const auto available=host.choices().stone_count(owner.selection.current);const int remaining=recovered::signed_bits(available-used);
            if(host.choices().difficulty()==4&&owner.category.current==1&&!host.choices().extra_unlocked(owner.selection.current))host.sound(16);
            else if(remaining<1&&owner.category.current!=1)host.sound(16);else{host.sound(7);accepted=true;}
        }
        if(!accepted){if(!host.pressed(0x106))break;host.sound(9);owner.selection.select(owner.saved_selection);host.select_profile(owner.category.current-1,owner.saved_selection);for(int slot=0;slot<4;++slot)set_slot(host.player(),slot,host.choices().selected_profile(slot),false);host.refresh_overlay(host.player().fields_00[2]);environment().interrupt(owner.animation_handles[0],2);}
        rebuild_inventory(host);environment().interrupt(owner.animation_handles[1],1);owner.state=1;recovered::timer_set(owner.age,0);environment().interrupt(owner.animation_handles[5],1);environment().interrupt(owner.animation_handles[0],owner.category.current+7);if(owner.category.current>0)owner.animation_handles[4]=host.spawn_named(*owner.file,30,nullptr,0);break;}
    case 5:if(owner.age.current>=8)owner.visible=3;if(owner.age.current>=40)hide(owner);break;
    case 6:if(owner.age.current<20)break;begin_meters(owner,host);[[fallthrough]];
    case 7:update_meters(owner,host);break;
    }
    return 1;
}
void open(StoneMenuInf& owner,int mode,UpdateEnvironment& host){
    if(mode==0){
        owner.visible=1;if(host.choices().difficulty()==4)while(!host.choices().extra_unlocked(host.choices().selected_profile(0)))host.select_profile(0,(host.choices().selected_profile(0)+1)%8);
        for(int slot=0;slot<4;++slot)set_slot(host.player(),slot,host.choices().selected_profile(slot),true);rebuild_inventory(host);
        effects::Parameters parameters;effects::construct_parameters(parameters);parameters.vector_00={450,240,0};parameters.value_18=parameters.value_1c=0;parameters.value_20=0xffff0000;parameters.value_24=std::bit_cast<unsigned>(192.f);owner.animation_handles[0]=host.spawn_effect(12,parameters,false);recovered::timer_set(owner.age,0);owner.category.count=5;environment().interrupt(owner.animation_handles[0],7);owner.category.select(0);set_handle_color(host,owner.animation_handles[0],stone_colors[host.choices().selected_profile(0)]);color_transition(*host.animation(owner.animation_handles[0],false),stone_colors[host.player().fields_00[3]]);owner.animation_handles[6]=host.spawn_named(*owner.file,27,nullptr,0);owner.state=1;
    }else if(mode==1){recovered::timer_set(owner.age,0);owner.state=6;}
    else if(mode==2){recovered::timer_set(owner.age,0);owner.state=8;const sprite::Vec3 position{1000,800,0};owner.animation_handles[8]=host.spawn_named(*owner.file,1,&position,4);host.immediate_interrupt(owner.animation_handles[8],4);}
}
int update(StoneMenuInf& owner){return update(owner,update_environment());}
void open(StoneMenuInf& owner,int mode){open(owner,mode,update_environment());}
}
