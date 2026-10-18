#include "Dialogue.hpp"
namespace th09 {
namespace {DialogueResource portrait_resource(i32 n){return n==0?DialogueResource::left_portrait:n==1?DialogueResource::right_portrait:DialogueResource::additional_portraits;}}
void Dialogue::setup(){
    for(auto& a:animations)std::memset(&a,0,sizeof(a));animations[8].scriptIndex=animations[9].scriptIndex=-1;
    colors[0]=0xe8f0ff;colors[1]=0xf0e8ff;colors[2]=0xe8d0ff;colors[3]=0xd8ffff;for(auto& s:shadows)s=0;
    time=box_time=Timer{0,0,0};wait_frames=0;minimum_wait=6;font_size=24;counter=0;speaker=0;new_page=1;line_number=0;previous_speaker=255;skippable=box_visible=1;invalid=false;
    services.start_animation(animations[8],DialogueResource::ascii,0);services.start_animation(animations[9],DialogueResource::ascii,1);
    for(u32 i=8;i<10;++i){animations[i].fontWidth=animations[i].fontHeight=u8(font_size);services.text(animations[i],colors[0],shadows[0]," ");}
}
bool Dialogue::begin(i32 script,i32 flip){
    if(flip<0||flip>1||!resources[flip]||script<0||u32(script)>=resources[flip]->entries.size()||!resources[flip]->entries[script].offset)return false;
    current=resources[flip];cursor=current->entries[script].offset;id=script;inverted=flip;setup();return true;
}
bool Dialogue::begin_victory(i32 winner,i32 opponent,Rng& rng){
    if(winner<0||winner>1||!resources[winner])return false;const i32 script=resources[winner]->victory_script(opponent,rng);
    if(!begin(script,winner))return false;id=0;return true;
}
bool Dialogue::show_portrait(i32 index){
    if(index<0||index>3)return false;const auto resource=portrait_resource(index);const i32 base=index<2?services.characters[index].face_script+index:index;
    services.start_animation(animations[index*2],resource,base);services.start_animation(animations[index*2+1],resource,base+4);return true;
}
bool Dialogue::expression(i32 index,i32 face){
    if(index<0||index>3)return false;const auto resource=portrait_resource(index);const i32 base=index<2?services.characters[index].face_sprite:0;
    services.set_sprite(animations[index*2],resource,base+face);services.set_sprite(animations[index*2+1],resource,base+face+(index<2?9:6));return true;
}
bool Dialogue::line(i32 who,i32 n,const std::string& text){
    if(who<0||who>3||n<0||n>2)return false;auto& vm=animations[8+n];services.start_animation(vm,DialogueResource::ascii,n);vm.fontWidth=vm.fontHeight=u8(font_size);services.text(vm,colors[who],shadows[who],text);wait_frames=0;return true;
}
bool Dialogue::text_instruction(const MessageInstruction& i,u32 offset,std::string& out){if(!MessageResource::decode_text(i,offset,out)){invalid=true;return false;}return true;}
void Dialogue::advance_animations(const GameInput& input){
    for(u32 i:{0u,2u,1u,3u,4u,6u,5u,7u,8u,9u,10u})services.advance_animation(animations[i]);
    const bool skip=skippable&&(input.held&0x100);
    if(time.current<60&&skip)time.reset(60);
    if(box_time.current>=1){if(box_time.current<60&&skip)time.reset(60);box_time.tick(services.timing);}
}
i32 Dialogue::update(const GameInput& input){
    if(id<0)return -1;if(!current||invalid)return -1;if(counter)counter=wrapping_sub(counter,1);
    MessageInstruction instruction;if(!current->instruction(cursor,instruction)){invalid=true;return -1;}
    if(skippable&&(input.held&0x100))time.reset(instruction.time);
    bool halted=false;u32 budget=100000;
    while(time.current>=instruction.time){
        if(!budget--){invalid=true;return -1;}const auto& i=instruction;const auto iv=[&](u32 n){return i.integer(n);};const auto sv=[&](u32 n){return i.short_value(n);};
        switch(i.opcode){
        case 0:id=-1;return -1;
        case 1:{i32 n=sv(0);if(n<2&&inverted)n=1-n;if(!show_portrait(n)){invalid=true;return -1;}break;}
        case 2:{i32 n=sv(0);if(n<2&&inverted)n=1-n;if(!expression(n,sv(1))){invalid=true;return -1;}break;}
        case 3:{
            const i32 who=sv(0),n=sv(1);if(who<0||who>3){invalid=true;return -1;}
            if(n==0&&animations[9].scriptIndex>=0)services.text(animations[9],colors[who],shadows[who]," ");
            std::string text;if(!text_instruction(i,4,text)||!line(who,n,text)){invalid=true;return -1;}break;
        }
        case 4:
            if(!(skippable&&(input.held&0x100))){
                if(!(input.pressed&1)||wait_frames<minimum_wait){if(wait_frames<iv(0)){wait_frames=wrapping_add(wait_frames,1);halted=true;break;}new_page=1;minimum_wait=30;}
                else{new_page=1;minimum_wait=8;}
            }break;
        case 5:{const i32 n=sv(0);if(n<0||n>3||i.size<3){invalid=true;return -1;}animations[n*2].pendingInterrupt=animations[n*2+1].pendingInterrupt=u8(i.arguments[2]);break;}
        case 6:counter=wrapping_add(counter,1);break;
        case 7:services.music(iv(0)<0?-1:services.mode==GameMode::versus?services.stage_music:iv(0));break;
        case 8:services.start_animation(animations[10],DialogueResource::right_portrait,services.characters[1].attack_script+4);wait_frames=0;break;
        case 9:services.show_results();services.fade_music();break;
        case 10:halted=true;break;
        case 11:services.transition(services.mode==GameMode::versus?DialogueTransition::match_complete:services.mode==GameMode::extra||!inverted?DialogueTransition::next_stage:DialogueTransition::game_over);halted=true;break;
        case 12:services.fade_music();break;
        case 13:skippable=u8(iv(0));break;
        case 14:services.white_transition();break;
        case 15:{
            i32 who=iv(0);if(who<0){for(u32 n=0;n<4;++n)animations[n].pendingInterrupt=4;}
            else{
                if(inverted)who=1-who;if(who<0||who>3){invalid=true;return -1;}
                if(previous_speaker!=who)for(u32 n=0;n<4;++n)if(i32(n)!=who)animations[n*2].pendingInterrupt=animations[n*2+1].pendingInterrupt=4;
                animations[who*2].pendingInterrupt=animations[who*2+1].pendingInterrupt=3;previous_speaker=speaker=u8(who);
                if(iv(1)>=0&&!expression(inverted,iv(1))){invalid=true;return -1;}
                if(iv(2)>=0&&!expression(1-inverted,iv(2))){invalid=true;return -1;}
            }new_page=1;break;
        }
        case 16:{
            if(speaker>3){invalid=true;return -1;}
            if(new_page){if(animations[9].scriptIndex>=0)services.text(animations[9],colors[speaker],shadows[speaker]," ");line_number=0;}
            std::string text;if(!text_instruction(i,0,text)||!line(speaker,line_number,text)){invalid=true;return -1;}new_page=0;line_number=u8(line_number+1);break;
        }
        case 17:{
            i32 who=iv(0);if(who<2&&inverted)who=1-who;if(who<0||who>3){invalid=true;return -1;}
            if(previous_speaker!=who)for(u32 n=0;n<4;++n)if(i32(n)!=who)animations[n*2].pendingInterrupt=animations[n*2+1].pendingInterrupt=4;
            animations[who*2].pendingInterrupt=animations[who*2+1].pendingInterrupt=3;previous_speaker=u8(who);
            if(iv(1)>=0&&!expression(who,iv(1))){invalid=true;return -1;}speaker=u8(who);new_page=1;break;
        }
        case 18:box_visible=u8(iv(0));break;
        case 23:services.enter_player(iv(0));break;
        case 24:box_time.reset(1);break;
        case 25:services.show_huds();break;
        case 26:new_page=1;speaker=u8(iv(0));break;
        case 28:services.background_setting(0,iv(0));services.background_setting(1,iv(0));break;
        default:break;
        }
        if(halted)break;cursor+=4+i.size;if(!current->instruction(cursor,instruction)){invalid=true;return -1;}
    }
    if(!halted)time.tick(services.timing);advance_animations(input);return 0;
}
void Dialogue::draw(){
    if(id<0)return;
    const std::array<u32,6> order=speaker<2?std::array<u32,6>{6,7,4,5,0,1}:speaker==2?std::array<u32,6>{6,7,0,1,4,5}:std::array<u32,6>{4,5,0,1,6,7};
    for(u32 n:order)services.draw_animation(animations[n],false);services.draw_animation(animations[2],true);services.draw_animation(animations[3],true);
    const float height=box_time.current<40?box_time.value()*1.5f:60.f;
    if(height>0&&box_visible){const float bottom=364.f+height;const AttackColorVertex quad[]={{{64,364,0},1,0xd0000000},{{576,364,0},1,0xd0000000},{{64,bottom,0},1,0x90000000},{{576,bottom,0},1,0x90000000}};services.draw_panel(quad,4);}
    for(u32 n=8;n<11;++n)services.draw_animation(animations[n],false);
}
}
