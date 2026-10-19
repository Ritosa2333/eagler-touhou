#include "SpellFlow.hpp"
namespace th08 {
bool SpellFlow::start(i32 slot,AnmLoaded* file,i32 script){
    if(!file||!file->scripts||u32(script)>=file->scriptCount)return false;auto& vm=state.spell_vms[slot];vm.anmFile=file;vm.scriptIndex=i16(script);anm.start(*file,vm,file->scripts[script]);return !anm.invalid;
}
bool SpellFlow::setup(){
    auto& s=state;static_cast<SpellState&>(s)=SpellState{};const bool practice=s.game_flags&0x4000;const i32 spell=s.current_spell;
    if(context.initial){
        s.spell_banners=resources.load(15,"face_cdbg.anm");if(!s.spell_banners)return false;
        if(!practice){
            const char* human="face_rm00.anm";const char* youkai="face_yk00.anm";
            switch(s.shot){case 1:case 6:case 7:human="face_mr00.anm";youkai="face_al00.anm";break;case 2:case 8:case 9:human="face_sk00.anm";youkai="face_rs00.anm";break;case 3:case 10:case 11:human="face_ym00.anm";youkai="face_yy00.anm";break;}
            s.spell_human_face=resources.load(16,human);if(!s.spell_human_face)return false;s.spell_youkai_face=resources.load(17,youkai);if(!s.spell_youkai_face)return false;
        }
    }else{
        s.spell_banners=resources.get(15);if(!practice){s.spell_human_face=resources.get(16);s.spell_youkai_face=resources.get(17);}
    }
    if(context.keep_resources){s.spell_enemy_face=resources.get(18);s.spell_enemy_face2=resources.get(19);}
    else {
        const char* enemy=nullptr;const char* second=nullptr;
        if(!practice){
            switch(s.stage){case 0:enemy="face_st01.anm";break;case 1:enemy="face_st02.anm";break;case 3:enemy="face_st04a.anm";break;case 4:enemy="face_st04b.anm";break;case 5:enemy="face_st05.anm";second="face_st05b.anm";break;case 6:enemy="face_st06.anm";break;case 7:enemy="face_st06.anm";second="face_st07.anm";break;case 8:enemy="face_st08m.anm";second="face_st08.anm";break;default:enemy="face_st03.anm";break;}
        }else switch(s.stage){
            case 0:enemy="face_st01sp.anm";break;case 1:enemy="face_st02sp.anm";break;case 2:enemy="face_st03sp.anm";break;
            case 3:switch(spell){case 216:enemy="face_sksp.anm";break;case 217:enemy="face_ymsp.anm";break;case 218:enemy="face_alsp.anm";break;case 219:enemy="face_rssp.anm";break;case 220:enemy="face_yysp.anm";break;case 221:enemy="face_yksp.anm";break;default:enemy="face_st04asp.anm";break;}break;
            case 4:enemy="face_st04bsp.anm";break;
            case 5:enemy=spell==212?"face_st05msp.anm":"face_st05sp.anm";break;
            case 6:enemy="face_st06sp.anm";break;
            case 7:enemy=spell>=147&&spell<=150?"face_st06sp.anm":"face_st07sp.anm";break;
            case 8:if((spell>=191&&spell<=193)||spell==213)enemy="face_st08msp.anm";else if((spell>=194&&spell<=204)||spell==211)enemy="face_st08sp.anm";break;
            default:enemy="face_st03.anm";break;
        }
        if(enemy){s.spell_enemy_face=resources.load(18,enemy);if(!s.spell_enemy_face)return false;}
        if(second){s.spell_enemy_face2=resources.load(19,second);if(!s.spell_enemy_face2)return false;}
    }
    // Original initialization starts both portrait VMs from the human file;
    // the enemy file is selected when an enemy announcement actually begins.
    if(s.spell_human_face&&(!start(0,s.spell_human_face,0)||!start(1,s.spell_human_face,0)))return false;
    if(!start(6,presentation.context.text,4)||!start(7,presentation.context.text,5)||!start(10,drawing.digits,1)||!start(11,drawing.digits,0)||!start(13,drawing.digits,2)||!start(12,drawing.digits,4))return false;
    for(i32 slot:{0,2,4,6,1,3,5,7}){s.spell_vms[slot].currentInstruction=nullptr;s.spell_vms[slot].visible=false;}
    s.spell_vms[6].fontWidth=s.spell_vms[6].fontHeight=s.spell_vms[7].fontWidth=s.spell_vms[7].fontHeight=15;s.spell_time_items=0;return true;
}
void SpellFlow::release(){if(!context.keep_resources){resources.release(18);resources.release(19);}if(context.release_resources)for(i32 slot:{15,16,17})resources.release(slot);}
bool SpellFlow::attach(Chain& owner){
    detach();if(!setup())return false;chain=&owner;
    calculation.set_callback([](void* p){return static_cast<SpellFlow*>(p)->spells.update()?JobResult::Continue:JobResult::Error;});calculation.argument=this;calculation.deleted=[](void* p){static_cast<SpellFlow*>(p)->release();return 0;};
    if(owner.add(&calculation,12)){chain=nullptr;return false;}
    display.set_callback([](void* p){return static_cast<SpellFlow*>(p)->drawing.draw()?JobResult::Continue:JobResult::Error;});display.argument=this;owner.add(&display,15,true);return true;
}
void SpellFlow::detach(){if(chain){chain->cut(&calculation);chain->cut(&display);chain=nullptr;}}
}
