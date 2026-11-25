#pragma once
#include "encounter_scene.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>

namespace soew {

struct TitleDemoSpec {
    unsigned number;
    const char *filename;
    int stage, shot; // Zero-based TH02 scene and shot IDs.
};
inline constexpr std::array<TitleDemoSpec,3> title_demos{{
    {1,"DEMO1.REC",3,0}, {2,"DEMO2.REC",2,2}, {3,"DEMO3.REC",1,1},
}};

inline constexpr unsigned title_demo_idle_ticks=641; // th02/op_01.cpp:619, idle_frame > 640.
inline constexpr unsigned title_demo_score_ticks=351; // th02/op_04.cpp:172–191, do/while frame <= 350.
inline constexpr unsigned title_demo_file_words=7000;
// th02_main.asm:2051–2069 clears word 6949 before gameplay sees it.
inline constexpr unsigned title_demo_play_ticks=title_demo_file_words-51;
inline constexpr unsigned title_demo_file_bytes=2*title_demo_file_words;
using TitleDemoRecording=std::array<std::uint16_t,title_demo_play_ticks>;

inline TitleDemoRecording decode_title_demo(std::span<const std::byte> bytes){
    if(bytes.size()!=title_demo_file_bytes)throw std::runtime_error("Invalid title demo recording size");
    TitleDemoRecording recording{};
    constexpr auto supported=Up|Down|Left|Right|Shot|Bomb|UpLeft|UpRight|DownLeft|DownRight;
    for(std::size_t i=0;i<recording.size();++i){
        const auto lo=std::to_integer<unsigned>(bytes[2*i]);
        const auto hi=std::to_integer<unsigned>(bytes[2*i+1]);
        const auto word=lo|(hi<<8);
        if(word&~unsigned(supported))throw std::runtime_error("Unsupported title demo input at word "+std::to_string(i));
        recording[i]=static_cast<std::uint16_t>(word);
    }
    // The unread suffix is not an input stream; stock DEMO2 contains arbitrary words there.
    return recording;
}

template<class Scene> struct TitleDemoRun {
    std::unique_ptr<Scene> scene;
    TitleDemoRecording recording;
};

using TitleDemoVerify=void (*)(const TitleDemoSpec &,std::span<const std::byte>);
inline TitleDemoRun<EncounterScene> load_title_demo(const std::filesystem::path &root,const TitleDemoSpec &spec,
                                                   Language language=Language::Japanese,TitleDemoVerify verify=nullptr){
    std::ifstream file(root/spec.filename,std::ios::binary|std::ios::ate);
    if(!file)throw std::runtime_error(std::string("Missing title demo: ")+spec.filename);
    if(file.tellg()!=std::streamoff(title_demo_file_bytes))throw std::runtime_error("Invalid title demo recording size");
    std::array<std::byte,title_demo_file_bytes> bytes{};
    file.seekg(0);
    if(!file.read(reinterpret_cast<char*>(bytes.data()),std::streamsize(bytes.size())))
        throw std::runtime_error("Title demo recording read failed");
    if(verify)verify(spec,bytes);
    auto recording=decode_title_demo(bytes);

    // Stock demo settings (th02/op_01.cpp:357–368, th02_main.asm:1997–2034).
    // The deterministic native RNG initialization uses the seed verified by the demo probe.
    auto scene=std::make_unique<EncounterScene>(EncounterScene::load(root,spec.stage,spec.shot,1,2,3,language));
    auto &s=*scene;
    s.combat_enabled=s.survival_enabled=s.pickup_enabled=s.transitions_enabled=true;
    s.resident_frame=18;
    s.enable_effects(18);
    s.enable_bombing();
    s.pickups.power=s.life.power=80;
    s.shots.level=s.pickups.level();
    s.bullet_core.playperf=s.life.playperf=12;
    s.midboss3_enabled=s.stage==2;
    s.midboss2_enabled=s.stage==1;
    // The verified Stage 4 demo prefix does not need truncated-script compatibility.
    s.enable_preboss();
    return {std::move(scene),std::move(recording)};
}

struct TickTitleDemoScene {
    static void check(const EncounterScene &s){
        if(s.runtime.blocked)throw std::runtime_error("Title demo blocked: "+s.runtime.blocked->result.reason);
        if(s.weapon_blocked)throw std::runtime_error("Title demo weapon blocked");
        if(!s.transition_error.empty())throw std::runtime_error("Title demo transition failed: "+s.transition_error);
        // Only the gameplay sampler phase has evidence. Never pass REC words to
        // dialogue, continue, a map end, or any other non-sampler phase.
        if(s.paused||s.life.game_over||s.dialogue.active()||s.transition.active()||s.runtime.scroll.done||
           s.preboss_started||s.stage2_preview_boundary||s.stage3_preview_boundary||s.stage5_entry_only||s.stage5_result||
           s.practice_completed||s.mima_delay||s.extra_settlement.phase!=ExtraSettlement::Phase::Inactive)
            throw std::runtime_error("Title demo left the verified gameplay phase");
    }
    void operator()(EncounterScene &s,std::uint16_t input,unsigned index)const{
        check(s);
        if(s.ticks!=index)throw std::runtime_error("Title demo gameplay clock desynchronized");
        s.tick(input,false,false);
        check(s);
        if(s.ticks!=index+1)throw std::runtime_error("Title demo gameplay tick did not advance");
    }
};

enum class TitleDemoPhase {Idle,Load,Play,Score,Rearm};
enum class TitleDemoOutcome {None,Natural,InputAbort,Failed};

// The loader and scene step are injectable; no keyboard, renderer, audio,
// profile, or replay-slot service participates in this state machine.
template<class Scene,class Step> class BasicTitleDemoController {
public:
    using Phase=TitleDemoPhase;
    using Outcome=TitleDemoOutcome;

    explicit BasicTitleDemoController(bool enabled=false):enabled_(enabled){}
    Phase phase()const{return phase_;}
    Outcome outcome()const{return outcome_;}
    const TitleDemoSpec *current()const{return current_;}
    const TitleDemoSpec &next()const{return title_demos[next_];}
    unsigned played_ticks()const{return played_;}
    const std::string &error()const{return error_;}
    Scene *scene(){return scene_.get();}
    const Scene *scene()const{return scene_.get();}

    // Call once per host logic step. `eligible` means focused main title in Idle
    // (not a submenu), and foreground ownership in the other phases. Return true
    // when this step belongs to the demo and real input must not reach the menu.
    template<class Loader> bool update(bool eligible,bool real_input,Loader &&load){
        if(phase_==Phase::Idle){
            if(!enabled_||!eligible||real_input){idle_=0;return false;}
            if(++idle_<title_demo_idle_ticks)return false;
            current_=&next();
            next_=(next_+1)%title_demos.size();
            played_=0;
            outcome_=Outcome::None;
            phase_=Phase::Load;
            return true;
        }
        if(!eligible)return true;
        // Genuine input wins even when it arrives on the last playback tick.
        if(real_input&&(phase_==Phase::Load||phase_==Phase::Play)){
            finish(Outcome::InputAbort);
            return true;
        }
        try{
            switch(phase_){
            case Phase::Load:{
                auto run=std::forward<Loader>(load)(*current_);
                if(!run.scene)throw std::runtime_error("Title demo scene unavailable");
                recording_=std::move(run.recording);
                scene_=std::move(run.scene);
                // The host can attach audio before the first simulation tick.
                phase_=Phase::Play;
                break;
            }
            case Phase::Play:
                Step{}(*scene_,recording_[played_],played_);
                if(++played_==title_demo_play_ticks)finish(Outcome::Natural);
                break;
            case Phase::Score:
                if(!real_input)score_can_dismiss_=true;
                if((real_input&&score_can_dismiss_)||!--score_left_)phase_=Phase::Rearm;
                break;
            case Phase::Rearm:
                if(!real_input){idle_=0;phase_=Phase::Idle;}
                break;
            case Phase::Idle:break;
            }
        }catch(const std::exception &e){
            error_=e.what();
            enabled_=false; // Do not retry broken or unsupported assets on every idle loop.
            finish(Outcome::Failed);
        }
        return true;
    }
private:
    void finish(Outcome outcome){
        scene_.reset();
        outcome_=outcome;
        if(outcome==Outcome::Natural){
            score_left_=title_demo_score_ticks;
            score_can_dismiss_=false;
            phase_=Phase::Score;
        }else phase_=Phase::Rearm;
    }

    bool enabled_;
    Phase phase_=Phase::Idle;
    Outcome outcome_=Outcome::None;
    std::size_t next_=0;
    const TitleDemoSpec *current_=nullptr;
    unsigned idle_=0,played_=0,score_left_=0;
    bool score_can_dismiss_=false;
    std::string error_;
    std::unique_ptr<Scene> scene_;
    TitleDemoRecording recording_{};
};
using TitleDemoController=BasicTitleDemoController<EncounterScene,TickTitleDemoScene>;

} // namespace soew
