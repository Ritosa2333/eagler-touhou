#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../platform_services/configuration.hpp"
#include "../program_entry/device.hpp"
#include "../../native_recovered/native_core.hpp"
#include <d3d9.h>

namespace th20::source::gameplay {
class GameController;
// Only references are stored here. The actual subsystem owners are still
// defined by their own reconstructed modules, never fabricated by gameplay.
enum class Owner {
    session_overlay, global_005c6120, global_005c60fc,
    global_005c069c,global_005c06a0,global_005c60bc,global_005c06a4,
    player_primary,global_005c6114
};
enum class Cleanup {
    fn_004feff0,fn_00485340,fn_004c47a0,fn_004d5be0,fn_00510830,fn_00513cc0,
    fn_004b64a0,fn_004bc220,fn_004aa940,fn_004a80f0,fn_0049d400,fn_00477fd0,fn_00488690,fn_00532f40
};
class Services {
public:
    virtual ~Services()=default;
    virtual std::uint32_t& input_latch()=0;               // 5b8858 = WindowState+2100
    virtual Device& device()=0;                           // 412730 on GraphicsState
    virtual runtime::Worker& worker()=0;                 // 5c5ad0 = GraphicsState+d90
    virtual std::uint32_t& session_flags()=0;             // GameSession+6c
    virtual std::int32_t session_mode()=0;                // GameSession+70
    virtual std::int32_t scene()=0;                      // 4ac0c0: GraphicsState+b0c
    virtual runtime::CallbackOwner* owner(Owner)=0;       // actual nullable global/selected-owner values
    virtual void preserve_background_as_secondary()=0;   //5c06a0 =5c069c after prior secondary retirement
    virtual void load(GameController&)=0;                // 4bad40, required entity/stage initialization
    virtual void commit_progress_if_present()=0;         // nullable5c6108 ->50f660
    virtual void reset_clock_scale()=0;                  // 4292a0(global,1.0f)
    virtual void clear_surface_callbacks()=0;            // 5c4d30/34 = 0
    virtual void screen_transition(float,float)=0;       // 4a0aa0 on5c0698
    virtual bool stage_selection_changed()=0;            // 498f40 ->4bd5c0/474d80
    virtual void increment_continue_count()=0;           // 4bccc0 ->4bccf0(first player,1)
    virtual void cleanup(Cleanup)=0;                     // named, still-required owning-subsystem operations
    virtual void remove_callback(scheduler::Node*)=0;    // 4124b0 shared scheduler
    virtual void stop_music()=0;                         // 4d9bc0 ->audio enqueue3/4
    virtual void clear_queued_music_name()=0;             // 4bd960: SoundInf+2444[0]=0
    virtual void stop_all_effects()=0;                   // 4ba440: requests[0].id=-1 then stop90
    virtual void set_final_clear_color(std::uint32_t)=0; // 5c5b20 = GraphicsState+de0
};

#pragma pack(push,4)
class GameController final : public runtime::CallbackOwner {
public:
    recovered::Timer frame_timer;                         // +10, ctor422d90
    recovered::Timer secondary_timer;                     // +20
    std::uint32_t load_stage,field_34;                     // +30/+34
    platform::Configuration configuration;                // +38, ctor4b9c10
    std::uint32_t game_flags;                             // +e8, distinct from base flags+4
    std::uint32_t field_ec,field_f0,field_f4;
    double field_f8,field_100;
    std::int32_t restart_mode;                            // +108
    std::uint32_t field_10c;
    Services* services;                                  // source-only +110

    explicit GameController(Services&);                   // factory4b9b10 ->4b9df0
    ~GameController() override;                          // 4b9ef0 +41fdf0
    bool update_suppressed() const noexcept {return ((game_flags&1u)|((game_flags>>2)&1u))!=0;} //424030
    bool animation_frozen() const noexcept {return ((game_flags>>1)&1u)!=0;} //424060
    std::int32_t restart() const noexcept {return restart_mode;} //488830
    void clear_flag_6() noexcept {game_flags&=~0x40u;}      // 4bd920
};
#pragma pack(pop)
static_assert(offsetof(GameController,frame_timer)==0x10);
static_assert(offsetof(GameController,configuration)==0x38);
static_assert(offsetof(GameController,game_flags)==0xe8);
static_assert(offsetof(GameController,restart_mode)==0x108);
static_assert(offsetof(GameController,services)==0x110);
extern GameController* controller;                        // actual global5ba828
GameController* create(Services&,std::int32_t);             // 4bec70
void destroy(Services&,GameController*);                    // 4bd3d0
void start_loading(Services&);                             // 4b99f0 ->40b1d0 ->4bcca0
bool animation_frozen() noexcept;
bool suppress_primary_update() noexcept;                   // 450880 composition
}
