#include "ItemSystem.hpp"
#include "../multiplayer/ResourceTrace.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/JournalTouch.hpp"
#endif

namespace th08 {

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY

ItemSystem::ItemSystem(PlayerSimulation& p,GameGlobals& g,GameValues& v,GameGauge& gauge,GameRank& r,HighScore& h,Rng& random,AnmLibrary& a,AnmExecutor& e,AnmRenderer& graphics,ItemSystemActions& services)
    :player(p),globals(g),animations(a),executor(e),renderer(graphics),actions(services),rank(r),high_score(h),pool(*state,random,*this),rewards(reward_input,g,v,gauge,r,h,pool,*this),updater(*state,pool,input,p.profile(false),p.profile(true),*this){state->reset();pool.snapshot();}

void ItemSystem::synchronize(){
    auto& c=player.status().context;c.power=Scalar::truncate(globals.power);c.lives=Scalar::truncate(globals.lives);c.bombs=Scalar::truncate(globals.bombs);c.time_orbs=globals.time_orbs;c.last_spell_requirement=globals.last_spell_requirement;c.gauge=globals.gauge;
    input.power=c.power;
}
ItemState* ItemSystem::spawn(const Vec3& position,i32 type,i32 mode){executor.timing=player.timing;return pool.spawn(position,type,mode,Scalar::truncate(globals.power),player.status().life.state);}
bool ItemSystem::touching(u32 seat,const Vec3& p,const Vec3& size){(void)seat;return player.collision().item(p,size);}
void ItemSystem::collect(u32 seat,ItemState& item){
    (void)seat;read_hud();auto& p=player.status();p.context.replay_flags=input.replay_flags;reward_input.hud_flags=p.context.hud_flags;reward_input.power_flag=p.context.miss_control;reward_input.gauge_lock=input.gauge_lock;
    rewards.collect(item);failed|=rewards.failed;input.replay_flags=p.context.replay_flags;p.context.hud_flags=reward_input.hud_flags;p.context.miss_control=reward_input.power_flag;write_hud();synchronize();
}
void ItemSystem::item_sound(u32 seat,i32 index,i32 mode){(void)seat;actions.sound(index,mode);}
void ItemSystem::removed(ItemState&){ }
u32 ItemSystem::owner_for(ItemState&){return 0;}
bool ItemSystem::update(){
    if(failed)return false;pool.snapshot();auto& p=player.status();synchronize();read_hud();executor.timing=updater.timing=player.timing;
    input.player=p.motion.movement.position;input.height=p.context.extent.y;input.focused=p.motion.form.focused;input.character=p.context.character;input.player_state=p.life.state;
    input.shooting=p.shots.shooting_timer;input.gauge_lock=p.item_gauge_lock;input.replay_flags=p.context.replay_flags;
    reward_input.collect_line=player.profile(false).item_collect_line;reward_input.difficulty=difficulty;reward_input.bomb_triggered=p.bomb.triggered;reward_input.bomb_active=p.bomb.active;reward_input.focused=p.motion.form.focused;reward_input.time_spell=p.context.time_spell;
    updater.update();p.item_gauge_lock=input.gauge_lock;p.context.replay_flags=input.replay_flags;synchronize();return !failed;
}
bool ItemSystem::draw(const Vec2& offset){if(failed)return false;pool.draw(offset);return !failed;}
void ItemSystem::collect_all(){pool.collect_all();}
void ItemSystem::collect_all(u32 seat){if(seat==0)pool.collect_all();}
void ItemSystem::cancel_homing(){pool.cancel_homing();}
void ItemSystem::cancel_homing(u32 seat){if(seat==0)pool.cancel_homing();}
void ItemSystem::time_orb(){time_orb(0);}
void ItemSystem::time_orb(u32 seat){
    (void)seat;read_hud();auto& p=player.status();reward_input.hud_flags=p.context.hud_flags;reward_input.bomb_triggered=p.bomb.triggered;reward_input.bomb_active=p.bomb.active;reward_input.focused=p.motion.form.focused;reward_input.gauge_lock=p.item_gauge_lock;
    rewards.time_orb(nullptr);failed|=rewards.failed;p.context.hud_flags=reward_input.hud_flags;write_hud();synchronize();
}
void ItemSystem::reset(){state->reset();failed=false;rewards.failed=false;}

#else

ItemSystem::ItemSystem(PlayerSimulation& p,PlayerResourceView& r,PlayerValues& v,GameGauge& gauge,GameRank& rank_value,HighScore& h,Rng& random,AnmLibrary& a,AnmExecutor& e,AnmRenderer& graphics,ItemSystemActions& services)
    :player(p),globals(r.shared),animations(a),executor(e),renderer(graphics),actions(services),rank(rank_value),high_score(h),pool(*state,random,*this),updater(*state,pool,*this){
    item_owners.fill(no_owner);gift_recipients.fill(no_owner);state->reset();pool.snapshot();bind_player(0,p,r,v,gauge,services);
}
u32 ItemSystem::item_index(const ItemState& item)const noexcept{
    const auto first=reinterpret_cast<std::uintptr_t>(state->items);
    const auto address=reinterpret_cast<std::uintptr_t>(&item);
    if(address<first||address>=first+sizeof(state->items))return ItemPoolState::capacity;
    const auto offset=address-first;
    return offset%sizeof(ItemState)?ItemPoolState::capacity:u32(offset/sizeof(ItemState));
}
u32 ItemSystem::nearest_owner(const Vec3& origin)const noexcept{
    u32 result=no_owner;float best=0;
    for(u32 seat=0;seat<3;++seat){const auto& owner=owners[seat];if(!owner.available||!owner.player)continue;const auto& position=owner.player->status().motion.movement.position;
        const float x=Scalar::sub(position.x,origin.x),y=Scalar::sub(position.y,origin.y);const float distance=Scalar::add(Scalar::mul(x,x),Scalar::mul(y,y));
        if(result==no_owner||distance<best){result=seat;best=distance;}
    }
    return result;
}
void ItemSystem::bind_player(u32 seat,PlayerSimulation& p,PlayerResourceView& r,PlayerValues& v,GameGauge& gauge,ItemSystemActions& player_actions){
    if(seat>=3)return;auto& owner=owners[seat];owner.player=&p;owner.resources=&r;owner.values=&v;owner.gauge=&gauge;owner.actions=&player_actions;owner.available=true;
    owner.rewards.emplace(owner.reward_input,r,v,gauge,rank,high_score,pool,player_actions);updater.bind_player(seat,owner.input,p.profile(false),p.profile(true));updater.set_player_available(seat,true);
}
void ItemSystem::set_player_available(u32 seat,bool available){
    if(seat>=3||!owners[seat].player)return;
    owners[seat].available=available;updater.set_player_available(seat,available);
    // Permanently unavailable/spirit recipients release outstanding gifts.
    // Reviving later must not reclaim a gift which another survivor now owns.
    if(!available)for(u32 i=0;i<ItemPoolState::capacity;++i){
        if(gift_recipients[i]==seat)gift_recipients[i]=no_owner;
        if(item_owners[i]==seat)item_owners[i]=no_owner;
    }
}
void ItemSystem::synchronize(Owner& owner){auto& c=owner.player->status().context;auto& r=*owner.resources;c.power=Scalar::truncate(r.power);c.lives=Scalar::truncate(r.lives);c.bombs=Scalar::truncate(r.bombs);c.time_orbs=r.time_orbs;c.last_spell_requirement=r.last_spell_requirement;c.gauge=r.gauge;owner.input.power=c.power;}
ItemState* ItemSystem::spawn(const Vec3& position,i32 type,i32 mode){
    executor.timing=player.timing;
    const u32 nearest=nearest_owner(position);
    const i8 life=nearest<3?owners[nearest].player->status().life.state:player.status().life.state;
    // TH08 has personal Power in multiplayer. There is no single shared
    // Power value which may reinterpret an authored Power drop as the retail
    // full-Power point-item type. Preserve the authored item type here; the
    // eventual collector's own Power decides the normal collection reward.
    auto* item=pool.spawn(position,type,mode,0,life);
    if(item&&item_index(*item)<ItemPoolState::capacity){
        item_owners[item_index(*item)]=no_owner;gift_recipients[item_index(*item)]=no_owner;
    }
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Event("item.spawn",int(nearest),type,item);
#endif
    return item;
}
bool ItemSystem::spawn_for_player(const Vec3& position,i32 type,i32 mode,u32 seat){
    if(seat>=3||!owners[seat].available)return false;
    auto* item=spawn(position,type,mode);
    if(!item)return false;
    const u32 index=item_index(*item);
    if(index>=ItemPoolState::capacity)return false;
    item_owners[index]=u8(seat);
    gift_recipients[index]=u8(seat);
    return true;
}
bool ItemSystem::spawn_power_gift(const Vec3& position,u32 seat){
    if(seat>=3||!owners[seat].available)return false;
    u32 free=0;for(u32 i=0;i<ItemPoolState::capacity&&free<6;++i)free+=!state->items[i].active;
    if(free<6)return false;
    ItemState* created[6]{};u32 count=0;
    for(i32 i=0;i<6;++i){
        Vec3 spawn_position=position;
        spawn_position.x=Scalar::add(spawn_position.x,float((i%3-1)*10));
        spawn_position.y=Scalar::add(spawn_position.y,float((i/3-1)*8));
        auto* item=spawn(spawn_position,i<2?2:0,1);
        if(!item||item_index(*item)>=ItemPoolState::capacity){
            for(u32 j=0;j<count;++j){removed(*created[j]);pool.remove(*created[j]);}
            return false;
        }
        item_owners[item_index(*item)]=u8(seat);
        gift_recipients[item_index(*item)]=u8(seat);
        created[count++]=item;
    }
    return true;
}
u32 ItemSystem::owner_for(ItemState& item){
    const u32 index=item_index(item);if(index>=ItemPoolState::capacity)return no_owner;
    const u8 recipient=gift_recipients[index];
    if(recipient<3&&owners[recipient].available&&owners[recipient].player){
        item_owners[index]=recipient;return recipient;
    }
    gift_recipients[index]=no_owner;
    const u8 previous=item_owners[index];
    if(item.state==1&&previous<3&&owners[previous].available&&owners[previous].player)return previous;
    const u32 result=nearest_owner(item.position);item_owners[index]=u8(result);
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    if(previous!=result)multiplayer::diagnostic::Event("item.owner",int(result),previous,&item);
#endif
    return result;
}
u32 ItemSystem::auto_collect_owner(ItemState& item){
    const u32 index=item_index(item);if(index>=ItemPoolState::capacity||item.state!=0)return no_owner;
    // Directed gifts are an explicit cooperation promise and cannot be stolen
    // by another pilot crossing the Point of Collection line.
    const u8 recipient=gift_recipients[index];
    if(recipient<3&&owners[recipient].available&&owners[recipient].player)return no_owner;
    u32 claimant=no_owner;
    for(u32 seat=0;seat<3;++seat){
        const auto& owner=owners[seat];if(!owner.available||!owner.player||!owner.resources)continue;
        const auto& input=owner.input;const auto& profile=owner.player->profile(false);
        if(input.player.y<profile.item_collect_line&&
           (input.power>=128||input.focused||input.character==1||input.character==6)){claimant=seat;break;}
    }
    if(claimant==no_owner)return no_owner;
    // One retail player crossing the POC claims the field. Multiplayer keeps
    // that semantic; the seat order is only the deterministic tie-break for
    // the simultaneous multi-claim edge, never a round-robin item split.
    item_owners[index]=u8(claimant);
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Event("item.poc",int(claimant),0,&item);
#endif
    return claimant;
}
bool ItemSystem::touching(u32 seat,const Vec3& p,const Vec3& size){return seat<3&&owners[seat].available&&owners[seat].player&&owners[seat].player->collision().item(p,size);}
void ItemSystem::collect(u32 seat,ItemState& item){
    if(seat>=3||!owners[seat].player||!owners[seat].resources||!owners[seat].values||!owners[seat].actions||!owners[seat].rewards)return;auto& owner=owners[seat];auto& p=*owner.player;auto& context=owner.reward_input;auto& state_context=p.status().context;
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Scope trace("item.collect",int(seat),item.type,&item);
#endif
    state_context.replay_flags=owner.input.replay_flags;context.hud_flags=state_context.hud_flags;context.power_flag=state_context.miss_control;context.gauge_lock=owner.input.gauge_lock;owner.rewards->collect(item);failed|=owner.rewards->failed;owner.input.replay_flags=state_context.replay_flags;state_context.hud_flags=context.hud_flags;state_context.miss_control=context.power_flag;synchronize(owner);
}
void ItemSystem::item_sound(u32 seat,i32 index,i32 mode){if(seat<3&&owners[seat].actions)owners[seat].actions->sound(index,mode);else actions.sound(index,mode);}
void ItemSystem::removed(ItemState& item){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Event("item.remove",-1,0,&item);
#endif
    const u32 index=item_index(item);if(index<ItemPoolState::capacity){item_owners[index]=no_owner;gift_recipients[index]=no_owner;}
}
void ItemSystem::award_team_extend(){
    bool awarded=false;
    for(auto& owner:owners){
        if(!owner.available||!owner.resources||!owner.values||!owner.player)continue;
        auto& resources=*owner.resources;
        if(Scalar::truncate(resources.lives)<8){
            failed|=!owner.values->add_lives(1);
            owner.reward_input.hud_flags=(owner.reward_input.hud_flags&~3u)|2;
            awarded=true;
        }else if(Scalar::truncate(resources.bombs)<8){
            failed|=!owner.values->add_bombs(1);
            owner.reward_input.hud_flags=(owner.reward_input.hud_flags&~12u)|8;
            awarded=true;
        }
        owner.player->status().context.hud_flags=owner.reward_input.hud_flags;
        synchronize(owner);
    }
    if(awarded){actions.sound(28,0);rank.add(200);}
}
void ItemSystem::convert_power_items(ItemState& collected){
    (void)collected;
    // Retail's global conversion assumes one player's Power is the world's
    // Power. TH08MP has personal Power, so converting the shared field when
    // one/all currently-operable pilots reach 128 corrupts other pilots'
    // resources and can create point talismans after a teammate becomes a
    // Spirit. Keep the authored field items unchanged.
}
bool ItemSystem::update(){
    if(failed)return false;pool.snapshot();executor.timing=updater.timing=player.timing;
    for(u32 seat=0;seat<3;++seat){auto& owner=owners[seat];if(!owner.player||!owner.resources)continue;auto& p=*owner.player;auto& c=p.status().context;synchronize(owner);owner.input.player=p.status().motion.movement.position;owner.input.height=c.extent.y;owner.input.focused=p.status().motion.form.focused;owner.input.character=c.character;owner.input.player_state=p.status().life.state;owner.input.shooting=p.status().shots.shooting_timer;owner.input.gauge_lock=p.status().item_gauge_lock;owner.input.replay_flags=c.replay_flags;owner.reward_input.collect_line=p.profile(false).item_collect_line;owner.reward_input.difficulty=difficulty;owner.reward_input.bomb_triggered=p.status().bomb.triggered;owner.reward_input.bomb_active=p.status().bomb.active;owner.reward_input.focused=p.status().motion.form.focused;owner.reward_input.time_spell=c.time_spell;}
    updater.update();
    for(u32 seat=0;seat<3;++seat){auto& owner=owners[seat];if(!owner.player)continue;auto& p=*owner.player;auto& c=p.status().context;p.status().item_gauge_lock=owner.input.gauge_lock;c.replay_flags=owner.input.replay_flags;synchronize(owner);}
    return !failed;
}
bool ItemSystem::draw(const Vec2& offset){if(failed)return false;pool.draw(offset);return !failed;}
void ItemSystem::collect_all(){for(auto* item=state->head.next;item;item=item->next){const u32 seat=owner_for(*item);if(seat<3){item_owners[item_index(*item)]=u8(seat);item->state=1;item->velocity={0,-.5f,0};}}}
void ItemSystem::collect_all(u32 seat){if(seat>=3||!owners[seat].available)return;for(auto* item=state->head.next;item;item=item->next)if(owner_for(*item)==seat){item_owners[item_index(*item)]=u8(seat);item->state=1;item->velocity={0,-.5f,0};}}
void ItemSystem::cancel_homing(){for(auto* item=state->head.next;item;item=item->next)if(item->state==1){item->state=0;item->velocity={0,-.9f,0};}}
void ItemSystem::cancel_homing(u32 seat){
    if(seat>=3)return;
    for(auto* item=state->head.next;item;item=item->next)if(item->state==1&&owner_for(*item)==seat){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
        multiplayer::diagnostic::Scope trace("item.cancel_homing",int(seat),0,item);
#endif
        item->state=0;item->velocity={0,-.9f,0};
    }
}
void ItemSystem::time_orb(){time_orb(0);}
void ItemSystem::time_orb(u32 seat){if(seat>=3||!owners[seat].player||!owners[seat].rewards)return;auto& owner=owners[seat];auto& p=*owner.player;auto& c=p.status().context;auto& context=owner.reward_input;context.hud_flags=c.hud_flags;context.bomb_triggered=p.status().bomb.triggered;context.bomb_active=p.status().bomb.active;context.focused=p.status().motion.form.focused;context.gauge_lock=p.status().item_gauge_lock;owner.rewards->time_orb(nullptr);failed|=owner.rewards->failed;c.hud_flags=context.hud_flags;synchronize(owner);}
void ItemSystem::reset(){
    if(pool.rollback_journal&&(pool.rollback_journal->IsFrameOpen()||pool.rollback_journal->FrameCount())){failed=true;return;}
    state->reset();item_owners.fill(no_owner);gift_recipients.fill(no_owner);failed=false;pool.rollback_failed=false;
    for(auto& owner:owners)if(owner.rewards)owner.rewards->failed=false;
}

#endif
}
