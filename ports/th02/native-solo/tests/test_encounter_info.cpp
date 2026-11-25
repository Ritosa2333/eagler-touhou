#include "encounter_info.hpp"
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace {
using namespace soew;
using Kind=EncounterInfoKind;
using Protection=EncounterProtectionKind;
using TargetState=EncounterTargetState;
unsigned checks=0;
void check(bool v,const char *message){++checks;if(!v)throw std::runtime_error(message);}

void early_encounters(){
    EncounterScene s;
    check(!encounter_info(s).active,"empty scene");
    s.midboss_enabled=true;s.midboss.active=s.midboss.present=true;s.midboss.damage_taken=300;s.runtime.scroll.progress=183;
    auto x=encounter_info(s);
    check(x.kind==Kind::Midboss1&&x.health==1&&x.max_health==301&&x.timeout==1,"midboss1 threshold");
    s.midboss.damage_taken=301;s.runtime.scroll.progress=184;x=encounter_info(s);
    check(!x.health&&x.pending&&x.timeout_pending&&!x.dying,"midboss1 coordinate/departure gates");
    s.midboss.dying=true;x=encounter_info(s);
    check(x.dying&&!x.pending&&!x.has_timeout,"midboss1 retirement has no timer");
    s.midboss.present=false;s.midboss2_enabled=true;s.midboss2.active=s.midboss2.present=true;s.midboss2.damage_taken=380;s.midboss2.phase_frame=1600;
    x=encounter_info(s);check(x.kind==Kind::Midboss2&&x.health==1&&x.timeout==1,"midboss2 threshold");
    s.midboss2.damage_taken=381;s.midboss2.phase_frame=1601;x=encounter_info(s);
    check(!x.health&&!x.timeout&&x.timeout_pending&&x.pending,"midboss2 timing/coordinate boundaries");
    s.midboss2.active=false;s.rika_enabled=true;s.rika.damage_taken=699;x=encounter_info(s);
    check(x.kind==Kind::Rika&&x.phase==0&&x.health==1,"rika phase 1");
    s.rika.damage_taken=700;x=encounter_info(s);
    check(x.phase==1&&x.health==700&&x.total_health==1561&&x.phase_cut_count==2&&x.phase_cuts[0]==861&&x.phase_cuts[1]==1561,"rika phase 2 and stable full-bar cuts");
    s.rika.damage_taken=1400;x=encounter_info(s);
    check(x.phase==2&&x.health==861&&!x.has_timeout,"rika phase 3 has no timer");
    s.rika.done=true;s.meira_enabled=true;s.meira.state=1;s.meira.damage_taken=1501;x=encounter_info(s);
    check(x.kind==Kind::Meira&&x.phase==1&&x.pending&&x.health==0&&x.total_health==899,"meira phase wait");
    s.meira.damage_taken=1511;x=encounter_info(s);
    check(x.pending&&x.total_health==889&&x.protection.kind==Protection::None,"meira can still take damage during the wait");
    MeiraCore::Hooks meira_hit;
    meira_hit.damage=[](int,int,int,int){return 11u;};
    s.meira.phase_frame=1;s.meira.update(0,208,336,1,meira_hit);
    x=encounter_info(s);
    check(s.meira.state==1&&s.meira.damage_taken==1522&&x.pending&&x.protection.kind==Protection::None,
        "meira's real damage query accrues damage before advancing the phase");
    s.meira.damage_taken=1511;
    s.meira.state=2;x=encounter_info(s);
    check(!x.pending&&x.phase==2&&x.health==889&&x.phase_cuts[0]==899&&x.phase_cuts[1]==1699,"meira advances only at the core's phase boundary");
}

void midboss_damage_protection(){
    EncounterScene s;
    unsigned queries=0;
    Midboss1Core::Hooks h;
    h.damage=[&](int,int,int,int){++queries;return 24;};
    auto &one=s.midboss;
    s.midboss_enabled=true;one.trigger(116);
    for(unsigned i=0;i<17;++i)one.update(0,1,116,h);
    auto p=encounter_info(s).protection;
    check(one.phase_frame==17&&!queries&&!one.damage_taken&&p.kind==Protection::TimedLock&&p.frames==1,
        "midboss1 frame 17: no query and one update until damage");
    one.update(0,1,116,h);
    check(one.phase_frame==18&&queries==1&&one.damage_taken==24&&encounter_info(s).protection.kind==Protection::None,
        "midboss1 frame 18: first damage query accepts full damage");
    one.active=false;

    auto &two=s.midboss2;
    s.midboss2_enabled=true;two.trigger(80);queries=0;
    p=encounter_info(s).protection;
    check(p.kind==Protection::TimedLock&&p.frames==2,"midboss2 initialization skips its damage query");
    for(unsigned i=0;i<319;++i)two.update(0,1,h);
    p=encounter_info(s).protection;
    check(two.phase_frame==319&&queries==318&&two.damage_taken==318&&p.kind==Protection::SingleDamage&&p.frames==1,
        "midboss2 frame 319: one point per query, one update until full damage");
    two.update(0,1,h);
    check(two.phase_frame==320&&queries==319&&two.damage_taken==342&&encounter_info(s).protection.kind==Protection::None,
        "midboss2 frame 320: query accepts full damage");
}

void third_stage(){
    using Flag=StonesCore::Flag;
    using Phase=StonesCore::Phase;
    EncounterScene s;
    s.midboss3_enabled=true;s.midboss3.active=true;s.midboss3.phase_frame=240;
    s.midboss3.actor[0].damage=280;s.midboss3.actor[1].damage=100;s.midboss3.cycles=2;
    auto x=encounter_info(s);
    check(x.kind==Kind::Midboss3&&x.target_count==2&&!x.has_health&&!x.total_max_health&&x.timeout==2,"stage3 midboss has separate health and real cycle count");
    check(x.targets[0].health==1&&x.targets[1].health==181&&x.targets[1].max_health==281,"stage3 midboss actor thresholds");
    s.midboss3.actor[0].flag=Midboss3Core::Flag::Removed;x=encounter_info(s);
    check(x.target_count==1&&std::wstring_view(x.targets[0].label)==L"RIGHT","stage3 removed actor no longer displayed");
    s.midboss3.actor[1].flag=Midboss3Core::Flag::Dying;x=encounter_info(s);
    check(x.dying&&x.targets[0].state==TargetState::Dying&&!x.targets[0].health&&!x.has_timeout,"stage3 timed midboss retirement");
    s.midboss3.active=false;s.stones_enabled=true;x=encounter_info(s);
    check(x.kind==Kind::Stones&&!x.has_health&&!x.target_count&&!x.has_timeout,"dormant stones lack an artificial pooled HP bar");
    s.stones.phase=Phase::Inner;s.stones.elapsed=1300;s.stones.stones[0].flag=Flag::Active;s.stones.stones[1].flag=Flag::Active;
    s.stones.stones[0].damage=139;s.stones.stones[1].damage=40;x=encounter_info(s);
    check(x.target_count==2&&x.targets[0].health==1&&x.targets[1].health==100&&x.timeout==1,"inner pair independent health/time");
    s.stones.phase=Phase::Outer;s.stones.stones[2].flag=Flag::Active;s.stones.stones[3].flag=Flag::Dying;x=encounter_info(s);
    check(x.phase==1&&x.target_count==2&&x.targets[0].max_health==120&&x.targets[1].state==TargetState::Dying,"outer pair separate actor threshold");
    s.stones.phase=Phase::NorthFirstAttacks;s.stones.stones[4].flag=Flag::Active;s.stones.stones[4].damage=500;x=encounter_info(s);
    check(x.has_health&&x.health==430&&!x.has_timeout&&std::wstring_view(x.status)==L"DAMAGE CAPPED","north stone uses the 930 knockout threshold, not the 500 cap");
    s.stones.phase=Phase::NorthSecondAttacks;s.stones.stones[4].flag=Flag::Dormant;x=encounter_info(s);
    check(x.phase==3&&x.health==430&&std::wstring_view(x.status)==L"SEALED"&&!x.has_timeout,"north stone retains damage while sealed");
    s.stones.phase=Phase::NorthFinalAttacks;s.stones.stones[4].flag=Flag::Active;s.stones.elapsed=2001;x=encounter_info(s);
    check(x.phase==4&&x.has_timeout&&x.timeout==0&&x.timeout_pending&&!x.dying,"north timeout gates at a later attack boundary");
    s.stones.phase=Phase::NorthDeath;x=encounter_info(s);
    check(x.dying&&!x.health&&!x.has_timeout,"north timeout retirement zeros health");
}

void north_stone_damage_protection(){
    using Flag=StonesCore::Flag;
    using Phase=StonesCore::Phase;
    EncounterScene s;
    s.stones_enabled=true;
    auto &core=s.stones;
    auto &north=core.stones[4];
    StonesCore::Hooks h;
    unsigned hits=60,queries=0,inner_hits=0;
    h.stripe_tile=[](int,int,unsigned){};
    auto p=encounter_info(s).protection;
    check(p.kind==Protection::TimedLock&&p.frames==65,"opening pair activates after the damage query on frame 64");
    h.damage=[&](int x,int y,int,int){
        if(x==north.x&&y==north.y){++queries;return hits;}
        if(x==core.stones[0].x&&y==core.stones[0].y){++inner_hits;return 7u;}
        return 0u;
    };
    for(unsigned i=0;i<63;++i)core.update(0,208,336,h);
    check(encounter_info(s).protection.frames==2&&!core.stones[0].damage,"opening countdown is not the encounter timeout");
    core.update(0,208,336,h);
    check(core.phase==Phase::Inner&&encounter_info(s).protection.kind==Protection::None&&!core.stones[0].damage,
        "frame 64 queries the dormant inner stone before activation");
    core.update(0,208,336,h);
    check(inner_hits==65&&core.stones[0].damage==7,"frame 65 accepts inner stone damage");

    core.phase=Phase::NorthOpening;core.phase_frame=0;queries=0;
    core.update(0,208,336,h);
    p=encounter_info(s).protection;
    check(queries==1&&!north.damage&&p.kind==Protection::NorthSeal&&!p.frames,
        "sealed north stone consumes a query without taking damage");
    for(unsigned i=0;i<160&&north.flag!=Flag::Active;++i)core.update(0,208,336,h);
    p=encounter_info(s).protection;
    check(core.phase==Phase::NorthFirstAttacks&&north.flag==Flag::Active&&!north.damage&&p.kind==Protection::NorthDamageCap,
        "north stone opens after the query and advertises its cap before reaching it");
    hits=490;core.update(0,208,336,h);
    check(north.damage==490&&encounter_info(s).protection.kind==Protection::NorthDamageCap,"north stone accepts partial progress under the cap");
    hits=60;core.update(0,208,336,h);
    check(north.damage==500&&encounter_info(s).protection.kind==Protection::NorthDamageCap,"north first sequence clamps damage after a hit");
    core.phase=Phase::NorthClosing;core.phase_frame=2;north.pattern=160;hits=10;
    check(encounter_info(s).protection.kind==Protection::None,"closing starts without the prior cap or a premature seal");
    core.update(0,208,336,h);
    check(north.damage==510&&north.flag==Flag::Dormant&&encounter_info(s).protection.kind==Protection::NorthSeal,
        "north stone still takes damage on the update that seals it");
    core.update(0,208,336,h);
    check(north.damage==510&&encounter_info(s).protection.kind==Protection::NorthSeal,"sealed north stone stops accruing damage");
}

void marisa_damage_protection(){
    EncounterScene s;
    s.marisa_enabled=true;
    auto &core=s.marisa;
    using Phase=MarisaCore::Phase;
    core.phase=Phase::AttackWait;core.rounds=2;core.body_damage_enabled=true;
    auto &orb=core.orbs[0];orb.active=true;orb.hits=109;
    MarisaCore::Hooks h;
    unsigned queries=0,orb_queries=0;
    h.damage=[&](int,int,int,int){++queries;return 25;};
    h.orb_damage=[&](int,int,int,int){++orb_queries;return 99;};
    h.random8=[](){return 0u;};
    auto x=encounter_info(s);
    check(x.protection.kind==Protection::MarisaOrbs&&x.target_count==1&&x.targets[0].state==TargetState::Alive,
        "Marisa body is shielded while the separate orb can take hits");
    core.update(h);
    x=encounter_info(s);
    check(queries==1&&orb_queries==1&&!core.damage_taken&&orb.dying&&orb.death_frame==1&&
        x.protection.kind==Protection::MarisaOrbRetirement&&x.targets[0].state==TargetState::Dying,
        "lethal hit leaves orb shielding Marisa while it retires");
    orb.death_frame=47;
    core.update(h);
    x=encounter_info(s);
    check(queries==2&&orb_queries==1&&!core.damage_taken&&!orb.active&&x.protection.kind==Protection::None&&!x.target_count,
        "the last orb retirement tick samples the old shield, then exposes the next query");
    core.update(h);
    check(queries==3&&core.damage_taken==25,"the body accepts damage one tick after the last orb's retirement");
    core.rounds=1;core.body_damage_enabled=false;
    x=encounter_info(s);
    check(x.protection.kind==Protection::MarisaRound&&x.protection.round==2&&!x.protection.frames,"exposed body still needs to open round two");
    core.update(h);
    check(queries==4&&core.damage_taken==25&&core.body_flash,"body can flash while its round gate discards damage");
    core.phase_frame=30;core.exposed_selections=2;
    core.update(h);
    check(core.rounds==2&&core.body_damage_enabled&&core.damage_taken==50&&encounter_info(s).protection.kind==Protection::None,
        "Marisa starts accepting damage on the tick that opens round two");
}

void fourth_and_fifth_stage(){
    EncounterScene s;
    s.stage=3;s.midboss4.active=true;s.midboss4.entries=1;s.midboss4.damage_taken=400;s.midboss4.cycles=12;
    auto x=encounter_info(s);
    check(x.kind==Kind::Midboss4&&std::wstring_view(x.label)==L"MIDBOSS 4A"&&x.pending&&x.timeout_pending&&!x.dying,"midboss4 damage/departure gates are not immediate");
    s.midboss4.entries=2;s.midboss4.damage_taken=9;s.midboss4.cycles=2;x=encounter_info(s);
    check(std::wstring_view(x.label)==L"MIDBOSS 4B"&&x.health==391&&x.timeout==10,"midboss4 second appearance resets HP and cycles");
    s.midboss4.active=false;s.marisa_enabled=true;x=encounter_info(s);
    check(x.kind==Kind::Marisa&&x.phases==1&&x.health==900&&x.timeout==7&&std::string_view(x.timeout_unit)=="rounds","marisa single body damage pool and rounds");
    s.marisa.phase=MarisaCore::Phase::AttackWait;s.marisa.rounds=2;s.marisa.body_damage_enabled=true;
    s.marisa.orbs[0].active=true;s.marisa.orbs[0].hits=109;s.marisa.orbs[1].active=s.marisa.orbs[1].dying=true;
    x=encounter_info(s);
    check(x.target_count==2&&x.targets[0].health==1&&x.targets[0].max_health==110&&x.targets[1].state==TargetState::Dying,"marisa counts independent non-zero orb hit queries");
    check(std::wstring_view(x.status)==L"SHIELDED"&&x.health==900&&x.timeout==5,"active/dying orbs shield Marisa");
    s.marisa.orbs[0].active=s.marisa.orbs[1].active=false;s.marisa.damage_taken=899;x=encounter_info(s);
    check(!*x.status&&x.health==1&&!x.target_count,"marisa body becomes vulnerable");
    s.marisa.defeated=true;x=encounter_info(s);
    check(x.dying&&!x.health&&!x.has_timeout,"marisa round retirement zeros the bar");
    s.marisa.cleared=true;s.mima_enabled=true;x=encounter_info(s);
    check(x.kind==Kind::Mima&&x.phases==5&&x.health==600&&!x.has_timeout,"mima opening shows upcoming gate, not a timer");
    s.mima.phase=MimaCore::Phase::First;s.mima.damage_taken=599;s.mima.rounds=1;x=encounter_info(s);
    check(x.health==1&&x.timeout==10&&std::wstring_view(x.status)==L"DAMAGE LOCKED","mima damage gate vs vulnerability lock");
    s.mima.phase=MimaCore::Phase::Second;s.mima.damage_taken=700;s.mima.body_damage_enabled=true;s.mima.rounds=13;x=encounter_info(s);
    check(x.phase==1&&x.max_health==700&&x.pending&&x.timeout_pending&&!x.dying,"mima waits for round boundary at either threshold");
    s.mima.phase=MimaCore::Phase::Shift2;s.mima.damage_taken=50;x=encounter_info(s);
    check(x.phase==1&&!x.health&&!x.has_timeout&&std::wstring_view(x.status)==L"TRANSITION","mima timeout transition shows completed gate without carrying HP");
    s.mima.phase=MimaCore::Phase::Mixed;s.mima.damage_taken=1499;x=encounter_info(s);
    check(x.phase==3&&x.health==1&&x.max_health==1500,"mima mixed form-1 gate");
    s.mima.phase=MimaCore::Phase::Form1Dialogue;s.continues_used=1;x=encounter_info(s);
    check(x.dying&&x.phase==3&&x.phases==4&&!x.has_timeout,"continued run has no final form");
    s.continues_used=0;s.mima.form2=true;s.mima.phase=MimaCore::Phase::Final;s.mima.damage_taken=1099;x=encounter_info(s);
    check(x.phase==4&&x.phases==5&&x.health==1&&x.max_health==1100,"mima final uses round damage gate, not 6000 score/invincibility threshold");
    s.mima.finished=true;check(!encounter_info(s).active,"mima completion removes encounter");
}

void mima_damage_protection(){
    using Phase=MimaCore::Phase;
    EncounterScene s;
    s.mima_enabled=true;
    auto &core=s.mima;
    MimaCore::Hooks h;
    h.random8=[](){return 0u;};
    h.bullet=[](const auto &){};
    unsigned queries=0;
    h.damage=[&](int,int,int,int){++queries;return 19u;};
    const MimaCore::Player player;
    auto x=encounter_info(s);
    check(x.protection.kind==Protection::MimaRound&&x.protection.round==2&&!x.protection.frames,
        "Mima's opening waits for round two, not just the end of the opening animation");
    core.update(h,player);
    check(queries==1&&!core.damage_taken,"Mima queries body collision during the opening but discards damage");
    for(unsigned i=0;i<100;++i)core.update(h,player);
    check(core.phase==Phase::First&&core.rounds==0&&!core.damage_taken,"opening gives way to the first locked attack group");
    for(unsigned chapter=1;chapter<=5;++chapter){
        core.start_practice_phase(chapter,h);
        const auto active_phase=core.phase;
        const unsigned unlock=chapter==4?3:2;
        check(core.queries_body_damage()&&core.damage_unlock_round()==unlock,"Mima exposes the actual attack's query state and round gate");
        for(unsigned round=0;round<unlock;++round){
            x=encounter_info(s);
            check(x.protection.kind==Protection::MimaRound&&x.protection.round==unlock&&!x.protection.frames&&
                core.rounds==round&&!core.body_damage_enabled&&!core.damage_taken,"Mima phase stays locked until its own round boundary");
            const auto previous_queries=queries;
            // Select an attack whose end branch resets the timer at this frame.
            core.selection=0;core.phase_frame=1000;core.update(h,player);
            check(queries==previous_queries+1&&core.phase==active_phase,"Mima queries after advancing the round counter");
        }
        x=encounter_info(s);
        check(core.rounds==unlock&&core.body_damage_enabled&&core.damage_taken==19&&x.protection.kind==Protection::None,
            "Mima accepts damage on the tick that opens the vulnerable round");
        const auto phase_hp=x.max_health;
        core.damage_taken=std::uint16_t(phase_hp);
        core.phase_frame=1;core.update(h,player);
        x=encounter_info(s);
        check(core.phase==active_phase&&core.damage_taken==phase_hp+19&&x.pending&&x.protection.kind==Protection::None,
            "Mima can still accrue damage beyond the gate before a round ends");
        if(chapter<=3){
            core.selection=0;core.phase_frame=1000;core.update(h,player);
            x=encounter_info(s);
            check(core.phase==Phase(unsigned(active_phase)+1)&&x.protection.kind==Protection::MimaRound&&x.protection.round==(chapter==3?3u:2u),
                "Mima's shift already anticipates the next form's vulnerability round");
        }
    }
    h.damage=[&](int,int,int,int){++queries;return 6000u;};
    core.update(h,player);
    check(!core.queries_body_damage()&&encounter_info(s).protection.kind==Protection::None,
        "Mima's defeated body is not waiting for another vulnerable round");
    const auto last_query=queries;
    core.update(h,player);
    check(queries==last_query,"a defeated body does not run another shot query");
}

void protection_copy(){
    constexpr auto ja=Language::Japanese,en=Language::English,zh=Language::ChineseSimplified,tw=Language::ChineseTraditional;
    const auto timed=EncounterProtection{Protection::TimedLock,1};
    check(encounter_protection_lines(timed,ja)[1]==L"残り 1 論理フレーム"&&
        encounter_protection_lines(timed,en)[1]==L"Left: 1 logic frame"&&
        encounter_protection_lines(timed,zh)[1]==L"剩余 1 逻辑帧"&&
        encounter_protection_lines(timed,tw)[1]==L"剩餘 1 邏輯幀","one-frame count is localized in all four languages");
    const auto shield=EncounterProtection{Protection::MarisaOrbs};
    check(encounter_protection_lines(shield,zh)[1]==L"先击破天仪"&&
        encounter_protection_lines(shield,tw)[1]==L"先擊破天儀","both Chinese variants distinguish Marisa's shield targets from her body");
    const auto cap=EncounterProtection{Protection::NorthDamageCap};
    const auto seal=EncounterProtection{Protection::NorthSeal};
    for(auto l:{ja,en,zh,tw})
        check(encounter_protection_lines(cap,l)[0]!=encounter_protection_lines(seal,l)[0],"the cap is never labelled as a seal");
}

void extra_stage(){
    EncounterScene s;
    s.stage=5;s.extra_road_enabled=true;s.extra_midboss.active=true;s.extra_midboss.frame=420;
    auto x=encounter_info(s);
    check(x.kind==Kind::ExtraMidboss&&!x.has_health&&!x.total_max_health&&!x.target_count&&x.has_timeout&&x.timeout==1&&
        x.protection.kind==Protection::None,"EX tile encounter is timer-only, not invulnerability");
    s.extra_midboss.active=false;s.sigma_enabled=true;x=encounter_info(s);
    check(x.kind==Kind::Sigma&&x.phase==0&&x.phases==5&&x.health==1800&&!x.has_timeout,"sigma opening shares first stage's damage accumulator");
    s.sigma.phase=3;s.sigma.damage_taken=2000;s.sigma.cycled=true;x=encounter_info(s);
    check(x.phase==1&&!x.health&&x.pending&&!x.has_timeout&&!x.dying,"sigma gate waits for attack reset, cycled does not mean timeout");
    s.sigma.phase=4;s.sigma.damage_taken=60;x=encounter_info(s);
    check(x.phase==2&&x.health==1740&&std::wstring_view(x.status)==L"TRANSITION","sigma even-state damage belongs to next gate");
    SigmaCore::Hooks sigma_hit;
    sigma_hit.bullet=[](const auto &){};
    sigma_hit.damage=[](int,int,int,int){return 10u;};
    sigma_hit.score=sigma_hit.sound=sigma_hit.turns_max=[](unsigned){};
    sigma_hit.randring2_next=[](){return 0u;};
    sigma_hit.player_hit=[](){};
    sigma_hit.laser=[](int,int,int,unsigned,unsigned){};
    s.sigma.update(0,1,sigma_hit);
    check(s.sigma.phase==5&&s.sigma.damage_taken==70&&encounter_info(s).protection.kind==Protection::None,
        "sigma's entry state advances and accepts real damage on the same update");
    s.sigma.phase=9;s.sigma.damage_taken=1299;x=encounter_info(s);
    check(x.phase==4&&x.health==1&&x.max_health==1300&&!x.has_timeout,"sigma final uses retirement threshold, not the dispatch reset threshold");
    s.sigma.damage_taken=1300;x=encounter_info(s);
    check(x.pending&&!x.health&&!x.dying&&!x.has_timeout,"sigma still alive on the actual crossing tick");
    s.sigma.dying=true;x=encounter_info(s);
    check(x.dying&&!x.pending&&!x.has_timeout,"sigma retirement follows the simulation flag");
    s.sigma.dying=false;s.sigma.phase=10;x=encounter_info(s);
    check(x.active&&!x.has_health&&!x.has_timeout,"sigma state 10 has no fabricated damage or timer gate");
    s.sigma.done=true;check(!encounter_info(s).active,"sigma completion removes encounter");
}
}

int main(){
    try{
        early_encounters();midboss_damage_protection();third_stage();north_stone_damage_protection();
        fourth_and_fifth_stage();marisa_damage_protection();mima_damage_protection();protection_copy();extra_stage();
        std::cout<<"encounter_info_checks="<<checks<<" failures=0\n";
        return 0;
    }catch(const std::exception &e){std::cerr<<e.what()<<'\n';return 1;}
}
