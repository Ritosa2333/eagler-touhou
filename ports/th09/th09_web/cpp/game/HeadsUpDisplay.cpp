#include "HeadsUpDisplay.hpp"
#include <algorithm>
namespace th09 {
namespace {void visible(AnmVm& a,bool b){a.flags=(a.flags&~2u)|(b?2u:0u);}}
void HeadsUpDisplay::advance(u32 first,u32 count){for(u32 n=first;n<first+count;++n)resources.advance(animations[n]);}
void HeadsUpDisplay::initialize(bool versus){
    for(u32 n=0;n<11;++n){start(n,i32(n)+7);animations[n].pos2=animations[n].pos;}
    for(u32 n=0;n<5;++n)start(11+n,i32(n)+35);
    for(u32 n=0;n<7;++n)start(16+n,i32(n)+52);
    for(u32 n=0;n<10;++n)start(24+n,i32(n)+20);
    start(23,59);if(!versus)for(u32 n=0;n<7;++n)start(51+n,i32(n)+40);
    for(u32 n=0;n<5;++n)start(58+n,i32(n)+47);
    blink.reset();std::memset(&animations[34],0,sizeof(AnmVm));
}
void HeadsUpDisplay::begin_charge(){for(u32 n=0;n<4;++n)start(35+n,i32(n)+68);charge_active=1;}
void HeadsUpDisplay::end_charge(){for(u32 n=35;n<39;++n)animations[n].pendingInterrupt=1;charge_active=0;}
void HeadsUpDisplay::charge_level(i32 level){for(u32 n=0;n<3;++n)start(39+n,i32(n)+72);sprite(41,53+level);}
void HeadsUpDisplay::health_notice(i32 kind){start(42,75);start(43,76+kind);}
void HeadsUpDisplay::begin_survival(i32 frames){for(u32 n=0;n<6;++n)start(45+n,i32(n)+79);survival_time(frames);}
void HeadsUpDisplay::survival_time(i32 frames){
    if(frames<0){for(u32 n=45;n<51;++n)std::memset(&animations[n],0,sizeof(AnmVm));return;}
    const i32 seconds=frames/60,hundredths=(frames%60)*100/60;
    if(seconds/100==0)std::memset(&animations[45],0,sizeof(AnmVm));else sprite(45,53+seconds/100);
    sprite(46,53+(seconds/10)%10);sprite(47,53+seconds%10);sprite(49,53+hundredths/10);sprite(50,53+hundredths%10);
}
void HeadsUpDisplay::portrait(u32 layer,i32 script,AnimationFile file){if(layer<2)resources.start(file,portraits[layer],script);}
void HeadsUpDisplay::update(const HudFrame& f){
    advance(0,11);const i32 hits=std::min(f.combo.hits,999);
    if(hits>=100)sprite(2,8+hits/100);visible(animations[2],hits>=100);
    if(hits>=10)sprite(3,8+(hits/10)%10);visible(animations[3],hits>=10);sprite(4,8+hits%10);
    const bool warning=f.combo.chain_time.current>0&&f.combo.chain_time.current<20;
    const u32 color=warning?(f.combo.chain_time.current&1?0xff0000u:0xffffffu):0xd08080u;
    for(u32 n=1;n<5;++n)animations[n].color1.d3dColor=i32((u32(animations[n].color1.d3dColor)&0xff000000)|color);
    i32 value=f.combo.display_score;for(u32 n=0,divisor=100000;n<6;++n,divisor/=10){sprite(5+n,8+value/i32(divisor));value%=i32(divisor);}
    value=i32(f.score.displayed);visible(animations[24],value>=100000000);
    if(value>=100000000){sprite(24,8+value/100000000);value%=100000000;}
    for(u32 n=0,divisor=10000000;n<8;++n,divisor/=10){sprite(25+n,8+value/i32(divisor));value%=i32(divisor);}
    sprite(33,8);advance(24,10);
    i32 health=f.health;for(u32 n=11;n<16;++n){resources.advance(animations[n]);sprite(n,35+std::min(health,2));health=std::max(health-2,0);}
    advance(16,7);animations[16].scale.x=f.available*.0025f;animations[17].scale.x=f.charge*.0025f;
    const bool odd=blink.current%2!=0;u32 charge_color=0xffc0c0c0;
    if(f.charge>=400)charge_color=0xffff0000u+(odd?0xffffu:0);
    else if(f.charge>=300)charge_color=0xfff0f0c0u+(odd?0xffffff40u:0);
    else if(f.charge>=200)charge_color+=odd?0x302f40u:0;
    else if(f.charge>=100)charge_color+=odd?0x302fc0u:0;
    animations[17].color1.d3dColor=i32(charge_color);
    animations[16].color1.d3dColor=i32(f.available>=400?0xffc08080u:f.available>=300?0xffb08080u:f.available>=200?0xffa08080u:0xff808080u);
    sprite(19,8+f.spell_level/10);sprite(20,8+f.spell_level%10);sprite(21,8+f.boss_level/10);sprite(22,8+f.boss_level%10);
    for(auto& p:portraits)resources.advance(p);
    animations[23].scale.x=f.combo.display_time.value()*.009523809887468814f;advance(23,1);advance(34,2);
    sprite(36,53+i32(f.charge)/100);advance(36,1);sprite(37,53+i32(f.available)/100);advance(37,2);
    advance(39,3);advance(42,2);advance(45,6);advance(44,1);
    if(!f.versus){for(u32 n=0;n<7;++n)visible(animations[51+n],i32(n)<i32(f.score.lives));advance(51,7);}
    const i32 wins=std::min(f.wins,5);for(u32 n=0;n<5;++n)visible(animations[58+n],i32(n)<wins);advance(58,5);
    if(wipe_state==1)wipe.tick(f.timing);else if(wipe_state==2){wipe.decrement(1,f.timing);if(wipe.current<=0)wipe_state=0;}
    blink.tick(f.timing);
}
void HeadsUpDisplay::draw_range(const HudFrame& f,u32 first,u32 count,bool rotated,bool relative,bool zero_z){
    for(u32 n=first;n<first+count;++n){auto& a=animations[n];Vec3 p=a.pos2;if(relative){p.x=f.player.x+p.x;p.y=f.player.y+p.y;}
        const Vec3 screen=f.geometry.to_screen(p);a.pos.x=screen.x;a.pos.y=screen.y;if(zero_z)a.pos.z=0;
        if(first==39&&charge_active)a.pos.y-=24;output.hud_animation(a,rotated);}
}
void HeadsUpDisplay::draw(const HudFrame& f){
    output.begin_hud(side);draw_range(f,0,11,true,false,true);draw_range(f,11,5,true,false,true);draw_range(f,16,7,true,false,true);
    for(auto& p:portraits){p.pos=f.geometry.to_screen(p.pos2);output.hud_animation(p,false);}
    draw_range(f,23,1,false,false,true);draw_range(f,24,10,false,false,true);draw_range(f,34,1,false);
    draw_range(f,35,4,false,true);draw_range(f,39,3,false,true);draw_range(f,45,6,false,true);draw_range(f,42,2,false,true);draw_range(f,44,1,false,true);
    draw_range(f,51,7,false);draw_range(f,58,5,true);
    if(!wipe_state)return;const float t=wipe.value(),sign=wipe_state==1?1.f:-1.f;
    const auto triangle=[&](Vec2 a,Vec2 b,Vec2 c){AttackColorVertex v[3];const Vec2 points[]{a,b,c};for(u32 n=0;n<3;++n)v[n]={f.geometry.to_screen({points[n].x,points[n].y,0}),1,0xff000000};output.hud_triangle(v);};
    const float a=sign*-144.f,b=sign*144.f;triangle({a,0},{b,0},{a,std::min(t,30.f)*14.933333396911621f});
    triangle({a,448},{b,448},{b,448.f-t*14.933333396911621f});
    if(wipe_state==1){triangle({-144,0},{-144,448},{t*9.600000381469727f-144.f,448});triangle({144,448},{144,0},{144.f-t*9.600000381469727f,0});}
    else {triangle({-144,448},{-144,0},{t*9.600000381469727f-144.f,0});triangle({144,0},{144,448},{144.f-t*9.600000381469727f,448});}
}
}
