// Naming and recovered algorithms informed by GensokyoClub/th08 (MIT).
#include "Dialogue.hpp"
#include "ContentAccess.hpp"
#include "Presentation.hpp"
#include <utility>
namespace th08 {
namespace {
template<class T>T arg(const u8* p){T value;std::memcpy(&value,p,sizeof(value));return value;}
constexpr i32 songs[9][3]={{1,2,0},{3,4,0},{5,6,0},{7,8,0},{7,9,0},{10,11,0},{12,13,15},{12,14,15},{16,17,0}};
constexpr i32 clear_bonuses[]={1000000,1500000,2000000,2500000,2500000,3000000,4000000,6000000,6660000};
}
bool Dialogue::load(const u8* bytes,u32 size){release();if(!program.load(bytes,size))return false;state.file=program.bytes();state.message=-1;state.instruction=nullptr;return true;}
void Dialogue::release(){program.clear();state.file=nullptr;presentation_valid=false;}
void Dialogue::snapshot_presentation(){
    const auto sample=[](const AnmVm& vm){return PresentationVm{vm.pos,vm.pos2,vm.scriptIndex,vm.visible};};
    for(u32 i=0;i<4;++i)presentation_portraits[i]=sample(state.portraits[i]);for(u32 i=0;i<2;++i){presentation_lines[i]=sample(state.lines[i]);presentation_intro[i]=sample(state.intro[i]);}
    presentation_timer=state.timer.value().to_float();presentation_message=state.message;presentation_valid=true;
}
AnmVm Dialogue::presentation_vm(const AnmVm& source,const PresentationVm& before)const{
    AnmVm draw=source;if(!presentation::active||!presentation_valid||presentation_message!=state.message||before.script!=source.scriptIndex||before.visible!=source.visible)return draw;
    const float dx=source.pos.x-before.pos.x,dy=source.pos.y-before.pos.y;if(dx*dx+dy*dy<16384.0f)draw.pos={presentation::lerp_world(before.pos.x,source.pos.x),presentation::lerp_world(before.pos.y,source.pos.y),presentation::lerp_world(before.pos.z,source.pos.z)};
    const float ox=source.pos2.x-before.pos2.x,oy=source.pos2.y-before.pos2.y;if(ox*ox+oy*oy<16384.0f)draw.pos2={presentation::lerp_world(before.pos2.x,source.pos2.x),presentation::lerp_world(before.pos2.y,source.pos2.y),presentation::lerp_world(before.pos2.z,source.pos2.z)};
    return draw;
}
bool Dialogue::start(AnmVm& vm,AnmLoaded* file,i32 script){
    if(!file||script<0||u32(script)>=file->scriptCount)return false;vm.scriptIndex=i16(script);executor.start(*file,vm,file->scripts[script]);return true;
}
bool Dialogue::sprite(AnmVm& vm,AnmLoaded* file,i32 index){if(!file||index<0||u32(index)>=file->spriteCount)return false;file->SetSprite(&vm,index);return true;}
bool Dialogue::read(i32 index){
    const auto* file=state.file;std::memset(&state,0,sizeof(state));state.file=file;
    if(index==0){
        if(context.stage==5)actions.copy_enemy_name(22);
        else if(context.stage==6)context.background_state=2;
        else if(context.stage==7||context.stage==8){std::swap(context.faces[2],context.faces[3]);context.background_state=2;actions.copy_enemy_name(context.stage==7?24:25);}
    }else if(index==10){
        if(context.stage==5){
            index=1;
            if(default_all_content_unlocked&&!(context.flags&8))index=3;
            else if(!globals.retries){
                if(context.flags&8){if(context.replay_clear==2)index=3;else if(context.replay_clear==1)index=2;}
                else if(context.character>=0&&context.character<12){const auto& record=context.clears[context.character];bool cleared_a=false,cleared_b=context.character>3;
                    for(i32 d=0;d<4;++d){cleared_a|=bool(record.with_retries[d]&64);cleared_b|=bool(record.without_retries[d]&128);}index=cleared_b?3:cleared_a?2:1;}
            }
            state.selected=index!=1;context.flags=(context.flags&~0x1800u)|(u32(state.selected)<<11);
        }
    }else if(index>=6&&context.stage==7&&globals.clock_time>=12)index=5;
    state.message=index;state.instruction=program.entry(index);if(!state.instruction){invalid();return false;}
    state.lines[0].scriptIndex=state.lines[1].scriptIndex=-1;state.textbox_visible=true;state.font_size=15;
    const u32 colors[]={0xe8f0ff,0xf0e8ff,0xffe8f0,0xffe8f0};std::memcpy(state.colors,colors,sizeof(colors));
    state.skippable=true;state.minimum_wait=6;state.reset_lines=true;state.portrait=255;
    actions.clear_bullets();actions.despawn_enemies();actions.collect_items();return true;
}
bool Dialogue::configure(i32 portrait){
    if(portrait<0||portrait>=4)return false;
    if(state.portrait!=portrait)for(i32 i=0;i<4;++i)state.portraits[i].pendingInterrupt=state.portrait==i&&state.portrait/2!=portrait/2?6:4;
    state.portraits[portrait].pendingInterrupt=3;state.portrait=state.color=u8(portrait);state.reset_lines=true;return true;
}
void Dialogue::erase_second(i32 color){if(color>=0&&color<4&&state.lines[1].scriptIndex>=0)text_writer.draw(state.lines[1],TextAlignment::Left,state.colors[color],state.outline_colors[color]," ");}
bool Dialogue::line(i32 index,i32 color,const u8* encoded,u32 size){
    // Skipping a wait does not reset the original line cursor. Several stock
    // scripts therefore use the following intro VMs as a third/fourth line.
    if(index<0||index>=4||color<0||color>=4)return false;auto& vm=index<2?state.lines[index]:state.intro[index-2];if(!start(vm,context.text,index))return false;
    vm.fontWidth=vm.fontHeight=u8(state.font_size);char text[256];if(!MessageProgram::text(encoded,size,text,sizeof(text)))return false;
    text_writer.draw(vm,TextAlignment::Left,state.colors[color],state.outline_colors[color],text);state.paused_frames=0;return true;
}
bool Dialogue::stage_results(){
    if(context.stage<0||context.stage>=9)return false;
    display.clear_power=Scalar::truncate(globals.power);display.clear_points=globals.points_stage;display.clear_time=globals.time_orbs;display.clear_graze=globals.graze_stage;
    display.clear_clock_old=i32(globals.clock_time)*30+660;display.clock_increment=context.stage<6?(globals.time_orbs>=globals.last_spell_requirement?1:2):context.stage<8?0:4;values.add_clock(i8(display.clock_increment));
    display.clear_stage=clear_bonuses[context.stage];display.clear_clock=i32(globals.clock_time)*30+660;display.clear_clock_display=display.clear_clock_old;display.clear_clock_delay=0;display.clear_frames=1;context.flags|=512;
    const bool ending=context.stage==6||context.stage==7||context.stage==8;
    if(!ending){if(!start(display.stage_rank,context.ascii,3)||!sprite(display.stage_rank,context.ascii,display.clock_increment+128))return false;}else display.stage_rank.currentInstruction=nullptr;
    display.stage_rank.pendingInterrupt=1;
    if(!ending){
        if(!start(display.loading_portrait,gui.loading_portrait,0)||!start(display.arcade,context.capture,1)||!display.arcade.loadedSprite)return false;actions.capture_arcade(*display.arcade.loadedSprite);
        for(u32 i=0;i<8;++i){auto& vm=display.arcade_blur[i];if(!start(vm,context.capture,2))return false;vm.counterVar0=i*4+3;vm.color1.a=u8(64-i*2);}
    }else globals.point_extends=0xffffffff;
    if(!ending&&Scalar::truncate(globals.bombs)<3&&(context.character==3||context.character==10||context.character==11)){values.add_bombs(1);actions.sound(35);gui.flags.bombs=2;}
    return true;
}
i32 Dialogue::update(){
    if(state.message<0)return -1;
    snapshot_presentation();
    if(state.ignore_wait)--state.ignore_wait;MessageInstruction instruction;if(!program.instruction(state.instruction,instruction))return invalid();
    if(state.skippable&&(context.input&256))state.timer.set(instruction.time);
    if(context.player_state!=2)actions.collect_items();
    for(u32 budget=0;;++budget){
        if(budget>=65536||!program.instruction(state.instruction,instruction))return invalid();
        if(state.timer.current<instruction.time)break;const u8* args=state.instruction+4;
        switch(instruction.opcode){
        case MessageOpcode::Delete:state.message=-1;return -1;
        case MessageOpcode::ConfigureAll:
            if(!configure(arg<i32>(args)))return invalid();for(i32 i=0;i<4;++i){const i32 index=arg<i32>(args+4+i*4);if(index>=0&&!sprite(state.portraits[i],context.faces[i],index))return invalid();}break;
        case MessageOpcode::ConfigurePortrait:{const i32 portrait=arg<i32>(args),index=arg<i32>(args+4);if(!configure(portrait)||(index>=0&&!sprite(state.portraits[portrait],context.faces[portrait],index)))return invalid();break;}
        case MessageOpcode::PortraitScript:case MessageOpcode::PortraitSprite:{
            const i32 portrait=arg<i16>(args),index=arg<i16>(args+2);if(portrait<0||portrait>=4)return invalid();auto& vm=state.portraits[portrait];auto* file=context.faces[portrait];
            if(instruction.opcode==MessageOpcode::PortraitScript){if(!start(vm,file,index)||!vm.loadedSprite)return invalid();vm.pos2.x=vm.loadedSprite->widthPx>128?-112:0;}
            else {if(!sprite(vm,file,index)||!vm.loadedSprite)return invalid();if(vm.loadedSprite->widthPx>256){vm.pos2.x=-208;vm.pos2.y=-50;}else vm.pos2.x=vm.loadedSprite->widthPx>128?-80:0;}break;
        }
        case MessageOpcode::Text:{const i32 color=arg<i16>(args),index=arg<i16>(args+2);if(index==0)erase_second(color);if(!line(index,color,args+4,instruction.size-4))return invalid();break;}
        case MessageOpcode::SpeakerText:
            if(state.reset_lines){erase_second(state.color);state.next_line=0;}if(!line(state.next_line,state.color,args,instruction.size))return invalid();state.reset_lines=false;++state.next_line;break;
        case MessageOpcode::TopLine:case MessageOpcode::BottomLine:if(!line(instruction.opcode==MessageOpcode::BottomLine,0,args,instruction.size))return invalid();break;
        case MessageOpcode::Selection:
            if(pressed(16)){if(state.selected==1)actions.sound(12);state.selected=0;}
            if(pressed(32)){if(state.selected==0)actions.sound(12);state.selected=1;}
            if(state.selected>1)return invalid();state.lines[state.selected].color1.d3dColor=-1;state.lines[1-state.selected].color1.d3dColor=i32(0xe0606060u);
            if(!pressed(1)||state.paused_frames<60){if(state.paused_frames>=arg<i32>(args)){state.reset_lines=true;state.minimum_wait=30;break;}state.paused_frames=wrapping_add(state.paused_frames,1);goto animate;}actions.sound(10);break;
        case MessageOpcode::ReadSelected:context.flags=(context.flags&~0x1800u)|(u32(state.selected&3)<<11);if(!read(state.selected+1))return -1;continue;
        case MessageOpcode::Wait:{
            if(!state.skippable||!(context.input&256)){
                if(!pressed(1)||state.paused_frames<state.minimum_wait){if(state.paused_frames>=arg<i32>(args)){state.reset_lines=true;state.minimum_wait=30;break;}state.paused_frames=wrapping_add(state.paused_frames,1);goto animate;}
                state.reset_lines=true;state.minimum_wait=8;
            }break;}
        case MessageOpcode::PortraitInterrupt:{const i32 index=arg<i16>(args);if(index<0||index>=4)return invalid();state.portraits[index].pendingInterrupt=args[2];break;}
        case MessageOpcode::ResumeEcl:++state.ignore_wait;break;
        case MessageOpcode::Music:{const i32 index=arg<i32>(args);if(index<0)actions.stop_audio();else{
            if(index>=3||context.stage<0||context.stage>=9)return invalid();if(!start(display.stage_text[3],gui.stage_text,3)||!sprite(display.stage_text[3],gui.stage_text,index+3))return invalid();
            const i32 song=songs[context.stage][index];if(actions.play_music(index,song))actions.play_audio(context.song_paths[index],song);
        }break;}
        case MessageOpcode::Intro:if(!start(state.intro[0],context.faces[2],1))return invalid();state.paused_frames=0;break;
        case MessageOpcode::StageResults:if(!stage_results())return invalid();break;
        case MessageOpcode::Halt:goto animate;
        case MessageOpcode::FadeMusic:actions.fade_music(4);break;
        case MessageOpcode::FadeScreen:actions.fade_screen(442,0xffffff,21);context.hud_redraw=442;break;
        case MessageOpcode::StageEnd:if(context.stage>=6&&context.stage<=8)context.flags=(context.flags&~0x60u)|0x40;goto animate;
        case MessageOpcode::Skippable:state.skippable=args[0]!=0;break;
        case MessageOpcode::TextboxVisible:state.textbox_visible=args[0]!=0;break;
        default:break;
        }
        state.instruction+=4+instruction.size;
    }
    state.timer.tick(executor.timing);
animate:
    for(auto& vm:state.portraits)executor.execute(vm);for(auto& vm:state.lines)executor.execute(vm);for(auto& vm:state.intro)executor.execute(vm);
    if(state.timer.current<60&&state.skippable&&(context.input&256))state.timer.set(60);return 0;
}
i32 Dialogue::draw(){
    if(state.message<0)return -1;
    float timer=state.timer.value().to_float();if(presentation::active&&presentation_valid&&presentation_message==state.message)timer=presentation::lerp_world(presentation_timer,timer);
    const float height=timer<60?(number(timer)*number(48)/number(60)).to_float():48;
    AnmVm portraits[4];for(u32 i=0;i<4;++i)portraits[i]=presentation_vm(state.portraits[i],presentation_portraits[i]);
    for(i32 first:{0,2}){auto& a=portraits[first];auto& b=portraits[first+1];if(a.pos.z>=b.pos.z){renderer.draw_no_rotation(a);renderer.draw_no_rotation(b);}else{renderer.draw_no_rotation(b);renderer.draw_no_rotation(a);}}
    renderer.flush();if(state.textbox_visible)renderer.draw_dialogue_background(Scalar::add(context.arcade_x,16),384,(number(context.arcade_x)+number(384)-number(16)).to_float(),Scalar::add(384,height));
    for(u32 i=0;i<2;++i){auto vm=presentation_vm(state.lines[i],presentation_lines[i]);renderer.draw_no_rotation(vm);}for(u32 i=0;i<2;++i){auto vm=presentation_vm(state.intro[i],presentation_intro[i]);renderer.draw_no_rotation(vm);}return 0;
}
}
