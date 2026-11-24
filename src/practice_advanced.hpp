#pragma once
#include "core_contract.hpp"
#include "frame_pacing.hpp"
#include "locale.hpp"
#include <array>

namespace soew {
struct PracticeAdvanced {
    enum Option { LastLife, DisableBomb, DisableFocus, DisableShot, ForceFocus, LockTimer, FastRetry };
    static constexpr unsigned native_speed=4, max_preset=12, max_fps=6000, retry_frames=15;
    unsigned speed=native_speed;
    bool free_fps=false, speed_in_fps=false;
    std::uint8_t flags=0;

    bool enabled(Option option)const{return (flags&(1u<<option))!=0;}
    void toggle(Option option){flags^=std::uint8_t(1u<<option);}
    bool changes_input()const{return (flags&((1u<<DisableBomb)|(1u<<DisableFocus)|(1u<<DisableShot)|(1u<<ForceFocus)))!=0;}
    std::uint16_t filter(std::uint16_t input,bool gameplay)const{
        if(!gameplay)return input;
        if(enabled(DisableBomb))input&=~Bomb;
        if(enabled(DisableFocus))input&=~Focus;
        if(enabled(DisableShot))input&=~Shot;
        if(enabled(ForceFocus))input|=Focus;
        return input;
    }
    static unsigned preset_to_fps(unsigned preset){
        return (preset*pc98_refresh_num+2*pc98_refresh_den)/(4*pc98_refresh_den);
    }
    static unsigned fps_to_preset(unsigned fps){
        return std::clamp((fps*4*pc98_refresh_den+pc98_refresh_num/2)/pc98_refresh_num,1u,max_preset);
    }
    unsigned draft()const{
        if(free_fps==speed_in_fps)return speed;
        return free_fps?preset_to_fps(speed):fps_to_preset(speed);
    }
    // Rate as a fraction of the locked TH02 display cadence.
    unsigned rate_num()const{return speed_in_fps?speed*pc98_refresh_den:speed;}
    unsigned rate_den()const{return speed_in_fps?pc98_refresh_num:native_speed;}
    bool operator==(const PracticeAdvanced &)const=default;
};

inline constexpr std::array<LocalizedText,7> practice_advanced_labels={{{
    L"最後の残機だけを保護",L"Protect only the last life (F2)",L"锁残仅保留最后一命（F2）",L"鎖殘僅保留最後一命（F2）"},
    {L"X キー無効",L"Disable X (Shift+D)",L"禁用 X（Shift+D）",L"禁用 X（Shift+D）"},
    {L"Shift キー無効",L"Disable Shift (Shift+A)",L"禁用 Shift（Shift+A）",L"禁用 Shift（Shift+A）"},
    {L"Z キー無効",L"Disable Z (Shift+S)",L"禁用 Z（Shift+S）",L"禁用 Z（Shift+S）"},
    {L"常に低速移動",L"Force focused movement",L"强制低速移动",L"強制低速移動"},
    {L"時間固定時の計測",L"Time locked boss phases",L"启用锁时计时",L"啟用鎖時計時"},
    {L"ミス後に再挑戦",L"Retry on miss (Shift+F)",L"中弹后快速重试（Shift+F）",L"中彈後快速重試（Shift+F）"}}};

struct PracticeReaction {
    enum State { Idle, WaitHold, Waiting, Ready, Early, Result } state=Idle;
    bool release=false,progress=false;
    int trials=5,count=0;
    std::uint64_t start=0,deadline=0,delay=0;
    std::int64_t last=0,total=0; // microseconds

    bool active()const{return state==WaitHold||state==Waiting||state==Ready;}
    void reset_series(){state=Idle;count=0;last=total=0;}
    void begin(std::uint64_t now,std::uint64_t wait){
        if(count>=trials)reset_series();
        state=release?WaitHold:Waiting;start=now;delay=wait;deadline=now+delay;
    }
    void observe(std::uint64_t now,bool arrows){
        if(state==WaitHold){if(arrows){start=now;deadline=now+delay;state=Waiting;}return;}
        if(state==Waiting&&now>=deadline)state=Ready;
        if((state!=Waiting&&state!=Ready)||arrows==release)return;
        if(state==Waiting&&!progress){state=Early;return;}
        last=std::int64_t(now)-std::int64_t(deadline);total+=last<0?-last:last;++count;state=Result;
    }
};
}
