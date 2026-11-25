#include "meira_core.hpp"
#include <iostream>
#include <source_location>
#include <string>
#include <vector>
namespace {
unsigned checks=0;
void check(bool condition,const std::source_location where=std::source_location::current()){
    ++checks;if(!condition)throw std::runtime_error("Meira contract failed="+std::to_string(checks)+" line="+std::to_string(where.line()));
}
soew::MeiraCore make(unsigned rank=1){soew::MeiraCore b;b.init(rank,{});return b;}
void at(soew::MeiraCore& b,unsigned frame,const soew::MeiraCore::Hooks& h={},int px=400,int py=350){
    b.phase_frame=std::uint16_t(frame-1);b.update(0,px,py,0,h);
}
}
int main(){try{
    // asm13300..13355: only documented init writes; no guessed reset of the
    // phase/death, pool, marker parity, trail, travel or clone globals.
    {
        soew::MeiraCore::Inherited old;old.phase_frame=17;old.death_frame=21;old.marker_pattern=119;old.effect_parity=7;
        old.effects[4]={1,25,100,200,64,3,48,120,8};old.clones[1][2]={50,60};
        old.trail_group=26;old.trail_speed=64;old.direction=1;old.trail_delay=300;old.stream_speed=121;old.aimed_speed=132;
        old.origin_x=111;old.origin_y=112;old.destination_x=211;old.destination_y=212;old.travel_step=8;
        for(unsigned rank=0;rank<5;++rank){soew::MeiraCore b;b.init(rank,old);
            check(b.left[0]==192&&b.left[1]==192&&b.top[0]==48&&b.top[1]==48);
            check(b.pattern==141&&b.state==0&&b.attack_index==0&&!b.dying&&!b.clones_enabled&&b.damage_taken==0);
            check(b.phase_frame==17&&b.death_frame==21&&b.effects[4].delay==8&&b.effects[4].age==3);
            check(b.clones[1][2].left==50&&b.marker_pattern==119&&b.effect_parity==7&&b.trail_delay==300);
            check(b.trail_group==26&&b.trail_speed==64&&b.direction==1&&b.stream_speed==121&&b.aimed_speed==132);
            check(b.origin_x==111&&b.origin_y==112&&b.destination_x==211&&b.destination_y==212&&b.travel_step==8);
            check(b.special_turns_max==2&&b.special_angle==0);
        }
    }
    // Entrance exact event boundaries, no frame50 move,40 six-pixel steps.
    {
        auto b=make();std::vector<unsigned> sounds;soew::MeiraCore::Hooks h;h.sound=[&](unsigned n){sounds.push_back(n);};
        for(unsigned f=1;f<=91;++f){b.begin_frame(f&1);b.update(f&1,400,350,377,h);
            if(f==9)check(b.pattern==141&&b.top[f&1]==48);
            if(f==10)check(b.pattern==142&&b.top[f&1]==48);
            if(f==50)check(b.pattern==143&&b.top[f&1]==48);
            if(f==90)check(b.top[f&1]==288&&b.render_y==265);
        }
        check(b.phase_frame==0&&b.attack_index==1&&b.pattern==141&&sounds==std::vector<unsigned>({9,3}));
    }
    // Pascal DWORD movement args are high-word dx,low-word dy.
    {
        auto b=make();b.attack_index=1;b.left[0]=192;b.top[0]=288;
        at(b,20,{},400);check(b.direction==1&&b.pattern==145&&b.trail_delay==16&&b.marker_pattern==120&&b.trail_speed==64);
        at(b,21);check(b.left[0]==200&&b.top[0]==280&&b.trail_delay==18);
        check(b.effects[0].left==212&&b.effects[0].top==292&&b.effects[0].delay==15&&b.effects[0].tiny_pattern==120);
        at(b,56);check(b.pattern==144&&b.marker_pattern==121&&b.trail_delay==16);
        at(b,57);check(b.left[0]==192&&b.top[0]==272&&b.phase_frame==56&&b.trail_delay==18);
        b.top[0]=48;at(b,57);check(b.phase_frame==57&&b.top[0]==40);
        at(b,80);check(b.pattern==148&&b.trail_delay==16&&b.marker_pattern==120);
        at(b,81);check(b.left[0]==176&&b.top[0]==48&&b.trail_delay==18);
        b.left[0]=32;b.top[0]=50;at(b,82);check(b.top[0]==48&&b.pattern==141&&b.phase_frame==82);
        at(b,83);check(b.phase_frame==0&&b.attack_index==2);
        b.direction=0;b.left[0]=366;b.top[0]=0;b.trail_move(0,8,8);check(b.left[0]==368&&b.top[0]==8);
        b.direction=1;b.left[0]=18;b.trail_move(0,8,8);check(b.left[0]==16&&b.top[0]==16);
    }
    {
        auto b=make();b.attack_index=2;b.direction=1;b.trail_speed=64;
        at(b,50);check(b.pattern==142&&b.trail_group==26);
        at(b,99);check(b.pattern==148&&b.marker_pattern==121&&b.trail_delay==80);
        at(b,100);check(b.left[0]==200&&b.top[0]==56&&b.trail_delay==79);
        at(b,120);check(b.pattern==145&&b.marker_pattern==120&&b.trail_delay==78);
        at(b,136);check(b.left[0]==208&&b.top[0]==48&&b.trail_delay==77);
        at(b,156);check(b.left[0]==206&&b.pattern==141);
        b.left[0]=192;at(b,157);check(b.phase_frame==0&&b.attack_index==3);
    }
    // Gate uses previous accumulated damage and only after pattern completion.
    {
        auto b=make();b.damage_taken=701;at(b,91);check(b.state==0&&b.attack_index==1&&b.phase_frame==0);
        b.left[0]=32;b.top[0]=48;at(b,81);check(b.state==1&&b.attack_index==0&&b.phase_frame==0);
        b.damage_taken=1500;at(b,120);check(b.state==1&&b.attack_index==1);
        b.damage_taken=1501;at(b,224);check(b.state==2&&b.attack_index==0);
        auto c=make();c.damage_taken=700;c.attack_index=3;at(c,116);check(c.state==0&&c.attack_index==0);
        c.damage_taken=701;c.attack_index=3;at(c,116);check(c.state==1&&c.attack_index==0);
    }
    // Rank-dependent group byte, odd ticks and exact incremented speed.
    for(unsigned rank=0;rank<5;++rank){
        auto b=make(rank);b.attack_index=3;std::vector<soew::EnemyBulletRequest> bullets;
        soew::MeiraCore::Hooks h;h.bullet=[&](const auto& r){bullets.push_back(r);};
        for(unsigned f=50;f<=116;++f)at(b,f,h);
        check(bullets.size()==8&&b.phase_frame==0&&b.pattern==141);
        for(unsigned i=0;i<8;++i)check(bullets[i].left==216&&bullets[i].top==80&&bullets[i].group==(rank?21:9)&&bullets[i].angle==64&&bullets[i].speed==32+11*i);
        bullets.clear();b.state=1;b.attack_index=0;b.left[0]=176;
        for(unsigned f=50;f<=120;++f)at(b,f,h);
        check(bullets.size()==10&&b.left[0]==192&&b.attack_index==1&&b.phase_frame==0);
        for(unsigned i=0;i<10;++i)check(bullets[i].group==(rank?23:11)&&bullets[i].angle==0&&bullets[i].speed==32+11*i);
        bullets.clear();b.attack_index=2;
        for(unsigned f=50;f<=117;++f)at(b,f,h);
        check(bullets.size()==3&&b.phase_frame==0&&b.attack_index==0);
        for(const auto& r:bullets)check(r.group==(rank?23:11)&&r.speed==74&&r.angle==0);
    }
    {
        auto b=make();b.state=1;b.attack_index=1;at(b,50,{},400);check(b.direction==1&&b.trail_speed==48);
        at(b,99);check(b.pattern==149&&b.marker_pattern==121&&b.trail_delay==20);
        at(b,100);check(b.left[0]==200&&b.top[0]==56&&b.trail_delay==22);
        at(b,116);check(b.pattern==148&&b.marker_pattern==120&&b.trail_delay==20);
        at(b,136);check(b.left[0]==192&&b.top[0]==64);
        at(b,152);check(b.pattern==144&&b.marker_pattern==121&&b.trail_delay==20);
        at(b,172);check(b.left[0]==184&&b.top[0]==56);
        at(b,188);check(b.pattern==145&&b.marker_pattern==120&&b.trail_delay==20);
        at(b,208);check(b.left[0]==192&&b.top[0]==48);
        at(b,224);check(b.phase_frame==0&&b.attack_index==2&&b.pattern==141);
    }
    //40-slot pool: delay includes insertion tick; marker X inclusive,Y strict;
    //26..33 each persist four updates,then shoot and keep flag2 until bg pass.
    {
        auto b=make();b.marker_pattern=120;check(b.add_effect(100,100,64,25,54,2));
        unsigned hits=0;std::vector<unsigned> sounds;std::vector<soew::EnemyBulletRequest> bullets;
        soew::MeiraCore::Hooks h;h.hit=[&]{++hits;};h.sound=[&](unsigned n){sounds.push_back(n);};h.bullet=[&](auto r){bullets.push_back(r);};
        b.update(0,84,85,350,h);check(hits==1&&b.effects[0].delay==1&&b.effect_draw_count==1&&b.effect_draws[0].tiny&&b.effect_draws[0].top==50);
        b.update(0,100,100,350,h);check(hits==1&&b.effects[0].delay==0);
        for(unsigned f=0;f<32;++f){b.update(0,400,350,0,h);check(b.effect_draw_count==1&&!b.effect_draws[0].tiny&&b.effect_draws[0].pattern==26+f/4&&b.effects[0].age==f+1);}
        check(sounds==std::vector<unsigned>({10,9})&&b.effects[0].left==92&&b.effects[0].top==92);
        b.update(0,400,350,0,h);check(bullets.size()==1&&bullets[0].left==104&&bullets[0].top==104&&bullets[0].group==25&&bullets[0].angle==64&&bullets[0].speed==54&&b.effects[0].flag==2&&b.effect_draw_count==0);
        b.update(0,400,350,0,h);check(b.effects[0].flag==2&&bullets.size()==1);
        b.begin_frame(1);check(b.effects[0].flag==0);
        auto c=make();for(unsigned i=0;i<40;++i)check(c.add_effect(100,100,0,26,48,300));
        check(!c.add_effect(100,100,0,26,48,1)&&c.effects[39].delay==44);
    }
    for(unsigned rank=0;rank<5;++rank){
        auto b=make(rank);for(unsigned i=0;i<4;++i)b.effects[i]={1,26,100,100,0,32,48,120,0};
        unsigned bullets=0;soew::MeiraCore::Hooks h;h.bullet=[&](auto){++bullets;};b.update(0,400,350,0,h);
        check(bullets==(rank?4:2)&&b.effect_parity==4);for(unsigned i=0;i<4;++i)check(b.effects[i].flag==2);
    }
    // Boss collision asymmetry,48x48 damage box,lethal tick still flashes and
    // updates effects; phase threshold changes do not see same-tick damage.
    {
        auto b=make();b.attack_index=3;b.damage_taken=700;unsigned hits=0,score=0,inv=0,sound=0;
        soew::MeiraCore::Hooks h;h.hit=[&]{++hits;};h.damage=[&](int x,int y,int w,int height){check(x==200&&y==48&&w==48&&height==48);return 1u;};
        h.score=[&](unsigned n){score+=n;};h.invincibility=[&](unsigned n){inv=n;};h.sound=[&](unsigned n){sound=n;};
        at(b,116,h,192,33);check(hits==1&&b.state==0&&b.damage_taken==701&&b.hit_flash&&sound==4);
        h.damage={};at(b,1,h,224,33);check(hits==1);at(b,1,h,192,32);check(hits==1);at(b,1,h,192,80);check(hits==1);
        b.damage_taken=2399;h.damage=[](int,int,int,int){return 1u;};b.effects[0]={1,25,100,100,0,32,48,120,0};
        at(b,1,h);check(b.dying&&b.visible&&b.hit_flash&&score==30000&&inv==200&&b.effects[0].flag==2);
    }
    // Explicit RNG consumption:16-bit destination,8-bit Y and bounce angles.
    {
        auto b=make();b.state=2;std::vector<soew::EnemyBulletRequest> bullets;unsigned turns=0,r8=0,r16=0,s9=0;
        soew::MeiraCore::Hooks h;h.special_turns=[&](unsigned n){turns=n;};h.random8=[&]{return ++r8;};h.random16=[&]{++r16;return 639u;};h.bullet=[&](auto r){bullets.push_back(r);};h.sound=[&](unsigned n){if(n==9)++s9;};
        at(b,50,h);check(turns==5&&b.special_turns_max==5);
        at(b,99,h);check(bullets.size()==3&&r8==3&&r16==0&&b.attack_index==1&&b.phase_frame==0&&b.clones_enabled);
        for(unsigned i=0;i<3;++i)check(bullets[i].kind==soew::EnemyBulletRequest::Bullet16&&bullets[i].angle==i+1&&bullets[i].group==0x87&&bullets[i].pattern==122+(i&1)&&bullets[i].speed==54);
        at(b,50,h);check(r16==1&&r8==4&&b.destination_x==351&&b.destination_y==20&&b.travel_step==1&&b.clone_draw_count==0);
        at(b,51,h);check(b.left[0]==194&&b.top[0]==48&&b.clone_draw_count==0);
        // Run every travel tick to include the source's pre-move8-frame trail.
        for(unsigned f=52;f<=114;++f)at(b,f,h);
        check(b.left[0]==351&&b.top[0]==20&&b.travel_step==65&&s9==1);
        check(b.clone_draw_count>0);at(b,115,h);check(b.clone_draw_count==0&&s9==1);
        at(b,164,h);check(bullets.size()==5&&r8==6&&b.phase_frame==0&&b.attack_index==1&&bullets[3].pattern==122&&bullets[4].pattern==123);
        b.clones[0][0]={100,100};b.clones[0][1]={b.left[0],100};b.clones[0][2]={100,b.top[0]};b.clone_draw_count=0;b.clone_render(0,377);
        check(b.clone_draw_count==1&&b.clone_draws[0].top==77);b.begin_frame(1);check(b.clones[1][0].left==100);
    }
    // Product division truncates toward zero and wraps to the signed low word.
    {
        auto b=make();b.state=2;b.attack_index=1;b.origin_x=0;b.origin_y=0;b.destination_x=32767;b.destination_y=-1;b.travel_step=2;
        at(b,51);check(b.left[0]==0&&b.top[0]==0&&b.travel_step==3);
        bool rejected=false;b.phase_frame=49;try{b.update(0,400,350,0,{});}catch(const std::runtime_error&){rejected=true;}check(rejected);
    }
    // Death helper uses independent inherited counter; two explosion arguments
    // and speed60,count1 spark persist on completion, effects do not.
    {
        auto b=make();b.dying=true;b.phase_frame=30;unsigned sparks=0,explosions=0;std::vector<int> args;
        soew::MeiraCore::Hooks h;h.explosion=[&](int x,int y,int n){check(x==216&&y==72);args.push_back(n);++explosions;};h.spark=[&](int x,int y,int speed,int count,bool sprite){check(x==240&&y==96&&speed==60&&count==1&&!sprite);++sparks;};
        b.update(0,400,350,377,h);check(b.visible&&!b.death_zoom&&b.death_pattern==7&&args[0]==0&&args[1]==-24&&b.render_y==25);
        b.update(0,400,350,377,h);check(b.death_zoom&&b.death_pattern==7);
        for(unsigned f=2;f<96;++f)b.update(0,400,350,0,h);
        check(b.done&&!b.visible&&b.death_frame==0&&sparks==96&&explosions==192);
        b.update(0,400,350,0,h);check(sparks==96);
        auto c=make();c.dying=true;c.death_frame=0x7fff;c.update(0,400,350,0,{});check(!c.done&&c.death_frame==0x8000);
        c.death_frame=0xffff;c.update(0,400,350,0,{});check(!c.done&&c.death_frame==0&&c.death_pattern==7);
        c.death_frame=95;c.effects[0]={1,25,100,100,0,32,48,120,0};c.update(0,400,350,0,{});check(c.done&&c.effects[0].flag==1);
    }
    std::cout<<"meira_core_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
