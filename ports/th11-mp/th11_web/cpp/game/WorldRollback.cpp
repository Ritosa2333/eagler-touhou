#include "WorldRollback.hpp"
#ifdef TH11_MULTIPLAYER
#include "RollbackCapture.hpp"
namespace th11 {
namespace {
bool economy_values(Netplay::RollbackJournal& journal,GameEconomy& value,bool shared_score){
    bool ok=rollback_capture_contiguous_values(journal,value.power,value.point_value,value.communication,
        value.lives,value.life_fragments,value.difficulty,value.rank,value.max_power,
        value.power_step,value.graze,value.max_point_value);
    if(shared_score)ok=rollback_capture_values(journal,value.score_units)&&ok;
    return ok;
}
}
bool WorldRollback::CanCapture()const noexcept{
    const auto& s=session_;const auto* b=s.battle.get();
    if(!s.multiplayer_active||s.state.phase!=GameSessionPhase::stage||!b||!b->mp_enabled||
       b->mp_failed||b->mp_paused||b->mp_finished||b->game_over_requested||!b->initialized||
       (!b->transitioning&&!b->stage_active)||!b->stage||b->completion.state.exit!=StageExit::None||
       s.pause_menu||s.ending||s.title||s.multiplayer_options.seat_count<2||
       s.multiplayer_options.seat_count>3||b->mp_options.seat_count!=s.multiplayer_options.seat_count)return false;
    for(unsigned i=0;i<s.multiplayer_options.seat_count;++i)
        if(!b->pilots[i]||!b->pilots[i]->player||!b->pilots[i]->bomb)return false;
    return true;
}
bool WorldRollback::BoundaryRequested()const noexcept{
    return session_.multiplayer_rollback_boundary_requested||
        (session_.battle&&session_.battle->mp_rollback_boundary_requested);
}
bool WorldRollback::ObserversIdle()const noexcept{
    const auto* b=session_.battle.get();
    if(!b||!b->stage||session_.animations.RollbackFrameActive()||b->mp_presentation.RollbackFrameActive()||
       b->hitbox_presentation.RollbackFrameActive()||b->enemies.RollbackFrameActive()||
       b->bullets.RollbackFrameActive()||b->items.RollbackFrameActive()||b->lasers.RollbackFrameActive()||
       b->stage->RollbackFrameActive()||
       (b->outgoing_stage&&b->outgoing_stage->RollbackFrameActive())||
       b->deformation_pool.RollbackFrameActive())return false;
    for(unsigned i=0;i<session_.multiplayer_options.seat_count;++i)
        if(!b->pilots[i]||!b->pilots[i]->bomb||b->pilots[i]->bomb->RollbackFrameActive())return false;
    return true;
}
bool WorldRollback::BeginRollbackFrame(Netplay::RollbackJournal& journal,WorldRollbackFrame& frame){
    if(active_||frame.ready_||!journal.IsFrameOpen()||journal.Failed()||!CanCapture()||!ObserversIdle())return false;
    // No game state writes occur in the capture walk below. Each owner only
    // copies metadata, acquires leases and registers preimages. Complete the
    // batch before returning to any Update, callback, geometry or texture write.
    if(!journal.BeginReadOnlyCapture())return false;
    auto& s=session_;auto& b=*s.battle;
    frame.owner_=this;frame.identity_=identity_;frame.frame_=journal.OpenFrame();
    frame.battle_=&b;frame.stage_=b.stage.get();frame.resource_=s.resources.stage.get();
    frame.outgoing_stage_=b.outgoing_stage.get();frame.previous_resource_=s.resources.previous_stage.get();
    frame.dialogue_=b.dialogue.get();frame.seat_count_=s.multiplayer_options.seat_count;
    frame.events_=b.events;frame.text_requests_=b.dialogue_text_requests;
    frame.ascii_requests_=b.ascii.requests;frame.fades_=b.fades.effects;frame.shakes_=b.shakes.effects;
    frame.damage_remainders_=b.mp_damage_remainders;
    frame.session_error_=s.error;frame.battle_error_=b.error;
    if(b.dialogue)frame.dialogue_error_=b.dialogue->error;
    active_=&frame;s.multiplayer_rollback_boundary_requested=false;b.mp_rollback_boundary_requested=false;
    bool ok=true;
    const auto capture=[&](auto&... values){if(!rollback_capture_contiguous_values(journal,values...))ok=false;};
    capture(s.state,s.multiplayer_frame,s.multiplayer_pause_held,s.game_input,
        s.battle_attached,s.last_held,s.last_pause,s.paused_rate,s.menu_input,
        s.spell_records,s.clear_records);
    if(!economy_values(journal,s.economy,true))ok=false;
    auto& combat=static_cast<EnemyFrameWorld&>(b);
    capture(combat.practice_time_lock,combat.player_state,combat.special_active,combat.spell_id,
        combat.player_flags,combat.spell_flags,combat.bomb_animation_flags,combat.special_ending,
        combat.target_locked,combat.target,combat.countdown_seconds,combat.countdown_hundredths,
        combat.spell_elapsed,combat.spell_bonus,combat.shared_spell_state,combat.camera_position);
    auto& laser_world=static_cast<LaserWorld&>(b);
    capture(laser_world.rate,laser_world.player,static_cast<ShotWorld&>(b).enemies);
    capture(b.transition_timer,b.transitioning,b.transition_started,b.stage_active,b.result_animation,
        b.player_input,b.cancellation,b.communication,b.spell_backgrounds,b.callback_shared_visual,
        b.spell_titles,b.spell_circle,b.frame,b.initialized,b.player_live,b.game_over_requested,
        b.practice_high,b.replaying,b.demo,b.replay_entry,b.last_error,b.shakes.offset,
        b.popups,b.spell_timing,b.ascii.spacing);
    auto& e=b.enemy_environment;
    capture(e.random,e.rate,e.player_position,e.camera_delta,e.rank,e.difficulty,e.character,e.subtype,
        e.shared_integers,e.enemy_count,e.boss);
    auto& commands=b.enemy_commands;
    capture(commands.practice_time_lock,commands.bosses,commands.manager_flags,commands.health_segments,
        commands.remaining_phases,commands.stage_section,commands.section_frame);
    capture(b.completion.mode,b.completion.state);
    auto& spell=b.spells;
    capture(spell.timer,spell.initial_bonus,spell.timeout,spell.frame_count,spell.circle_position,
        spell.name,spell.selection,spell.stage,spell.translation_rank,spell.replay,
        spell.bomb_active,spell.practice_time_lock);
    auto& hud=b.hud;
    capture(hud.lives,hud.digits,hud.communication,hud.indicator,hud.elapsed,hud.previous_seconds,
        hud.score,hud.displayed_health,hud.target_health,hud.health,hud.frame_animation,hud.boss_name,
        hud.stars,hud.difficulty_intro,hud.difficulty_label,hud.bonus_digits,hud.spell_notice,
        hud.item_notice,hud.spell_time_animation,hud.show_spell_time,hud.practice_skip_logo,hud.multiplayer);
    auto& compositor=s.compositor;
    capture(compositor.copies,compositor.full_camera,compositor.play_camera,compositor.world_camera,
        compositor.region,compositor.clear_color);
    capture(b.mp_paused,b.mp_finished,b.mp_failed,b.mp_life_icons,b.mp_communication_icons,
        b.mp_communication_modes,b.mp_communication_hidden,b.mp_display_lives,b.mp_display_fragments,
        b.mp_fragments,b.mp_wipe_frames,b.mp_tick,b.mp_effect_seat,b.mp_graze_mask,b.mp_rank,
        b.mp_enemy_graze,b.hitbox_marker);
    if(b.dialogue){auto& d=*b.dialogue;capture(d.state,d.active,d.character,d.stage,d.last_opcode,d.end);}
    // Each manager captures its own embedded/pool payloads. Avoid subrange
    // registration of any VM that was already captured as part of an owner.
    if(!s.animations.BeginRollbackFrame(journal,frame.animations_))ok=false;
    if(!b.mp_presentation.BeginRollbackFrame(journal,frame.presentation_))ok=false;
    if(!b.hitbox_presentation.BeginRollbackFrame(journal,frame.hitbox_))ok=false;
    if(!b.enemies.BeginRollbackFrame(journal,frame.enemies_))ok=false;
    if(!b.bullets.BeginRollbackFrame(journal))ok=false;
    if(!b.items.BeginRollbackFrame(journal))ok=false;
    if(!b.lasers.BeginRollbackFrame(journal))ok=false;
    if(!b.stage->BeginRollbackFrame(journal,frame.stage_state_))ok=false;
    if(b.outgoing_stage&&!b.outgoing_stage->BeginRollbackFrame(journal,frame.outgoing_stage_state_))ok=false;
    if(!b.deformation_pool.BeginRollbackFrame(journal,frame.deformations_))ok=false;
    for(unsigned i=0;i<frame.seat_count_;++i){
        auto& p=*b.pilots[i];frame.pilots_[i]=&p;frame.players_[i]=p.player;frame.bombs_[i]=p.bomb;
        capture(p.input,p.keys,p.ghost,p.life_latched,p.force_attract,p.life_hold,p.power_taps,
            p.power_gap,p.ghost_clock,p.ghost_dx,p.ghost_dy,p.ghost_random,p.challenge_misses,
            static_cast<ShotWorld&>(p).enemies);
        if(i){if(!economy_values(journal,p.economy,false))ok=false;capture(p.communication);}
        if(!p.player->CaptureRollbackFrame(journal))ok=false;
        if(!p.bomb->BeginRollbackFrame(journal,frame.bomb_deformations_[i]))ok=false;
    }
    const bool copied=journal.EndReadOnlyCapture();
    frame.ready_=ok&&copied&&!journal.Failed();
    if(!frame.ready_)EndRollbackFrame();
    return frame.ready_;
}
void WorldRollback::EndRollbackFrame()noexcept{
    if(!active_)return;
    auto& frame=*active_;auto& b=*frame.battle_;
    session_.animations.EndRollbackFrame();b.mp_presentation.EndRollbackFrame();b.hitbox_presentation.EndRollbackFrame();
    b.enemies.EndRollbackFrame();b.bullets.EndRollbackFrame();b.items.EndRollbackFrame();b.lasers.EndRollbackFrame();
    frame.stage_->EndRollbackFrame();b.deformation_pool.EndRollbackFrame();
    if(frame.outgoing_stage_)frame.outgoing_stage_->EndRollbackFrame();
    for(unsigned i=0;i<frame.seat_count_;++i)if(frame.bombs_[i])frame.bombs_[i]->EndRollbackFrame();
    active_=nullptr;
}
bool WorldRollback::ValidateRollbackFrame(const WorldRollbackFrame& frame,u32 restored_frame)const noexcept{
    const auto& s=session_;const auto* b=s.battle.get();
    if(active_||!frame.valid()||frame.owner_!=this||frame.identity_.lock()!=identity_||
       frame.frame_!=restored_frame||!b||frame.battle_!=b||frame.stage_!=b->stage.get()||
       frame.resource_!=s.resources.stage.get()||frame.dialogue_!=b->dialogue.get()||
       frame.outgoing_stage_!=b->outgoing_stage.get()||frame.previous_resource_!=s.resources.previous_stage.get()||
       frame.seat_count_!=s.multiplayer_options.seat_count||
       !frame.animations_.valid()||!frame.presentation_.valid()||!frame.hitbox_.valid()||
       !frame.enemies_.valid()||!frame.deformations_.valid()||!ObserversIdle()||
       !b->stage->ValidateRollbackFrame(frame.stage_state_,restored_frame))return false;
    if(b->outgoing_stage&&!b->outgoing_stage->ValidateRollbackFrame(frame.outgoing_stage_state_,restored_frame))return false;
    for(unsigned i=0;i<frame.seat_count_;++i){
        const auto* p=b->pilots[i].get();
        if(!p||frame.pilots_[i]!=p||frame.players_[i]!=p->player||frame.bombs_[i]!=p->bomb||
           !frame.bomb_deformations_[i].valid())return false;
    }
    return true;
}
bool WorldRollback::RestoreRollbackFrame(const WorldRollbackFrame& frame,u32 restored_frame){
    if(!ValidateRollbackFrame(frame,restored_frame))return false;
    auto& s=session_;auto& b=*s.battle;
    // Caller validates BEFORE UndoTo. This method is only the typed half:
    // complete all raw journal copies before replacing maps/leases here.
    if(!s.animations.RestoreRollbackFrame(frame.animations_,restored_frame)||
       !b.mp_presentation.RestoreRollbackFrame(frame.presentation_,restored_frame)||
       !b.hitbox_presentation.RestoreRollbackFrame(frame.hitbox_,restored_frame)||
       !b.enemies.RestoreRollbackFrame(frame.enemies_,restored_frame)||
       !b.stage->RestoreRollbackFrame(frame.stage_state_,restored_frame)||
       !b.deformation_pool.RestoreRollbackFrame(frame.deformations_,restored_frame))return false;
    if(b.outgoing_stage&&!b.outgoing_stage->RestoreRollbackFrame(frame.outgoing_stage_state_,restored_frame))return false;
    for(unsigned i=0;i<frame.seat_count_;++i)
        if(!b.pilots[i]->bomb->RestoreRollbackFrame(frame.bomb_deformations_[i],restored_frame))return false;
    b.events=frame.events_;b.dialogue_text_requests=frame.text_requests_;b.ascii.requests=frame.ascii_requests_;
    b.fades.effects=frame.fades_;b.shakes.effects=frame.shakes_;b.mp_damage_remainders=frame.damage_remainders_;
    s.error=frame.session_error_;b.error=frame.battle_error_;
    if(b.dialogue)b.dialogue->error=frame.dialogue_error_;
    s.multiplayer_rollback_boundary_requested=false;b.mp_rollback_boundary_requested=false;
    return true;
}
}
#endif
