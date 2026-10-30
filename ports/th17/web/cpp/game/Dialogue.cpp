#include "Dialogue.hpp"
#include "AnmCoordinates.hpp"
#include <algorithm>
#include <cstdlib>
namespace th17 {
bool Dialogue::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
AnmVm* Dialogue::find(u32& handle){auto* vm=animations.registry.find(handle);if(!vm)handle=0;return vm;}
bool Dialogue::interrupt(u32 handle,i32 label,bool immediate){return check(animations.interrupt(handle,label,immediate),"Dialogue animation interrupt failed");}
bool Dialogue::text(u32 handle,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& value){auto* vm=animations.registry.find(handle);if(!check(vm!=nullptr,"Dialogue text animation unavailable"))return false;if(!check(host.dialogue_text(*vm,color,shadow,style,x,y,value),"Dialogue text upload unavailable"))return false;vm->visual.flags|=1;return true;}
i32 Dialogue::bank(i32 index)const noexcept{return u32(index)<resources.enemy.size()?resources.enemy[index]:-1;}
u32 Dialogue::create(i32 resource,i32 script,const Vec3& position){const auto handle=animations.create(resource,script,-1,0,position);check(handle!=0,"Dialogue animation creation failed");return handle;}
Dialogue::~Dialogue(){clear();}
bool Dialogue::clear(){bool ok=animations.retire(player_portrait);for(auto& h:boss_portraits)ok=animations.retire(h)&&ok;ok=animations.retire(name)&&ok;for(auto& h:text_handles)ok=animations.retire(h)&&ok;ok=animations.retire(boss_label)&&ok;ok=animations.retire(bubble)&&ok;initialized=false;return check(ok,"Dialogue animation retirement failed");}
bool Dialogue::initialize(const MessageResource& file,i32 entry){
    if(initialized)return check(false,"Dialogue already active");if(entry<0||u32(entry)>=file.scripts.size())return check(false,"Dialogue entry outside message resource");script=&file.scripts[entry];cursor=0;id=entry;age.set(0);script_age.set(0);wait.set(0);
    for(u32 i=0;i<4;i++){text_handles[i]=create(resources.text,i<2?0:1);if(!text_handles[i])return false;if(i&1)if(!interrupt(text_handles[i],7,true))return false;auto* vm=find(text_handles[i]);vm->visual.render_flags|=0x1000;}
    if(!check(host.clear_dialogue_field(),"Dialogue field clear unavailable"))return false;initialized=true;return true;
}
void Dialogue::reset_text(i32 value){speaker=value;for(auto& h:text_handles)if(auto* vm=find(h))vm->visual.child_anchor.y=0;flags&=~2u;line=cleared=0;}
bool Dialogue::speaking(i32 value,i32 portrait){
    speaker=value;if(speaker==0){for(auto handle:boss_portraits)if(!interrupt(handle,3,true))return false;if(!interrupt(player_portrait,2,true)||!interrupt(name,2))return false;}
    else {if(!interrupt(player_portrait,3,true))return false;for(u32 i=0;i<4;i++)if(!interrupt(boss_portraits[i],speaker==1&&i==u32(portrait)?2:3,true))return false;if(!interrupt(name,3))return false;}
    reset_text(value);return true;
}
void Dialogue::position_text(){if(speaker==0){if(auto* vm=find(text_handles[0]))vm->visual.translation=position;}else if(speaker==1||speaker==2)for(auto& h:text_handles)if(auto* vm=find(h))vm->visual.translation=position;}
bool Dialogue::child_width(float value){auto* root=find(bubble);if(!root)return true;for(i32 index=0;;index++){auto* child=animations.registry.find_child_script(*root,-1,index);if(!child)break;child->variables.floats[0]=value;}return true;}
bool Dialogue::balloon(i32 style,float value){if(!animations.retire(bubble))return false;bubble=create(resources.front,287+style,{position.x,position.y,0});if(!bubble)return false;bubble_style=style;return child_width(value);}
bool Dialogue::lines(const std::string& value){
    const bool second=line!=0;const u32 index=second?1:0,annotation=second?3:2;
    if(!second&&!cleared){width=0;for(u32 i=0;i<4;i++)if(!text(text_handles[i],colors[speaker],0,(flags>>1)&1,0,i>=2?1:0,"  "))return false;cleared=1;for(auto h:text_handles)if(!interrupt(h,3))return false;}
    if(!value.empty()&&value[0]=='|'){
        const auto first=value.find(','),last=first==std::string::npos?first:value.find(',',first+1);if(first==std::string::npos||last==std::string::npos)return check(false,"Invalid dialogue annotation");
        const i32 x=std::atoi(value.c_str()+1),y=std::atoi(value.c_str()+first+1);auto* vm=find(text_handles[annotation]);if(!vm)return check(false,"Dialogue annotation animation unavailable");vm->visual.render_flags|=0x1000;
        if(!text(text_handles[annotation],0,0xa0a0a0,2,x,y,value.substr(last+1)))return false;vm->visual.translation=position;return interrupt(text_handles[annotation],2,!second);
    }
    const u32 rounded=(u32(value.size())*8&~15u)-28;const float requested=float(float(rounded)+float(rounded));width=std::max(width,requested);
    if(!balloon((((flags>>2)&15)+(second?8:0))*3+speaker,width)||!child_width(float(width+16.f)))return false;
    if(!text(text_handles[index],colors[speaker],0,(flags>>1)&1,0,0,value))return false;position_text();
    if(!interrupt(text_handles[index],2,true))return false;if(second)line=cleared=0;else line++;return true;
}
bool Dialogue::follow_balloon(){
    auto* root=find(bubble);if(!root)return true;auto* child=animations.registry.find_child_script(*root,bubble_style+183,0);if(!child)return true;Vec3 p;if(!check(anm_position(*child,p),"Dialogue bubble position failed"))return false;
    const float scale=float(2.f/animations.scene_environment().resolution_scale);p.x=float(p.x*scale);p.y=float(p.y*scale);
    if(speaker==0)p.x=float(p.x-36.f);else if(speaker==1){if(child->visual.scale.x>=1)p.x=float(p.x+26.f);else p.x=float(float(float(float(child->visual.scale.x+.125f)*32.f)-6.f)+p.x);}else if(speaker==2)p.x=float(float(float(float(child->visual.scale.x+.125f)*16.f)-6.f)+p.x);
    for(auto& h:text_handles)if(auto* vm=find(h))vm->visual.translation=p;return true;
}
i32 Dialogue::update(const DialogueInput& input){
    if(!initialized||!error.empty())return check(false,"Dialogue unavailable"),-1;animations.rate=input.rate;if(complete_pulse>0)complete_pulse--;
    if(input.pressed&0x201)flags|=0x40;const auto skipping=[&](){return (flags&0x41)==0x41&&(((input.held&0x200)&&input.skip_frames>19)||((input.held&1)&&input.shoot_frames>19));};
    if(cursor>=script->instructions.size())return check(false,"Dialogue instruction outside resource"),-1;
    if(skipping())script_age.set(script->instructions[cursor].time);
    while(cursor<script->instructions.size()&&script->instructions[cursor].time<=script_age.current){
        const auto& instruction=script->instructions[cursor];const auto integer=[&](u32 i=0){return instruction.argument<i32>(i);};const auto number=[&](u32 i=0){return instruction.argument<float>(i);};bool ok=true;
        const auto visual=[&](i32 row)->const StageVisualSet*{if(u32(row)>=stage.visuals.size()){check(false,"Dialogue visual row outside stage");return nullptr;}return &stage.visuals[row];};
        switch(instruction.opcode){
        case 0:return 1;
        case 1:{static constexpr i32 scripts[]={40,46,33};if(u32(resources.character)>=3)return check(false,"Dialogue character outside TH17 range"),-1;player_portrait=create(integer()==0?resources.player:bank(5),integer()==0?scripts[resources.character]:11);ok=player_portrait!=0;break;}
        case 2:{const auto* v=visual(integer());if(!v)return -1;if(u32(integer())>=boss_portraits.size())return check(false,"Dialogue portrait slot outside range"),-1;boss_portraits[integer()]=create(bank(v->portrait_bank),v->portrait_script);portrait_mode=0;ok=boss_portraits[integer()]!=0;break;}
        case 4:ok=interrupt(player_portrait,1);player_portrait=0;break;
        case 5:{const i32 index=integer();if(u32(index)>=boss_portraits.size())return check(false,"Dialogue portrait slot outside range"),-1;ok=interrupt(boss_portraits[index],1)&&interrupt(boss_label,1);boss_portraits[index]=0;break;}
        case 6:for(auto h:text_handles)ok=interrupt(h,1)&&ok;ok=animations.retire(bubble)&&ok;break;
        case 7:ok=speaking(0);break;
        case 8:ok=speaking(1,integer());break;
        case 9:ok=speaking(2);break;
        case 10:flags=(flags&~1u)|(u32(integer())&1);break;
        case 11:{if(wait.current<1){wait.current=integer();wait.fractional=float(integer());if(!(wait.flags&1)){wait.rate_index=0;wait.flags|=1;}}wait.decrement(&input.rate);
            if(!(input.pressed&0x80001)&&wait.current>0){if(!skipping()){if(!follow_balloon())return -1;age.tick(&input.rate);return 0;}}
            else ok=host.dialogue_sound(0);wait.set(0);line=cleared=0;break;}
        case 12:complete_pulse=1;break;
        case 13:ok=interrupt(player_portrait,integer()+17,true);break;
        case 14:{const i32 index=integer(1);if(u32(index)>=boss_portraits.size())return check(false,"Dialogue portrait slot outside range"),-1;ok=interrupt(boss_portraits[index],integer()+17,true);break;}
        case 15:case 16:{const u32 index=instruction.opcode==15?0:1;ok=text(text_handles[index],colors[speaker],0,((flags>>1)&1)+4,0,0,instruction.text)&&interrupt(text_handles[index],2);break;}
        case 17:ok=lines(instruction.text);break;
        case 18:ok=animations.retire(bubble);for(auto h:text_handles)ok=interrupt(h,3)&&ok;break;
        case 19:ok=host.dialogue_boss_music()&&create(resources.logo,2)!=0;break;
        case 20:{const auto* v=visual(integer());if(!v)return -1;boss_label=create(bank(v->name_bank),v->name_script);ok=boss_label!=0&&host.dialogue_boss_name();break;}
        case 21:ok=host.dialogue_next_stage();break;
        case 22:case 27:{float seconds=instruction.opcode==22?(stage.stage==6?2.f:8.f):number();if(input.rate!=0&&input.rate<=1)seconds=float(seconds/input.rate);ok=host.dialogue_music_fade(truncate_int(seconds));break;}
        case 23:ok=interrupt(player_portrait,7);break;
        case 24:ok=interrupt(boss_portraits[0],7)&&interrupt(boss_portraits[1],7);break;
        case 25:for(auto& h:text_handles)if(auto* vm=find(h))vm->visual.child_anchor.y=float(integer());break;
        case 26:flags|=2;break;
        case 28:position.x=float(number()+number());position.y=float(number(1)+number(1));break;
        case 29:flags=(flags&~0x3cu)|(u32(integer())<<2&0x3c);break;
        case 31:{const auto& v=stage.visuals[1];boss_portraits[1]=create(bank(v.portrait_bank),v.portrait_script);portrait_mode=0;ok=boss_portraits[1]!=0;break;}
        case 32:ok=interrupt(name,3);reset_text(integer());break;
        case 33:case 34:{const i32 label=instruction.opcode==33?3:2;if(!integer())ok=interrupt(player_portrait,label,true);else {const i32 index=integer(1);if(u32(index)>=boss_portraits.size())return check(false,"Dialogue portrait slot outside range"),-1;ok=interrupt(boss_portraits[index],label,true);}break;}
        case 35:{const auto h=create(resources.front,60);ok=h&&host.dialogue_notice(h);break;}
        default:break; // The original dispatcher advances unknown message opcodes.
        }if(!check(ok,"Dialogue scene service failed"))return -1;cursor++;
    }
    script_age.tick(&input.rate);if(!follow_balloon())return -1;age.tick(&input.rate);return 0;
}
}
