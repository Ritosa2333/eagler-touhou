#pragma once
#include "../game/GameSessionResources.hpp"
#include "../game/StageFiles.hpp"
#include <algorithm>
#include <cstdio>

#if PORT_TITLE == 14
namespace th11::sdl {
// Only the host callback imports completed CPU packets. GPU work and ANM
// templates stay at GameSessionResources' existing phase boundary.
class StageDecodeTh14 {
public:
    using Phase=StageResourceEffects::Phase;
    using Result=StageResourceEffects::DecodeResult;
    struct Token {u64 generation;StageResources* owner;Phase phase;u32 step;};
    Result begin(GameResources& source,StageResources& stage,Phase phase,u32 selection){
        if(owner_&&(&stage!=owner_||&source!=source_))return reject("TH14 stage CPU owner changed");
        if(owner_&&phase_==phase){
            if(phase==Phase::Hud&&selection_!=selection)return reject("TH14 stage CPU dialogue owner changed");
            return result();
        }
        if(owner_&&active_)return reject("TH14 stage CPU phase still pending");
        if(failed_)return Result::Failed;
        if(u32(phase)>u32(Phase::Timeline))return reject("TH14 stage CPU phase invalid");
        ++generation_;source_=&source;owner_=&stage;phase_=phase;selection_=selection;step_=0;
        active_=true;queued_=false;error_.clear();return Result::Waiting;
    }
    bool dispatchable()const noexcept{return active_&&!queued_&&!suspended_&&!failed_;}
    Token dispatch(){queued_=true;return {generation_,owner_,phase_,step_};}
    std::vector<std::string> names()const{
        std::vector<std::string> names;if(!dispatchable())return names;
        char name[64];auto add=[&](const char* format){std::snprintf(name,sizeof name,format,owner_->stage_number);names.emplace_back(name);};
        if(phase_==Phase::Hud){add(stage_files.logo);stage_dialogue_name(name,sizeof name,owner_->stage_number,selection_);names.emplace_back(name);}
        else if(phase_==Phase::Scene){add(stage_files.scene);add(stage_files.background);}
        else if(step_==0){
            add(stage_files.timeline);
            for(const char* format:{stage_files.boss,stage_files.midboss}){
                std::snprintf(name,sizeof name,format,owner_->stage_number);if(source_->has(name))names.emplace_back(name);
            }
            if(source_->has("default.ecl"))names.emplace_back("default.ecl");
        }else if(step_<=owner_->ecl_animations.size())names.push_back(owner_->ecl_animation_names[step_-1]);
        else{
            std::snprintf(name,sizeof name,stage_files.enemies,owner_->stage_number);
            if(std::find(owner_->ecl_animation_names.begin(),owner_->ecl_animation_names.end(),name)==owner_->ecl_animation_names.end())names.emplace_back(name);
        }
        return names;
    }
    // Generation checks precede every owner dereference, including cancellation
    // while no GPU phase has been requested and the bundle has been destroyed.
    bool complete(const Token& token,bool delivered){
        if(token.generation!=generation_||token.owner!=owner_||token.phase!=phase_||token.step!=step_
            ||!owner_||!active_||!queued_||suspended_||failed_)return false;
        queued_=false;
        if(!delivered){reject("TH14 background stage resource decode failed");return true;}
        bool ok=false;
        if(phase_==Phase::Hud)ok=source_->load_stage_hud(owner_->stage_number,*owner_,selection_);
        else if(phase_==Phase::Scene)ok=source_->load_stage_scene(owner_->stage_number,*owner_);
        else if(step_==0)ok=source_->load_stage_timeline_plan(owner_->stage_number,*owner_);
        else if(step_<=owner_->ecl_animations.size())ok=source_->load_stage_animation(*owner_,step_-1);
        else ok=source_->complete_stage_timeline(owner_->stage_number,*owner_);
        if(!ok){reject("TH14 stage CPU import "+last_resource+": "+source_->error());return true;}
        if(phase_!=Phase::Timeline||step_>owner_->ecl_animations.size())active_=false;
        else{++step_;while(step_<=owner_->ecl_animations.size()&&owner_->ecl_animation_names[step_-1].empty())++step_;}
        return true;
    }
    void suspend()noexcept{if(active_){++generation_;queued_=false;suspended_=true;}}
    void resume()noexcept{suspended_=false;}
    void cancel(StageResources& stage)noexcept{if(owner_==&stage)clear();}
    void finish(StageResources& stage)noexcept{if(owner_==&stage&&!active_&&!failed_)clear();}
    bool owns(const StageResources& stage)const noexcept{return owner_==&stage;}
    bool pending()const noexcept{return active_;}
    const std::string& error()const noexcept{return error_;}
    Result result()const noexcept{return failed_?Result::Failed:active_?Result::Waiting:Result::Ready;}
private:
    Result reject(std::string message){error_=std::move(message);failed_=true;return Result::Failed;}
    void clear()noexcept{++generation_;owner_=nullptr;source_=nullptr;active_=queued_=suspended_=failed_=false;error_.clear();}
    GameResources* source_=nullptr;StageResources* owner_=nullptr;
    Phase phase_=Phase::Hud;u32 selection_=0,step_=0;u64 generation_=0;
    bool active_=false,queued_=false,suspended_=false,failed_=false;
    std::string error_;
};
}
#endif
