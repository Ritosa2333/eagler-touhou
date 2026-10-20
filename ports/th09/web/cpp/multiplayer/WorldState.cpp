#include "WorldState.hpp"

namespace th09::multiplayer {
WorldState::WorldState() { enemies.reserve(260);effects.reserve(1400);attacks.reserve(257);good=journal.Reset({1,FirstWriteByteBudget,FirstWriteBlockBudget,true,true}); }
#if !defined(NDEBUG) && (!defined(TH09_DEVELOPMENT_HARNESS) || TH09_DEVELOPMENT_HARNESS)
bool WorldState::JournalRangeDisjoint(const void* data,std::size_t size) const {
    if(!size)return true;const auto address=reinterpret_cast<std::uintptr_t>(data);
    for(std::size_t i=0;i<bulletRangeCount;++i){
        const auto& instances=bulletRanges[i].visuals->instances;
        const auto base=reinterpret_cast<std::uintptr_t>(instances.data());
        const auto bytes=instances.size()*sizeof(instances[0]);
        // Compare differences rather than overflowing either range's end.
        if(address<base?size>base-address:address-base<bytes)return false;
    }
    return true;
}
#endif
void WorldState::BeforeEnemy(EclVm& e){
    std::size_t slot=enemySaved.size();
    for(std::size_t i=0;i<enemyRanges.size();++i){
        const auto& range=enemyRanges[i];
        if(range.prototype==&e){slot=i*EnemySlotsPerRange;break;}
        if(!range.pool)continue;
        const auto base=reinterpret_cast<std::uintptr_t>(range.pool);
        const auto address=reinterpret_cast<std::uintptr_t>(&e);
        if(address>=base&&address-base<(EnemySlotsPerRange-1)*sizeof(EclVm)&&
            (address-base)%sizeof(EclVm)==0){
            slot=i*EnemySlotsPerRange+1+(address-base)/sizeof(EclVm);break;
        }
    }
    if(slot<enemySaved.size()){
        if(enemySaved[slot])return;enemySaved.set(slot);
    }else for(std::size_t i=0;i<enemyCount;++i)if(enemies[i].first==&e)return;
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
void WorldState::BeforeBullet(BulletManager& x,std::size_t index) {
    if(index>=x.pool.size()){good=false;return;}
    for(std::size_t i=0;i<bulletRangeCount;++i)if(bulletRanges[i].owner==&x){
        good=bulletRanges[i].capture.TouchSlot(x.pool.data(),&x.pool[index],
            [this](void* p,std::size_t n){return Touch(p,n);})&&good;
        return;
    }
    good=false;
}
void WorldState::BeforeBulletVisual(BulletVisuals& x,std::size_t index) {
    if(index>=x.instances.size()){good=false;return;}
    for(std::size_t animation=0;animation<x.instances[index].size();++animation)BeforeBulletAnimation(x,index,animation);
}
void WorldState::BeforeBulletAnimation(BulletVisuals& x,std::size_t index,std::size_t animation) {
    if(index>=x.instances.size()||animation>=x.instances[index].size()){good=false;return;}
    for(std::size_t i=0;i<bulletRangeCount;++i)if(bulletRanges[i].visuals==&x){
        auto& range=bulletRanges[i];auto& saved=range.visualSaved;
        const auto at=index*x.instances[index].size()+animation;
        if(at>=saved.size()){good=false;return;}if(saved[at])return;
        if(!good||journal.Failed()||!journal.IsFrameOpen()||
            FirstWriteBytes()>FirstWriteByteBudget-sizeof(AnmVm)||
            FirstWriteBlocks()>=FirstWriteBlockBudget){good=false;return;}
        auto& page=range.visualPages[at/BulletRange::VMsPerPage];
        if(!page){page.reset(new u8[BulletRange::PageBytes]);++visualPageCount;}
        const auto offset=(at%BulletRange::VMsPerPage)*sizeof(AnmVm);
        std::memcpy(page.get()+offset,&x.instances[index][animation],sizeof(AnmVm));
        saved.set(at);range.visualOrder[range.visualCount++]=u16(at);++visualFirstWriteBlocks;
        return;
    }
    good=false;
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
void WorldState::BeforePlayerShot(PlayerShots& x,std::size_t index) {
    if(index>=x.shots.size()){good=false;return;}
    for(std::size_t i=0;i<shotRangeCount;++i)if(shotRanges[i].owner==&x){
        good=shotRanges[i].capture.TouchSlot(x.shots.data(),&x.shots[index],
            [this](void* p,std::size_t n){return Touch(p,n);})&&good;
        return;
    }
    good=false;
}
void WorldState::Shots(PlayerShots& x) {
    static_assert(std::is_trivially_copyable_v<PlayerShot>);
    // Keep the independent dense/hash path and complete per-slot bytes. The
    // sparse path changes capture timing, never the fields a live shot owns.
    if(!sparse||hashEnabled){NamedFields("s.shots",x.shots);return;}
    if(shotRangeCount==shotRanges.size()){good=false;return;}
    auto& range=shotRanges[shotRangeCount++];range.owner=&x;
    good=range.capture.Capture(x.shots.data(),[](const PlayerShot& shot){return shot.state!=0;},
        [this](void* p,std::size_t n){return Touch(p,n);})&&good;
    Bind(x.checkpoint);
}
void WorldState::PlayerState(Player& x) {
    NamedFields("x.pending_charge,x.charge_generation",x.pending_charge,x.charge_generation);
    NamedFields("x.frame",x.frame);NamedFields("x.motion",x.motion);NamedFields("x.control",x.control);NamedFields("x.body",x.body);NamedFields("x.input",x.input);NamedFields("x.hazards",x.hazards);NamedFields("x.cpu",x.cpu);NamedFields("x.combo_state",x.combo_state);NamedFields("x.secondary_target",x.secondary_target);NamedFields("x.target_priority",x.target_priority);NamedFields("x.attack_levels",x.attack_levels);NamedFields("x.initialized",x.initialized);
    auto& s=x.shots;Shots(s);NamedFields("s.player_position",s.player_position);NamedFields("s.option_positions",s.option_positions);NamedFields("s.target",s.target);NamedFields("s.player_scale",s.player_scale);NamedFields("s.side",s.side);NamedFields("s.beam_time",s.beam_time);NamedFields("s.beam",s.beam);NamedFields("s.areas",s.areas);
    NamedFields("x.items.items",x.items.items);NamedFields("x.items.attraction",x.items.attraction);
    auto& l=x.life;NamedFields("l.input_controller",l.input_controller);NamedFields("l.display_frames",l.display_frames);NamedFields("l.hidden",l.hidden);NamedFields("l.knockback_z",l.knockback_z);NamedFields("l.shield_active",l.shield_active);NamedFields("l.shield_position",l.shield_position);
}
void WorldState::BackgroundState(Background& x) {
    // primitives never reallocates between match construction and retirement.
    // Every VM is captured, so keep the same complete bytes in one journal
    // range instead of one registration/host copy per animation. The hash
    // oracle retains its independent per-VM ordering and diagnostics.
    static_assert(std::is_trivially_copyable_v<AnmVm>);
    if(hashEnabled)for(auto& p:x.primitives)NamedFields("p",p);
    else if(!x.primitives.empty())Touch(x.primitives.data(),x.primitives.size()*sizeof(x.primitives[0]));
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
bool WorldState::Begin() {
    Unbind();hashParts.clear();digest={};if(journal.IsFrameOpen())journal.EndFrame();journal.DiscardBefore(1);undo.clear();ResetEnemyCapture();effectCount=attackCount=0;effectRangeCount=effectSlots=0;effectSaved.reset();attackSaved.reset();ownedBytes=visualFirstWriteBlocks=0;good=journal.BeginFrame(0);
    bulletRangeCount=shotRangeCount=0;
    return good;
}
bool WorldState::Save(GameSession& s) {
    if(!Begin())return false;
    // This adapter is specifically the stable live two-human versus world.
    if(!s.world||s.is_replay||s.world->rules.mode!=GameMode::versus)return good=false;
    NamedFields("s.state,s.animations.timing,s.animations.executed,s.animations.invalid,s.initial,s.current,s.inputs,s.scores,s.starting_replay_stage,s.demo,s.demo_input,s.demo_frames,s.warmed_stage,s.result,s.phase,s.continues,s.recordable,s.frames,s.timing,s.motion_input",s.state,s.animations.timing,s.animations.executed,s.animations.invalid,s.initial,s.current,s.inputs,s.scores,s.starting_replay_stage,s.demo,s.demo_input,s.demo_frames,s.warmed_stage,s.result,s.phase,s.continues,s.recordable,s.frames,s.timing,s.motion_input);Text(s.error);
    Recording(s.recording);
    auto& charge=s.charge_replay;NamedFields("charge.enabled,charge.playing,charge.invalid,charge.cursors",charge.enabled,charge.playing,charge.invalid,charge.cursors);for(auto& entries:charge.stages)AppendOnly(entries);
    auto& m=s.motion;NamedFields("m.ticks,m.cursors,m.playing,m.recording,m.invalid,m.active,m.unlimited,m.target_x,m.target_y",m.ticks,m.cursors,m.playing,m.recording,m.invalid,m.active,m.unlimited,m.target_x,m.target_y);for(auto& v:m.stages)AppendOnly(v);
    auto& w=*s.world;NamedFields("w.configuration,w.combined_input,w.timing,w.markers,w.music_track,w.ready,w.paused,w.transition_pending,w.transition",w.configuration,w.combined_input,w.timing,w.markers,w.music_track,w.ready,w.paused,w.transition_pending,w.transition);Text(w.error);
    auto& r=w.rules;NamedFields("r.progress,r.mode,r.scores,r.attack_levels",r.progress,r.mode,r.scores,r.attack_levels);Vector(w.screen_effects.effects);
    auto& ss=w.scene_services();NamedFields("ss.game_flags,ss.wins_required,ss.wins,ss.starting_extra_lives,ss.opponent_character,ss.music_track,ss.cpu_level,ss.route,ss.players,ss.geometry",ss.game_flags,ss.wins_required,ss.wins,ss.starting_extra_lives,ss.opponent_character,ss.music_track,ss.cpu_level,ss.route,ss.players,ss.geometry);
    auto& b=*w.battle;NamedFields("b.state,b.patterns",b.state,b.patterns);Text(b.error);
    // Register both pools before capturing either side: a boss alias can point
    // across them without first being saved through the unknown-owner path.
    for(std::size_t side=0;side<enemyRanges.size();++side){
        auto& manager=*b.fields[side].enemies;
        enemyRanges[side]={&manager.prototype,manager.enemies.data()};
    }
    for(auto& f:b.fields){
        NamedFields("f.script,f.enemy_player,f.focus_aura,f.capture_effect,f.shield,f.targeted_enemy,f.cpu_level,f.spells,f.bosses,f.counters",f.script,f.enemy_player,f.focus_aura,f.capture_effect,f.shield,f.targeted_enemy,f.cpu_level,f.spells,f.bosses,f.counters);
        PlayerState(*f.player);Enemy(*f.enemies);Effects(*f.effects);
        auto& bullets=*f.bullets;NamedFields("bullets.total,bullets.first_count,bullets.second_count,bullets.cancel_frames,bullets.frame,bullets.lifetime,bullets.draw_heads",bullets.total,bullets.first_count,bullets.second_count,bullets.cancel_frames,bullets.frame,bullets.lifetime,bullets.draw_heads);
        // Keep complete POD bytes for live runs, plus dormant slots before
        // spawn/reset. Shared slot identity prevents a later first write from
        // overlapping a run already in the journal. The dense oracle still
        // visits every slot independently, including all dormant bytes.
        static_assert(std::is_trivially_copyable_v<Bullet>);
        if(hashEnabled)for(auto& bullet:bullets.pool)NamedFields("bullet",bullet);
        else if(!sparse)Touch(bullets.pool.data(),bullets.pool.size()*sizeof(bullets.pool[0]));
        else {
            if(bulletRangeCount>=bulletRanges.size()||bullets.pool.size()!=BulletManager::update_count+1)return good=false;
            auto& range=bulletRanges[bulletRangeCount++];range.owner=&bullets;range.visuals=f.bullet_visuals.get();range.visualSaved.reset();range.visualCount=0;
            good=range.capture.Capture(bullets.pool.data(),[](const Bullet& b){return b.state!=0;},
                [this](void* p,std::size_t n){return Touch(p,n);})&&good;
            // Hazards can retain a slot across a paused/frozen player update.
            // A later hit writes state=5 even if that slot has since gone cold.
            const auto& hazards=f.player->hazards;
            if(hazards.count>hazards.entries.size())return good=false;
            for(u32 i=0;i<hazards.count;++i)if(auto* p=hazards.entries[i].bullet){
                const auto base=reinterpret_cast<std::uintptr_t>(bullets.pool.data());
                const auto address=reinterpret_cast<std::uintptr_t>(p);
                if(address<base||address-base>=bullets.pool.size()*sizeof(Bullet)||(address-base)%sizeof(Bullet))return good=false;
                BeforeBullet(bullets,(address-base)/sizeof(Bullet));
            }
            Bind(bullets.checkpoint);
        }
        // initialize() is the sole writer of these resource templates, before
        // the match/checkpoint horizon. Live animation writes target instances.
        // The independent dense/hash oracle still includes every template byte.
        if(!sparse||hashEnabled)NamedFields("f.bullet_visuals->appearances",f.bullet_visuals->appearances);
        auto& visuals=*f.bullet_visuals;
        if(!sparse||hashEnabled)for(auto& v:visuals.instances)Fields(v);
        else {
            if(visuals.instances.size()!=bullets.pool.size())return good=false;
            // Body/fade/spawn animations have independent state. Save a whole
            // VM on its actual first write (including Draw), not all five merely
            // because the bullet is alive. Full-slot writes share that identity;
            // the dense/hash oracle above still reads every VM independently.
            Bind(visuals.checkpoint);
        }
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
    // VM ranges are disjoint from the journal and from each other. Restore all
    // POD before any owning/texture undo can observe it, as the journal did.
    for(std::size_t i=0;i<bulletRangeCount;++i){
        auto& range=bulletRanges[i];
        for(std::size_t n=range.visualCount;n;--n){
            const auto at=range.visualOrder[n-1];
            const auto* page=range.visualPages[at/BulletRange::VMsPerPage].get();
            assert(page&&range.visualSaved[at]);
            std::memcpy(&range.visuals->instances[at/5][at%5],
                page+(at%BulletRange::VMsPerPage)*sizeof(AnmVm),sizeof(AnmVm));
        }
        range.visualSaved.reset();range.visualCount=0;
    }
    visualFirstWriteBlocks=0;
    for(auto& action:undo)action();
    for(std::size_t i=0;i<enemyCount;++i){auto& e=enemies[i];good=e.second.Restore(*e.first)&&good;}
    for(std::size_t i=0;i<effectCount;++i){auto& e=effects[i];good=e.second.Restore(*e.first)&&good;}
    for(std::size_t i=0;i<attackCount;++i){auto& a=attacks[i];good=a.second.Restore(*a.first)&&good;}
    return Good();
}
}
