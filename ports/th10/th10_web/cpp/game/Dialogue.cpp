#include "Dialogue.hpp"
namespace th10 {
static void set_timer(Timer& timer,u32& flags,i32 value,float* rate){if(!(flags&1)){timer.rate=rate;flags|=1;}timer.previous=wrapping_add(value,-1);timer.current=value;timer.fractional=Extended::from_int(value).to_float();}
// 0x417010. The key increment itself increases by 16 after each byte.
char* Dialogue::decode(const u8* input,char* output) noexcept {char* begin=output;u8 key=0x77,step=7,value;do{value=*input++^key;*output++=static_cast<char>(value);key+=step;step+=16;}while(value);return begin;}
// 0x415b00 / 0x415db0. Text animations exist before the gameplay clear.
void Dialogue::initialize(MessageInstruction* script,DialogueEnvironment& env){
    std::memset(this,0,sizeof(*this));set_timer(elapsed,elapsed_flags,0,env.rate);set_timer(script_time,script_time_flags,0,env.rate);set_timer(wait,wait_flags,0,env.rate);instruction=script;
    lines[0]=env.create_animation(DialogueAnimationFile::Text,0);lines[1]=env.create_animation(DialogueAnimationFile::Text,1);
    for(auto& line:lines){env.registry->find_and_clear(line)->text_settings[0]=16;env.registry->find_and_clear(line)->text_settings[1]=16;}
    line_positions[0]={8,0,0};line_positions[1]={24,0,0};colors[0]=0xf8f08f;colors[1]=0x8088ff;env.clear_projectiles_and_enemies();
}
void Dialogue::start(i32 index,DialogueEnvironment& env){u32 offset;std::memcpy(&offset,env.gui->message_file+4+static_cast<u32>(index)*8,4);auto* dialogue=env.allocate();dialogue->initialize(reinterpret_cast<MessageInstruction*>(env.gui->message_file+offset),env);env.gui->dialogue=dialogue;dialogue->id=index;env.game->select_section(wrapping_add(index,1));}
void Dialogue::release(const AnmRegistry& registry) noexcept {for(auto& id:portraits)registry.delete_and_clear(id);registry.delete_and_clear(text_box);for(auto& id:lines)registry.delete_and_clear(id);registry.delete_and_clear(name_tag);}
// 0x415e30. The overall dialogue clock also advances while an input wait blocks
// the message clock; it stops on the frame containing the end instruction.
i32 Dialogue::tick(DialogueEnvironment& env){if(update(env))return 1;elapsed.tick();return 0;}
static u32 child(AnmRegistry& registry,u32& id,i32 script){auto* vm=registry.find_and_clear(id);if(!vm)return 0;for(auto* node=&vm->child_node;node;node=node->next)if(node->value->script_index==script)return node->value->id;return 0;}
// 0x415e90. All 24 message opcodes; wait instructions retain the current
// instruction and timer while blocked, so advancement occurs exactly once.
i32 Dialogue::update(DialogueEnvironment& env){
    if(blocking_frames>0)--blocking_frames;
    if((flags&1)&&(*env.keys&0x100))set_timer(script_time,script_time_flags,instruction->time,env.rate);
    auto& registry=*env.registry;
    auto write_line=[&](u32 line,const char* text){auto* vm=registry.find_and_clear(lines[line]);env.draw_text(vm,colors[speaker],text);};
    auto write_instruction=[&](u32 line){auto* vm=registry.find_and_clear(lines[line]);const auto* text=decode(instruction->payload(),env.decoded_text);env.draw_text(vm,colors[speaker],text);};
    while(instruction->time<=script_time.current){
        switch(instruction->opcode){
        case 0:env.game->select_section(0);return -1;
        case 1:if(env.game->character==0||env.game->character==1)portraits[0]=env.create_animation(DialogueAnimationFile::Player,29);break;
        case 2:if(env.game->character==0||env.game->character==1){if(env.game->stage==6)portraits[1]=env.create_animation(DialogueAnimationFile::Boss,32);else if(env.game->stage==7&&id<1)portraits[1]=env.create_animation(DialogueAnimationFile::Boss,37);else portraits[1]=env.create_animation(DialogueAnimationFile::Player,30);}break;
        case 3:text_box=env.create_animation(DialogueAnimationFile::Interface,90);break;
        case 4:registry.interrupt(portraits[0],1);portraits[0]=0;break;
        case 5:registry.interrupt(portraits[1],1);portraits[1]=0;registry.interrupt(name_tag,1);break;
        case 6:registry.interrupt(text_box,1);registry.interrupt(lines[0],1);registry.interrupt(lines[1],1);break;
        case 7:case 8:{const u32 active=instruction->opcode-7;registry.interrupt(portraits[1-active],3);registry.interrupt(portraits[active],2);speaker=active;registry.set_position(lines[0],line_positions[active],false);registry.set_position(lines[1],line_positions[speaker],false);next_line=0;break;}
        case 9:flags=(flags&~1u)|(instruction->argument<u8>()&1);break;
        case 10:
            if(wait.current<1)set_timer(wait,wait_flags,instruction->argument<i32>(),env.rate);wait.advance(-1.0f);
            if(!(*env.pressed&0x1001)&&wait.current>0){if(!(flags&1)||!(*env.keys&0x100))return 0;set_timer(wait,wait_flags,0,env.rate);next_line=0;}
            else{env.play_sound(0);set_timer(wait,wait_flags,0,env.rate);next_line=0;}break;
        case 11:blocking_frames=1;break;
        case 12:if(env.game->character==0||env.game->character==1){const i32 base=env.game->character==0?52:45;auto id=child(registry,portraits[0],23);if(auto* vm=registry.find(id))env.bind_sprite(*vm,DialogueAnimationFile::Player,wrapping_add(instruction->argument<i32>(),base),false);id=child(registry,portraits[0],26);if(auto* vm=registry.find(id))env.bind_sprite(*vm,DialogueAnimationFile::Player,wrapping_add(instruction->argument<i32>(),base+8),false);}break;
        case 13:if((env.game->character==0||env.game->character==1)&&env.game->stage>=1&&env.game->stage<=7){
            constexpr i32 offsets[]={15,21,34,37,22,48,46},spacing[]={8,9,9,9,9,9,9};i32 base=offsets[env.game->stage-1],first=24,second=27,gap=spacing[env.game->stage-1];if(env.game->stage==6){first=28;second=31;}else if(env.game->stage==7&&id<1){base=68;first=33;second=36;}
            auto target=child(registry,portraits[1],first);if(auto* vm=registry.find(target))env.bind_sprite(*vm,DialogueAnimationFile::Boss,wrapping_add(instruction->argument<i32>(),base),true);target=child(registry,portraits[1],second);if(auto* vm=registry.find(target))env.bind_sprite(*vm,DialogueAnimationFile::Boss,wrapping_add(instruction->argument<i32>(),base+gap),true);
        }break;
        case 14:case 15:{const u32 line=instruction->opcode-14;write_instruction(line);registry.interrupt(lines[line],2);break;}
        case 16:if(!next_line){write_line(0," ");write_line(1," ");write_instruction(0);registry.interrupt(lines[0],2);registry.interrupt(lines[1],3);++next_line;}else{write_instruction(1);registry.interrupt(lines[1],2);next_line=0;}break;
        case 17:registry.interrupt(lines[0],3);registry.interrupt(lines[1],3);break;
        case 18:env.start_music();env.create_animation(DialogueAnimationFile::MusicCaption,2);break;
        case 19:if(env.game->stage>=1&&env.game->stage<=7){constexpr i32 scripts[]={13,16,20,21,14,35,29};name_tag=env.create_animation(DialogueAnimationFile::Boss,scripts[env.game->stage-1]);}break;
        case 20:env.complete_stage();break;
        case 21:env.fade_music(env.game->stage==6?8.0f:2.0f);break;
        case 22:case 23:registry.interrupt(portraits[instruction->opcode-22],7);break;
        }
        instruction=instruction->next();
    }
    script_time.tick();return 0;
}
}
