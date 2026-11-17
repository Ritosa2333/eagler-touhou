#pragma once
#include "../game/GameSession.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/ConfirmedInputClock.hpp>
#include <eagler/netplay/InputReplay.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

namespace th11 {
std::array<u32,8> th14_room_status(const GameSession&,u32 next);
struct RoomSetupTh14 {
    u64 id=0;
    bool challenge=false;
    static constexpr u32 max_prediction=12;
    u32 difficulty=0,seed=0,seats=0,local=0,delay=0,prediction=0;
    std::array<Th14CoopLoadout,3> loadouts{};
    std::array<u32,4> build{};
    bool valid()const;
    u32 abi()const;
    std::vector<u8> describe()const;
    bool read(const Netplay::InputReplayInfo&,const std::array<u32,4>&);
};
class RoomTh14 {
public:
    struct Frame {
        u32 number=Netplay::INVALID_FRAME,stage=0,active_stage=0;
        u8 boss_slots=0;
        bool terminal=false,ending=false,results=false,cleared=false,restart=false;
        FrameImageTh14 image;
        BulletManager::RoomSlots bullets;
        ItemManager::RoomSlots items;
        std::vector<BattleEvent> events;
        std::vector<DialogueText> text;
        std::array<u32,8> status{};
        void clear_output(){number=Netplay::INVALID_FRAME;terminal=ending=results=cleared=restart=false;events.clear();text.clear();}
        void clear(){clear_output();image.clear();bullets.clear();items.clear();}
        void release_checkpoint(){image.release_storage();bullets.release_storage();items.release_storage();}
        std::size_t checkpoint_bytes()const noexcept{
            return image.reserved_bytes()+bullets.reserved_bytes()+items.reserved_bytes();
        }
        void reuse_checkpoint_storage(Frame& other) noexcept{
            image.swap_storage(other.image);
            bullets.swap_storage(other.bullets);
            items.swap_storage(other.items);
        }
    };
    RoomSetupTh14 setup;
    Netplay::BrowserPeerTransport transport;
    Netplay::SessionGate gate;
    Netplay::RollbackCore inputs;
    Netplay::ConfirmedInputClock capture_clock;
    Netplay::SessionChannel channel{transport};
    Netplay::InputReplay replay;
    Netplay::SpectatorStagePublisher publisher;
    Netplay::SpectatorStagePlayback viewer{Netplay::SpectatorStagePlayback::RestoreMode::Run,true};
    Netplay::SpectatorStageView view;
    u32 frame=0,captured=0,confirmed=0,replay_until=0;
    u32 corrections=0,resimulated=0,combat_corrections=0,blocked=Netplay::INVALID_FRAME;
    u32 restored_checkpoint=Netplay::INVALID_FRAME;
    u32 memory_wait_frames=0;
    u32 replay_stage_offset=0;
    u32 stage_start_frame=0;
    struct SpectatorMark {u32 frame,stage;u8 boss_slots;bool checkpoint;};
    std::vector<SpectatorMark> spectator_marks;
    std::size_t published_marks=0;
    u32 recorded_stage=0,published_stage=0;
    bool bookmark_restart=false;
    bool cleared=false;
    u64 publication_deadline=0;
    std::array<Frame,RoomSetupTh14::max_prediction+1> history;
    // Keep a spare allocation after confirming or discarding a frame.
    // The history ring keeps only storage actually in use by a checkpoint.
    Frame spare_checkpoint;
    std::array<std::array<u32,8>,Netplay::INPUT_HISTORY_SIZE> confirmed_status{};
    bool started=false,finished=false,local_ending=false,local_results=false,playback=false,spectator=false,publish=true,paused=false;
    bool begin(const char*,const char*,u64);
    bool poll(u64);
    bool capture(u64 now_us,unsigned lead,const Netplay::FrameInput&);
    bool record(u32 number,const Netplay::InputReplay::Frame&,const Frame&);
    void publish_confirmed();
    bool finish_publication(u64 now);
    bool receive();
    std::size_t checkpoint_bytes()const noexcept{
        std::size_t bytes=spare_checkpoint.checkpoint_bytes();
        for(const auto& frame:history)bytes+=frame.checkpoint_bytes();
        return bytes;
    }
    std::array<u32,3> checkpoint_breakdown()const noexcept{
        std::array<u32,3> bytes{};
        const auto add=[&](const Frame& frame){
            bytes[0]+=u32(frame.image.reserved_bytes());
            bytes[1]+=u32(frame.bullets.reserved_bytes());
            bytes[2]+=u32(frame.items.reserved_bytes());
        };
        add(spare_checkpoint);
        for(const auto& frame:history)add(frame);
        return bytes;
    }
    void recycle_checkpoint(Frame& frame){
        frame.clear();
        if(frame.checkpoint_bytes()>spare_checkpoint.checkpoint_bytes())
            frame.reuse_checkpoint_storage(spare_checkpoint);
        frame.release_checkpoint();
    }
    Frame& state_at(u32 index){return history[index%history.size()];}
    static GameSessionInput to_input(const Netplay::InputReplay::Frame&);
};
}
