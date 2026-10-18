#include "SpellCard.hpp"
#include "Localization.hpp"
namespace th15 {
bool SpellCard::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool SpellCard::banners(i32 label){for(u32 i=1;i<=3;i++)if(!check(host.interrupt(handles[i],label),"Spell banner interrupt failed"))return false;return true;}
bool SpellCard::begin(const SpellStartRequest& request,const SpellStartContext& c){
    error.clear();if(request.id<0||request.id>=119||request.name.size()>=64){error="Spell metadata outside original range";return false;}
    age.set(0);status.frame=age.current;identifier=request.id;name=request.name;replay=c.replay;status.flags=(status.flags|3u)&~0x98u;
    if(!replay&&!check(host.history_begin(identifier,name),"Spell history start failed"))return false;
    if(!check(host.prepare_hud(true),"Spell HUD start failed"))return false;
    status.flags&=~0x20u;if(c.bomb_state==1&&c.character!=3)status.flags|=0x20u;status.flags&=~0x40u;active_frames=1;
    const i32 resources[]={c.visuals.ascii,c.visuals.name,c.visuals.ascii};const i32 scripts[]={0,2,1};
    for(u32 i=0;i<3;i++){handles[i+1]=host.create_visual(resources[i],scripts[i]);if(!check(handles[i+1]!=0,"Spell banner creation failed"))return false;}
    if(!check(host.text(handles[2],Localization::SpellName(u32(identifier),name.c_str(),u32(c.difficulty))),"Spell title text failed")||!check(host.sound(33),"Spell start sound failed"))return false;
    handles[4]=host.create_visual(c.visuals.effect,13);if(!check(handles[4]!=0,"Spell Boss ring creation failed")||!check(host.boss_position(anchor),"Spell Boss position unavailable")||!check(host.position(handles[4],anchor),"Spell Boss ring positioning failed"))return false;
    for(i32 script:{11,12})if(!check(host.child_integer(handles[4],script,2,request.duration),"Spell Boss ring timer unavailable"))return false;
    duration=request.duration;status.bonus=wrapping_mul(wrapping_add(c.stage,c.difficulty),1000000);maximum_bonus=status.bonus>=1000000000?999999999:status.bonus;
    if(!check(host.create_visual(c.visuals.effect,20)!=0,"Spell start effect creation failed"))return false;
    const auto& background=c.visuals.backgrounds[c.visuals.secondary?1:0];handles[0]=host.create_visual(background.resource,background.script);if(!check(handles[0]!=0,"Spell background creation failed"))return false;
    status.flags=(status.flags&~0x200u)|(background.independent?0x200u:0);
    const auto& overlay=c.visuals.backgrounds[(identifier==112||identifier==116)?2:(c.visuals.secondary?1:0)];
    if(overlay.overlay_resource!=-1&&!check(host.create_visual(overlay.overlay_resource,overlay.overlay_script)!=0,"Spell secondary background creation failed"))return false;
    // The target build ignores the passed bonus and survival parameters.
    // Its initial bonus comes from the current stage and difficulty instead.
    return true;
}
bool SpellCard::update(const SpellFrameContext& c){
    if(!(status.flags&1))return true;active_frames=wrapping_add(active_frames,1);
    if(age.current>=60&&!(status.flags&0x200)&&!check(host.background_visible(false),"Spell background visibility failed"))return false;
    // Purple 41fdf5 changes JL to JMP: bypass bonus decay only. The spell
    // animation timer, active frames and banner placement still advance.
    if(!c.lock_time&&age.current>=300&&!(status.flags&8)){
        const i32 divisor=wrapping_sub(duration,300);if(!divisor){error="Invalid original spell bonus divisor";return false;}
        const i32 amount=wrapping_sub(maximum_bonus,maximum_bonus/3);const i64 quotient=i64(amount)/divisor;
        if(quotient<INT32_MIN||quotient>INT32_MAX){error="Original spell bonus division overflow";return false;}
        status.bonus=wrapping_mul(wrapping_sub(status.bonus,i32(quotient))/10,10);
    }
    age.tick(&c.rate);status.frame=age.current;
    if(age.current>=120){const bool lower=status.flags&0x100;bool change=false;
        if(!(status.flags&4)){change=lower?c.player_y>352:c.player_y<96;if(change){if(!banners(3))return false;status.flags|=4;}}
        else{change=lower?c.player_y<320:c.player_y>128;if(change){if(!banners(2))return false;status.flags&=~4u;}}
    }
    Vec3 boss;if(!check(host.boss_position(boss),"Spell Boss tracking unavailable"))return false;
    anchor.x=float(anchor.x+float(float(boss.x-anchor.x)*.05f));anchor.y=float(anchor.y+float(float(boss.y-anchor.y)*.05f));anchor.z=float(anchor.z+float(float(boss.z-anchor.z)*.05f));
    if(!check(host.position(handles[4],anchor),"Spell Boss ring update failed"))return false;
    if((status.flags&0x20)&&c.bomb_state!=1)status.flags&=~0x20u;return true;
}
bool SpellCard::finish(){
    if(!(status.flags&1))return true;
    if(!check(host.background_visible(true),"Spell background restoration failed")||!banners(1))return false;
    status.flags&=~1u;if(!check(host.retire(handles[0]),"Spell background retirement failed"))return false;status.flags&=~0x20u;
    if(!check(host.prepare_hud(false),"Spell HUD finish failed")||!check(host.retire(handles[4]),"Spell Boss ring retirement failed"))return false;
    if(status.flags&2){const u32 next=u32(score)+u32(status.bonus/10);score=signed_bits(next>=1000000000u?999999999u:next);
        if(!check(host.result(status.bonus,false),"Spell capture result failed"))return false;
        if(!replay&&!check(host.history_capture(identifier),"Spell capture history failed"))return false;
        if(!check(host.sound(46),"Spell capture sound failed"))return false;
    }else if(!check(host.result(0,true),"Spell failure result failed"))return false;
    return !(status.flags&0x80)||check(host.sound(69),"Spell auxiliary sound failed");
}
bool SpellCard::abort(){
    if(!(status.flags&1))return true;if(!check(host.background_visible(true),"Spell background restoration failed"))return false;
    for(u32 i=1;i<=3;i++)if(!check(host.retire(handles[i]),"Spell banner retirement failed"))return false;
    status.flags&=~1u;if(!check(host.retire(handles[0]),"Spell background retirement failed"))return false;status.flags&=~0x20u;
    if(!check(host.prepare_hud(false),"Spell HUD abort failed")||!check(host.retire(handles[4]),"Spell Boss ring retirement failed"))return false;
    status.flags&=~0x18u;return true;
}
}
