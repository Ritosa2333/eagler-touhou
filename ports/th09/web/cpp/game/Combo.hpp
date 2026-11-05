#pragma once
#include "EclVariables.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct ComboState {
    i32 hits=0,best_hits=0,score=0,display_score=0;
    Timer display_time{0,0,0},chain_time{0,0,0};
    i32 normal_attack=0,spirit_attack=0,character_attack=0,kills=0,pending=0;
    void reset_chain()noexcept{chain_time.reset();hits=0;kills=0;}
};
struct ComboActions {
    virtual ~ComboActions()=default;
    virtual bool rewards_blocked()const=0;
    virtual bool opponent_has_boss()const=0;
    virtual i32 opposing_spirits()const=0;
    virtual void attack(i32 type,i32 level)=0;
    virtual void add_score(i32 points)=0;
    virtual void score_popup(const Vec3&,i32 score,u32 color)=0;
    virtual void character_meter(ComboState&,const Vec3&)=0;
    virtual TransferParameters* create_transfer(i32 effect,const Vec3&,const Vec3& control)=0;
};
class Combo {
    EclWorldState& world;ComboActions& actions;FrameTiming timing;PlayfieldGeometry geometry;float opposing_width; i32 side;
    TransferParameters* transfer(const Vec3&,bool spirit);
public:
    Combo(EclWorldState& w,ComboActions& a,const FrameTiming& t,const PlayfieldGeometry& g,float width,i32 s):world(w),actions(a),timing(t),geometry(g),opposing_width(width),side(s){}
    bool add(ComboState&,const Vec3&,i32 normal,i32 spirit,i32 character,i32 score);
    bool kill(ComboState& state,const Vec3& p,i32 normal,i32 spirit,i32 character,i32 score){state.kills=wrapping_add(state.kills,1);return add(state,p,normal,spirit,character,score);}
    void flush(ComboState& state){state.reset_chain();actions.add_score(state.display_score);state.display_score=state.score=0;state.display_time.reset();}
    void update_timers(ComboState& state){
        if(state.chain_time.current>0){state.pending=0;state.chain_time.decrement(1,timing);if(state.chain_time.current<=0)state.reset_chain();}
        if(state.display_time.current>0){state.display_time.decrement(1,timing);if(state.display_time.current<=0){actions.add_score(state.display_score);state.display_score=state.score=0;state.display_time.reset();}}
    }
};
}
