#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error RemotePlayerPresentation is multiplayer-only
#endif

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace th10::multiplayer {

// Host presentation values only. Never journal this object, use it as input,
// or install its interpolated position in a native Player. Publication reads
// complete predicted frontiers; historical Update/Draw cannot publish here.
class RemotePlayerPresentation {
public:
    using u32=std::uint32_t;
    static constexpr u32 Capacity=3,NoFrame=0xffffffffu,LiveRole=1;
    enum PairFlag:u32 {Valid=1u,Snap=2u,Consumed=4u};
    struct Position {float x=0,y=0,z=0;};
    struct Snapshot {
        const void* player=nullptr;
        u32 deaths=0;
        std::int32_t state=0;
        Position position{};
    };
    using Snapshots=std::array<Snapshot,Capacity>;
    struct Identity {
        const void* world=nullptr;
        const void* session=nullptr;
        std::uint64_t session_id=0;
        u32 generation=0,stage=0,screen=0,role=0,local_seat=NoFrame,player_count=0;
        bool operator==(const Identity& other)const noexcept {
            return world==other.world&&session==other.session&&session_id==other.session_id&&
                generation==other.generation&&stage==other.stage&&screen==other.screen&&
                role==other.role&&local_seat==other.local_seat&&player_count==other.player_count;
        }
    };
    struct Progress {
        bool usable=true,correcting=false,frame_open=false,resimulating=false;
        bool Complete()const noexcept{return usable&&!correcting&&!frame_open&&!resimulating;}
    };
    struct Pair {
        Snapshot from{},to{};
        u32 epoch=0,from_frame=NoFrame,to_frame=NoFrame;
        u32 from_revision=0,to_revision=0,flags=0;
    };

    void Invalidate()noexcept {
        known_=published_=frozen_=false;
        identity_=Identity{};pairs_={};anchors_={};
        frame_=anchor_frame_=NoFrame;revision_=anchor_revision_=0;
    }

    // Freeze only the FIRST complete pre-restore endpoint. Nested correction
    // calls fail before invoking read_current or observing historical identity.
    // A cold cache may be seeded here; ordinary freezes reuse its complete data.
    template<class ReadCurrent>
    bool Freeze(const Identity& identity,const Progress& progress,u32 frame,u32 revision,
                ReadCurrent read_current){
        if(!admit(identity,progress)||frame==NoFrame)return false;
        if(frozen_)return true;
        if(!published_||frame_!=frame||revision_!=revision){
            if(!Publish(identity,progress,frame,revision,false,read_current))return false;
        }
        for(u32 seat=0;seat<Capacity;++seat)
            anchors_[seat]=(pairs_[seat].flags&Valid)?pairs_[seat].to:Snapshot{};
        anchor_frame_=frame_;anchor_revision_=revision_;frozen_=true;
        return true;
    }

    // One call at Application::step's FINAL complete frontier, after authored
    // Draw. Do not publish EndCorrection and then publish its forward tick:
    // doing so would discard the correction displacement we need to display.
    template<class ReadCurrent>
    bool Publish(const Identity& identity,const Progress& progress,u32 frame,u32 revision,
                 bool forward,ReadCurrent read_current){
        if(!admit(identity,progress)||frame==NoFrame)return false;
        if(published_&&frame==frame_&&revision==revision_)return false;
        if(published_&&frame<frame_){Invalidate();return false;}
        Snapshots current{};
        read_current(current);
        const bool advanced=published_&&forward&&frame>frame_;
        for(u32 seat=0;seat<Capacity;++seat){
            auto& pair=pairs_[seat];
            if(seat>=identity.player_count||seat==identity.local_seat||!valid(current[seat])){
                pair=Pair{};continue;
            }
            const Snapshot previous=frozen_?anchors_[seat]:pair.to;
            const bool continuous=(pair.flags&Valid)&&same_owner(previous,current[seat])&&
                nearby(previous.position,current[seat].position);
            const u32 old_epoch=pair.epoch;
            Pair next;
            next.epoch=continuous?old_epoch:next_epoch();
            next.to=current[seat];next.to_frame=frame;next.to_revision=revision;
            if(advanced&&continuous){
                next.from=previous;
                next.from_frame=frozen_?anchor_frame_:frame_;
                next.from_revision=frozen_?anchor_revision_:revision_;
                next.flags=Valid;
            }else{
                next.from=next.to;next.from_frame=frame;next.from_revision=revision;
                next.flags=Valid|Snap;
            }
            pair=next;
        }
        frame_=frame;revision_=revision;published_=true;
        // A no-forward recovery is shown at alpha=1 by the existing clock.
        // Until that real Present occurs, retain the old complete anchor: a
        // following forward tick can still bridge over an unshown correction.
        if(advanced)frozen_=false;
        return true;
    }

    // Pure read. The current native target must match byte-for-byte; a stale
    // cache cannot silently replace a newer logical position or reused owner.
    bool Get(u32 seat,const Identity& identity,const Progress& progress,u32 frame,u32 revision,
             const Snapshot& current,Pair& output)const noexcept {
        output=Pair{};
        if(!progress.Complete()||!valid_identity(identity)||!known_||!published_||
           !(identity==identity_)||seat>=identity.player_count||seat==identity.local_seat||
           frame!=frame_||revision!=revision_)return false;
        const auto& pair=pairs_[seat];
        if(!(pair.flags&Valid)||!valid(current)||!same_owner(pair.to,current)||
           !same_position(pair.to.position,current.position))return false;
        output=pair;return true;
    }

    // Call only AFTER an actual alpha=1/authored endpoint Present. Later alpha
    // changes cannot replay the already-shown pair backwards. Duplicate and
    // stale acknowledgements cannot consume a newer publication.
    void Consume(u32 frame,u32 revision)noexcept {
        if(!published_||frame!=frame_||revision!=revision_)return;
        for(auto& pair:pairs_)if(pair.flags&Valid){
            pair.from=pair.to;pair.from_frame=pair.to_frame;
            pair.from_revision=pair.to_revision;pair.flags=Valid|Snap|Consumed;
        }
        frozen_=false;
    }

private:
    static bool valid_identity(const Identity& identity)noexcept {
        return identity.world&&identity.session&&identity.session_id&&identity.role==LiveRole&&
            identity.player_count>=2&&identity.player_count<=Capacity&&
            identity.local_seat<identity.player_count;
    }
    static bool valid(const Snapshot& source)noexcept {
        return source.player&&std::isfinite(source.position.x)&&
            std::isfinite(source.position.y)&&std::isfinite(source.position.z);
    }
    static bool same_owner(const Snapshot& a,const Snapshot& b)noexcept {
        return a.player&&a.player==b.player&&a.deaths==b.deaths&&a.state==b.state;
    }
    static bool same_position(const Position& a,const Position& b)noexcept {
        static_assert(sizeof(Position)==3*sizeof(float));
        return std::memcmp(&a,&b,sizeof(Position))==0;
    }
    static bool nearby(const Position& a,const Position& b)noexcept {
        const float x=b.x-a.x,y=b.y-a.y;
        return std::isfinite(x)&&std::isfinite(y)&&x*x+y*y<16384.f;
    }
    bool admit(const Identity& identity,const Progress& progress)noexcept {
        if(progress.correcting||progress.frame_open||progress.resimulating)return false;
        if(!progress.usable||!valid_identity(identity)){Invalidate();return false;}
        if(!known_||!(identity==identity_)){
            Invalidate();identity_=identity;known_=true;
        }
        return true;
    }
    u32 next_epoch()noexcept{if(!++epoch_)++epoch_;return epoch_;}

    Identity identity_{};
    std::array<Pair,Capacity> pairs_{};
    Snapshots anchors_{};
    u32 frame_=NoFrame,revision_=0,anchor_frame_=NoFrame,anchor_revision_=0,epoch_=0;
    bool known_=false,published_=false,frozen_=false;
};

} // namespace th10::multiplayer
