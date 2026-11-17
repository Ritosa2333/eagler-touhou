#pragma once
#include "../game/GameSessionResources.hpp"
#include <array>
#include <utility>

#if PORT_TITLE == 14
namespace th11::sdl {

// Owns only upload work references. The host owns textures and releases them
// after cancel() has removed these references. Main prepares each file's ANM
// templates after its final chunk, before the next resource can be uploaded.
class StageUploadsTh14 {
public:
    using Phase = StageResourceEffects::Phase;
    struct Job { AnmResource* resource=nullptr; u16 file=0; u32 chunk=0; };

#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    bool begin_file(StageResources& stage,Phase phase,AnmResource& resource,u16 file){
        if(failed_||(owner_&&owner_!=&stage))return false;
        if(pending())return phase_==phase&&queued_==1&&jobs_[cursor_].resource==&resource&&jobs_[cursor_].file==file;
        jobs_={};jobs_[0]={&resource,file,0};owner_=&stage;phase_=phase;queued_=1;cursor_=0;return true;
    }
    bool complete_phase(StageResources& stage,Phase phase){
        if(failed_||(owner_&&owner_!=&stage)||u32(phase)>2)return false;
        // A later phase may already have offered its file before Main
        // consumes this Ready. Retain that job and its upload cursor.
        if(pending()&&u32(phase_)<=u32(phase))return false;
        owner_=&stage;completed_phases_|=1u<<u32(phase);return true;
    }
#endif

    bool begin(StageResources& stage,Phase phase) {
        if(owner_) {
            if(owner_!=&stage||failed_)return false;
            if(phase_==phase)return true;
            if(pending())return false;
        }
        std::array<Job,5> next{};
        u32 count=0;
        switch(phase) {
        case Phase::Hud:next[count++]={&stage.logo,6};break;
        case Phase::Scene:next[count++]={&stage.background,u16((stage.stage_number&1)+3)};break;
        case Phase::Timeline:
            // 42256a loads exactly the ECL ANIM list at file 10+i.
            // stage.enemies is a legacy duplicate, not another native upload.
            for(u32 i=0;i<stage.ecl_animations.size();++i)
                if(!stage.ecl_animation_names[i].empty())next[count++]={&stage.ecl_animations[i],u16(10+i)};
            break;
        default:return false;
        }
        jobs_=next;owner_=&stage;phase_=phase;queued_=count;cursor_=0;failed_=false;
        return true;
    }

    bool pending()const noexcept {return owner_&&cursor_<queued_;}
    u32 pending_count()const noexcept {return owner_?queued_-cursor_:0;}
    const StageResources* owner()const noexcept {return owner_;}
    bool failed()const noexcept {return failed_;}

    template<class Prepare> bool advance(Prepare&& prepare) {
        return advance(std::forward<Prepare>(prepare),[](const Job&){return true;});
    }
    template<class Prepare,class Complete> bool advance(Prepare&& prepare,Complete&& complete) {
        if(failed_)return false;
        if(!pending())return true;
        // A failure can follow partial texture preparation. Keep this job and
        // every subsequent resource reference until the host cancels owner_.
        if(!std::forward<Prepare>(prepare)(jobs_[cursor_])) {failed_=true;return false;}
        // Original 47d720 calls 47d230 once: one ANM chunk per pump.
        // The last chunk also initializes templates and clears the file wait.
        auto& job=jobs_[cursor_];
        if(job.chunk+1<job.resource->textures.size()){++job.chunk;return true;}
        if(!std::forward<Complete>(complete)(job)){failed_=true;return false;}
        job={};++cursor_;
        return true;
    }

    bool ready(Phase phase)const noexcept {
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
        if(owner_&&!failed_&&u32(phase)<3&&(completed_phases_&(1u<<u32(phase))))return true;
#endif
        return owner_&&phase_==phase&&!failed_&&!pending();
    }
    void finish(StageResources& stage)noexcept {
        if(owner_==&stage&&!failed_&&!pending())clear();
    }
    void cancel(StageResources& stage)noexcept {if(owner_==&stage)clear();}
    void cancel_all()noexcept {clear();}

private:
    void clear()noexcept {jobs_={};owner_=nullptr;queued_=cursor_=0;phase_=Phase::Hud;failed_=false;
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
        completed_phases_=0;
#endif
    }
    std::array<Job,5> jobs_{};
    StageResources* owner_=nullptr;
    Phase phase_=Phase::Hud;
    u32 queued_=0,cursor_=0;
    bool failed_=false;
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    u32 completed_phases_=0;
#endif
};
}
#endif
