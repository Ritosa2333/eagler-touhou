#pragma once
#include "ItemRewards.hpp"
#include "ItemUpdate.hpp"
#include "PlayerSimulation.hpp"
#include "AnmLibrary.hpp"
#include "AnmRenderer.hpp"
#include "GuiState.hpp"
#include <memory>
namespace th08 {
struct ItemSystemActions:ItemRewardActions {
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
};
// Resource-backed owner of the original item phases. It shares the player's
// collision geometry and values rather than maintaining another game state.
class ItemSystem:private ItemPoolActions,private ItemUpdateActions,private ItemRewardActions {
    PlayerSimulation& player;GameGlobals& globals;AnmLibrary& animations;AnmExecutor& executor;AnmRenderer& renderer;ItemSystemActions& actions;GameRank& rank;
    std::unique_ptr<ItemPoolState> state=std::make_unique<ItemPoolState>();ItemPool pool;
    ItemUpdateContext input;ItemRewardContext reward_input;ItemRewards rewards;ItemUpdate updater;bool failed=false;
    GuiState* hud=nullptr;
    void read_hud(){if(hud)std::memcpy(&player.status().context.hud_flags,&hud->flags,4);}
    void write_hud(){if(hud)std::memcpy(&hud->flags,&player.status().context.hud_flags,4);}
    void synchronize();
    void animation(AnmVm& vm,i32 script)override{failed|=!animations.start(6,script,vm,executor);}
    void sprite(AnmVm& vm,i32 index)override{auto* file=animations.get(6);failed|=!file;if(file)failed|=file->SetSprite(&vm,index)!=0;}
    void effect(i32 kind,const Vec3& p,i32 count,u32 color)override{actions.effect(kind,p,count,color);}
    void draw(AnmVm& vm)override{renderer.draw_2d(vm);}
    bool touching(const Vec3& p,const Vec3& size)override{return player.collision().item(p,size);}
    void collect(ItemState&)override;
    void animation_step(AnmVm& vm)override{executor.execute(vm);failed|=executor.invalid;}
    void sound(i32 index,i32 mode)override{actions.sound(index,mode);}
    void subtract_rank(i32 value)override{rank.subtract(value);}
    void popup(const Vec3& p,i32 value,u32 color,bool small)override{actions.popup(p,value,color,small);}
    void gui_popup(i32 value,i32 kind)override{actions.gui_popup(value,kind);}
    void cancel_bullets()override{actions.cancel_bullets();}
    void spell_time(i32 value)override{actions.spell_time(value);}
public:
    i32 difficulty=0;
    void bind_hud(GuiState& value){hud=&value;}
    ItemSystem(PlayerSimulation&,GameGlobals&,GameValues&,GameGauge&,GameRank&,HighScore&,Rng&,AnmLibrary&,AnmExecutor&,AnmRenderer&,ItemSystemActions&);
    ItemState* spawn(const Vec3&,i32 type,i32 mode);
    bool update();
    bool draw(const Vec2& offset);
    void collect_all(){pool.collect_all();}
    void cancel_homing(){pool.cancel_homing();}
    void time_orb();
    void reset(){state->reset();failed=false;rewards.failed=false;}
    bool invalid()const noexcept{return failed;}
    const ItemPoolState& status()const noexcept{return *state;}
    i32 time_orb_count()const{return pool.time_orb_count();}
};
}
