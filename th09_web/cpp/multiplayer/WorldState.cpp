#include "WorldState.hpp"

namespace th09::multiplayer {
WorldState::WorldState() { enemies.reserve(260);effects.reserve(1400);attacks.reserve(257);good=journal.Reset({1,32*1024*1024,8192,true,true}); }
void WorldState::BeforeEnemy(EclVm& e){
    for(std::size_t i=0;i<enemyCount;++i)if(enemies[i].first==&e)return;
    if(enemyCount==enemies.size())enemies.emplace_back();
    auto& saved=enemies[enemyCount++];saved.first=&e;
    good=saved.second.Save(e)&&good;if(hashEnabled)HashPart("dynamic enemy",saved.second.Fingerprint(e));ownedBytes+=saved.second.Bytes();
}
void WorldState::Enemy(EnemyManager& x) {
    BeforeEnemy(x.prototype);
    for(auto& e:x.enemies)if(!sparse||hashEnabled||(e.behavior_flags&1)||&e==&x.enemies.back())BeforeEnemy(e);
    for(auto* e:x.bosses)if(e)BeforeEnemy(*e);
    if(sparse&&!hashEnabled)Bind(x.checkpoint);
    NamedFields("x.bosses,x.timeline,x.timeline_events,x.timeline_control,x.attack_control,x.alive,x.normal_alive,x.attack_alive,x.spirit_alive,x.capture_lifetime,x.focus_time,x.frame_time,x.pattern_index,x.priority_target,x.first_target,x.allocation_failed,x.bindings,x.timing,x.frame_step,x.difficulty_mask",x.bosses,x.timeline,x.timeline_events,x.timeline_control,x.attack_control,x.alive,x.normal_alive,x.attack_alive,x.spirit_alive,x.capture_lifetime,x.focus_time,x.frame_time,x.pattern_index,x.priority_target,x.first_target,x.allocation_failed,x.bindings,x.timing,x.frame_step,x.difficulty_mask);
    for(auto& list:x.draw_lists)Vector(list);
}
void WorldState::BeforeEffect(EffectManager& x,std::size_t index) {
    std::size_t offset=effectSaved.size();
    for(std::size_t i=0;i<effectRangeCount;++i)if(effectRanges[i].owner==&x){offset=effectRanges[i].offset;break;}
    if(index>=x.actors.size()||offset+index>=effectSaved.size()){good=false;return;}
    if(effectSaved[offset+index])return;effectSaved.set(offset+index);
    auto& e=x.actors[index];if(effectCount==effects.size())effects.emplace_back();
    auto& s=effects[effectCount++];s.first=&e;good=s.second.Save(e)&&good;
    if(hashEnabled)HashPart("dynamic enemy/effect",s.second.Fingerprint(e));ownedBytes+=s.second.Bytes();
}
void WorldState::BeforeAttack(AttackQueue& x,std::size_t index) {
    if(index>=x.actors.size()){good=false;return;}
    if(attackSaved[index])return;attackSaved.set(index);
    auto& a=x.actors[index];if(attackCount==attacks.size())attacks.emplace_back();
    auto& s=attacks[attackCount++];s.first=&a;good=s.second.Save(a)&&good;
    if(hashEnabled)HashPart("dynamic attack",s.second.Fingerprint(a));
}
void WorldState::Effects(EffectManager& x) {
    NamedFields("x.side,x.count,x.frame,x.cursor,x.capacity,x.reserved_slots",x.side,x.count,x.frame,x.cursor,x.capacity,x.reserved_slots);
    if(effectRangeCount==effectRanges.size()||effectSlots+x.actors.size()>effectSaved.size()){good=false;return;}
    effectRanges[effectRangeCount++]={&x,effectSlots};effectSlots+=x.actors.size();
    // Dormant owners can be destroyed by update; reserved slots may be written
    // through long-lived Field pointers. Keep both, plus the overflow sentinel.
    for(std::size_t i=0;i<x.actors.size();++i){auto& e=x.actors[i];
        if(!sparse||hashEnabled||i>=x.capacity||e.active||e.animation||e.burst||!e.colors.empty()||!e.texture.empty())BeforeEffect(x,i);
    }
    if(sparse&&!hashEnabled)Bind(x.checkpoint);
    for(auto& list:x.draw_lists)Vector(list);
}
void WorldState::Attacks(AttackQueue& x) {
    NamedFields("x.counts,x.limits,x.invalid",x.counts,x.limits,x.invalid);
    for(std::size_t i=0;i<x.actors.size();++i){auto& a=x.actors[i];
        if(!sparse||hashEnabled||i==x.capacity||a.active||a.state||!a.animations.empty())BeforeAttack(x,i);
    }
    if(sparse&&!hashEnabled)Bind(x.checkpoint);
    for(auto& list:x.draw_lists)Vector(list);
}
void WorldState::PlayerState(Player& x) {
    NamedFields("x.frame",x.frame);NamedFields("x.motion",x.motion);NamedFields("x.control",x.control);NamedFields("x.body",x.body);NamedFields("x.input",x.input);NamedFields("x.hazards",x.hazards);NamedFields("x.cpu",x.cpu);NamedFields("x.combo_state",x.combo_state);NamedFields("x.secondary_target",x.secondary_target);NamedFields("x.target_priority",x.target_priority);NamedFields("x.attack_levels",x.attack_levels);NamedFields("x.initialized",x.initialized);
    auto& s=x.shots;NamedFields("s.shots",s.shots);NamedFields("s.player_position",s.player_position);NamedFields("s.option_positions",s.option_positions);NamedFields("s.target",s.target);NamedFields("s.player_scale",s.player_scale);NamedFields("s.side",s.side);NamedFields("s.beam_time",s.beam_time);NamedFields("s.beam",s.beam);NamedFields("s.areas",s.areas);
    NamedFields("x.items.items",x.items.items);NamedFields("x.items.attraction",x.items.attraction);
    auto& l=x.life;NamedFields("l.input_controller",l.input_controller);NamedFields("l.display_frames",l.display_frames);NamedFields("l.hidden",l.hidden);NamedFields("l.knockback_z",l.knockback_z);NamedFields("l.shield_active",l.shield_active);NamedFields("l.shield_position",l.shield_position);
}
void WorldState::BackgroundState(Background& x) {
    // primitives never reallocates between match construction and retirement.
    for(auto& p:x.primitives)NamedFields("p",p);
    NamedFields("x.camera,x.goals,x.starts,x.end_tangents,x.start_tangents,x.fov_goal,x.fov_start,x.durations,x.interpolation_times,x.interpolation_modes,x.script_time,x.instruction,x.frame,x.position,x.clear_color,x.fog,x.fog_start,x.fog_goal,x.fog_duration,x.fog_time,x.requested_label,x.transition_state,x.transition_frames,x.overlays,x.boss_animations,x.boss_state,x.boss_frames,x.boss_count,x.boss_parameter,x.tint,x.next_position,x.next_position_time,x.previous_position,x.previous_position_time,x.jumped,x.sway,x.distance_squared,x.invalid",x.camera,x.goals,x.starts,x.end_tangents,x.start_tangents,x.fov_goal,x.fov_start,x.durations,x.interpolation_times,x.interpolation_modes,x.script_time,x.instruction,x.frame,x.position,x.clear_color,x.fog,x.fog_start,x.fog_goal,x.fog_duration,x.fog_time,x.requested_label,x.transition_state,x.transition_frames,x.overlays,x.boss_animations,x.boss_state,x.boss_frames,x.boss_count,x.boss_parameter,x.tint,x.next_position,x.next_position_time,x.previous_position,x.previous_position_time,x.jumped,x.sway,x.distance_squared,x.invalid);
}
void WorldState::Recording(ReplayArchive& x) {
    NamedFields("x.active",x.active);Text(x.error);
    for(auto& stage:x.stages){
        NamedFields("stage.present,stage.headers",stage.present,stage.headers);
        auto& r=stage.recording;NamedFields("r.frame_number,r.ending",r.frame_number,r.ending);
        // Record sizes, not addresses of vector elements: appending a chunk
        // can reallocate the outer vector during the speculative interval.
        std::vector<std::array<std::size_t,4>> sizes;
        for(auto& part:r.parts)sizes.push_back({part.inputs[0].size(),part.inputs[1].size(),part.inputs[2].size(),part.rates.size()});
        undo.emplace_back([&r,sizes=std::move(sizes)]{r.parts.resize(sizes.size());for(std::size_t i=0;i<sizes.size();++i){for(unsigned j=0;j<3;++j)r.parts[i].inputs[j].resize(sizes[i][j]);r.parts[i].rates.resize(sizes[i][3]);}});
    }
}
bool WorldState::Save(GameSession& s) {
    Unbind();hashParts.clear();digest={};if(journal.IsFrameOpen())journal.EndFrame();journal.DiscardBefore(1);undo.clear();enemyCount=effectCount=attackCount=0;effectRangeCount=effectSlots=0;effectSaved.reset();attackSaved.reset();ownedBytes=0;good=journal.BeginFrame(0);
    // This adapter is specifically the stable live two-human versus world.
    if(!s.world||s.is_replay||s.world->rules.mode!=GameMode::versus)return good=false;
    NamedFields("s.state,s.animations.timing,s.animations.executed,s.animations.invalid,s.initial,s.current,s.inputs,s.scores,s.starting_replay_stage,s.demo,s.demo_input,s.demo_frames,s.warmed_stage,s.result,s.phase,s.continues,s.recordable,s.frames,s.timing,s.motion_input",s.state,s.animations.timing,s.animations.executed,s.animations.invalid,s.initial,s.current,s.inputs,s.scores,s.starting_replay_stage,s.demo,s.demo_input,s.demo_frames,s.warmed_stage,s.result,s.phase,s.continues,s.recordable,s.frames,s.timing,s.motion_input);Text(s.error);
    Recording(s.recording);
    auto& m=s.motion;NamedFields("m.ticks,m.cursors,m.playing,m.recording,m.invalid,m.active,m.unlimited,m.target_x,m.target_y",m.ticks,m.cursors,m.playing,m.recording,m.invalid,m.active,m.unlimited,m.target_x,m.target_y);for(auto& v:m.stages)AppendOnly(v);
    auto& w=*s.world;NamedFields("w.configuration,w.combined_input,w.timing,w.markers,w.music_track,w.ready,w.paused,w.transition_pending,w.transition",w.configuration,w.combined_input,w.timing,w.markers,w.music_track,w.ready,w.paused,w.transition_pending,w.transition);Text(w.error);
    auto& r=w.rules;NamedFields("r.progress,r.mode,r.scores,r.attack_levels",r.progress,r.mode,r.scores,r.attack_levels);Vector(w.screen_effects.effects);
    auto& ss=w.scene_services();NamedFields("ss.game_flags,ss.wins_required,ss.wins,ss.starting_extra_lives,ss.opponent_character,ss.music_track,ss.cpu_level,ss.route,ss.players,ss.geometry",ss.game_flags,ss.wins_required,ss.wins,ss.starting_extra_lives,ss.opponent_character,ss.music_track,ss.cpu_level,ss.route,ss.players,ss.geometry);
    auto& b=*w.battle;NamedFields("b.state,b.patterns",b.state,b.patterns);Text(b.error);
    for(auto& f:b.fields){
        NamedFields("f.script,f.enemy_player,f.focus_aura,f.capture_effect,f.shield,f.targeted_enemy,f.cpu_level,f.spells,f.bosses,f.counters",f.script,f.enemy_player,f.focus_aura,f.capture_effect,f.shield,f.targeted_enemy,f.cpu_level,f.spells,f.bosses,f.counters);
        PlayerState(*f.player);Enemy(*f.enemies);Effects(*f.effects);
        auto& bullets=*f.bullets;NamedFields("bullets.total,bullets.first_count,bullets.second_count,bullets.cancel_frames,bullets.frame,bullets.lifetime,bullets.draw_heads",bullets.total,bullets.first_count,bullets.second_count,bullets.cancel_frames,bullets.frame,bullets.lifetime,bullets.draw_heads);
        // Fixed-size, contiguous POD pool: keep every slot byte, but pay for
        // one journal lookup/copy per field instead of one per bullet. The
        // diagnostic oracle retains its independent per-slot traversal.
        static_assert(std::is_trivially_copyable_v<Bullet>);
        if(hashEnabled)for(auto& bullet:bullets.pool)NamedFields("bullet",bullet);
        else Touch(bullets.pool.data(),bullets.pool.size()*sizeof(bullets.pool[0]));
        NamedFields("f.bullet_visuals->appearances",f.bullet_visuals->appearances);
        auto& visuals=*f.bullet_visuals;
        for(std::size_t i=0;i<visuals.instances.size();++i)if(!sparse||hashEnabled||bullets.pool[i].state)Fields(visuals.instances[i]);
        if(sparse&&!hashEnabled)Bind(visuals.checkpoint);
        NamedFields("f.lasers->pool",f.lasers->pool);
        auto& a=*f.attacks;NamedFields("a.side,a.name,a.time,a.notices,a.animations,a.parameters,a.active_variants",a.side,a.name,a.time,a.notices,a.animations,a.parameters,a.active_variants);
    }
    Effects(*b.cross_effects);Attacks(*b.attack_queue);
    for(auto& background:w.backgrounds)BackgroundState(*background);
    for(auto& h:w.huds)NamedFields("h->animations,h->portraits,h->charge_active,h->wipe_state,h->wipe,h->blink",h->animations,h->portraits,h->charge_active,h->wipe_state,h->wipe,h->blink);
    auto& d=*w.dialogue;NamedFields("d.current,d.cursor,d.id,d.inverted,d.animations,d.colors,d.shadows,d.time,d.wait_frames,d.minimum_wait,d.font_size,d.box_time,d.speaker,d.new_page,d.line_number,d.previous_speaker,d.counter,d.skippable,d.box_visible,d.invalid",d.current,d.cursor,d.id,d.inverted,d.animations,d.colors,d.shadows,d.time,d.wait_frames,d.minimum_wait,d.font_size,d.box_time,d.speaker,d.new_page,d.line_number,d.previous_speaker,d.counter,d.skippable,d.box_visible,d.invalid);
    auto& c=*w.scene;NamedFields("c.animations,c.borders,c.flash_colors,c.flash_frames,c.displayed_music,c.phase,c.ending_frames,c.winner,c.result,c.retry,c.rewards_blocked",c.animations,c.borders,c.flash_colors,c.flash_frames,c.displayed_music,c.phase,c.ending_frames,c.winner,c.result,c.retry,c.rewards_blocked);
    // Keep the record open for the platform-owned UI and texture fields.
    return Good();
}
bool WorldState::Restore() {
    Unbind();
    if(!Good()||(journal.IsFrameOpen()&&!journal.EndFrame())||!journal.UndoTo(0))return false;
    for(auto& action:undo)action();
    for(std::size_t i=0;i<enemyCount;++i){auto& e=enemies[i];good=e.second.Restore(*e.first)&&good;}
    for(std::size_t i=0;i<effectCount;++i){auto& e=effects[i];good=e.second.Restore(*e.first)&&good;}
    for(std::size_t i=0;i<attackCount;++i){auto& a=attacks[i];good=a.second.Restore(*a.first)&&good;}
    return Good();
}
}
