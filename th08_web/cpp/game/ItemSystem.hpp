#pragma once
#include "ItemRewards.hpp"
#include "ItemUpdate.hpp"
#include "PlayerSimulation.hpp"
#include "AnmLibrary.hpp"
#include "AnmRenderer.hpp"
#include "GuiState.hpp"
#include <memory>
#include <array>
#include <optional>
namespace th08 {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace multiplayer {class PoolsJournal;}
#endif
struct ItemSystemActions:ItemRewardActions {
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
};
// Resource-backed owner of the original item phases. It shares the player's
// collision geometry and values rather than maintaining another game state.
class ItemSystem:private ItemPoolActions,private ItemUpdateActions,private ItemRewardActions {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    friend class multiplayer::PoolsJournal;
#endif
    PlayerSimulation& player;GameGlobals& globals;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    using ResourceView=PlayerResourceView;
    using ValueStore=PlayerValues;
#else
    using ResourceView=GameGlobals;
    using ValueStore=GameValues;
#endif
    AnmLibrary& animations;AnmExecutor& executor;AnmRenderer& renderer;ItemSystemActions& actions;GameRank& rank;HighScore& high_score;
    std::unique_ptr<ItemPoolState> state=std::make_unique<ItemPoolState>();ItemPool pool;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    u32 participant_count=1;
    struct Owner {
        PlayerSimulation* player=nullptr;ResourceView* resources=nullptr;ValueStore* values=nullptr;GameGauge* gauge=nullptr;ItemSystemActions* actions=nullptr;
        ItemUpdateContext input;ItemRewardContext reward_input;std::optional<ItemRewards> rewards;bool available=false;
    } owners[3];
    std::array<u8,ItemPoolState::capacity+1> item_owners{};
    // A directed donation is not just a transient homing assignment. Native
    // cancel/collect operations may change motion, not the promised recipient.
    std::array<u8,ItemPoolState::capacity+1> gift_recipients{};
    static constexpr u8 no_owner=3;
#else
    ItemUpdateContext input;ItemRewardContext reward_input;ItemRewards rewards;
#endif
    ItemUpdate updater;bool failed=false;
    GuiState* hud=nullptr;
    void read_hud(){if(hud)std::memcpy(&player.status().context.hud_flags,&hud->flags,4);}
    void write_hud(){if(hud)std::memcpy(&hud->flags,&player.status().context.hud_flags,4);}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    void synchronize(Owner&);
    u32 nearest_owner(const Vec3&)const noexcept;
    u32 item_index(const ItemState&)const noexcept;
#else
    void synchronize();
#endif
    void animation(AnmVm& vm,i32 script)override{failed|=!animations.start(6,script,vm,executor);}
    void sprite(AnmVm& vm,i32 index)override{auto* file=animations.get(6);failed|=!file;if(file)failed|=file->SetSprite(&vm,index)!=0;}
    void effect(i32 kind,const Vec3& p,i32 count,u32 color)override{actions.effect(kind,p,count,color);}
    void draw(AnmVm& vm)override{renderer.draw_2d(vm);}
    bool touching(u32 seat,const Vec3& p,const Vec3& size)override;
    void collect(u32 seat,ItemState&)override;
    void item_sound(u32 seat,i32 index,i32 mode)override;
    void removed(ItemState&)override;
    u32 owner_for(ItemState&)override;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    u32 auto_collect_owner(ItemState&)override;
#endif
    void animation_step(AnmVm& vm)override{executor.execute(vm);failed|=executor.invalid;}
    void sound(i32 index,i32 mode)override{actions.sound(index,mode);}
    void subtract_rank(i32 value)override{rank.subtract(value);}
    void popup(const Vec3& p,i32 value,u32 color,bool small)override{actions.popup(p,value,color,small);}
    void gui_popup(i32 value,i32 kind)override{actions.gui_popup(value,kind);}
    void cancel_bullets()override{actions.cancel_bullets();}
    void spell_time(i32 value)override{actions.spell_time(value);}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    void team_extend()override{award_team_extend();}
    void convert_team_power(ItemState& collected)override{convert_power_items(collected);}
#endif
public:
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    u32 diagnostic_owner(u32 slot)const{return slot<item_owners.size()?item_owners[slot]:3;}
    u32 diagnostic_recipient(u32 slot)const{return slot<gift_recipients.size()?gift_recipients[slot]:3;}
#endif
    i32 difficulty=0;
    void bind_hud(GuiState& value){hud=&value;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ItemSystem(PlayerSimulation&,PlayerResourceView&,PlayerValues&,GameGauge&,GameRank&,HighScore&,Rng&,AnmLibrary&,AnmExecutor&,AnmRenderer&,ItemSystemActions&);
    void bind_player(u32 seat,PlayerSimulation&,PlayerResourceView&,PlayerValues&,GameGauge&,ItemSystemActions&);
    void set_player_count(u32 count){participant_count=count;}
    void set_player_available(u32 seat,bool available);
    void award_team_extend();
    void convert_power_items(ItemState& collected);
    bool spawn_for_player(const Vec3& position,i32 type,i32 mode,u32 seat);
    bool spawn_power_gift(const Vec3& position,u32 seat);
    u32 assigned_gifts(u32 seat)const noexcept{
        if(seat>=3)return 0;u32 count=0;
        for(u32 i=0;i<ItemPoolState::capacity;++i)
            count+=state->items[i].active&&gift_recipients[i]==seat?1u:0u;
        return count;
    }
#else
    ItemSystem(PlayerSimulation&,GameGlobals&,GameValues&,GameGauge&,GameRank&,HighScore&,Rng&,AnmLibrary&,AnmExecutor&,AnmRenderer&,ItemSystemActions&);
#endif
    ItemState* spawn(const Vec3&,i32 type,i32 mode);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ItemState* spawn_single(const Vec3&,i32 type,i32 mode);
#endif
    bool update();
    bool draw(const Vec2& offset);
    void collect_all();
    void collect_all(u32 seat);
    void cancel_homing();
    void cancel_homing(u32 seat);
    void time_orb();
    void time_orb(u32 seat);
    void reset();
    bool invalid()const noexcept{return failed
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        ||pool.rollback_failed
#endif
        ;}
    const ItemPoolState& status()const noexcept{return *state;}
    i32 time_orb_count()const{return pool.time_orb_count();}
};
}
