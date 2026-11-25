#pragma once
#include "encounter_scene.hpp"
#include <algorithm>
#include <array>
#include <span>

namespace soew {
enum class EncounterInfoKind { None, Midboss1, Midboss2, Rika, Meira, Midboss3, Stones, Midboss4, Marisa, Mima, ExtraMidboss, Sigma };
enum class EncounterTargetState { Alive, Dying };
enum class EncounterProtectionKind { None, TimedLock, SingleDamage, NorthSeal, NorthDamageCap,
    MarisaOrbs, MarisaOrbRetirement, MarisaRound, MimaRound };
struct EncounterProtection {
    EncounterProtectionKind kind=EncounterProtectionKind::None;
    // Updates through the one that lifts this restriction; zero means no countdown.
    unsigned frames=0,round=0;
};
struct EncounterTargetInfo {
    const wchar_t *label=L"";
    unsigned health=0,max_health=0;
    EncounterTargetState state=EncounterTargetState::Alive;
};
struct EncounterInfo {
    EncounterInfoKind kind=EncounterInfoKind::None;
    const wchar_t *label=L"",*status=L"";
    unsigned phase=0,phases=0,health=0,max_health=0,total_health=0,total_max_health=0;
    bool active=false,dying=false,pending=false,has_health=false,has_timeout=false,timeout_pending=false;
    unsigned timeout=0;const char *timeout_unit="";
    EncounterProtection protection;
    // Cut positions measured in remaining health, starting at the left (last phase).
    std::array<unsigned,4> phase_cuts{};
    unsigned phase_cut_count=0;
    // Independent targets are never added together into a health pool.
    std::array<EncounterTargetInfo,4> targets{};
    unsigned target_count=0;
};

inline std::wstring_view encounter_info_text(std::wstring_view key,Language language){
    if(language==Language::English)return key;
    struct Entry {const wchar_t *key,*ja,*hans,*hant;};
    constexpr Entry labels[]={
        {L"RIKA",L"里香",L"里香",L"里香"},{L"MEIRA",L"明羅",L"明罗",L"明羅"},
        {L"FIVE STONES",L"五魔石",L"五魔石",L"五魔石"},{L"NORTH STONE",L"北の魔石",L"北魔石",L"北魔石"},
        {L"MARISA",L"魔梨沙",L"魔理沙",L"魔理沙"},{L"MIMA",L"魅魔",L"魅魔",L"魅魔"},
        {L"MIMA FINAL",L"魅魔・最終形態",L"魅魔·最终形态",L"魅魔・最終形態"},{L"SIGMA",L"イビルアイΣ",L"邪眼Σ",L"邪眼Σ"},
        {L"MIDBOSS 1",L"１面中ボス",L"一面中 Boss",L"一面中 Boss"},
        {L"MIDBOSS 2",L"２面中ボス",L"二面中 Boss",L"二面中 Boss"},
        {L"MIDBOSS 3",L"３面中ボス",L"三面中 Boss",L"三面中 Boss"},
        {L"MIDBOSS 4A",L"４面中ボスＡ",L"四面中 Boss A",L"四面中 Boss A"},
        {L"MIDBOSS 4B",L"４面中ボスＢ",L"四面中 Boss B",L"四面中 Boss B"},
        {L"EX TERRAIN",L"EX 地形",L"EX 地形",L"EX 地形"},
        {L"LEFT",L"左",L"左",L"左"},{L"RIGHT",L"右",L"右",L"右"},
        {L"ORB 1",L"天儀１",L"天仪 1",L"天儀 1"},{L"ORB 2",L"天儀２",L"天仪 2",L"天儀 2"},
        {L"ORB 3",L"天儀３",L"天仪 3",L"天儀 3"},{L"ORB 4",L"天儀４",L"天仪 4",L"天儀 4"},
        {L"OPENING",L"登場中",L"登场中",L"登場中"},{L"INNER PAIR",L"内側２体",L"内侧双石",L"內側雙石"},
        {L"OUTER PAIR",L"外側２体",L"外侧双石",L"外側雙石"},{L"SEALED",L"封印中",L"封印中",L"封印中"},
        {L"DAMAGE CAPPED",L"ダメージ上限",L"达到伤害上限",L"達到傷害上限"},
        {L"SHIELDED",L"天儀が本体を防御",L"天仪保护本体",L"天儀保護本體"},
        {L"BODY DAMAGE LOCKED",L"ボス本体は無敵",L"本体无敌",L"本體無敵"},
        {L"DAMAGE LOCKED",L"ボス本体は無敵",L"本体无敌",L"本體無敵"},
        {L"TRANSITION",L"移行中",L"转阶段中",L"轉階段中"},{L"NO DAMAGE GATE",L"撃破条件なし",L"无击破门槛",L"無擊破門檻"},
        {L"APPROACH",L"接近中",L"接近中",L"接近中"},{L"SURVIVE",L"耐久",L"生存",L"生存"}};
    for(const auto& e:labels)if(e.key==key)return localized(language,e.ja,e.key,e.hans,e.hant);
    return key;
}
inline std::wstring_view encounter_timer_unit(std::string_view key,Language language){
    if(key=="rounds")return localized(language,L"ラウンド",L"rounds",L"轮",L"輪");
    if(key=="cycles")return localized(language,L"周期",L"cycles",L"周期",L"週期");
    if(key=="progress")return localized(language,L"進行",L"progress",L"进度",L"進度");
    return localized(language,L"フレーム",L"logic",L"逻辑帧",L"邏輯幀");
}

inline std::array<std::wstring,2> encounter_protection_lines(EncounterProtection p,Language language){
    using Kind=EncounterProtectionKind;
    const auto tr=[&](const wchar_t *ja,const wchar_t *en,const wchar_t *hans,const wchar_t *hant){
        return localized(language,ja,en,hans,hant);
    };
    const auto frames=[&]{
        return tr(L"残り ",L"Left: ",L"剩余 ",L"剩餘 ")+std::to_wstring(p.frames)+
            tr(L" 論理フレーム",p.frames==1?L" logic frame":L" logic frames",L" 逻辑帧",L" 邏輯幀");
    };
    const auto round=[&]{
        return tr(L"第",L"Wait for round ",L"等待第 ",L"等待第 ")+std::to_wstring(p.round)+
            tr(L"ラウンド開始待ち",L"",L" 轮开始",L" 輪開始");
    };
    const auto locked=tr(L"本体は無敵",L"Body protected",L"本体无敌",L"本體無敵");
    switch(p.kind){
    case Kind::None:return {};
    case Kind::TimedLock:return {tr(L"無敵",L"Damage locked",L"无敌",L"無敵"),frames()};
    case Kind::SingleDamage:return {tr(L"ダメージ制限：1/回",L"Damage limited: 1/hit",L"伤害受限：每次 1 点",L"傷害受限：每次 1 點"),frames()};
    case Kind::NorthSeal:return {tr(L"北の魔石は封印中",L"North stone sealed",L"北魔石封印中",L"北魔石封印中"),
        tr(L"北の魔石の開放待ち",L"Wait for north to open",L"等待北魔石开启",L"等待北魔石開啟")};
    case Kind::NorthDamageCap:return {tr(L"ダメージ上限：500",L"Damage cap: 500",L"伤害上限：500",L"傷害上限：500"),
        tr(L"第１攻撃の終了待ち",L"Wait for attacks to end",L"等待首轮攻击结束",L"等待首輪攻擊結束")};
    case Kind::MarisaOrbs:return {locked,tr(L"先に天儀を撃破",L"Defeat the orbs first",L"先击破天仪",L"先擊破天儀")};
    case Kind::MarisaOrbRetirement:return {locked,tr(L"天儀の退場待ち",L"Wait for orbs to fade",L"等待护卫退场",L"等待護衛退場")};
    case Kind::MarisaRound:case Kind::MimaRound:return {locked,round()};
    }
    return {};
}

inline EncounterInfo encounter_info(const EncounterScene &s){
    EncounterInfo out{};
    const auto damage_of=[](unsigned raw){return unsigned(std::max(0,enemy_s16(raw)));};
    const auto remaining=[](unsigned damage,unsigned limit){return limit-std::min(damage,limit);};
    const auto start=[&](EncounterInfoKind kind,const wchar_t *label,unsigned phases=1,unsigned phase=0){
        out.kind=kind;out.label=label;out.active=true;out.phases=phases;out.phase=phase;
    };
    const auto hp=[&](unsigned damage,unsigned limit){
        out.has_health=true;out.max_health=out.total_max_health=limit;
        out.health=out.total_health=out.dying?0:remaining(damage,limit);
    };
    const auto split_hp=[&](unsigned damage,unsigned first,unsigned second,unsigned total){
        const unsigned lo=out.phase==0?0:out.phase==1?first:second;
        const unsigned hi=out.phase==0?first:out.phase==1?second:total;
        hp(std::clamp(damage,lo,hi)-lo,hi-lo);
        out.total_max_health=total;out.total_health=out.dying?0:remaining(damage,total);
        out.phase_cuts={total-second,total-first};out.phase_cut_count=2;
    };
    const auto timer=[&](unsigned current,unsigned limit,const char *unit,bool wait_at_limit=false){
        if(out.dying)return;
        out.has_timeout=true;out.timeout=remaining(current,limit);out.timeout_unit=unit;
        out.timeout_pending=wait_at_limit&&!out.timeout;
    };
    const auto target=[&](const wchar_t *label,unsigned damage,unsigned limit,EncounterTargetState state){
        out.targets[out.target_count++]={label,state==EncounterTargetState::Dying?0:remaining(damage,limit),limit,state};
    };
    if(s.rika_enabled&&!s.rika.done){
        const unsigned damage=damage_of(s.rika.damage_taken);
        start(EncounterInfoKind::Rika,L"RIKA",3,damage<700?0:damage<1400?1:2);
        out.dying=s.rika.dying;split_hp(damage,700,1400,2261);return out;
    }
    if(s.meira_enabled&&!s.meira.done){
        const unsigned damage=damage_of(s.meira.damage_taken);
        start(EncounterInfoKind::Meira,L"MEIRA",3,std::min(2u,s.meira.state));
        out.dying=s.meira.dying;split_hp(damage,701,1501,2400);
        out.pending=out.phase<2&&!out.health&&!out.dying;
        return out;
    }
    if(s.stones_enabled&&!s.stones.done()){
        using Phase=StonesCore::Phase;
        using Flag=StonesCore::Flag;
        const auto &b=s.stones;
        const auto pair=[&](unsigned first,unsigned limit){
            for(unsigned i=first;i<first+2;++i){
                const auto &a=b.stones[i];
                if(a.flag==Flag::Active||a.flag==Flag::Dying)
                    target(i==first?L"LEFT":L"RIGHT",damage_of(a.damage),limit,a.flag==Flag::Dying?EncounterTargetState::Dying:EncounterTargetState::Alive);
            }
            if(b.stones[first].flag==Flag::Active||b.stones[first+1].flag==Flag::Active)timer(b.elapsed,1301,"logic");
        };
        switch(b.phase){
        case Phase::Opening:
            start(EncounterInfoKind::Stones,L"FIVE STONES",5,0);out.status=L"OPENING";
            // The frame-64 activation follows the damage queries.
            if(b.phase_frame<64)out.protection={EncounterProtectionKind::TimedLock,65-b.phase_frame};
            break;
        case Phase::Inner:start(EncounterInfoKind::Stones,L"FIVE STONES",5,0);out.status=L"INNER PAIR";pair(0,140);break;
        case Phase::Outer:start(EncounterInfoKind::Stones,L"FIVE STONES",5,1);out.status=L"OUTER PAIR";pair(2,120);break;
        default:{
            const bool first=b.phase==Phase::NorthOpening||b.phase==Phase::NorthFirstAttacks||b.phase==Phase::NorthClosing;
            const bool second=b.phase==Phase::NorthSecondAttacks||b.phase==Phase::NorthReopening;
            start(EncounterInfoKind::Stones,L"NORTH STONE",5,first?2:second?3:4);
            out.dying=b.phase==Phase::NorthDeath;
            const auto &a=b.stones[4];
            hp(damage_of(a.damage),930);
            if(a.flag==Flag::Dormant){
                out.status=L"SEALED";out.protection={EncounterProtectionKind::NorthSeal};
            }else if(a.flag==Flag::Active&&b.phase==Phase::NorthFirstAttacks){
                // The first sequence clamps after processing hits (not on query).
                out.protection={EncounterProtectionKind::NorthDamageCap};
                if(a.damage==500)out.status=L"DAMAGE CAPPED";
            }
            if(b.phase==Phase::NorthFinalAttacks)timer(b.elapsed,2001,"logic",true);
            break;
        }
        }
        return out;
    }
    if(s.marisa_enabled&&!s.marisa.cleared){
        using Phase=MarisaCore::Phase;
        const auto &b=s.marisa;
        start(EncounterInfoKind::Marisa,L"MARISA");
        out.dying=b.defeated;hp(damage_of(b.damage_taken),900);
        if(out.dying)return out;
        unsigned shields=0,live_shields=0;
        constexpr std::array orb_labels{L"ORB 1",L"ORB 2",L"ORB 3",L"ORB 4"};
        for(unsigned i=0;i<b.orbs.size();++i){
            const auto &a=b.orbs[i];
            if(!a.active)continue;
            ++shields;if(!a.dying)++live_shields;
            target(orb_labels[i],a.hits,110,a.dying?EncounterTargetState::Dying:EncounterTargetState::Alive);
        }
        if(b.phase==Phase::Opening)out.status=L"OPENING";
        else if(shields)out.status=L"SHIELDED";
        else if(!b.body_damage_enabled)out.status=L"BODY DAMAGE LOCKED";
        // The next body query samples all active orbs before any orb damage
        // or retirement that tick. Even a dying orb shields until removed.
        if(live_shields)out.protection={EncounterProtectionKind::MarisaOrbs};
        else if(shields)out.protection={EncounterProtectionKind::MarisaOrbRetirement};
        else if(!b.body_damage_enabled)out.protection={EncounterProtectionKind::MarisaRound,0,2};
        timer(b.rounds,7,"rounds");
        return out;
    }
    if(s.mima_enabled&&!s.mima.finished){
        using Phase=MimaCore::Phase;
        const auto &b=s.mima;
        // Each form-1 attack group resets damage. Round completion, not a
        // shot landing, consumes the gate; Final is conditional on no Continue.
        const unsigned phases=s.continues_used?4:5;
        unsigned phase=0,limit=600,rounds_limit=10;
        bool transition=false;
        switch(b.phase){
        case Phase::Opening:out.status=L"OPENING";break;
        case Phase::First:break;
        case Phase::Shift1:transition=true;break;
        case Phase::Second:phase=1;limit=700;rounds_limit=12;break;
        case Phase::Shift2:phase=1;limit=700;transition=true;break;
        case Phase::Third:phase=2;limit=800;rounds_limit=12;break;
        case Phase::Shift3:phase=2;limit=800;transition=true;break;
        case Phase::Mixed:phase=3;limit=1500;rounds_limit=200;break;
        case Phase::Form1Dialogue:phase=3;limit=1500;out.dying=true;break;
        case Phase::Final:phase=4;limit=1100;rounds_limit=200;break;
        case Phase::FinalDialogue:phase=4;limit=1100;out.dying=true;break;
        }
        start(EncounterInfoKind::Mima,b.form2?L"MIMA FINAL":L"MIMA",phases,phase);
        hp(transition?limit:damage_of(b.damage_taken),limit);
        if(out.dying)return out;
        const bool round_locked=b.queries_body_damage()&&!b.body_damage_enabled;
        if(round_locked){
            // During shifts, the existing core round still belongs to the outgoing attack.
            const unsigned unlock_round=b.phase==Phase::Shift3?3u:
                b.phase==Phase::Opening||transition?2u:b.damage_unlock_round();
            if(unlock_round)out.protection={EncounterProtectionKind::MimaRound,0,unlock_round};
        }
        if(transition){out.status=L"TRANSITION";return out;}
        if(b.phase!=Phase::Opening){
            if(round_locked)out.status=L"DAMAGE LOCKED";
            out.pending=!out.health;
            timer(b.rounds,rounds_limit+1,"rounds",true);
        }
        return out;
    }
    if(s.sigma_enabled&&!s.sigma.done){
        const auto &b=s.sigma;
        const unsigned phase=std::min(b.phase/2,4u);
        start(EncounterInfoKind::Sigma,L"SIGMA",5,phase);
        out.dying=b.dying;
        if(b.phase>9){out.status=L"NO DAMAGE GATE";return out;}
        // In state 9, 1300 is the retirement threshold. The source's 5000
        // comparison can reset it only on an attack boundary, not a timeout.
        hp(damage_of(b.damage_taken),phase==4?1300:SigmaCore::cycles[phase].threshold);
        if(!out.dying){
            out.pending=!out.health;
            if(b.phase==0)out.status=L"OPENING";
            else if(!(b.phase&1))out.status=L"TRANSITION";
        }
        return out;
    }
    if(s.midboss_enabled&&s.midboss.active&&s.midboss.present){
        const auto &b=s.midboss;start(EncounterInfoKind::Midboss1,L"MIDBOSS 1");
        out.dying=b.dying;hp(damage_of(b.damage_taken),301);
        out.pending=!out.dying&&!out.health;
        // The core increments phase_frame before querying damage.
        if(!out.dying&&b.phase_frame<18)out.protection={EncounterProtectionKind::TimedLock,18u-b.phase_frame};
        timer(s.runtime.scroll.progress,184,"progress",true);return out;
    }
    if(s.midboss2_enabled&&s.midboss2.active&&s.midboss2.present){
        const auto &b=s.midboss2;start(EncounterInfoKind::Midboss2,L"MIDBOSS 2");
        out.dying=b.dying;hp(damage_of(b.damage_taken),381);
        out.pending=!out.dying&&!out.health;
        if(!out.dying){
            if(!b.phase_frame)out.protection={EncounterProtectionKind::TimedLock,2};
            else if(b.phase_frame<320)out.protection={EncounterProtectionKind::SingleDamage,320u-b.phase_frame};
        }
        timer(b.phase_frame,1601,"logic",true);return out;
    }
    if(s.midboss3_enabled&&s.midboss3.active){
        using Flag=Midboss3Core::Flag;
        const auto &b=s.midboss3;
        start(EncounterInfoKind::Midboss3,L"MIDBOSS 3");
        unsigned alive=0;
        for(unsigned i=0;i<b.actor.size();++i){
            const auto &a=b.actor[i];
            if(a.flag==Flag::Removed)continue;
            if(a.flag==Flag::Alive){++alive;out.pending|=damage_of(a.damage)>=281;}
            target(i?L"RIGHT":L"LEFT",damage_of(a.damage),281,a.flag==Flag::Dying?EncounterTargetState::Dying:EncounterTargetState::Alive);
        }
        if(!out.target_count)return {};
        out.dying=alive==0;
        // Both actors skip their hit queries on the initialization update.
        if(!out.dying&&b.phase_frame<=1)out.protection={EncounterProtectionKind::TimedLock,2u-b.phase_frame};
        timer(b.cycles,4,"cycles");return out;
    }
    if(s.stage==3&&s.midboss4.active){
        const auto &b=s.midboss4;
        start(EncounterInfoKind::Midboss4,b.entries==2?L"MIDBOSS 4B":L"MIDBOSS 4A");
        out.dying=b.dying;hp(damage_of(b.damage_taken),400);
        // The wrap-position condition plus another nonzero damage query is
        // required even after the damage target has been reached.
        out.pending=!out.dying&&!out.health;
        if(b.flying_in&&!out.dying)out.status=L"APPROACH";
        timer(b.cycles,12,"cycles",true);return out;
    }
    if(s.extra_road_enabled&&s.extra_midboss.active){
        start(EncounterInfoKind::ExtraMidboss,L"EX TERRAIN");
        out.status=L"SURVIVE";
        timer(s.extra_midboss.frame,ExtraMidbossCore::last_active_frame+1,"logic");return out;
    }
    return out;
}

struct CoopBossGauge {
    const wchar_t *label=L"";
    unsigned health=0,max_health=0;
    std::array<unsigned,4> phase_cuts{};
    unsigned phase_cut_count=0;
};

inline CoopBossGauge coop_boss_gauge(const EncounterScene &scene){
    const auto info=encounter_info(scene);
    CoopBossGauge gauge{info.label,info.total_health,info.total_max_health,info.phase_cuts,info.phase_cut_count};
    const auto phased=[&](std::span<const unsigned> sections){
        gauge.health=info.health;
        gauge.max_health=gauge.phase_cut_count=0;
        for(unsigned i=unsigned(sections.size());i>0;--i){
            gauge.max_health+=sections[i-1];
            if(i>1)gauge.phase_cuts[gauge.phase_cut_count++]=gauge.max_health;
            if(i-1>info.phase)gauge.health+=sections[i-1];
        }
    };
    const auto remaining=[](BossDamage damage,unsigned max){
        return unsigned(std::clamp(int(max)-enemy_s16(damage),0,int(max)));
    };
    switch(info.kind){
    case EncounterInfoKind::Midboss3:
        gauge.health=0;gauge.max_health=562;
        for(const auto &a:scene.midboss3.actor)
            if(a.flag==Midboss3Core::Flag::Alive)gauge.health+=remaining(a.damage,281);
        break;
    case EncounterInfoKind::Stones:{
        gauge.label=L"FIVE STONES";gauge.health=0;gauge.max_health=1450;
        constexpr std::array<unsigned,5> limits={140,140,120,120,930};
        for(unsigned i=0;i<limits.size();++i){
            const auto &stone=scene.stones.stones[i];
            if(stone.flag==StonesCore::Flag::Active||stone.flag==StonesCore::Flag::Dormant)
                gauge.health+=remaining(stone.damage,limits[i]);
        }
        gauge.phase_cuts={930,1170};gauge.phase_cut_count=2;
        break;
    }
    case EncounterInfoKind::Mima:{
        gauge.label=L"MIMA";
        constexpr std::array<unsigned,5> limits={600,700,800,1500,1100};
        phased(std::span(limits).first(scene.continues_used&&!scene.mima.form2?4:5));
        break;
    }
    case EncounterInfoKind::Sigma:{
        constexpr std::array<unsigned,5> limits={1800,1800,1800,1800,1300};
        phased(limits);
        break;
    }
    default:break; // EX terrain is timed and has no HP.
    }
    return gauge;
}
}
