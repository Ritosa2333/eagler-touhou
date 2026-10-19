// Pause and retry state transitions from TH08 1.00d, checked against the
// original functions and the MIT GensokyoClub/th08 reference.
#include "UiMenus.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
namespace th08 {
namespace {
constexpr u16 escape=8,up=16,down=32,quit=512,restart=16384,select=4097;
constexpr u32 normal=0xff505050,selected=0xffff8080;
void choose(AnmVm& a,AnmVm& b,bool first,u32 highlight=selected){
    a.color1.d3dColor=first?highlight:normal;b.color1.d3dColor=first?normal:highlight;
    a.pos2=first?Vec3{-4,-4,0}:Vec3{};b.pos2=first?Vec3{}:Vec3{-4,-4,0};
}
void interrupts(AnmVm* v,i32 begin,i32 end,i16 value){for(i32 i=begin;i<end;++i)v[i].pendingInterrupt=value;}
}
void UiMenus::start(AnmVm& vm,AnmLoaded* file,i32 script){if(!file)return;vm.anmFile=file;vm.scriptIndex=i16(script);executor.start(*file,vm,file->scripts[script]);}
bool UiMenus::capture(AnmVm& vm){
    start(vm,ascii.capture,0);if(!vm.loadedSprite)return false;const auto& sprite=*vm.loadedSprite;
    // Original uses height for destination width and width for height.
    const TextureCaptureRequest request{3,32,16,384,448,Scalar::truncate(sprite.startPixelInclusive.x),Scalar::truncate(sprite.startPixelInclusive.y),Scalar::truncate(sprite.heightPx),Scalar::truncate(sprite.widthPx)};
    if(!actions.capture(request))return false;vm.pos={32,16,0};return true;
}
void UiMenus::viewport(){auto v=renderer.viewport;v.x=u32(Scalar::truncate(context.arcade_origin.x));v.y=u32(Scalar::truncate(context.arcade_origin.y));v.width=u32(Scalar::truncate(context.arcade_size.x));v.height=u32(Scalar::truncate(context.arcade_size.y));renderer.set_viewport(v);}
void UiMenus::snapshot_pause(){auto& s=ascii.pause;for(size_t i=0;i<pause_previous.size();++i)pause_previous[i]={s.sprites[i].pos,s.sprites[i].pos2,s.sprites[i].scriptIndex,s.sprites[i].visible,presentation::VisualSample(s.sprites[i])};pause_background={s.background.pos,s.background.pos2,s.background.scriptIndex,s.background.visible,presentation::VisualSample(s.background)};pause_presentation_valid=true;}
void UiMenus::snapshot_retry(){auto& s=ascii.retry;for(size_t i=0;i<retry_previous.size();++i)retry_previous[i]={s.sprites[i].pos,s.sprites[i].pos2,s.sprites[i].scriptIndex,s.sprites[i].visible,presentation::VisualSample(s.sprites[i])};retry_background={s.background.pos,s.background.pos2,s.background.scriptIndex,s.background.visible,presentation::VisualSample(s.background)};retry_presentation_valid=true;}
void UiMenus::draw_presented(AnmVm& source,const PresentationVm& before,bool valid,bool force_no_z){
    TH08_AUDIT_SCOPE(Pause,&source,source.currentTimeInScript.current,context.pause_state?0:1);
    if(!presentation::render_only&&!force_no_z){renderer.draw_no_rotation(source);return;}AnmVm draw=source;
    if(presentation::active&&valid&&before.script==source.scriptIndex&&before.visible==source.visible){
        before.visual.apply(source,draw,presentation::alpha);
        const float dx=source.pos.x-before.pos.x,dy=source.pos.y-before.pos.y;if(dx*dx+dy*dy<16384.0f)draw.pos={presentation::lerp(before.pos.x,source.pos.x),presentation::lerp(before.pos.y,source.pos.y),presentation::lerp(before.pos.z,source.pos.z)};
        const float ox=source.pos2.x-before.pos2.x,oy=source.pos2.y-before.pos2.y;if(ox*ox+oy*oy<16384.0f)draw.pos2={presentation::lerp(before.pos2.x,source.pos2.x),presentation::lerp(before.pos2.y,source.pos2.y),presentation::lerp(before.pos2.z,source.pos2.z)};
    }
    if(force_no_z)draw.zWriteDisabled=true;renderer.draw_no_rotation(draw);
}
i32 UiMenus::update_pause(){
    auto& s=ascii.pause;auto& c=context;auto* vm=s.sprites;
    snapshot_pause();
    auto close=[&](u32 next){actions.sound(10);s.state=next;for(i32 i=0;i<10;++i)if(vm[i].visible)vm[i].pendingInterrupt=2;s.frames=0;};
    if(c.pressed(escape)&&s.state!=4){close(4);s.background.pendingInterrupt=1;}
    if(c.pressed(quit)&&s.state!=9)close(9);
    if(!c.replay()&&c.pressed(restart)&&s.state!=9)close(10);
    switch(s.state){
    case 0:
        for(i32 i=0;i<10;++i)start(vm[i],ascii.ascii,i+12);
        interrupts(vm,0,4,1);
        if(ascii.ascii)ascii.ascii->SetSprite(&vm[7],c.spell_practice()&&c.spell>=205?288:c.difficulty+283);
        if(!c.practice())vm[8].visible=false;if(!config.slow_mode)vm[9].visible=false;if(c.replay())vm[3].currentInstruction=nullptr;
        ++s.state;s.frames=0;if(c.lockable_backbuffer&&!capture(s.background)){s.state=0;return 0;}
        [[fallthrough]];
    case 1:case 2:case 3:{
        const u32 current=s.state;
        for(u32 i=1;i<4;++i){vm[i].color1.d3dColor=i==current?0xffffffff:normal;vm[i].pos2=i==current?Vec3{-4,-4,0}:Vec3{};}
        if(s.frames>=4){
            if(c.pressed(up)){s.state=current==1?(c.replay()?2:3):current==2?1:2;actions.sound(0);}
            if(c.pressed(down)){s.state=current==1?2:current==2?(c.replay()?1:3):1;actions.sound(0);}
            if(c.pressed(select)){actions.sound(10);interrupts(vm,0,4,2);s.frames=0;
                if(current==1){s.state=4;s.background.pendingInterrupt=1;}
                else {interrupts(vm,4,7,1);s.state=current==2?6:8;}
            }
        }break;
    }
    case 4:
        if(s.frames>=20){s.state=0;c.pause_state=0;for(i32 i=0;i<10;++i)vm[i].visible=false;actions.music(MenuMusic::Resume);c.system_time=actions.now();}break;
    case 5:case 7:
        choose(vm[5],vm[6],true);
        if(s.frames>=4){if(c.pressed(up)||c.pressed(down)){s.state=s.state==5?6:8;actions.sound(0);}
            if(c.pressed(select)){actions.sound(10);interrupts(vm,4,7,2);s.state=s.state==5?9:10;s.frames=0;}}
        break;
    case 6:case 8:
        choose(vm[5],vm[6],false);
        if(s.frames>=4){if(c.pressed(up)||c.pressed(down)){s.state=s.state==6?5:7;actions.sound(0);}
            if(c.pressed(select)){actions.sound(10);interrupts(vm,0,4,1);interrupts(vm,4,7,2);s.state=s.state==6?2:3;s.frames=0;}}
        break;
    case 9:
        if(s.frames>=20){s.state=0;c.supervisor_state=1;c.pause_state=0;c.system_time=actions.now();actions.save_score();}break;
    case 10:
        if(s.frames>=20){
            if(!c.spell_practice()&&!c.practice()&&c.difficulty!=4){s.state=0;c.supervisor_state=10;c.pause_state=0;c.system_time=actions.now();}
            else {if(c.spell_practice()&&!spell_music(c.spell).pause_in_practice){actions.music(MenuMusic::Resume);actions.music(MenuMusic::FadeIn,2);}else actions.music(MenuMusic::Stop);
                c.supervisor_state=11;actions.capture_arcade();c.pause_state=0;c.system_time=actions.now();return 0;}
        }break;
    }
    for(i32 i=0;i<10;++i)executor.execute(vm[i]);if(c.lockable_backbuffer)executor.execute(s.background);s.frames=wrapping_add(s.frames,1);return 0;
}
void UiMenus::continue_game(){
    auto& c=context;auto& s=ascii.retry;s.state=0;s.frames=0;c.show_retry=0;for(i32 i=0;i<4;++i)s.sprites[i].visible=false;
    ++globals.retries;globals.display_score=globals.retries;globals.score_increment=0;globals.score=globals.display_score;
    values.set_lives(config.lives);values.set_bombs(c.shot_bombs);
    globals.graze_stage=0;globals.points_stage=globals.points=0;values.set_power(0);
    globals.point_extends=0;globals.next_point_extend=100;c.timing_level=8;
    auto increment=[](u32& v){if(v<999999)++v;};
    if(c.difficulty>=0&&c.difficulty<5&&c.character>=0&&c.character<12){auto& difficulty=statistics.counts[c.difficulty];auto& total=statistics.counts[6];
        increment(difficulty.total);increment(total.total);increment(difficulty.characters[c.character]);increment(total.characters[c.character]);increment(difficulty.continues);increment(total.continues);}
    actions.music(MenuMusic::Resume);c.system_time=actions.now();
}
i32 UiMenus::update_retry(){
    auto& s=ascii.retry;auto& c=context;auto* vm=s.sprites;
    snapshot_retry();
    if(c.practice()&&!c.spell_practice()){c.show_retry=0;globals.display_score=globals.score;c.supervisor_state=6;return 1;}
    if(c.replay()){c.show_retry=0;c.supervisor_state=7;globals.display_score=globals.score;return 1;}
    switch(s.state){
    case 0:
        if(s.frames==0){
            if(!c.spell_practice()&&c.difficulty<4&&(globals.clock_time>=11||c.stage==7)){
                c.show_retry=0;globals.display_score=globals.score;
                if(c.difficulty>=4)c.supervisor_state=6;else {c.flags&=~16u;c.supervisor_state=9;}return 1;
            }
            if(c.spell_practice()&&!spell_music(c.spell).pause_in_practice)actions.music(MenuMusic::PartialFadeOut,1);else actions.music(MenuMusic::Pause);
            for(i32 i=0;i<3;++i){start(vm[i],ascii.ascii,i+22);vm[i].pendingInterrupt=1;}
            start(vm[3],c.times,1);if(c.times)c.times->SetSprite(&vm[3],globals.clock_time);
            if(c.lockable_backbuffer&&!capture(s.background)){s.state=0;return 0;}actions.update_game_time();
        }
        if(s.frames>8)break;
        s.state+=!c.spell_practice()&&c.difficulty<4?2:!c.spell_captured&&c.spell_practice()?1:2;s.frames=0;
        if(s.state==2)goto no_selected;
        [[fallthrough]];
    case 1:
        choose(vm[1],vm[2],true);
        if(s.frames>=4){
            if(c.pressed(up)||c.pressed(down)){s.state=2;actions.sound(0);}
            if(c.pressed(select)){actions.sound(10);
                if(!c.spell_practice()&&c.difficulty<4){vm[3].pendingInterrupt=1;s.state=3;s.frames=0;}
                else {if(c.spell_practice()&&!spell_music(c.spell).pause_in_practice){c.show_retry=0;actions.music(MenuMusic::Resume);actions.music(MenuMusic::PartialFadeIn,1);}else actions.music(MenuMusic::Stop);
                    c.supervisor_state=11;actions.capture_arcade();c.show_retry=0;c.system_time=actions.now();return 0;}
            }
        }break;
    case 2:
    no_selected:
        choose(vm[1],vm[2],false);
        if(s.frames>=30){if(c.pressed(up)||c.pressed(down)){s.state=1;actions.sound(0);}
            if(c.pressed(select)){actions.sound(10);interrupts(vm,0,4,2);s.state=4;s.frames=0;}}
        break;
    case 4:
        if(s.frames>=20){s.state=0;s.frames=0;c.show_retry=0;c.supervisor_state=6;for(i32 i=0;i<4;++i)vm[i].visible=false;globals.display_score=globals.score;c.system_time=actions.now();return 0;}break;
    case 3:
        if(s.frames==15){values.add_clock(1);if(c.times)c.times->SetSprite(&vm[3],globals.clock_time);}
        if(s.frames==60){s.background.pendingInterrupt=1;interrupts(vm,0,4,3);}
        if(s.frames>=90){continue_game();return 0;}break;
    }
    for(i32 i=0;i<4;++i)executor.execute(vm[i]);if(c.lockable_backbuffer)executor.execute(s.background);s.frames=wrapping_add(s.frames,1);return 0;
}
void UiMenus::draw_pause(){
    auto& s=ascii.pause;if(!context.pause_state)return;viewport();
    if(context.lockable_backbuffer&&s.state!=0)draw_presented(s.background,pause_background,pause_presentation_valid,true);
    for(size_t i=0;i<10;++i)if(s.sprites[i].visible)draw_presented(s.sprites[i],pause_previous[i],pause_presentation_valid);
}
void UiMenus::draw_retry(){
    auto& s=ascii.retry;if(!context.show_retry)return;viewport();
    if(context.lockable_backbuffer&&(s.state!=0||s.frames>2))draw_presented(s.background,retry_background,retry_presentation_valid);
    const i32 count=!context.spell_practice()&&context.difficulty<4?4:3;for(i32 i=0;i<count;++i)if(s.sprites[i].visible)draw_presented(s.sprites[i],retry_previous[i],retry_presentation_valid);
}
}
