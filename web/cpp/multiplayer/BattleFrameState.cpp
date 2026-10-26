#include "BattleFrameState.hpp"
#if TH15_MULTIPLAYER
#include "AnimationState.hpp"
#include "BulletFrameState.hpp"
#include "EnemyFrameState.hpp"
#include "FrameState.hpp"
#include "FrameValues.hpp"
#include "ItemFrameState.hpp"
#include "LaserFrameState.hpp"
#include "../game/RecordStore.hpp"
#include <optional>
namespace th15::multiplayer {
auto BattleFrameState::battle_fields(GameBattle& b){return std::tie(b.initialized,b.input,b.session,b.spell,b.score,b.bomb_enemies,b.stage,b.spell_visuals,b.mp_options,b.mp_enabled,b.mp_rebinding,b.mp_contact_seat,b.mp_item_targets,b.mp_gift_targets,b.mp_fragments,b.error);}
auto BattleFrameState::pilot_fields(MultiplayerPilot& p){return std::tie(p.owned_session,p.owned_life_piece_tier,p.keys,p.input,p.ghost,p.spell_startup_bomb,p.ghost_clock,p.ghost_random,p.ghost_dx,p.ghost_dy,p.life_hold,p.power_taps,p.power_gap,p.life_latched,p.graze_animations);}
auto BattleFrameState::visual_fields(PlayerAnmHost& v){return std::tie(v.hitbox_display,v.previous_focus_display,v.root,v.focus_handle,v.barrier_handle,v.option_handles);}
auto BattleFrameState::life_fields(PlayerLife& l){return std::tie(l.practice,l.state,l.base_bombs,l.age,l.invulnerability,l.last_death_position,l.error);}
auto BattleFrameState::frame_fields(PlayerFrame& f){return std::tie(f.context,f.input_age,f.ready_age,f.shooting,f.bounds,f.collision,f.error);}
auto BattleFrameState::shot_context_fields(PlayerShotContext& c){return std::tie(c.option_angles,c.laser_power,c.power,c.power_step,c.shoot_frame,c.enemy_scene,c.bomb_active,c.enemies,c.screen_origin,c.background_delta);}
auto BattleFrameState::shot_fields(PlayerShots& s){return std::tie(s.shots,s.error,s.rate,s.reward_counters);}
auto BattleFrameState::option_fields(PlayerOptions& o){return std::tie(o.power_level,o.auxiliary,o.error);}
auto BattleFrameState::spell_fields(SpellCard& s){return std::tie(s.clock,s.age,s.identifier,s.maximum_bonus,s.duration,s.active_frames,s.name,s.error,s.anchor,s.handles,s.replay);}
auto BattleFrameState::bomb_fields(BombController& b){return std::tie(b.frame_rate,b.position,b.angle,b.age,b.secondary_age,b.effective_against_spell,b.error,b.context.player_resource,b.context.hud_available,b.context.hud_collect);}
struct BattleFrameState::Data {
    struct PlayerImage {
        Player* owner=nullptr;BombController* bomb=nullptr;MultiplayerPilot* pilot=nullptr;int character=0;
        ShtHeader header;
        PlayerMotion motion;
        DamageSources damage;
        decltype(frame_values::copy(visual_fields(std::declval<PlayerAnmHost&>()))) visuals;
        decltype(frame_values::copy(life_fields(std::declval<PlayerLife&>()))) life;
        decltype(frame_values::copy(frame_fields(std::declval<PlayerFrame&>()))) frame;
        decltype(frame_values::copy(shot_context_fields(std::declval<PlayerShotContext&>()))) shot_context;
        decltype(frame_values::copy(shot_fields(std::declval<PlayerShots&>()))) shots;
        decltype(frame_values::copy(option_fields(std::declval<PlayerOptions&>()))) options;
        decltype(frame_values::copy(pilot_fields(std::declval<MultiplayerPilot&>()))) pilot_values;
        decltype(frame_values::copy(bomb_fields(std::declval<BombController&>()))) bomb_values;
        std::array<ReimuOrb,8> orbs{};u32 first=0,aura=0;bool orbs_available=false;i32 charges=0;
        i32 collection_power=1;std::string collection_error,error;bool initialized=false;
    };
    GameBattle* owner=nullptr;SessionState* progress_owner=nullptr;RecordStore* record_owner=nullptr;FrameScheduler* schedule_owner=nullptr;
    decltype(frame_values::copy(battle_fields(std::declval<GameBattle&>()))) battle;
    decltype(frame_values::copy(spell_fields(std::declval<SpellCard&>()))) spell;
    std::array<std::unique_ptr<PlayerImage>,3> players;
    std::array<bool,3> present{};
    EnemyFrameState enemies;LaserFrameState lasers;BulletFrameState bullets;ItemFrameState items;
    AnimationState animations;FrameState callbacks;
    AnmManager::ResourceLease resources;
    std::vector<AnmVm*> roots;
    AnmEnvironment environment;Rng visual;
    std::optional<SessionState> progress;
    std::optional<RecordStore> records;
    std::array<u32,1024> effect_handles{};i32 effect_cursor=0;
    std::string effect_error,animation_error;
    float animation_rate=1;u64 presentation_counter=0;
    bool ready=false;
};
BattleFrameState::BattleFrameState()=default;
BattleFrameState::~BattleFrameState()=default;
BattleFrameState::BattleFrameState(BattleFrameState&&)noexcept=default;
BattleFrameState& BattleFrameState::operator=(BattleFrameState&&)noexcept=default;
bool BattleFrameState::capture(GameBattle& battle,const std::vector<AnmVm*>& extra_roots){
    frame_values::IdentityGuard guard;error.clear();if(!saved_)saved_=std::make_unique<Data>();auto& s=*saved_;s.ready=false;
    s.owner=&battle;s.progress_owner=battle.progress;s.record_owner=battle.records;s.schedule_owner=battle.active_schedule;
    if(!battle.mp_enabled||!battle.active_schedule||!battle.enemies||!battle.laser_scene||!battle.bullet_scene||!battle.items){error="Battle owner is not ready for rollback";return false;}
    s.resources=battle.animations.retain_resources();
    s.roots.assign(extra_roots.begin(),extra_roots.end());
    for(auto& pilot:battle.pilots)if(pilot&&pilot->player){auto& v=pilot->player->visuals;s.roots.push_back(&v.root);for(auto& marker:v.hitbox_display.markers)s.roots.push_back(&marker);}
    BulletFrameState::append_roots(*battle.bullet_scene,battle.animations.registry,s.roots);LaserFrameState::append_roots(*battle.laser_scene,battle.animations.registry,s.roots);ItemFrameState::append_roots(*battle.items,battle.animations.registry,s.roots);
    if(!s.animations.capture(battle.animations.registry,s.roots)){error=s.animations.error;return false;}
    if(!s.callbacks.capture(*battle.active_schedule)){error=s.callbacks.error;return false;}
    s.battle=frame_values::copy(battle_fields(battle));s.spell=frame_values::copy(spell_fields(battle.spell_card));
    s.environment=battle.environment;s.visual=battle.visual_random;s.presentation_counter=guard.value;
    if(battle.progress)s.progress=*battle.progress;else s.progress.reset();
    if(battle.records)s.records=*battle.records;else s.records.reset();
    s.effect_handles=battle.effects.handles;s.effect_cursor=battle.effects.cursor;s.effect_error=battle.effects.error;
    s.animation_rate=battle.animations.rate;s.animation_error=battle.animations.error;
    for(size_t seat=0;seat<s.players.size();++seat){s.present[seat]=bool(battle.pilots[seat]);if(!s.present[seat])continue;
        auto& pilot=*battle.pilots[seat];if(!pilot.player||!pilot.bomb||!pilot.collection){error="Pilot owner is not ready for rollback";return false;}
        if(!s.players[seat])s.players[seat]=std::make_unique<Data::PlayerImage>();auto& image=*s.players[seat];auto& player=*pilot.player;
        image.owner=&player;image.bomb=pilot.bomb;image.pilot=&pilot;image.character=pilot.character;
        image.header=player.resource.header;image.motion=player.motion;image.damage=player.damage;image.visuals=frame_values::copy(visual_fields(player.visuals));
        image.life=frame_values::copy(life_fields(player.life));image.frame=frame_values::copy(frame_fields(player.frame));image.shot_context=frame_values::copy(shot_context_fields(player.shot_context));
        image.shots=frame_values::copy(shot_fields(player.shots));image.options=frame_values::copy(option_fields(player.options));image.pilot_values=frame_values::copy(pilot_fields(pilot));
        image.collection_power=pilot.collection->small_power_amount;image.collection_error=pilot.collection->error;image.error=player.error;image.initialized=player.initialized;
        image.bomb_values=frame_values::copy(bomb_fields(*pilot.bomb));
        switch(pilot.character){
        case 0:{auto& bomb=static_cast<BombReimu&>(*pilot.bomb);image.orbs=bomb.orbs;image.orbs_available=bomb.orbs_available;image.aura=bomb.aura;break;}
        case 1:{auto& bomb=static_cast<BombMarisa&>(*pilot.bomb);image.first=bomb.beam;image.aura=bomb.aura;break;}
        case 2:{auto& bomb=static_cast<BombSanae&>(*pilot.bomb);image.first=bomb.field;image.aura=bomb.aura;break;}
        case 3:{auto& bomb=static_cast<BombReisen&>(*pilot.bomb);image.first=bomb.barrier;image.aura=bomb.aura;image.charges=bomb.charges;break;}
        default:error="Unknown bomb rollback kind";return false;
        }
    }
    if(!s.enemies.capture(*battle.enemies)){error=s.enemies.error;return false;}
    if(!s.lasers.capture(*battle.laser_scene)){error=s.lasers.error;return false;}
    if(!s.bullets.capture(*battle.bullet_scene)){error=s.bullets.error;return false;}
    if(!s.items.capture(*battle.items)){error=s.items.error;return false;}
    s.ready=true;return true;
}
bool BattleFrameState::restore(GameBattle& battle)const{
    error.clear();if(!saved_||!saved_->ready){error="No battle frame snapshot";return false;}const auto& s=*saved_;
    if(s.owner!=&battle||s.progress_owner!=battle.progress||s.record_owner!=battle.records||s.schedule_owner!=battle.active_schedule){error="Battle rollback crossed an owner barrier";return false;}
    for(size_t seat=0;seat<s.players.size();++seat){if(s.present[seat]!=bool(battle.pilots[seat])){error="Pilot ownership changed inside correction window";return false;}if(!s.present[seat])continue;const auto& image=*s.players[seat];const auto& pilot=*battle.pilots[seat];if(image.pilot!=&pilot||image.owner!=pilot.player||image.bomb!=pilot.bomb||image.character!=pilot.character){error="Pilot factory crossed the correction window";return false;}}
    FrameState::detach(*battle.active_schedule);
    battle_fields(battle)=s.battle;spell_fields(battle.spell_card)=s.spell;
    // Window/raster changes belong to the presentation surface, not its past.
    const auto raster=battle.environment.raster_scale,scale=battle.environment.resolution_scale;
    const auto width=battle.environment.screen_width,height=battle.environment.screen_height;const bool alpha=battle.environment.render_target_has_alpha;
    battle.environment=s.environment;battle.environment.raster_scale=raster;battle.environment.resolution_scale=scale;battle.environment.screen_width=width;battle.environment.screen_height=height;battle.environment.render_target_has_alpha=alpha;
    battle.visual_random=s.visual;if(s.progress)*battle.progress=*s.progress;if(s.records)*battle.records=*s.records;
    battle.effects.handles=s.effect_handles;battle.effects.cursor=s.effect_cursor;battle.effects.error=s.effect_error;battle.animations.rate=s.animation_rate;battle.animations.error=s.animation_error;
    for(size_t seat=0;seat<s.players.size();++seat){if(!s.present[seat])continue;const auto& image=*s.players[seat];auto& pilot=*battle.pilots[seat];auto& player=*pilot.player;
        player.resource.header=image.header;player.motion=image.motion;player.damage=image.damage;visual_fields(player.visuals)=image.visuals;life_fields(player.life)=image.life;frame_fields(player.frame)=image.frame;
        player.frame.active_rate=nullptr;shot_context_fields(player.shot_context)=image.shot_context;shot_fields(player.shots)=image.shots;option_fields(player.options)=image.options;pilot_fields(pilot)=image.pilot_values;
        pilot.collection->small_power_amount=image.collection_power;pilot.collection->error=image.collection_error;player.error=image.error;player.initialized=image.initialized;
        bomb_fields(*pilot.bomb)=image.bomb_values;
        switch(pilot.character){
        case 0:{auto& bomb=static_cast<BombReimu&>(*pilot.bomb);bomb.orbs=image.orbs;bomb.orbs_available=image.orbs_available;bomb.aura=image.aura;break;}
        case 1:{auto& bomb=static_cast<BombMarisa&>(*pilot.bomb);bomb.beam=image.first;bomb.aura=image.aura;break;}
        case 2:{auto& bomb=static_cast<BombSanae&>(*pilot.bomb);bomb.field=image.first;bomb.aura=image.aura;break;}
        case 3:{auto& bomb=static_cast<BombReisen&>(*pilot.bomb);bomb.barrier=image.first;bomb.aura=image.aura;bomb.charges=image.charges;break;}
        }
    }
    if(!s.enemies.restore(*battle.enemies)){error=s.enemies.error;return false;}
    if(!s.lasers.restore(*battle.laser_scene)){error=s.lasers.error;return false;}
    if(!s.bullets.restore(*battle.bullet_scene)){error=s.bullets.error;return false;}
    if(!s.items.restore(*battle.items)){error=s.items.error;return false;}
    // Owners/embedded VMs must be restored before their graph and callbacks.
    if(!s.animations.restore(battle.animations.registry)){error=battle.animations.registry.error;return false;}
    if(!s.callbacks.restore(*battle.active_schedule)){error=s.callbacks.error;return false;}
    AnmVm::presentation_counter=s.presentation_counter;return true;
}
void BattleFrameState::discard()noexcept{if(saved_){saved_->enemies.discard();saved_->lasers.discard();saved_->items.discard();saved_->resources.clear();saved_->ready=false;}}
}
#endif
