#pragma once
#include "player.hpp"
namespace th20::source::player_entity {
class EventServices {
public:
    virtual ~EventServices()=default;
    virtual game_session::Session& session()=0;
    virtual void mark_enemies()=0;                           //478190, global player0
    virtual void notify_secondary(void*)=0;                  //487bb0
    virtual void sound(int)=0;                               //426d70(id,0)
    virtual void sound_at(int,float)=0;                      //426eb0
    virtual void spawn_hit_effect(game_session::Context&,const sprite::Vec3&)=0; //4790e0, effect script22
    virtual void reset_player_animation(void*)=0;             //4382b0(file+1c,animation+28,0,null)
    virtual std::uint32_t random_next()=0;                    //423ee0 stream0
    virtual void enqueue_graze(game_session::Context&,const sprite::Vec3&,std::uint32_t color,int delay)=0; //49dc60 type4
    virtual bool special_active()=0;                         //513dd0->485a40
    virtual bool selected_enemy_present()=0;                 //49be20, any nonzero slot0..15
    virtual void accumulate_reward(void* overlay,const sprite::Vec3&,int,int)=0; //4a9f80
    virtual void add_special_items(game_session::Player&,int)=0; //4a9fb0
};
void add_graze_count(game_session::Player&,int) noexcept;      //4fe8d0 +e4
void hit(void* player,EventServices&);                        //4f86f0
void graze(void* player,const sprite::Vec3&,std::uint32_t,EventServices&); //4f8b90
EventServices& event_services();
inline void hit(void* player){hit(player,event_services());}
inline void graze(void* player,const sprite::Vec3& p,std::uint32_t color){graze(player,p,color,event_services());}
}
