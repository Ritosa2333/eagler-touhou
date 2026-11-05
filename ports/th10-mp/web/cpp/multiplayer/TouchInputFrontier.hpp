#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error TouchInputFrontier is multiplayer-only
#endif

#include <array>
#include <cstdint>

namespace th10::multiplayer {

// A host input view, never a rollback owner or a presentation interpolation.
// Preserve the existing application_touch_state ABI: context, instance, ready,
// x, y, fast speed, slow speed and engine speed (the last five are float bits).
class TouchInputFrontier {
public:
    using Words=std::array<std::uint32_t,8>;
    static constexpr std::uint32_t NoFrame=~std::uint32_t(0);

    struct Identity {
        const void* world=nullptr;
        const void* session=nullptr;
        std::uint64_t session_id=0;
        std::uint32_t generation=0,seat=NoFrame,players=0,role=0;
        bool operator==(const Identity& other)const noexcept {
            return world==other.world&&session==other.session&&session_id==other.session_id&&
                   generation==other.generation&&seat==other.seat&&players==other.players&&role==other.role;
        }
    };
    struct Progress {
        bool correcting=false,frame_open=false,resimulating=false,usable=true;
        bool has_completed_tick=true;
        bool stable()const noexcept {return usable&&!correcting&&!frame_open&&!resimulating;}
        bool complete()const noexcept {return stable()&&has_completed_tick;}
        bool cold_start()const noexcept {return stable()&&!has_completed_tick;}
    };

    void Invalidate()noexcept {
        valid_=identity_known_=false;frontier_=NoFrame;
        advance_epoch();
    }
    void Observe(const Identity& identity)noexcept {
        if(identity_known_&&identity_==identity)return;
        identity_=identity;identity_known_=true;valid_=false;frontier_=NoFrame;
        advance_epoch();
    }

    // read_current must run only at a complete logical endpoint. In particular
    // a nested correction must not even read the historical Player/context.
    template<class ReadCurrent>
    bool Publish(const Identity& identity,const Progress& progress,
                 std::uint32_t frontier,ReadCurrent read_current){
        Observe(identity);
        if(!progress.complete())return false;
        Words next{};
        read_current(next);
        words_=next;frontier_=frontier;valid_=true;
        return true;
    }
    bool Copy(const Identity& identity,const Progress& progress,Words& output)noexcept {
        Observe(identity);
        output={};
        if(!progress.usable||!valid_)return false;
        output=words_;return true;
    }
    template<class ReadCurrent>
    bool Read(const Identity& identity,const Progress& progress,
              std::uint32_t frontier,ReadCurrent read_current,Words& output){
        if(progress.cold_start()){
            // Keep initial controls unchanged without claiming frame zero has
            // completed Update + Draw. Correcting at zero cannot enter here.
            Observe(identity);output={};read_current(output);return true;
        }
        Publish(identity,progress,frontier,read_current);
        return Copy(identity,progress,output);
    }

    std::uint64_t Epoch()const noexcept {return epoch_;}
    std::uint32_t Frontier()const noexcept {return frontier_;}
    bool Valid()const noexcept {return valid_;}

private:
    void advance_epoch()noexcept {if(!++epoch_)++epoch_;}
    Identity identity_{};
    Words words_{};
    std::uint64_t epoch_=0;
    std::uint32_t frontier_=NoFrame;
    bool valid_=false,identity_known_=false;
};

} // namespace th10::multiplayer
