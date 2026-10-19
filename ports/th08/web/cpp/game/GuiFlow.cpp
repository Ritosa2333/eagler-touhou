#include "GuiFlow.hpp"
namespace th08 {
namespace {
constexpr const char* loading[]={"loading00.anm","loading01.anm","loading02.anm","loading03.anm","loading00h.anm","loading00a.anm","loading01h.anm","loading01a.anm","loading02h.anm","loading02a.anm","loading03h.anm","loading03a.anm"};
constexpr const char* stage_text[]={"stg1txt.anm","stg2txt.anm","stg3txt.anm","stg4atxt.anm","stg4btxt.anm","stg5txt.anm","stg6txt.anm","stg7txt.anm","stg8txt.anm"};
constexpr const char* messages[9][4]={{"msg1a.dat","msg1b.dat","msg1c.dat","msg1d.dat"},{"msg2a.dat","msg2b.dat","msg2c.dat","msg2d.dat"},{"msg3a.dat","msg3b.dat","msg3c.dat","msg3d.dat"},{"msg4dm.dat","msg4ab.dat","msg4ac.dat","msg4dm.dat"},{"msg4ba.dat","msg4dm.dat","msg4dm.dat","msg4bd.dat"},{"msg5a.dat","msg5b.dat","msg5c.dat","msg5d.dat"},{"msg6a.dat","msg6b.dat","msg6c.dat","msg6d.dat"},{"msg7a.dat","msg7b.dat","msg7c.dat","msg7d.dat"},{"msg8a.dat","msg8b.dat","msg8c.dat","msg8d.dat"}};
}
bool GuiFlow::setup(){
    auto& c=controller;auto& g=c.gui;auto& d=c.display;const auto& s=c.scene;const bool spell=s.flags&0x4000;
    if(s.stage<0||s.stage>=9||s.character<0||s.character>=12)return false;
    if(context.initial){
        g.implementation=&d;
        std::memset(&d,0,sizeof(d));g.front=resources.load(10,"front.anm");if(!g.front)return false;c.reset_clear();
        g.times=resources.load(14,"times.anm");if(!g.times)return false;g.loading_portrait=resources.load(12,loading[s.character]);if(!g.loading_portrait)return false;
        if(!c.start(d.nullify,s.ascii,26)||!c.start(d.difficulty,s.ascii,25))return false;
        if(s.ascii->SetSprite(&d.difficulty,spell&&context.spell_number>=205?288:c.context.difficulty+283))return false;
    }else{
        c.reset_clear();if(!c.start(d.arcade,s.capture,1))return false;d.arcade.pendingInterrupt=1;
        for(i32 i=0;i<14;++i)for(i32 j=0;j<12;++j){auto& vm=d.transition[i*12+j];if(!c.start(vm,s.capture,((i+j)&1)+3))return false;vm.counterVar0=i+j*2;vm.pos={float(j*32)+15.5f,float(i*32)+15.5f,0};vm.uvScrollPos={float(j)/16,float(i)/16};}d.transition_count=168;
    }
    c.clock(3);if(!c.start(d.clock_intro,g.times,0,true)||g.times->SetSprite(&d.clock_intro,c.globals.clock_time))return false;
    // The clock intro only plays with the opening stage title. Spell practice
    // and section warps skip that sequence, so the clock must never execute
    // (it starts at alpha 0 and would otherwise freeze mid-script on screen).
    c.clock_intro_enabled=!spell&&!context.section_warp;
    if(!spell){const i32 team=s.character<4?s.character:(s.character-4)/2;const auto bytes=resources.message(messages[s.stage][team]);if(!dialogue.load(bytes.data(),bytes.size()))return false;}
    if(!context.keep_resources){g.stage_text=resources.load(13,stage_text[spell&&context.spell_number>=205?8:s.stage]);if(!g.stage_text)return false;}
    if(context.initial)for(i32 i=0;i<16;++i)if(!c.start(d.front[i],g.front,i))return false;
    g.frame=0;g.boss_present=false;d.boss_life_state=0;g.boss_life_max=g.boss_life=0;
    // Upstream th08_disable_title (0x439568): section warps skip the opening
    // stage title sequence; the tied clock intro never executes either.
    if(!spell&&!context.section_warp){for(i32 i=0;i<4;++i){if(!c.start(d.stage_text[i],g.stage_text,i,true))return false;d.stage_text[i].baseSpriteIndex=d.stage_text[i].activeSpriteIndex;}}
    else{const auto& music=spell_music(context.spell_number);if(!context.keep_resources||music.pause_in_practice){if(!c.start(d.stage_text[0],g.stage_text,3,true))return false;d.stage_text[0].baseSpriteIndex=d.stage_text[0].activeSpriteIndex;if(g.stage_text->SetSprite(&d.stage_text[0],music.name_sprite+3))return false;}}
    d.dialogue.message=-1;d.clear_frames=0;d.bonus.display=d.popup.display=d.spell_bonus.display=0;g.flags.lives=g.flags.bombs=g.flags.graze=g.flags.points=g.flags.power=g.flags.time=2;
    if(!c.start(d.stage_rank,s.ascii,3))return false;c.scene.hud_redraw=16;d.clear_clock_display=0;return true;
}
void GuiFlow::release(){
    if(!context.keep_resources){resources.release(13);controller.gui.stage_text=nullptr;}dialogue.release();
    if(context.release_resources){for(i32 slot:{10,12,11,14})resources.release(slot);controller.gui.front=controller.gui.loading_portrait=controller.gui.times=nullptr;controller.gui.implementation=nullptr;}
}
void GuiFlow::update(){auto& c=controller;c.snapshot_presentation();if(c.context.time_stopped)return;c.update_stage();dialogue.update();if((c.context.input&256)&&c.scene.hud_redraw<8)c.scene.hud_redraw=8;++c.gui.frame;}
void GuiFlow::draw(){auto& c=controller;if(c.display.clear_frames)c.draw_clear();dialogue.draw();c.draw_stage();c.draw_hud();c.draw_popups();}
bool GuiFlow::attach(Chain& owner){
    detach();chain=&owner;if(context.initial){controller.gui={};controller.gui.implementation=&controller.display;}
    calculation.set_callback([](void* p){static_cast<GuiFlow*>(p)->update();return JobResult::Continue;});calculation.argument=this;
    calculation.added=[](void* p){return static_cast<GuiFlow*>(p)->setup()?0:-1;};calculation.deleted=[](void* p){static_cast<GuiFlow*>(p)->release();return 0;};
    if(owner.add(&calculation,15))return false;
    drawing.set_callback([](void* p){static_cast<GuiFlow*>(p)->draw();return JobResult::Continue;});drawing.argument=this;owner.add(&drawing,17,true);return true;
}
void GuiFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&drawing);chain=nullptr;}}
}
