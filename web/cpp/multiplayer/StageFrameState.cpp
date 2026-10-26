#include "StageFrameState.hpp"
#include "FrameValues.hpp"
namespace th15::multiplayer {
auto StageFrameState::stage_fields(StageGameplay& s){return std::tie(s.input,s.prepared,s.started,s.suspended,s.entry_updates,s.background_ready,s.main_started,s.music,s.dialogue_host.error,s.messages.script_index,s.messages.error,s.reward.state,s.reward.error,s.error);}
auto StageFrameState::background_fields(StageScene& s){return std::tie(s.initialized,s.deforming,s.rate,s.view_direction,s.script.state,s.script.error,s.error,s.color,s.frames,s.drawn_instances,s.culled_instances,s.drawn_primitives,s.draw_objects,s.object_flags,s.instance_flags);}
auto StageFrameState::hud_fields(GameHud& h){return std::tie(h.life_icons,h.bomb_icons,h.boss_icons,h.root,h.pointdevice,h.background_notice,h.mode_notice,h.chapter_notice,h.difficulty_notice,h.difficulty_label,h.retry_intro,h.bonus_digits,h.result_banners,h.flags,h.intro_age,h.boss,h.boss_animations,h.boss_stars,h.boss_banner,h.result_notice,h.frame_age,h.result,h.last_countdown,h.displayed_score,h.score_increment,h.clear_bonus,h.result_grazes,h.result_deaths,h.boss_state,h.spell_state,h.tutorial_state,h.collection_state,h.error);}
auto StageFrameState::popup_fields(PopupManager& p){return std::tie(p.glyph,p.presentation_generations,p.next_number,p.saved_next,p.saved,p.entries,p.error);}
auto StageFrameState::session_fields(SessionGameplay& s){auto& r=s.runtime;return std::tie(s.frame,s.live,s.attached,s.input_enabled,s.error,s.auto_focus,r.age,r.ending_frames,r.requested_chapter,r.frame_skip,r.sampling_mode,r.error);}
struct StageFrameState::Data {
    StageGameplay* owner=nullptr;SessionGameplay* session=nullptr;StageDeformation* deformation=nullptr;Dialogue* dialogue=nullptr;
    decltype(frame_values::copy(stage_fields(std::declval<StageGameplay&>()))) stage;
    decltype(frame_values::copy(background_fields(std::declval<StageScene&>()))) background;
    decltype(frame_values::copy(hud_fields(std::declval<GameHud&>()))) hud;
    decltype(frame_values::copy(popup_fields(std::declval<PopupManager&>()))) popups;
    decltype(frame_values::copy(session_fields(std::declval<SessionGameplay&>()))) session_values;
    ScreenGridObject mesh;std::string deformation_error;bool deformation_enabled=false;
    std::vector<AnmVm> primitives;std::array<AnmVm,8> embedded;
    std::vector<AnmVm*> roots;
    BattleFrameState battle;bool ready=false;
};
StageFrameState::StageFrameState()=default;
StageFrameState::~StageFrameState()=default;
StageFrameState::StageFrameState(StageFrameState&&)noexcept=default;
StageFrameState& StageFrameState::operator=(StageFrameState&&)noexcept=default;
void StageFrameState::discard()noexcept{if(saved_){saved_->battle.discard();saved_->ready=false;}}
bool StageFrameState::capture(StageGameplay& stage,SessionGameplay* session,const std::vector<AnmVm*>& roots){
    frame_values::IdentityGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;
    if(stage.messages.active()){error="Dialogue is a confirmed frame boundary";return false;}
    s.owner=&stage;s.session=session;s.dialogue=stage.messages.dialogue.get();s.deformation=stage.background_deformation.get();
    s.stage=frame_values::copy(stage_fields(stage));s.background=frame_values::copy(background_fields(stage.background));s.hud=frame_values::copy(hud_fields(stage.hud));s.popups=frame_values::copy(popup_fields(stage.popups));if(session)s.session_values=frame_values::copy(session_fields(*session));
    if(s.deformation){s.mesh=s.deformation->mesh;s.deformation_error=s.deformation->error;s.deformation_enabled=s.deformation->enabled;}
    s.primitives=stage.background.primitives;s.embedded=stage.background.embedded;
    s.roots.assign(roots.begin(),roots.end());for(auto& vm:stage.background.primitives)s.roots.push_back(&vm);for(auto& vm:stage.background.embedded)s.roots.push_back(&vm);s.roots.push_back(&stage.popups.glyph);
    AnmVm::presentation_counter=guard.value;
    if(!s.battle.capture(stage.battle,s.roots)){error=s.battle.error;return false;}s.ready=true;return true;
}
bool StageFrameState::restore(StageGameplay& stage,SessionGameplay* session)const{
    error.clear();if(!saved_||!saved_->ready){error="No stage frame snapshot";return false;}const auto& s=*saved_;
    if(s.owner!=&stage||s.session!=session||s.deformation!=stage.background_deformation.get()||s.dialogue!=stage.messages.dialogue.get()||s.primitives.size()!=stage.background.primitives.size()){error="Stage rollback crossed a scene factory boundary";return false;}
    stage_fields(stage)=s.stage;background_fields(stage.background)=s.background;hud_fields(stage.hud)=s.hud;popup_fields(stage.popups)=s.popups;if(session)session_fields(*session)=s.session_values;
    if(s.deformation){s.deformation->mesh=s.mesh;s.deformation->error=s.deformation_error;s.deformation->enabled=s.deformation_enabled;}
    std::copy(s.primitives.begin(),s.primitives.end(),stage.background.primitives.begin());stage.background.embedded=s.embedded;
    if(!s.battle.restore(stage.battle)){error=s.battle.error;return false;}return true;
}
}
