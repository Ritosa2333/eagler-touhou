#include "SpellCard.hpp"
namespace th17 {
bool SpellCard::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool SpellCard::banners(i32 label){for(u32 i=1;i<=3;i++)if(!check(host.interrupt(handles[i],label),"Spell banner interrupt failed"))return false;return true;}
bool SpellCard::begin(const SpellStartRequest& request,const SpellStartContext& c){
    error.clear();if(u32(request.id)>=101||request.name.size()>=64||u32(c.character)>=3||u32(c.animal)>=3||u32(c.difficulty)>=5||u32(request.mode)>=c.visuals.backgrounds.size()){error="Spell metadata outside TH17 range";return false;}
    age.set(0);status.frame=0;status.id=request.id;name=request.name;replay=c.replay;practice=c.practice;loadout=c.character*3+c.animal;status.flags=(status.flags&~0x98u)|3;
    if(!replay&&!check(records.begin(status.id,loadout,practice,name),"Spell record start failed"))return false;
    if(!check(host.prepare_hud(true),"Spell HUD start failed"))return false;active_frames=1;status.flags&=~0x60u;
    const i32 resources[]={c.visuals.ascii,c.visuals.name,c.visuals.ascii},scripts[]={0,2,1};for(u32 i=0;i<3;i++){handles[i+1]=host.create_visual(resources[i],scripts[i]);if(!check(handles[i+1]!=0,"Spell banner creation failed"))return false;}
    if(!check(host.text(handles[2],name),"Spell title text failed")||!check(host.sound(33,true),"Spell start sound failed"))return false;
    handles[4]=host.create_visual(c.visuals.effect,13);if(!check(handles[4]!=0,"Spell boss ring creation failed")||!check(host.boss_position(anchor),"Spell boss position unavailable")||!check(host.position(handles[4],anchor),"Spell boss ring position failed"))return false;
    for(i32 script:{11,12})if(!check(host.child_integer(handles[4],script,2,request.time),"Spell boss ring timer unavailable"))return false;
    duration=request.time;static constexpr i32 bonuses[]={500000,1000000,1500000,2000000,1000000};status.bonus=wrapping_mul(bonuses[c.difficulty],c.stage);maximum_bonus=status.bonus>999999999?999999999:status.bonus;
    if(!check(host.create_visual(c.visuals.effect,20)!=0,"Spell start effect creation failed"))return false;
    const auto& background=c.visuals.backgrounds[request.mode];handles[0]=host.create_visual(background.resource,background.script);if(!check(handles[0]!=0,"Spell background creation failed"))return false;
    status.flags=(status.flags&~0x200u)|(background.independent?0x200u:0);
    if(background.overlay_resource!=-1&&!check(host.create_visual(background.overlay_resource,background.overlay_script)!=0,"Spell overlay creation failed"))return false;
    roaring_flags&=~0x800u;return true;
}
bool SpellCard::update(const SpellFrameContext& c){
    if(!(status.flags&1))return true;active_frames=wrapping_add(active_frames,1);
    if(age.current>=60&&!(status.flags&0x200)&&!check(host.background_visible(false),"Spell background visibility failed"))return false;
    if(age.current>=300&&!(status.flags&8)){const i32 divisor=wrapping_sub(duration,300);if(!divisor){error="Invalid TH17 spell bonus divisor";return false;}const i32 amount=wrapping_sub(maximum_bonus,maximum_bonus/3);const i64 quotient=i64(amount)/divisor;if(quotient<INT32_MIN||quotient>INT32_MAX){error="TH17 spell bonus division overflow";return false;}status.bonus=wrapping_mul(wrapping_sub(status.bonus,i32(quotient))/10,10);}
    age.tick(&c.rate);status.frame=age.current;
    if(age.current>=120){const bool lower=status.flags&0x100;if(!(status.flags&4)){if(lower?c.player_y>352:c.player_y<96){if(!banners(3))return false;status.flags|=4;}}else if(lower?c.player_y<320:c.player_y>128){if(!banners(2))return false;status.flags&=~4u;}}
    Vec3 boss;if(!check(host.boss_position(boss),"Spell boss tracking unavailable"))return false;anchor={float(anchor.x+float(float(boss.x-anchor.x)*.05f)),float(anchor.y+float(float(boss.y-anchor.y)*.05f)),float(anchor.z+float(float(boss.z-anchor.z)*.05f))};if(!check(host.position(handles[4],anchor),"Spell boss ring update failed"))return false;
    if((status.flags&32)&&c.bomb_state!=1)status.flags&=~32u;return true;
}
bool SpellCard::finish(){
    if(!(status.flags&1))return true;if(!check(host.background_visible(true),"Spell background restoration failed")||!banners(1))return false;status.flags&=~1u;if(!check(host.retire(handles[0]),"Spell background retirement failed"))return false;status.flags&=~32u;if(!check(host.prepare_hud(false),"Spell HUD finish failed")||!check(host.retire(handles[4]),"Spell boss ring retirement failed"))return false;
    if(status.flags&2){score=wrapping_add(score,status.bonus/10);if(u32(score)>999999999u)score=999999999;if(!check(host.result(status.bonus,false),"Spell capture result failed"))return false;if(!replay){bool all=false;if(!check(records.capture(status.id,loadout,practice,all),"Spell capture history failed"))return false;if(all&&!check(host.trophy(32),"Spell trophy unavailable"))return false;}if(!check(host.sound(46,true),"Spell capture sound failed"))return false;}else if(!check(host.result(0,true),"Spell failure result failed"))return false;
    return !(status.flags&128)||check(host.sound(69,true),"Spell auxiliary sound failed");
}
}
