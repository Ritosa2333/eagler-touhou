#include "draw.hpp"
namespace th20::source::stone_menu {
namespace {
constexpr unsigned colors[]{0xffff8080,0xffff4040,0xffc0c0ff,0xff8080ff,0xffd0d040,0xffd0d000,0xff80ff80,0xff40ff40,0xffa0a0a0};
constexpr const char* quantities[]{"\x82O","\x82P","\x82Q","\x82R","\x82S","\x82T","\x82U","\x82V","\x82W","\x82X"};
void divide(sprite::Vec3& value,float divisor){value.x/=divisor;value.y/=divisor;value.z/=divisor;}
void restore_text(DrawEnvironment& host){host.text_field(TextField::font,2);host.text_style(1,1);host.text_field(TextField::color,0xffffffff);host.text_field(TextField::shadow,0xff000000);}
}
int draw(StoneMenuInf& owner,DrawEnvironment& host){
    const int category=owner.category.current;
    if(owner.state==3){
        if(category==0){
            host.text_style(2,1);host.text_field(TextField::font,12);
            constexpr sprite::Vec3 positions[]{{420,100,0},{320,270,0},{620,270,0},{470,400,0}};
            for(int slot=0;slot<4;++slot){
                host.text_field(TextField::blend,colors[host.selected_profile(slot)]);host.text_field(TextField::shadow,0xff000000);host.text_field(TextField::color,0xffffffff);
                const int selected=host.selected_profile(slot),index=slot+host.character()*36+selected*4;
                if(slot==3)host.text_style(1,1);host.write_text(positions[slot],owner.names[index]);
            }
            host.clear_ascii_lines();
        }else{
            host.text_style(0,1);
            int selected=host.selected_profile(category-1),index=category+host.character()*36-1+selected*4;
            if(host.difficulty()==4&&category==1){selected=host.selected_profile(category-1);index=selected+72+host.character()*8;}
            host.text_field(TextField::font,16);host.text_field(TextField::color,0xffffffff);host.text_field(TextField::blend,0xff000000);host.text_field(TextField::shadow,colors[host.selected_profile(category-1)]);
            auto position=host.animation_position(owner.animation_handles[3]);divide(position,host.screen_scale()*.5f);host.set_animation_position(owner.animation_handles[4],position);divide(position,2);
            int lines=0;
            if(category==1){int nonempty=0;for(const auto& line:owner.descriptions[index])if(line[0])++nonempty;position.y=float((5-nonempty)*20)+position.y;}
            host.write_text(position,owner.names[index]);host.text_field(TextField::font,3);host.text_field(TextField::blend,0xffffffff);host.text_field(TextField::shadow,0xff000000);position.y+=40;
            if(host.selected_profile(category-1)==8){
                // The generic option borrows its first two descriptions from
                // the selected MAIN stone (slot0), then adds its own text.
                const int actual=host.selected_profile(0),description_index=category+host.character()*36-1+actual*4;
                if(owner.descriptions[description_index][0][0])host.write_text(position,owner.descriptions[description_index][0]);position.y+=20;
                if(owner.descriptions[description_index][2][0])host.write_text(position,owner.descriptions[description_index][2]);position.y+=40;
                host.text_field(TextField::blend,0xffc0c0c0);
                const int count=category==4?2:1;
                for(int line=0;line<count;++line)if(owner.age.current>line){if(owner.descriptions[index][line][0])host.write_text(position,owner.descriptions[index][line]);position.y+=20;}
                lines=category==4?5:4;
            }else{
                for(int line=0;line<5;++line)if(owner.age.current>line){if(owner.descriptions[index][line][0]){host.write_text(position,owner.descriptions[index][line]);++lines;}position.y+=20;}
            }
            restore_text(host);host.set_animation_scale(owner.animation_handles[4],192.f/2.f,(float(lines)*40.f+80.f)/2.f);
        }
    }else if(owner.state==4&&owner.age.current>20&&category!=0){
        auto position=owner.selection_position;
        if(category!=1){
            host.text_style(2,1);host.text_field(TextField::font,0);host.text_field(TextField::color,0xffa0a0a0);host.text_field(TextField::blend,0xffffffff);
            position.x+=240;position.y-=226;divide(position,2);host.text_field(TextField::shadow,0xc0000000);host.write_text(position,"\x8f\x8a\x8e\x9d\x90\x94");position=owner.selection_position;
        }
        position.x-=240;position.y-=float(210-(category==1?20:0));divide(position,2);
        int name_index=host.character()*36;
        for(unsigned index=0;index<unsigned(category==1?8:9);++index){
            const unsigned available=host.stone_count(index),used=host.used_stone_count(index);
            int count=recovered::signed_bits(available-used);host.text_style(1,1);host.text_field(TextField::font,5);if(count>9)count=9;
            if(static_cast<unsigned>(owner.saved_selection)==index)host.text_field(TextField::color,0xffffffa0);
            else if(count==0&&category!=1&&index!=8)host.text_field(TextField::color,0xff606060);
            else host.text_field(TextField::color,0xffffffff);
            if(host.difficulty()==4&&category==1&&!host.extra_unlocked(index))host.text_field(TextField::color,0xff606060);
            host.text_field(TextField::blend,0xffffffff);host.text_field(TextField::shadow,0xc0000000);host.write_text(position,owner.names[name_index]);position.x+=224;
            if(category==1)host.write_text(position,"\x81[");else if(index==8)host.write_text(position,"\x81\x87");else host.write_text(position,quantities[count]);
            position.x-=224;position.y+=24;name_index+=4;
        }
        restore_text(host);
    }
    return 1;
}
int draw(StoneMenuInf& owner){return draw(owner,draw_environment());}
}
