#pragma once
#include "encounter_scene.hpp"
#include "native_replay.hpp"
#include "practice_session.hpp"
#include <initializer_list>
#include <type_traits>

namespace soew {

// All run-affecting launch options live here. The host attaches audio output
// and owns rendering, keyboard synthesis, focus, and fixed-clock scheduling.
inline EncounterScene load_replay_scene(const std::filesystem::path &root,const NativeReplayConfig &c,Language language=Language::Japanese){
    auto s=EncounterScene::load(root,c.stage,c.shot,c.rank,c.lives,c.bombs,language);
    if(c.extra)s.enable_extra();
    if(c.stage4_trial){
        if(c.stage!=3)throw std::runtime_error("Stage 4 trial requires Stage 4");
        s.script.enable_stage4_compatibility();s.enable_preboss();s.transitions_enabled=true;
    }
    if(c.stage==4&&c.stage5_trial){s.enable_stage5_preview();s.transitions_enabled=true;}
    s.reduced_effects=c.reduced;
    s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;
    s.enable_effects(c.seed);s.enable_bombing();
    if(c.stage==0){s.enable_midboss();s.enable_preboss();s.transitions_enabled=true;}
    if(c.stage==1){s.midboss2_enabled=true;s.enable_preboss();s.transitions_enabled=true;}
    return s;
}

inline constexpr NativeReplayEvent normalize_replay_tick(std::uint16_t input,bool group=false,bool fast=false){
    // Do not collapse contradictory directional keys: exact masks have
    // distinct source semantics. Group and fast-forward are independent.
    return {NativeReplayEventKind::Tick,std::uint16_t(input&native_input_mask),group,fast};
}

namespace replay_session_detail {
struct Digest {
    std::uint64_t value=14695981039346656037ull;
    void byte(std::uint8_t b){value=(value^b)*1099511628211ull;}
    template<class T> void add(T v){
        static_assert(std::is_integral_v<T>||std::is_enum_v<T>);
        // A documented width independent of host type sizes and padding.
        auto bits=static_cast<std::uint64_t>(v);
        for(unsigned i=0;i<8;++i){byte(std::uint8_t(bits));bits>>=8;}
    }
    template<class T,std::size_t N> void add(const std::array<T,N> &items){for(const auto &x:items)add(x);}
    template<class T> void add(const std::optional<T> &v){add(v.has_value());if(v)add(*v);}
    template<class... T> void fields(const T&... values){(add(values),...);}
    void gate(const StageDialogue::KeyDelay &k){fields(k.waiting_press,k.sample,k.elapsed);}
};
}

// Selected gameplay checkpoints; never hash raw C++ objects, framebuffer,
// asset paths, pointers, wall time, or frontend audio state.
inline std::uint64_t replay_state_digest(const EncounterScene &s){
    replay_session_detail::Digest h;
    const auto &r=s.runtime;const auto &sc=r.scroll;const auto &p=s.player;
    h.fields(s.stage,s.rank,s.ticks,s.resident_frame,s.run_start_lives,s.run_start_bombs,s.continues_used,
        s.highest_run_score,s.continue_choice,s.continue_yes,s.combat_enabled,s.survival_enabled,s.pickup_enabled,
        s.bomb_enabled,s.effects_enabled,s.stage5_entry_only,s.stage5_preview,s.extra_road_enabled,
        s.stage2_preview_boundary,s.stage3_preview_boundary,s.weapon_blocked);
    h.fields(r.page,r.ticks,r.spawns,r.frame_prepared,r.deferred_scroll_delta,sc.speed,sc.interval,sc.cycle,
        sc.line,sc.tile_line,sc.top_row,sc.display_pixels,sc.attempted_pixels,sc.progress,sc.pending_progress,sc.done,
        r.dispatch.cursor,r.dispatch.rows,r.dispatch.intents,r.dispatch.accepted,r.dispatch.suppressed,
        r.dispatch.capacity_rejected,r.dispatch.incomplete_boundary,r.enemies.high_water,r.random.cursor);
    for(auto b:r.random.bytes)h.byte(b);
    for(const auto &e:r.enemies.slots)for(auto b:e.raw)h.byte(b);
    h.fields(p.x,p.y,p.shot,p.back_page,p.frame,p.pose,p.last_dx,p.last_dy);
    // Unfocused runs retain the exact digest used by existing replays.
    if(p.focused)h.fields(p.focused,p.focus_rx,p.focus_ry);
    const auto &l=s.life;
    for(const auto &pos:l.positions)h.fields(pos.x,pos.y);
    h.fields(l.current.x,l.current.y,l.invincibility,l.miss_frame,l.lives,l.bombs,l.start_bombs,l.power,
        l.hit,l.miss_active,l.bomb_invincible,l.game_over,l.playperf,l.stage_misses,l.total_misses);
    const auto &q=s.score_core;const auto &k=s.pickups;const auto &i=s.items;
    h.fields(q.score,q.delta,q.hiscore,q.transferred,q.extends,k.power,k.overflow,k.collect_skill,k.points_collected,
        k.item_skill,k.frame_score,k.collected,i.ring_index,i.cycle,i.drop_cycle,i.bigpower_override,i.item_skill,i.spawned);
    for(const auto &slot:i.slots){h.add(slot.flag);if(slot.flag)h.fields(slot.type,slot.left,slot.top,slot.vy,slot.vx,slot.age);}
    const auto &b=s.bomb;
    h.fields(b.active,b.circle_done,b.clear_requested,b.finished,b.frame,b.circle_frame,b.shot_type,b.stage_used,
        b.total_used,b.slowdown,s.bombing_for_spawn);
    const auto &bs=s.bullet_core;
    h.fields(bs.rank,bs.playperf,bs.stack,bs.easy_skip,bs.spawned,bs.removed,bs.special.u1,bs.special.u2,bs.special.u3);
    for(const auto &slot:bs.bullets){h.add(slot.flag);if(slot.flag)h.fields(slot.kind,slot.position,slot.velocity,slot.angle,slot.pattern,slot.motion,slot.speed,slot.counter);}
    const auto &ss=s.shots;
    h.fields(ss.burst_a,ss.burst_b,ss.cool_a,ss.cool_b,ss.fired,ss.hit_events,ss.damage_events,ss.level,
        ss.shot_type,ss.spread,ss.option_lock,ss.counter_c,ss.option_left,ss.option_top,ss.target_x,ss.target_y,ss.last_target_x);
    for(const auto &slot:ss.slots){h.add(slot.flag);if(slot.flag)h.fields(slot.type,slot.pattern,slot.special,slot.age,slot.position,slot.vx,slot.vy);}
    h.fields(s.lasers.initial_delay,s.lasers.spawned,s.lasers.removed);
    for(const auto &slot:s.lasers.slots){h.add(slot.flag);if(slot.flag)h.fields(slot.state,slot.charge_age,slot.base_pattern,slot.x,slot.y,slot.delay,slot.active_frames);}
    const auto &d=s.dialogue;const auto &tr=s.transition;const auto &ex=s.extra_settlement;
    h.fields(d.phase,d.current,d.box_left,d.updates,d.end_box,d.postboss,d.continues_used,d.reveal.cursor,d.reveal.loops,
        d.reveal.waiting,s.preboss_enabled,s.preboss_started,s.appearance_frame,s.mima_delay,s.mima_delay_frame);
    h.gate(d.key_delay);
    h.fields(s.transitions_enabled,tr.phase,tr.frame,tr.bonus.rows,tr.bonus.subtotal,tr.bonus.point_items,tr.bonus.internal_bonus,
        ex.phase,ex.loss_frames,ex.bonus.rows,ex.bonus.subtotal,ex.bonus.point_items,ex.bonus.internal_bonus);
    h.gate(tr.gate);h.gate(ex.gate);
    h.fields(s.midboss_enabled,s.midboss2_enabled,s.midboss3_enabled,s.rika_enabled,s.meira_enabled,
        s.stones_enabled,s.marisa_enabled,s.mima_enabled,s.sigma_enabled);
    const auto add_midboss=[&](const auto &m){h.fields(m.active,m.dying,m.present,m.phase_frame,m.damage_taken,m.death_frame,m.left,m.top,m.target_x,m.target_y);};
    if(s.midboss_enabled)add_midboss(s.midboss);
    if(s.midboss2_enabled)add_midboss(s.midboss2);
    if(s.midboss3_enabled){
        const auto &m=s.midboss3;h.fields(m.active,m.phase_frame,m.angle,m.cycles,m.target_x,m.target_y);
        for(const auto &a:m.actor)h.fields(a.flag,a.left,a.top,a.damage,a.death_frame);
    }
    if(s.stage==3){const auto &m=s.midboss4;h.fields(m.active,m.dying,m.flying_in,m.entries,m.phase_frame,m.death_frame,m.attack,m.cycles,m.damage_taken,m.left,m.top,m.x,m.y,m.direction);}
    if(s.extra_road_enabled){
        const auto &m=s.extra_midboss;h.fields(m.started,m.active,m.finished,m.frame,m.distance);
        for(const auto &a:m.tiles){h.add(a.step);if(a.step)h.fields(a.age,a.left,a.y,a.group,a.angle,a.speed);}
    }
    if(s.rika_enabled){const auto &a=s.rika;h.fields(s.rika_stage_frame,a.state,a.phase_frame,a.damage_taken,a.attack_frame,a.death_frame,a.left,a.cached_x,a.sweep_angle,a.dying,a.done);}
    if(s.meira_enabled){const auto &a=s.meira;h.fields(a.state,a.phase_frame,a.damage_taken,a.death_frame,a.attack_index,a.left,a.top,a.special_angle,a.dying,a.done,a.effect_parity,a.trail_delay,a.stream_speed,a.aimed_speed,a.travel_step);}
    if(s.stones_enabled){const auto &a=s.stones;h.fields(a.phase,a.phase_frame,a.elapsed,a.attack_frame,a.north_attack,a.sweep_angle,a.second_angle,a.laser_stage,a.laser_x,a.aimed_angles);for(const auto &stone:a.stones)h.fields(stone.flag,stone.damage,stone.x,stone.y,stone.death_frame);}
    if(s.marisa_enabled){const auto &a=s.marisa;h.fields(a.phase,a.phase_frame,a.left,a.top,a.flight_x,a.flight_y,a.flight_direction,a.flight_angle,a.pending_attack,a.approach_dx,a.approach_dy,a.damage_taken,a.rounds,a.body_damage_enabled,a.defeated,a.death_frame,a.cleared);}
    if(s.mima_enabled){
        const auto &a=s.mima;h.fields(a.phase,a.event,a.blocked,a.phase_frame,a.damage_taken,a.left,a.top,a.target_x,
            a.target_y,a.selection,a.rounds,a.body_damage_enabled,a.form2,a.finished);
        for(const auto &orb:a.orbs)h.fields(orb.active,orb.x,orb.y,orb.pattern);
    }
    if(s.sigma_enabled){
        const auto &a=s.sigma;h.fields(a.phase,a.phase_frame,a.sigma_frame,a.subattack,a.damage_taken,a.left,a.top,
            a.dying,a.done,a.cycled,a.slowdown);
        for(const auto &burst:a.bursts){h.add(burst.state);if(burst.state)h.fields(burst.age,burst.radius,burst.x,burst.y,burst.target_radius);}
    }
    h.add(s.stage5_result.has_value());
    if(s.stage5_result){const auto &a=*s.stage5_result;h.fields(a.enter_form2,a.ending,a.good_ending,a.bonus);}
    return h.value;
}

// Classify the branch EncounterScene will enter on the upcoming tick. A
// dialog box and its blocking appearances share one context; stock, gates,
// and input still belong to EncounterScene, not to the replay parser.
inline NativeReplayContext replay_tick_context(const EncounterScene &s){
    using P=ExtraSettlement::Phase;
    if(s.extra_settlement.showing_bonus()||s.extra_settlement.phase==P::GameOver||
        (s.sigma_art&&s.life.game_over&&!s.sigma.done))return NativeReplayContext::StageClear;
    if(s.life.game_over&&s.transitions_enabled&&!s.sigma_art&&s.survival_enabled&&!s.practice_mode)
        return NativeReplayContext::Continue;
    if(s.transitions_enabled&&s.transition.active()&&s.transition.phase!=StageTransition::Phase::Postboss)
        return NativeReplayContext::StageClear;
    if(s.mima_delay||(s.preboss_enabled&&s.dialogue.active()))return NativeReplayContext::Dialogue;
    return NativeReplayContext::Gameplay;
}

// Practice v4 checks its launch recipe against both the initialized state and
// the decoded terrain/script/dialogue it will consume. No C++ snapshots or paths.
inline std::uint64_t practice_replay_start_digest(const EncounterScene &s){
    replay_session_detail::Digest h;
    h.fields(replay_state_digest(s),s.practice_mode,s.practice_show_dialogue,s.reduced_effects,
             s.script.stage4_compatibility,s.script.declared_formations,s.dialogue.pre_count,s.dialogue.total_count,
             s.background.map.size(),s.background.tiles.size(),s.script.enemies.size(),s.script.paths.size(),
             s.script.formations.size(),s.dialogue.boxes.size());
    for(auto value:s.background.map)h.byte(value);
    for(auto value:s.background.sections)h.byte(value);
    for(const auto &tile:s.background.tiles)for(auto value:tile)h.byte(value);
    for(const auto &enemy:s.script.enemies){h.fields(enemy.path_slot,enemy.path_reference);for(auto byte:enemy.raw)h.byte(byte);}
    for(const auto &path:s.script.paths){h.add(path.bytes.size());for(auto byte:path.bytes)h.byte(byte);}
    for(const auto &formation:s.script.formations){h.add(formation.trigger);for(auto byte:formation.selectors)h.byte(byte);}
    for(const auto &box:s.dialogue.boxes)for(const auto &line:box.lines)for(auto byte:line.text)h.byte(std::to_integer<std::uint8_t>(byte));
    return h.value;
}
inline std::unique_ptr<EncounterScene> load_practice_replay_scene(const std::filesystem::path &root,const NativeReplay &run){
    if(!run.practice||!native_replay_detail::valid_practice_config(run.config,*run.practice))
        throw std::runtime_error("Invalid Practice replay settings");
    const auto &p=*run.practice;
    PracticeBuilder builder(root,p.spec,{},p.spec.seed,run.config.reduced,p.language,p.start_rule);
    if(builder.status()!=PracticeBuilder::Status::Ready)throw std::runtime_error(builder.error());
    auto scene=builder.release();
    if(practice_replay_start_digest(*scene)!=p.start_hash)throw std::runtime_error("Practice replay start state or assets do not match");
    return scene;
}
inline NativeReplayEvent practice_replay_tick(const EncounterScene &s,std::uint16_t input,bool group,bool fast){
    auto event=normalize_replay_tick(input,group,fast);
    event.context=replay_tick_context(s);
    return event;
}
inline NativeReplaySummary practice_replay_summary(const EncounterScene &s,const PracticeSession &session){
    if(session.simulation_error())throw std::runtime_error(session.error());
    using Result=PracticeSession::Result;
    const auto result=session.result();
    replay_session_detail::Digest h;
    h.fields(replay_state_digest(s),result,s.practice_completed,s.practice_show_dialogue,s.preboss_ready);
    NativeReplaySummary out;
    out.outcome=result==Result::Success?NativeReplayOutcome::Clear:result==Result::Failure?NativeReplayOutcome::GameOver:NativeReplayOutcome::Aborted;
    out.final_stage=std::uint8_t(s.stage);out.final_score=s.score_core.score;out.state_hash=h.value;
    out.final_lives=s.life.lives;out.final_bombs=s.life.bombs;out.ticks=session.samples();
    out.stage_scores.at(s.stage)=out.final_score;out.stage_checks.at(s.stage)=NativeReplayBoundary{out.ticks,out.state_hash};
    return out;
}

enum class ReplayStepStatus { Running, Finished, Failed };
struct ReplayStep {
    ReplayStepStatus status=ReplayStepStatus::Running;
    NativeReplayEvent event;
    bool consumed_tick=false,stage_advanced=false;
    std::string error;
};

class ReplaySession {
    NativeReplayConfig config_;
    bool legacy_=false;
    std::uint32_t ticks_=0;
    std::array<std::optional<std::uint32_t>,6> stage_scores_{};
    std::array<std::optional<NativeReplayBoundary>,6> stage_checks_{};
    std::optional<NativeReplaySummary> summary_;
    std::optional<NativeReplayPractice> practice_;
    std::optional<PracticeSession> practice_session_;
    std::string error_;

    static bool ready(const EncounterScene &s){return s.transition.phase==StageTransition::Phase::Ready;}
    static std::optional<NativeReplayOutcome> natural_outcome(const EncounterScene &s){
        if(s.stage5_result||s.extra_settlement.phase==ExtraSettlement::Phase::Clear)return NativeReplayOutcome::Clear;
        if(s.continue_choice==EncounterScene::ContinueChoice::Declined||s.extra_settlement.phase==ExtraSettlement::Phase::Lost)
            return NativeReplayOutcome::GameOver;
        return {};
    }
    void observe(const EncounterScene &s){stage_scores_.at(s.stage)=s.score_core.score;}
    void finish(const EncounterScene &s,NativeReplayOutcome outcome){
        observe(s);
        const NativeReplayBoundary boundary{ticks_,replay_state_digest(s)};
        stage_checks_.at(s.stage)=boundary;
        summary_=NativeReplaySummary{outcome,std::uint8_t(s.stage),s.score_core.score,boundary.state_hash,stage_scores_,
            s.life.lives,s.life.bombs,ticks_,stage_checks_};
    }
    void advance(EncounterScene &s){
        observe(s);
        stage_checks_.at(s.stage)=NativeReplayBoundary{ticks_,replay_state_digest(s)};
        auto next=s.next_stage();
        if(next.stage==4&&config_.stage5_mainline)next.enable_stage5_preview();
        s=std::move(next);
        observe(s);
    }
    void check_scene(const EncounterScene &s)const{
        if(s.runtime.blocked)throw std::runtime_error(s.runtime.blocked->result.reason);
        if(const auto *reason=s.mima.blocked_reason())throw std::runtime_error(reason);
        if(s.sigma.blocked)throw std::runtime_error(s.sigma.blocked);
        if(s.weapon_blocked)throw std::runtime_error("Replay weapon simulation blocked");
        if(!s.transition_error.empty())throw std::runtime_error(s.transition_error);
    }
    ReplayStep fail(ReplayStep step,std::string error){
        error_=std::move(error);step.status=ReplayStepStatus::Failed;step.error=error_;return step;
    }
public:
    explicit ReplaySession(NativeReplayConfig config={},bool legacy=false):config_(config),legacy_(legacy){
        if(config.stage<0||config.stage>5)throw std::runtime_error("Invalid replay stage");
        stage_scores_[config.stage]=0;
    }
    explicit ReplaySession(const NativeReplay &run):ReplaySession(run.config,!run.summary){
        if(run.practice){
            if(!run.summary||!native_replay_detail::valid_practice_config(run.config,*run.practice))
                throw std::runtime_error("Invalid Practice replay session");
            practice_=run.practice;
            practice_session_.emplace(practice_->spec,std::nullopt,practice_->spec.seed);
        }
    }
    const std::optional<NativeReplaySummary> &summary()const{return summary_;}
    std::uint64_t ticks()const{return ticks_;}

    ReplayStep tick(EncounterScene &s,NativeReplayEvent event){
        ReplayStep step;
        if(!error_.empty())return fail(step,error_);
        if(summary_){step.status=ReplayStepStatus::Finished;return step;}
        if(event.kind!=NativeReplayEventKind::Tick)return fail(step,"Expected a replay input tick");
        if(s.paused)return step;
        step.event=normalize_replay_tick(event.input,event.dialogue_group,event.dialogue_fast_forward);
        if(!legacy_)step.event.context=replay_tick_context(s);
        try{
            s.tick(step.event.input,step.event.dialogue_group,step.event.dialogue_fast_forward);
            ++ticks_;step.consumed_tick=true;
            observe(s);
            // v1 timer batching could include inert Ready ticks. It supplies its
            // own validated zero-time command instead of auto-advancing here.
            if(!legacy_){
                check_scene(s);
                if(auto outcome=natural_outcome(s)){finish(s,*outcome);step.status=ReplayStepStatus::Finished;}
                else if(ready(s)){advance(s);step.stage_advanced=true;}
            }
        }catch(const std::exception &e){return fail(step,e.what());}
        return step;
    }

    ReplayStep cancel_continue(EncounterScene &s){
        ReplayStep step;
        if(!error_.empty())return fail(step,error_);
        if(summary_||s.continue_choice!=EncounterScene::ContinueChoice::Choose)
            return fail(step,"Replay Continue cancel outside the choice prompt");
        step.event={NativeReplayEventKind::CancelContinue};
        s.update_continue(Cancel,true);
        if(!legacy_){finish(s,NativeReplayOutcome::GameOver);step.status=ReplayStepStatus::Finished;}
        return step;
    }

    NativeReplaySummary stop(const EncounterScene &s){
        if(!error_.empty())throw std::runtime_error(error_);
        if(!summary_)finish(s,natural_outcome(s).value_or(NativeReplayOutcome::Aborted));
        return *summary_;
    }

    // Exactly one simulation tick per call at most. Commands adjacent to a
    // tick never consume an extra fixed-clock slot. V2 markers verify a
    // natural transition; they may not cause the scene to skip a stage.
    ReplayStep step_playback(EncounterScene &s,const NativeReplay &run,std::size_t &cursor,int stop_at_stage=-1){
        ReplayStep step;
        if(!error_.empty())return fail(step,error_);
        if(s.paused)return step;
        if(run.config!=config_||run.practice!=practice_||legacy_==run.summary.has_value()||cursor>run.events.size())
            return fail(step,"Replay session does not match its input stream");
        if(practice_){
            if(stop_at_stage!=-1||!s.practice_mode||s.stage!=config_.stage||summary_||cursor>=run.events.size())
                return fail(step,"Invalid Practice replay playback state");
            const auto &e=run.events[cursor];
            step.event=practice_replay_tick(s,e.input,e.dialogue_group,e.dialogue_fast_forward);
            if(e!=step.event)return fail(step,"Practice replay input context does not match the simulation");
            auto &session=*practice_session_;
            session.tick(s,e.input,e.dialogue_group,e.dialogue_fast_forward);
            ++cursor;++ticks_;step.consumed_tick=true;
            if(session.simulation_error())return fail(step,session.error());
            if(cursor==run.events.size()){
                summary_=practice_replay_summary(s,session);
                if(summary_!=run.summary)return fail(step,"Practice replay outcome, score or state digest does not match");
                step.status=ReplayStepStatus::Finished;
            }else if(session.result()!=PracticeSession::Result::Running)
                return fail(step,"Practice replay contains events after its natural outcome");
            return step;
        }
        const auto at_target=[&]{return step.stage_advanced&&s.stage==stop_at_stage;};
        const auto target_boundary=[&]{
            if(cursor==run.events.size()||run.events[cursor].kind!=NativeReplayEventKind::Tick)
                return fail(step,"Replay has no playable samples at the selected stage");
            return step;
        };
        const auto command=[&](const NativeReplayEvent &e){
            if(e.kind==NativeReplayEventKind::CancelContinue){
                const auto cancel=cancel_continue(s);
                if(cancel.status==ReplayStepStatus::Failed)throw std::runtime_error(cancel.error);
            }else if(e.kind==NativeReplayEventKind::NextStage&&legacy_&&ready(s)){
                advance(s);step.stage_advanced=true;
            }else throw std::runtime_error("Replay stage marker does not match the simulation");
        };
        try{
            while(cursor<run.events.size()){
                if(summary_)return fail(step,"Replay contains events after its natural outcome");
                const auto &e=run.events[cursor];
                if(e.kind!=NativeReplayEventKind::Tick){
                    command(e);++cursor;
                    if(at_target()){check_scene(s);return target_boundary();}
                    continue;
                }
                if(step.consumed_tick)break;
                if(!legacy_&&e.context!=replay_tick_context(s))
                    return fail(step,"Replay input context does not match the simulation");
                const bool preadvanced=step.stage_advanced;
                step=tick(s,e);++cursor;
                if(step.status==ReplayStepStatus::Failed)return step;
                if(legacy_&&stop_at_stage!=-1)check_scene(s);
                if(!legacy_&&step.stage_advanced){
                    if(cursor==run.events.size()||run.events[cursor]!=NativeReplayEvent{NativeReplayEventKind::NextStage})
                        return fail(step,"Replay is missing its natural stage marker");
                    const auto previous=std::size_t(s.stage-1);
                    if(stage_checks_[previous]!=run.summary->stage_checks[previous]||
                       stage_scores_[previous]!=run.summary->stage_scores[previous])
                        return fail(step,"Replay stage split score or state digest does not match");
                    ++cursor;
                    if(at_target())return target_boundary();
                }
                step.stage_advanced|=preadvanced;
            }
            if(cursor==run.events.size()){
                if(run.summary){
                    if(!summary_&&run.summary->outcome==NativeReplayOutcome::Aborted)stop(s);
                    if(summary_!=run.summary)return fail(step,"Replay outcome, score or state digest does not match");
                }else stop(s);
                step.status=ReplayStepStatus::Finished;
            }
        }catch(const std::exception &e){return fail(step,e.what());}
        return step;
    }
};

inline std::size_t story_replay_stage_start(const NativeReplay &run,int target){
    if(run.practice||run.config.extra||target<=run.config.stage||target>=5)
        throw std::runtime_error("Replay has no such Story stage");
    auto stage=run.config.stage;
    for(std::size_t i=0;i+1<run.events.size();++i)
        if(run.events[i].kind==NativeReplayEventKind::NextStage&&++stage==target){
            if(run.events[i+1].kind==NativeReplayEventKind::Tick)return i+1;
            break;
        }
    throw std::runtime_error("Replay has no playable samples at the selected stage");
}

} // namespace soew
