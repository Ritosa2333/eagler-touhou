#include "../web/cpp/multiplayer/RemotePlayerPresentation.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <type_traits>

namespace {
using Frontier=th10::multiplayer::RemotePlayerPresentation;
using U32=Frontier::u32;
using Snapshot=Frontier::Snapshot;
using Snapshots=Frontier::Snapshots;
using Pair=Frontier::Pair;

unsigned checks=0;

[[noreturn]] void fail(const char* expression,int line) {
    std::fprintf(stderr,"remote-player-presentation:%d: %s\n",line,expression);
    std::abort();
}
#define CHECK(expression) do { ++checks; if(!(expression)) fail(#expression,__LINE__); } while(false)

// Guard the complete source representation, including padding. A display
// helper may read these native-like values, but must never change one byte.
struct SourceGuard {
    const Snapshots& source;
    std::array<unsigned char,sizeof(Snapshots)> before{};
    explicit SourceGuard(const Snapshots& value):source(value) {
        static_assert(std::is_trivially_copyable_v<Snapshots>);
        std::memcpy(before.data(),&source,sizeof(source));
    }
    ~SourceGuard() { CHECK(std::memcmp(before.data(),&source,sizeof(source))==0); }
};

bool same_snapshot(const Snapshot& a,const Snapshot& b) {
    return a.player==b.player&&a.deaths==b.deaths&&a.state==b.state&&
        std::memcmp(&a.position,&b.position,sizeof(a.position))==0;
}

void same_pair(const Pair& a,const Pair& b) {
    CHECK(same_snapshot(a.from,b.from));
    CHECK(same_snapshot(a.to,b.to));
    CHECK(a.epoch==b.epoch);
    CHECK(a.from_frame==b.from_frame&&a.to_frame==b.to_frame);
    CHECK(a.from_revision==b.from_revision&&a.to_revision==b.to_revision);
    CHECK(a.flags==b.flags);
}

void snap(const Pair& pair,U32 frame,U32 revision,bool consumed=false) {
    CHECK(pair.flags==(Frontier::Valid|Frontier::Snap|
        (consumed?static_cast<U32>(Frontier::Consumed):0u)));
    CHECK(pair.epoch!=0);
    CHECK(same_snapshot(pair.from,pair.to));
    CHECK(pair.from_frame==frame&&pair.to_frame==frame);
    CHECK(pair.from_revision==revision&&pair.to_revision==revision);
}

void bridge(const Pair& pair,float from_x,float to_x,U32 from_frame,U32 to_frame,
            U32 from_revision,U32 to_revision) {
    CHECK(pair.flags==Frontier::Valid);
    CHECK(pair.from.position.x==from_x&&pair.to.position.x==to_x);
    CHECK(pair.from_frame==from_frame&&pair.to_frame==to_frame);
    CHECK(pair.from_revision==from_revision&&pair.to_revision==to_revision);
    CHECK(pair.epoch!=0);
}

struct Fixture {
    int world=1,session=2,other_world=3,other_session=4,replacement_player=5;
    std::array<int,Frontier::Capacity> players{{6,7,8}};
    Frontier presentation;
    Frontier::Identity identity{};
    Frontier::Progress progress{};
    Snapshots source{};
    U32 frame=40,revision=3;
    unsigned reads=0;

    Fixture() {
        identity.world=&world;
        identity.session=&session;
        identity.session_id=17;
        identity.generation=2;
        identity.stage=1;
        identity.screen=7;
        identity.role=Frontier::LiveRole;
        identity.local_seat=0;
        identity.player_count=Frontier::Capacity;
        for(U32 seat=0;seat<Frontier::Capacity;++seat) {
            source[seat].player=&players[seat];
            source[seat].state=1;
            source[seat].position={16.f*seat,32.f*seat,0.f};
        }
    }

    bool publish(bool forward=true) {
        SourceGuard guard(source);
        return presentation.Publish(identity,progress,frame,revision,forward,
            [this](Snapshots& out) { ++reads; out=source; });
    }
    bool freeze() {
        SourceGuard guard(source);
        return presentation.Freeze(identity,progress,frame,revision,
            [this](Snapshots& out) { ++reads; out=source; });
    }
    bool available(U32 seat=1) const {
        SourceGuard guard(source);
        Pair pair;
        const Snapshot absent{};
        const auto& current=seat<Frontier::Capacity?source[seat]:absent;
        return presentation.Get(seat,identity,progress,frame,revision,current,pair);
    }
    Pair get(U32 seat=1) const {
        SourceGuard guard(source);
        CHECK(seat<Frontier::Capacity);
        Pair pair;
        CHECK(presentation.Get(seat,identity,progress,frame,revision,source[seat],pair));
        return pair;
    }
    void consume(U32 submitted_frame,U32 submitted_revision) {
        SourceGuard guard(source);
        presentation.Consume(submitted_frame,submitted_revision);
    }
    void invalidate() {
        SourceGuard guard(source);
        presentation.Invalidate();
    }
};

void initial_and_forward_targets() {
    Fixture f;
    CHECK(!f.available());
    CHECK(f.publish(false));
    CHECK(f.reads==1);
    const auto first=f.get();
    snap(first,40,3);
    snap(f.get(2),40,3);
    CHECK(!f.available(0));
    CHECK(!f.available(Frontier::Capacity));

    f.frame=41;
    f.source[1].position.x=24.f;
    f.source[2].position.x=36.f;
    CHECK(f.publish());
    const auto moving=f.get();
    bridge(moving,16.f,24.f,40,41,3,3);
    CHECK(moving.epoch==first.epoch);
    bridge(f.get(2),32.f,36.f,40,41,3,3);
    // A genuine non-zero segment is available to repeated high-refresh Draw.
    CHECK((moving.from.position.x+moving.to.position.x)*0.5f==20.f);
    for(unsigned draw=0;draw<4;++draw) same_pair(f.get(),moving);
    CHECK(f.reads==2);
    ++f.frame; CHECK(!f.available()); --f.frame;
    ++f.revision; CHECK(!f.available()); --f.revision;
    same_pair(f.get(),moving);
}

void freeze_history_and_nested_recovery() {
    Fixture f;
    CHECK(f.publish(false));
    const auto original=f.get();
    CHECK(f.freeze());
    CHECK(f.reads==1); // A warm freeze reuses the complete published target.
    const auto identity=f.identity;
    const auto source=f.source;

    for(unsigned guard=0;guard<3;++guard) {
        f.progress={};
        if(guard==0) f.progress.correcting=true;
        if(guard==1) f.progress.frame_open=true;
        if(guard==2) f.progress.resimulating=true;
        // Even unusable, invalid historical identities must not reset a
        // previously complete anchor while one of these guards is active.
        f.progress.usable=false;
        f.identity.world=nullptr;
        f.identity.session_id=0;
        f.frame=4;
        f.revision=0;
        f.source[1].position.x=-500.f;
        CHECK(!f.freeze());
        CHECK(!f.publish());
        CHECK(!f.available());
        CHECK(f.reads==1);
        f.identity=identity;
        f.source=source;
        f.progress={};
        f.frame=40;
        f.revision=3;
        same_pair(f.get(),original);
    }

    // A nested Freeze cannot replace the first stable endpoint with a
    // partially recovered position, even if its caller repeats a stable tag.
    f.source[1].position.x=22.f;
    CHECK(f.freeze());
    CHECK(f.reads==1);
    f.frame=41;
    f.revision=4;
    f.source[1].position.x=28.f;
    CHECK(f.publish());
    bridge(f.get(),16.f,28.f,40,41,3,4);
    CHECK(f.reads==2);

    Fixture cold;
    CHECK(cold.freeze());
    CHECK(cold.reads==1);
    CHECK(cold.freeze());
    CHECK(cold.reads==1);
    cold.frame=41;
    cold.revision=4;
    cold.source[1].position.x=24.f;
    CHECK(cold.publish());
    bridge(cold.get(),16.f,24.f,40,41,3,4);
}

void repeat_publish_does_not_read_or_replace() {
    Fixture f;
    CHECK(f.publish(false));
    f.frame=41; f.source[1].position.x=24.f;
    CHECK(f.publish());
    const auto moving=f.get();
    const auto source=f.source;
    f.source[1].position.x=70.f;
    f.source[1].deaths=1;
    CHECK(!f.publish());
    CHECK(!f.publish(false));
    CHECK(f.reads==2);
    f.source=source;
    same_pair(f.get(),moving);
    f.consume(f.frame,f.revision);
    const auto consumed=f.get();
    snap(consumed,41,3,true);
    CHECK(!f.publish());
    CHECK(f.reads==2);
    same_pair(f.get(),consumed);
}

void no_forward_recovery_waits_for_real_present() {
    for(const bool presented:{false,true}) {
        Fixture f;
        CHECK(f.publish(false));
        const auto original_epoch=f.get().epoch;
        CHECK(f.freeze());
        ++f.revision;
        f.source[1].position.x=28.f;
        CHECK(f.publish(false));
        snap(f.get(),40,4);
        CHECK(f.get().epoch==original_epoch);
        CHECK(!f.publish(false));
        CHECK(f.reads==2);
        if(presented) {
            f.consume(40,4);
            snap(f.get(),40,4,true);
        } else {
            // An acknowledgement for the old revision cannot consume the
            // recovery snap or discard the unshown pre-restore anchor.
            f.consume(40,3);
            snap(f.get(),40,4);
        }
        f.frame=41; f.source[1].position.x=32.f;
        CHECK(f.publish());
        bridge(f.get(),presented?28.f:16.f,32.f,40,41,presented?4u:3u,4);
        CHECK(f.get().epoch==original_epoch);
    }
}

void consume_prevents_replaying_an_already_shown_endpoint() {
    Fixture f;
    CHECK(f.publish(false));
    f.frame=41; f.source[1].position.x=24.f;
    CHECK(f.publish());
    const auto moving=f.get();
    f.consume(40,3);
    same_pair(f.get(),moving);
    f.consume(41,2);
    same_pair(f.get(),moving);
    f.consume(41,3);
    const auto consumed=f.get();
    snap(consumed,41,3,true);
    CHECK(consumed.from.position.x==24.f);
    f.consume(41,3);
    same_pair(f.get(),consumed);

    f.frame=42; f.source[1].position.x=32.f;
    CHECK(f.publish());
    const auto next=f.get();
    bridge(next,24.f,32.f,41,42,3,3);
    f.consume(41,3); // Delayed previous-surface acknowledgement.
    same_pair(f.get(),next);
    CHECK(f.reads==3);
}

void multiple_forward_frontiers_use_the_last_complete_target() {
    Fixture f;
    CHECK(f.publish(false));
    for(U32 frame=41;frame<=43;++frame) {
        f.frame=frame;
        f.source[1].position.x=16.f+8.f*(frame-40);
        CHECK(f.publish());
        bridge(f.get(),f.source[1].position.x-8.f,f.source[1].position.x,
            frame-1,frame,3,3);
    }
    CHECK(f.freeze());
    CHECK(f.reads==4);
    f.frame=44; f.revision=4; f.source[1].position.x=44.f;
    CHECK(f.publish());
    bridge(f.get(),40.f,44.f,43,44,3,4);
    f.frame=45; f.source[1].position.x=52.f;
    CHECK(f.publish());
    bridge(f.get(),44.f,52.f,44,45,4,4);
}

void owner_reuse_death_and_state_changes_snap() {
    for(unsigned change=0;change<3;++change) {
        Fixture f;
        CHECK(f.publish(false));
        const auto old_epoch=f.get().epoch;
        if(change==0) f.source[1].player=&f.replacement_player;
        if(change==1) ++f.source[1].deaths; // Same address, new incarnation.
        if(change==2) ++f.source[1].state;
        f.frame=41; f.source[1].position.x=24.f;
        CHECK(f.publish());
        const auto reset=f.get();
        snap(reset,41,3);
        CHECK(reset.epoch!=old_epoch);
        f.frame=42; f.source[1].position.x=32.f;
        CHECK(f.publish());
        bridge(f.get(),24.f,32.f,41,42,3,3);
        CHECK(f.get().epoch==reset.epoch);
    }
}

void discontinuity_distance_boundary() {
    struct Move {float x,y;bool must_snap;};
    const std::array<Move,6> moves{{
        {127.5f,0.f,false},{128.f,0.f,true},{-128.f,0.f,true},
        {0.f,128.f,true},{96.f,96.f,true},{90.f,90.f,false}
    }};
    for(const auto& move:moves) {
        Fixture f;
        CHECK(f.publish(false));
        const auto old_epoch=f.get().epoch;
        f.frame=41;
        f.source[1].position.x+=move.x;
        f.source[1].position.y+=move.y;
        CHECK(f.publish());
        const auto pair=f.get();
        if(move.must_snap) {
            snap(pair,41,3);
            CHECK(pair.epoch!=old_epoch);
        } else {
            bridge(pair,16.f,16.f+move.x,40,41,3,3);
            CHECK(pair.epoch==old_epoch);
        }
    }
}

void invalid_snapshots_and_exact_native_target() {
    for(unsigned invalid=0;invalid<7;++invalid) {
        Fixture f;
        CHECK(f.publish(false));
        const auto original=f.source[1];
        const auto old_epoch=f.get().epoch;
        if(invalid==6) f.source[1].player=nullptr;
        else {
            const float value=invalid<3?std::numeric_limits<float>::quiet_NaN():
                std::numeric_limits<float>::infinity();
            switch(invalid%3) {
                case 0:f.source[1].position.x=value;break;
                case 1:f.source[1].position.y=value;break;
                default:f.source[1].position.z=value;break;
            }
        }
        ++f.frame;
        CHECK(f.publish());
        CHECK(!f.available());
        CHECK(f.available(2));
        f.source[1]=original;
        ++f.frame;
        CHECK(f.publish());
        snap(f.get(),42,3);
        CHECK(f.get().epoch!=old_epoch);
    }

    Fixture f;
    f.source[1].position={0.f,0.f,0.f};
    CHECK(f.publish(false));
    const auto original=f.source[1];
    for(unsigned mismatch=0;mismatch<7;++mismatch) {
        switch(mismatch) {
            case 0:f.source[1].player=&f.replacement_player;break;
            case 1:++f.source[1].deaths;break;
            case 2:++f.source[1].state;break;
            case 3:f.source[1].position.x=0.25f;break;
            case 4:f.source[1].position.x=-0.f;break;
            case 5:f.source[1].position.y=-0.f;break;
            default:f.source[1].position.z=-0.f;break;
        }
        CHECK(!f.available());
        f.source[1]=original;
        CHECK(f.available());
    }
    CHECK(f.reads==1);
}

void lifecycle_identity_cannot_bridge_between_worlds() {
    for(unsigned change=0;change<8;++change) {
        Fixture f;
        CHECK(f.publish(false));
        const auto old_epoch=f.get().epoch;
        switch(change) {
            case 0:f.identity.world=&f.other_world;break;
            case 1:f.identity.session=&f.other_session;break;
            case 2:++f.identity.session_id;break;
            case 3:++f.identity.generation;break;
            case 4:++f.identity.stage;break;
            case 5:++f.identity.screen;break;
            case 6:f.identity.local_seat=2;break;
            default:f.identity.player_count=2;break;
        }
        CHECK(!f.available());
        f.frame=41; f.source[1].position.x=24.f;
        CHECK(f.publish());
        snap(f.get(),41,3);
        CHECK(f.get().epoch!=old_epoch);
        CHECK(f.reads==2);
        if(change>=6) CHECK(!f.available(2));
    }

    for(unsigned invalid=0;invalid<9;++invalid) {
        Fixture f;
        CHECK(f.publish(false));
        const auto valid_identity=f.identity;
        const auto old_epoch=f.get().epoch;
        switch(invalid) {
            case 0:f.identity.world=nullptr;break;
            case 1:f.identity.session=nullptr;break;
            case 2:f.identity.session_id=0;break;
            case 3:f.identity.role=0;break;
            case 4:f.identity.role=Frontier::LiveRole+1;break;
            case 5:f.identity.player_count=1;break;
            case 6:f.identity.player_count=Frontier::Capacity+1;break;
            case 7:f.identity.local_seat=f.identity.player_count;break;
            default:f.identity.local_seat=Frontier::NoFrame;break;
        }
        CHECK(!f.freeze());
        CHECK(!f.publish());
        CHECK(!f.available());
        CHECK(f.reads==1);
        f.identity=valid_identity;
        CHECK(!f.available());
        CHECK(f.publish());
        snap(f.get(),40,3);
        CHECK(f.get().epoch!=old_epoch);
    }
}

void unusable_regression_and_explicit_reset() {
    Fixture f;
    CHECK(f.publish(false));
    const auto first_epoch=f.get().epoch;
    f.progress.usable=false;
    CHECK(!f.freeze());
    CHECK(!f.publish());
    CHECK(!f.available());
    CHECK(f.reads==1);
    f.progress={};
    CHECK(!f.available());
    CHECK(f.publish());
    CHECK(f.get().epoch!=first_epoch);

    const auto valid=f.get();
    f.frame=Frontier::NoFrame;
    CHECK(!f.freeze());
    CHECK(!f.publish());
    CHECK(!f.available());
    CHECK(f.reads==2);
    f.frame=40;
    same_pair(f.get(),valid);

    f.frame=39;
    CHECK(!f.publish());
    CHECK(f.reads==2);
    f.frame=40;
    CHECK(!f.available());
    CHECK(f.publish());
    CHECK(f.get().epoch!=valid.epoch);
    const auto before_reset=f.get().epoch;
    f.invalidate();
    CHECK(!f.available());
    CHECK(f.publish());
    snap(f.get(),40,3);
    CHECK(f.get().epoch!=before_reset);
}
} // namespace

int main() {
    initial_and_forward_targets();
    freeze_history_and_nested_recovery();
    repeat_publish_does_not_read_or_replace();
    no_forward_recovery_waits_for_real_present();
    consume_prevents_replaying_an_already_shown_endpoint();
    multiple_forward_frontiers_use_the_last_complete_target();
    owner_reuse_death_and_state_changes_snap();
    discontinuity_distance_boundary();
    invalid_snapshots_and_exact_native_target();
    lifecycle_identity_cannot_bridge_between_worlds();
    unusable_regression_and_explicit_reset();
    std::printf("{\"passed\":true,\"suite\":\"remote-player-presentation\","
        "\"groups\":11,\"checks\":%u,\"sourceByteGuards\":true}\n",checks);
    return 0;
}
