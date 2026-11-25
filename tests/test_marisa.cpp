#include "encounter_scene.hpp"
#include <cmath>
#include <iostream>
#include <numbers>
#include <vector>

namespace {
void check(bool ok,const char *message){if(!ok)throw std::runtime_error(message);}
std::uint32_t rgb(const soew::StageBackground &bg,unsigned index){
    const auto c=bg.palette[index];return (unsigned(c[0])<<16)|(unsigned(c[1])<<8)|c[2];
}
std::size_t pixel(int x,int y){return std::size_t(y*640+x);}

void test_art(const std::filesystem::path &root){
    using Scene=soew::EncounterScene;
    using Core=soew::MarisaCore;
    using Phase=soew::StageTransition::Phase;
    auto s=Scene::load(root,3,0,1,2,3);
    s.enable_preboss();s.preboss_started=true;s.dialogue.current=s.dialogue.pre_count;
    s.runtime.scroll.done=true;s.bind_marisa_battle();
    s.transitions_enabled=s.survival_enabled=true;
    s.reduced_effects=true;
    const auto &art=*s.stage4_boss_art;
    const auto col=[&](unsigned i){return rgb(s.background,i);};
    std::vector<std::uint32_t> actual(640*400),expected(actual.size());
    const auto plot=[&](int x,int y,std::uint32_t c){
        if(x>=32&&x<416&&y>=16&&y<384)expected[pixel(x,y)]=c;
    };
    const auto cel=[&](unsigned id,int left,int top,int scale=1){
        const auto &g=*art.sprites.at(id);
        for(unsigned y=0;y<g.height;++y)for(unsigned x=0;x<g.width;++x){
            const auto i=g.pixels[y*g.width+x];
            if(i==(scale==3?0:g.transparent))continue;
            for(int dy=0;dy<scale;++dy)for(int dx=0;dx<scale;++dx)
                plot(left+int(x)*scale+dx,top+int(y)*scale+dy,col(i));
        }
    };
    const auto length=[](int radius,int angle){
        const auto table=std::round(256*std::sin(angle*std::numbers::pi/128));
        return int(std::floor(radius*table/256));
    };
    s.marisa.defeated=true;s.marisa.left=245;s.marisa.top=114;
    s.marisa.orbs[3]={56,300,0,0,true};
    soew::MarisaStarfield stars;
    for(unsigned t=0;t<100;++t){
        stars.tick(t,true);
        s.tick(0);
        const auto period=t<32?1u:(t>=53&&t<=55)||(t>=77&&t<=79)?2u:3u;
        if(s.presentation_slowdown()!=period)
            throw std::runtime_error("Unexpected Marisa slowdown at t="+std::to_string(t));
        if(t!=0&&t!=24&&t!=30&&t!=31&&t!=32&&t!=33&&t!=48&&t!=53&&t!=56&&t!=57&&t!=64&&t!=77&&t!=80&&t!=85&&t!=99)continue;
        std::fill(expected.begin(),expected.end(),0);
        stars.render(expected,s.background.palette,soew::motion_subpixels);
        for(unsigned lag:{0u,24u,48u})if(t>=lag){
            const auto f=t-lag;
            if(f<30){
                const int r=240-8*int(f),width=1+r/64;
                for(int a=0;a<256;a+=2){
                    const int x=292+length(r,a+64),y=154+length(r,a);
                    if(x<25||x>415||y<9||y>383)continue;
                    for(int dy=0;dy<width;++dy)for(int dx=0;dx<width;++dx)
                        plot(x+dx,y+dy,col(f%2?15:4));
                }
            }else if(f<56&&f>=32){
                const int age=int(f)-32,r=16*age;
                const int shift=t>80||(t==80&&lag==48)?0:t>56||(t==56&&lag>=24)?32:-32;
                for(int a=0;a<256;a+=4){
                    const int x=285+length(r,a+age+64),y=154+length(r,a+age+shift);
                    if(x>=17&&x<=415&&y>=1&&y<=383)cel(unsigned(88+age/6),x,y);
                }
            }
        }
        if(t<31){cel(128,245,114);cel(129,293,114);}
        else cel(t==31?9:10+(t-32)/16,245,114,3);
        if(t<99){plot(72,316,col(3));cel(141,56,300);}
        // Transparent player disabled; combat effects must overdraw the starfield.
        const auto old_pattern=s.life.pattern;
        s.life.pattern.reset();s.render(actual);
        check(actual==expected,"Marisa effect or layer mismatch against original artwork");
        if(t==33||t==53||t==57||t==77||t==85||t==99){
            s.reduced_effects=false;s.render(actual);s.reduced_effects=true;
            const auto bright=[](std::uint32_t color){
                std::uint32_t value=0;
                for(unsigned component:{0u,8u,16u}){
                    const auto src=(color>>(4+component))&15;
                    value|=(17*(15-3*(15-src)/10))<<component;
                }
                return value;
            };
            const bool shifted=t!=53&&t!=77;
            const bool flashed=t!=53&&t!=77&&t!=99;
            for(int y=16;y<384;++y)for(int x=32;x<400;++x){
                const auto source=expected[pixel(x+(shifted?16:0),y)];
                check(actual[pixel(x,y)]==(flashed?bright(source):source),"Marisa one-word shift and tone composition");
            }
        }
        s.life.pattern=old_pattern;
    }
    check(s.marisa_terminal_frame()&&s.transition.phase==Phase::Battle,"public terminal frame is still a battle frame");
    s.life.pattern.reset();const auto ticks=s.ticks;s.tick(0);
    check(s.ticks==ticks&&!s.marisa_terminal_frame()&&s.dialogue.postboss&&s.transition.phase==Phase::Postboss,
        "clear enters postboss on the next tick");
    check(std::all_of(s.dialogue_snapshot.begin(),s.dialogue_snapshot.end(),[](auto c){return c==0;}),
        "terminal boss and ring are absent from the dialogue background");

    s.marisa={};s.marisa.left=176;s.marisa.top=32;
    s.dialogue.phase=soew::StageDialogue::Phase::Complete;
    s.transition.phase=Phase::Battle;s.dialogue_snapshot.clear();
    constexpr std::array<std::array<int,2>,4> centers={{{80,170},{320,170},{320,310},{80,310}}};
    const auto draw=[&]{std::fill(actual.begin(),actual.end(),0);s.draw_marisa(actual);};
    const auto at=[&](int x,int y){return actual[pixel(x,y)];};
    const auto body_pixels=[&](bool hit){
        draw();
        for(unsigned part=0;part<2;++part){
            const auto &g=*art.sprites.at(s.marisa.pattern+part);
            for(unsigned y=0;y<g.height;++y)for(unsigned x=0;x<g.width;++x){
                const auto i=g.pixels[y*g.width+x];
                check(at(s.marisa.left+int(part*48+x),s.marisa.top+int(y))==(i==g.transparent?0u:col(hit?4u:i)),
                    "Marisa body put mask or normal art mismatch");
            }
        }
    };
    s.marisa.left=177; // Non-byte-aligned put mask.
    body_pixels(false);
    Core::Hooks body_hit;body_hit.damage=[](int,int,int,int){return 1u;};
    s.marisa.update(body_hit);
    check(s.marisa.body_flash&&!s.marisa.body_damage_enabled&&s.marisa.damage_taken==0,"exposed hit gives feedback without opening the HP gate");
    body_pixels(true);
    s.reduced_effects=false;body_pixels(true);s.reduced_effects=true;
    s.marisa.defeated=true;body_pixels(false); // No palette-4 silhouette in the death branch.
    s.marisa.defeated=false;
    s.marisa.orbs[0].active=true;s.marisa.update(body_hit);
    check(!s.marisa.body_flash,"shielded hit must not set the body latch");
    s.marisa.orbs[0]={};body_pixels(false);
    check(at(80,170)==0&&at(200,170)==0,"zero active orbs produce no line");
    auto &orbs=s.marisa.orbs;
    for(unsigned i=0;i<4;++i)orbs[i]={centers[i][0]-16,centers[i][1]-16,0,0,true};
    draw();check(at(200,170)==col(3)&&at(320,240)==col(3)&&at(200,310)==col(3)&&at(80,240)==col(3),
        "four slots close in slot order");
    const auto &orb1=*art.sprites[139];
    check(orb1.pixels[16*32+16]!=orb1.transparent&&at(320,170)==col(orb1.pixels[16*32+16]),
        "live cel covers its line endpoint");
    orbs[1].hit_flash=true;draw();
    check(at(320,170)==col(15)&&at(200,170)==col(3),"nonlethal orb flash still covers its live line endpoint");
    orbs[1].hit_flash=false;orbs[1].dying=true;draw();
    check(at(200,170)==0&&at(200,240)==col(3)&&at(200,310)==col(3)&&at(80,240)==col(3),
        "three live slots skip a retiring middle slot and close a triangle");
    orbs[3].active=false;draw();
    check(at(200,240)==col(3)&&at(80,240)==0&&at(200,310)==0,"two active orbs share a single segment");
    orbs[0].active=false;draw();
    check(at(200,240)==0&&at(304,310)==0,"one active orb cannot create an extra segment");
    std::vector<std::array<int,2>> clip;
    Scene::marisa_orb_line({16,383},{420,399},[&](int x,int y){clip.push_back({x,y});});
    check(clip.front()==std::array{32,384}&&clip.back()==std::array{416,398},"original line clipping and rounded endpoints");
    clip.clear();Scene::marisa_orb_line({416,399},{416,399},[&](int x,int y){clip.push_back({x,y});});
    check(clip==std::vector{std::array{416,399}},"single-orb degenerate inclusive line endpoint");

    s.marisa={};s.marisa.phase=Core::Phase::DeployOrbs;s.marisa.phase_frame=129;
    s.marisa.update({});
    check(s.marisa.orbs[0].x==216&&s.marisa.orbs[1].y==104,"deployment moves immediately on the rendered page");
    const auto moved=s.marisa.orbs[0];
    Core::Hooks h;h.orb_damage=[](int,int,int,int){return 1u;};h.random8=[](){return 0u;};
    s.marisa.orbs[0].hits=109;s.marisa.update(h);
    check(s.marisa.orbs[0].dying&&s.marisa.orbs[0].x==moved.x,"lethal orb hit retires before its movement");
    s.marisa.phase=Core::Phase::OrbRelease;s.marisa.phase_frame=109;s.marisa.update(h);
    check(std::all_of(orbs.begin(),orbs.end(),[](const auto &o){return o.active&&o.dying;}),"release removes remaining live slots that tick");
}
}

int main(int argc,char **argv){try{
    using Core=soew::MarisaCore;
    Core m; Core::Hooks h;
    std::vector<soew::EnemyBulletRequest> bullets;
    std::vector<unsigned> sounds;
    h.bullet=[&](const auto &r){bullets.push_back(r);};
    h.sound=[&](unsigned n){sounds.push_back(n);};
    for(unsigned f=1;f<=130;++f){
        const auto old=bullets.size();m.update(h);
        const bool fires=f>100&&f<130&&f%3==0;
        check(bullets.size()==old+(fires?2:0),"opening cadence");
        if(fires)for(unsigned i=0;i<2;++i){const auto &r=bullets[old+i];
            check(r.left==(i?228:212)&&r.top==128&&r.angle==24+10*((f-102)/3)&&r.group==0x20&&r.speed==60,"opening bullet");}
        if(f==50||f==100||f==108||f==116)check(m.pattern==(f==50?130:f==100?132:f==108?134:136),"opening pose");
    }
    check(m.phase==Core::Phase::DeployOrbs&&m.phase_frame==0&&m.pattern==128,"opening transition");
    check(sounds==std::vector<unsigned>{9,10,10,10},"opening sounds");
    for(unsigned f=1;f<=154;++f){
        m.update(h);
        for(unsigned i=0;i<4;++i){const auto &o=m.orbs[i];
            check(o.active==(f>=130),"orb activation");
            if(f>=130){const unsigned movement= f-129;
                check(o.radius==8+2*int(f<154?f-130:23)&&o.angle==std::uint8_t(i*64+2*movement),"orb orbit timing");
            }
        }
        if(f==100)check(m.pattern==130&&sounds.back()==9,"orb charge pose");
        if(f==130)check(m.orbs[0].x==216&&m.orbs[0].y==96,"orb first position");
    }
    check(m.phase==Core::Phase::ArcFlight&&m.phase_frame==0&&m.pattern==128,"orb deployment leads into arc flight");
    unsigned flight_rolls=0;
    h.random8=[&]{++flight_rolls;return 0u;};
    for(unsigned f=1;f<=266;++f){
        const auto before=bullets.size();const auto orb=m.orbs[0];
        m.update(h,{0,0,0,300});
        check(bullets.size()==before+(f%16==0?1u:0u),"arc flight firing cadence");
        if(f%16==0){const auto &b=bullets.back();check(b.left==orb.x+12&&b.top==orb.y+12&&b.group==0x19&&b.speed==80,"orb volley uses position before orbit movement");}
        if(f==9)check(m.left==176&&m.top==64,"arc flight waits nine ticks");
        if(f==10)check(m.flight_direction==1&&m.flight_angle==193&&m.left==178&&m.top==64,"arc flight starts toward current player X");
    }
    check(m.phase==Core::Phase::AttackWait&&m.phase_frame==0&&flight_rolls==16,"arc ends on tick 266");
    for(unsigned f=1;f<=30;++f)m.update(h);
    check(m.phase==Core::Phase::AttackWait&&flight_rolls==16,"attack wait consumes no RNG");
    m.update(h);check(m.phase==Core::Phase::ArcFlight&&m.phase_frame==1&&m.pending_attack==1&&flight_rolls==17,"selected flight resumes at frame one");
    for(unsigned pick=0;pick<3;++pick){
        Core a;Core::Hooks hooks;
        a.phase=Core::Phase::AttackWait;a.phase_frame=30;
        std::vector<soew::EnemyBulletRequest> emitted;
        std::vector<unsigned> drifts;
        hooks.random8=[&]{return pick;};
        hooks.bullet=[&](const auto &b){emitted.push_back(b);};
        hooks.drift=[&](unsigned angle,unsigned speed,unsigned frames){check(speed==2&&frames==128,"star drift settings");drifts.push_back(angle);};
        a.update(hooks,{0,0,0,300,0});
        check(a.phase==Core::Phase::ExposedAttack&&a.pending_attack==-1-int(pick)&&a.phase_frame==1,"signed attack selection");
        const unsigned end=pick==1?193:130;
        for(unsigned f=2;f<=end;++f){
            const auto n=emitted.size();a.update(hooks,{0,0,0,300,0});
            const unsigned expected=pick==0?(f>100&&f<130&&f%3==0):pick==1?(f>=72&&f<=192&&f%24==0?2u:0u):(f>=100&&f<130&&f%4==0);
            check(emitted.size()==n+expected,"exposed attack cadence");
        }
        check(a.phase==Core::Phase::AttackWait&&a.phase_frame==0,"exposed attack returns to wait");
        if(pick==0)check(emitted.size()==10&&emitted.front().angle==24&&emitted.front().speed==50,"single sweep variant");
        if(pick==1)check(emitted.size()==12&&emitted[0].group==0x18&&emitted[1].group==0x12,"Easy spread variant");
        if(pick==2)check(drifts==std::vector<unsigned>{2}&&a.drift_angle==254&&emitted.size()==8&&emitted.front().pattern==85&&emitted.back().angle==47,"drifting star variant");
    }
    {
        Core a;Core::Hooks hooks;a.orbs[0].active=true;a.orbs[0].radius=54;
        a.phase=Core::Phase::AttackWait;a.phase_frame=30;
        unsigned shots=0;hooks.random8=[](){return 1u;};
        hooks.bullet=[&](const auto &b){++shots;check(b.speed==50&&b.group==0x20,"shielded expanding orb bullet");};
        a.update(hooks,{0,0,0,300,0});
        check(a.phase==Core::Phase::ExpandingOrbs,"select expanding orbs");
        for(unsigned f=2;f<=510;++f){a.update(hooks,{0,0,0,300,0});
            if(f==279)check(a.orbs[0].radius==284,"maximum radius");
            if(f==509)check(a.orbs[0].radius==54,"radius returns to initial value");
        }
        check(a.phase==Core::Phase::AttackWait&&shots==10,"Easy emission only on even ticks in shielded sweep");
    }
    {
        Core a;Core::Hooks hooks;unsigned rewards=0;
        hooks.score=[&](unsigned){++rewards;};
        // Seed the dispatcher at the finish of the exposed rounds; not a continuous encounter test.
        a.rounds=1;a.exposed_selections=2;a.phase=Core::Phase::AttackWait;a.phase_frame=30;
        a.update(hooks);check(a.rounds==2&&a.body_damage_enabled&&a.phase==Core::Phase::DeployOrbs&&!a.defeated,"second round enables body damage");
        a.rounds=6;a.exposed_selections=2;a.phase=Core::Phase::AttackWait;a.phase_frame=30;
        a.update(hooks);check(a.rounds==7&&a.defeated&&!a.cleared&&a.death_frame==1,"seventh round starts retirement");
        for(unsigned f=1;f<100;++f)a.update(hooks);
        check(a.cleared&&rewards==0,"round timeout clears without damage rewards");
    }
    // Run the actual dispatcher to the no-shot timeout, without forcing states.
    for(unsigned choice=0;choice<5;++choice){
        Core a;Core::Hooks hooks;
        unsigned rewards=0,stock_grants=0;std::size_t emitted=0;
        const auto selected=std::array{Core::Phase::ArcFlight,Core::Phase::ExpandingOrbs,
            Core::Phase::OrbVolley,Core::Phase::StarRain,Core::Phase::SplitOrbits}[choice];
        bool saw_selection=false,saw_release=false,saw_exposed=false;
        hooks.random8=[&]{return choice;};hooks.random16=[](){return 123u;};
        hooks.bullet=[&](const auto &){++emitted;};
        hooks.score=[&](unsigned){++rewards;};hooks.invincibility=[&](unsigned){++stock_grants;};
        for(unsigned tick=0;tick<30000&&!a.cleared;++tick){
            a.update(hooks,{208,336,0,208,1});
            saw_selection|=a.phase==selected&&a.pending_attack==int(choice)+1;
            saw_release|=a.phase==Core::Phase::OrbRelease;
            saw_exposed|=a.phase==Core::Phase::ExposedAttack;
        }
        check(saw_selection&&saw_release&&saw_exposed&&emitted>0,"continuous dispatcher traverses selected attack and orb release");
        check(a.cleared&&a.rounds==7&&a.death_frame==100&&rewards==0&&stock_grants==0,"continuous no-shot timeout through death and clear");
    }
    {
        // Continuous damage route: let the real dispatcher open the body gate.
        Core a;Core::Hooks hooks;unsigned body_rewards=0,orb_rewards=0,grants=0;
        hooks.random8=[](){return 0u;};
        hooks.orb_damage=[](int,int,int,int){return 1u;};
        hooks.damage=[](int,int,int,int){return 10u;};
        hooks.score=[&](unsigned n){if(n==20000)++body_rewards;if(n==5000)++orb_rewards;};
        hooks.invincibility=[&](unsigned n){check(n==200,"body defeat invincibility");++grants;};
        for(unsigned tick=0;tick<30000&&!a.cleared;++tick)a.update(hooks,{208,336,80,208,1});
        check(a.cleared&&a.damage_taken>=900&&a.rounds>=2&&a.rounds<7,"continuous damage clear before timeout");
        check(body_rewards==1&&orb_rewards>=8&&grants==1,"continuous clear awards defeat exactly once");
    }
    for(const unsigned power:{79u,80u}){
        Core combat;Core::Hooks hooks;
        for(unsigned i=0;i<260;++i)combat.update(hooks);
        check(combat.phase==Core::Phase::DeployOrbs&&combat.orbs[0].active,"natural orb setup");
        unsigned score=0,drops=0,contacts=0,rolls=0,orbit_rolls=0,sparks=0,queries=0;
        auto &orb=combat.orbs[0];orb.hits=108;
        hooks.orb_damage=[&](int x,int y,int w,int height){
            check(w==24&&height==32,"orb hitbox");
            return queries++%4==0&&x==orb.x+4&&y==orb.y?900u:0u;
        };
        hooks.random8=[&]{if(orb.hits==110&&!orb.dying)++rolls;else ++orbit_rolls;return 79u;};
        hooks.score=[&](unsigned n){score+=n;};
        hooks.item=[&](int x,int y,unsigned type){check(x==orb.x+8&&y==orb.y+8&&type==3,"orb drop parameters");++drops;};
        hooks.player_hit=[&]{++contacts;};
        hooks.spark=[&](const auto &s){check(s.x==orb.x+16&&s.y==orb.y+16&&s.speed==60&&s.count==2&&!s.sprite,"orb death spark");++sparks;};
        combat.update(hooks,{orb.x,orb.y,power});
        check(orb.hits==109&&orb.hit_flash&&!orb.dying&&score==0&&rolls==0&&contacts>0,"one count per nonzero hit and front-page contact");
        combat.update(hooks,{orb.x,orb.y,power});
        check(orb.dying&&orb.death_frame==1&&orb.death_pattern==10&&!orb.hit_flash&&score==5000&&rolls==1&&drops==(power==79?1u:0u)&&sparks==1,"lethal hit and power-dependent drop");
        const auto x=orb.x,y=orb.y;
        hooks.orb_damage={};
        for(unsigned i=2;i<=48;++i){
            combat.update(hooks,{-1000,-1000,power});
            check(orb.x==x&&orb.y==y,"dying orb stops moving");
            check(orb.active==(i<48),"48-tick orb retirement");
        }
        check(!orb.dying&&orb.hits==0&&orb.death_frame==0&&sparks==48&&score==5000&&rolls==1,"orb retirement clears transient state once");
    }
    {
        Core body;Core::Hooks hooks;
        unsigned dealt=899,queries=0,score=0,invincibility=0,contacts=0;
        std::vector<unsigned> feedback;
        hooks.damage=[&](int x,int y,int w,int height){
            check(x==200&&y==88&&w==48&&height==48,"Marisa body hitbox");
            ++queries;return dealt;
        };
        hooks.score=[&](unsigned n){score+=n;};
        hooks.invincibility=[&](unsigned n){invincibility=n;};
        hooks.sound=[&](unsigned id){feedback.push_back(id);};
        hooks.player_hit=[&]{++contacts;};
        body.update(hooks,{160,64,80});
        check(body.target_x==216&&body.target_y==104&&body.body_flash&&body.damage_taken==0&&contacts==0,"opening multiplier and strict contact edge");
        body.body_damage_enabled=true;body.orbs[0].active=true;body.orbs[0].dying=true;
        body.update(hooks,{161,64,80});
        check(body.target_x==-1&&body.target_y==-1&&!body.body_flash&&body.damage_taken==0&&contacts==1,"dying orb still shields the body");
        body.orbs[0]={};body.update(hooks,{-1000,-1000,80});
        check(body.damage_taken==899&&body.body_flash&&!body.defeated&&score==0,"exposed body takes damage below defeat threshold");
        dealt=1;body.update(hooks,{-1000,-1000,80});
        check(body.defeated&&body.damage_taken==900&&score==20000&&invincibility==200,"Marisa body defeat rewards");
        const auto stopped=body.phase_frame;
        check(body.death_frame==1&&!body.cleared,"lethal tick starts death without clearing");
        unsigned bursts=0;
        hooks.spark=[&](const auto &s){
            check(s.x==224&&s.y==112&&s.speed==136&&s.count==24&&s.sprite,"body death spark");
            ++bursts;
        };
        for(unsigned t=0;t<100;++t){
            if(t)body.update(hooks,{-1000,-1000,80});
            check(body.death_frame==t+1&&body.cleared==(t==99),"clear only at end of death timeline");
            check(body.death_effect_count==1u+unsigned(t>=24)+unsigned(t>=48),"staggered body death events");
            for(unsigned i=0;i<body.death_effect_count;++i){
                const auto &e=body.death_effects[i];
                const unsigned a=t>80||(t==80&&i==2)?0:t>56||(t==56&&i>=1)?0x20:0xe0;
                check(e.x==216&&e.y==104&&e.frame==t-24*i&&e.angle==a,"ordered effects and angle changes");
            }
            check(body.death_zoom==(t>=31&&t<99),"death zoom transition");
            if(t==31||t==32||t==96)check(body.death_pattern==(t==31?9u:t==32?10u:14u),"death sprite timing");
        }
        body.update(hooks,{-1000,-1000,80});
        check(body.cleared&&body.death_effect_count==0&&body.phase_frame==stopped&&queries==4&&score==20000&&bursts==3,"clear does not replay effects or rewards");
        check(feedback==std::vector<unsigned>{4,11,4,4,18,18,14,18,14,14},"death sounds follow ordered effect frames");
    }
    if(argc!=2)throw std::runtime_error("Expected original resources path");
    test_art(argv[1]);
    std::cout<<"Marisa combat, retirement and orb presentation passed\n";return 0;
}catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}}
