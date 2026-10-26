#if TH15_MULTIPLAYER
#include "AppFrameState.hpp"
#include "ApplicationState.hpp"
#include "../multiplayer/StageFrameState.hpp"
#include "../multiplayer/FrameState.hpp"
#include "../game/MultiplayerHud.hpp"
#include "../multiplayer/FrameValues.hpp"
namespace th15::sdl {
namespace fv=multiplayer::frame_values;
auto AppFrameState::fields(ApplicationState& a){return std::tie(a.pending_load,a.pending_page,a.pending_bank,a.frame_skip,a.pending_destination,a.current_destination,a.transition_overlay,a.restart_handle,a.restart_effect_handle,a.frames,a.exiting,a.completed_recording,a.projected,a.failure,a.manual_pressed,a.manual_repeated,a.touch);}
struct AppFrameState::Data {
    struct Fade {
        std::shared_ptr<ScreenFade> owner;ScreenFadeState state;
        float rate=1;bool shutdown=false,active=false,covering=false,full_screen=false,game_available=false;u32 game_flags=0;
    };
    struct Motion {
        std::shared_ptr<ScreenMotionFrame> owner;bool active=false;
        Timer age;Vec2 world_offset{},screen_offset{};
    };
    ApplicationState* owner=nullptr;RunSession* session=nullptr;RunPause* pause=nullptr;
    decltype(fv::copy(fields(std::declval<ApplicationState&>()))) values;
    multiplayer::StageFrameState stage;
    PauseState pause_state;
    PauseFrameInput pause_input;
    bool pause_selection=false;
    std::array<std::string,6> pause_errors;
    double playtime_origin=0;
    std::string session_error,flow_error,display_error,game_error,background_error,graphics_error;
    StageDrawState background;float background_rate=1;
    std::array<Vec2,4> view_offsets{};
    std::vector<Fade> fades;std::vector<Motion> motion;size_t fade_count=0,motion_count=0;
    std::vector<AsciiTextEntry> captions;std::string caption_error; i32 caption_spacing=9;
    std::vector<AnmVm*> roots;
    std::array<std::array<u32,8>,3> hud_lives{},hud_bombs{};
    std::array<std::array<i32,5>,3> hud_displayed{};
    std::vector<u32> hud_widgets;std::string hud_error;
    i32 ascii_frames=0;std::string ascii_error;
    bool ready=false;
};
AppFrameState::AppFrameState()=default;
AppFrameState::~AppFrameState()=default;
AppFrameState::AppFrameState(AppFrameState&&)noexcept=default;
AppFrameState& AppFrameState::operator=(AppFrameState&&)noexcept=default;
bool AppFrameState::ready(const ApplicationState& a){
    auto* stage=a.run?a.run->scene():nullptr;
    return a.multiplayer&&a.initialized&&!a.exiting&&a.failure.empty()&&!a.pending_load&&a.pending_destination<0&&!a.title&&!a.ending&&a.session&&a.session->driver()&&stage&&stage->spectator_ready()&&!stage->messages.active()&&a.display&&a.display->game&&!a.display->previous_background&&a.pause&&a.pause->state.screen==PauseScreen::Inactive&&a.session->driver()->runtime.age.current>30;
}
void AppFrameState::discard()noexcept{if(saved_){saved_->stage.discard();for(auto& e:saved_->fades)e.owner.reset();for(auto& e:saved_->motion)e.owner.reset();saved_->ready=false;}}
bool AppFrameState::capture(ApplicationState& a){
    fv::IdentityGuard guard;error.clear();if(!ready(a)){error="Application is outside the battle correction window";return false;}
    if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;s.owner=&a;s.session=a.session.get();s.pause=a.pause.get();s.values=fv::copy(fields(a));
    s.session_error=a.session->error;s.flow_error=a.flow?a.flow->error:std::string{};s.display_error=a.display->error;s.game_error=a.display->game->error;s.background_error=a.display->game->background.error;s.graphics_error=a.graphics.error;
    s.background=a.display->game->background.state;s.background_rate=a.display->game->background.rate;
    for(unsigned i=0;i<4;++i)s.view_offsets[i]=a.views.view(DrawCamera(i)).offset;
    s.pause_state=a.pause->state;s.pause_input=a.pause->frame.input;s.pause_selection=a.pause->selection;s.playtime_origin=a.pause->time.origin;
    s.pause_errors={a.pause->error,a.pause->frame.error,a.pause->activation.error,a.pause->time.error,a.pause->menu.error,a.pause->actions.error};
    s.captions=a.captions.entries;s.caption_error=a.captions.error;s.caption_spacing=a.captions.spacing;
    s.roots.clear();s.roots.push_back(&a.captions.glyph);
    if(auto* hud=a.display->game->multiplayer_hud.get()){s.hud_lives=hud->lives;s.hud_bombs=hud->bombs;s.hud_displayed=hud->displayed;s.hud_widgets=hud->widgets;s.hud_error=hud->error;s.roots.push_back(&hud->transfer_power_icon);}
    if(auto* ascii=a.display->caption_drawing.get()){s.ascii_frames=ascii->frames;s.ascii_error=ascii->error;}
    s.fade_count=a.fades.size();if(s.fades.size()<s.fade_count)s.fades.resize(s.fade_count);
    for(size_t i=0;i<s.fade_count;++i){auto& entry=s.fades[i];entry.owner=a.fades[i];const auto& fade=*entry.owner;entry.state=fade.state;entry.rate=fade.rate;entry.shutdown=fade.shutdown;entry.active=fade.active;entry.covering=fade.covering;entry.full_screen=fade.full_screen;entry.game_available=fade.game_available;entry.game_flags=fade.game_flags;s.roots.push_back(&entry.owner->presentation_vm);}
    for(size_t i=s.fade_count;i<s.fades.size();++i)s.fades[i].owner.reset();
    s.motion_count=a.motion.size();if(s.motion.size()<s.motion_count)s.motion.resize(s.motion_count);
    for(size_t i=0;i<s.motion_count;++i){auto& entry=s.motion[i];entry.owner=a.motion[i];const auto& motion=*entry.owner;entry.active=motion.active;if(motion.shake){entry.age=motion.shake->age;entry.world_offset=motion.shake->world_offset;entry.screen_offset=motion.shake->screen_offset;}else{entry.age=motion.nudge->age;entry.world_offset=motion.nudge->world_offset;entry.screen_offset=motion.nudge->screen_offset;}}
    for(size_t i=s.motion_count;i<s.motion.size();++i)s.motion[i].owner.reset();
    AnmVm::presentation_counter=guard.value;
    if(!s.stage.capture(*a.scene(),a.session->driver(),s.roots)){error=s.stage.error;return false;}s.ready=true;return true;
}
bool AppFrameState::restore(ApplicationState& a)const{
    error.clear();if(!saved_||!saved_->ready||saved_->owner!=&a||saved_->session!=a.session.get()||saved_->pause!=a.pause.get()){error="Application rollback crossed a run factory";return false;}const auto& s=*saved_;
    // Unlink before releasing future owners. Their destructors then have no
    // callback side effects; StageFrameState reinstalls the captured graph.
    multiplayer::FrameState::detach(a.scheduler);
    a.fades.clear();for(size_t i=0;i<s.fade_count;++i){const auto& e=s.fades[i];a.fades.push_back(e.owner);auto& f=*e.owner;f.state=e.state;f.rate=e.rate;f.shutdown=e.shutdown;f.active=e.active;f.covering=e.covering;f.full_screen=e.full_screen;f.game_available=e.game_available;f.game_flags=e.game_flags;}
    a.motion.clear();for(size_t i=0;i<s.motion_count;++i){const auto& e=s.motion[i];a.motion.push_back(e.owner);auto& m=*e.owner;m.active=e.active;if(m.shake){m.shake->age=e.age;m.shake->world_offset=e.world_offset;m.shake->screen_offset=e.screen_offset;}else{m.nudge->age=e.age;m.nudge->world_offset=e.world_offset;m.nudge->screen_offset=e.screen_offset;}}
    fields(a)=s.values;a.session->error=s.session_error;if(a.flow)a.flow->error=s.flow_error;a.display->error=s.display_error;a.display->game->error=s.game_error;a.display->game->background.error=s.background_error;a.graphics.error=s.graphics_error;
    a.display->game->background.state=s.background;a.display->game->background.rate=s.background_rate;for(unsigned i=0;i<4;++i)a.views.view(DrawCamera(i)).offset=s.view_offsets[i];
    a.pause->state=s.pause_state;a.pause->frame.input=s.pause_input;a.pause->selection=s.pause_selection;a.pause->time.origin=s.playtime_origin;
    std::tie(a.pause->error,a.pause->frame.error,a.pause->activation.error,a.pause->time.error,a.pause->menu.error,a.pause->actions.error)=std::tie(s.pause_errors[0],s.pause_errors[1],s.pause_errors[2],s.pause_errors[3],s.pause_errors[4],s.pause_errors[5]);
    a.captions.entries=s.captions;a.captions.error=s.caption_error;a.captions.spacing=s.caption_spacing;
    if(auto* hud=a.display->game->multiplayer_hud.get()){hud->lives=s.hud_lives;hud->bombs=s.hud_bombs;hud->displayed=s.hud_displayed;hud->widgets=s.hud_widgets;hud->error=s.hud_error;}
    if(auto* ascii=a.display->caption_drawing.get()){ascii->frames=s.ascii_frames;ascii->error=s.ascii_error;}
    if(!s.stage.restore(*a.scene(),a.session->driver())){error=s.stage.error;return false;}
    a.renderer.invalidate();return true;
}
}

#endif
