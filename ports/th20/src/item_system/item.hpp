#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_state/state.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
namespace th20::source::item {
struct Item {
    scheduler::Link link;
    scheduler::List* free_list; //14
    sprite::Animation animation,secondary_animation; //18,5fc
    std::uint32_t attachment;
    sprite::Vec3 position,velocity;
    float speed,angle;
    recovered::Timer timer,secondary_timer;
    std::int32_t state,type,draw_state;
    float attraction_speed;
    std::int32_t delay,generation;
    std::uint32_t extra;
    std::int32_t sound,view_index;
    game_session::Context* context;
};
static_assert(offsetof(Item,position)==0xbe4&&offsetof(Item,timer)==0xc04&&offsetof(Item,state)==0xc24&&sizeof(Item)==0xc4c);
class ItemInf final:public runtime::CallbackOwner {
public:
    std::uint32_t field_10;
    scheduler::Node* second_draw_node;
    Item pool[1536];
    scheduler::List active,ordinary_free,special_free;
    float speed_scale;
    std::int32_t processed,spawn_counter,point_counter,special_count,generation,bonus_counter,field_49c87c,field_49c880,attract;
    sprite::Vec3 attraction_center;
    std::int32_t view_index;
    game_session::Context* context;
    ItemInf(); //4c22e0
    ~ItemInf() override; //4c24c0
    void enable_callbacks() override; //4c4650
    void select_context(std::int32_t) noexcept; //4c4fd0
    void initialize_pool(); //4bc220
    int initialize(std::int32_t); //4c3c00
};
static_assert(offsetof(ItemInf,pool)==0x18&&offsetof(ItemInf,active)==0x49c818&&offsetof(ItemInf,speed_scale)==0x49c860&&sizeof(ItemInf)==0x49c89c);
class Environment {
public:
    virtual ~Environment()=default;
    virtual void bind_animation(ItemInf&,sprite::Animation&,int script)=0; //438380
    virtual void bind_special_animation(sprite::Animation&,int script)=0; //438380, file from51b960+10
    virtual void spawn_effect(Item&)=0; //4c42c0
    virtual void bonus_notification()=0; //4859d0->4865b0
    virtual void select_view(ItemInf&)=0; //4776a0
    virtual void configure_layer(ItemInf&,int)=0; //44f3d0
    virtual bool boss_collecting()=0; //478160
    virtual void activate_special(Item&)=0; //4c4420
    virtual void collect(Item&)=0; //pickup switch in4c25a0
    virtual void collect_sound(const Item&)=0; //426eb0
    virtual void update_animation(sprite::Animation&)=0; //42b5d0
    virtual void draw_animation(sprite::Animation&)=0; //44c570
    virtual void move_attachment(std::uint32_t&,const sprite::Vec3&)=0; //4502c0
    virtual void retire_attachment(std::uint32_t&)=0; //44fcd0
};
Environment& environment();
void construct_item(Item&); //4c21b0
void destroy_item(Item&); //4c2490
void select_context(Item&,std::int32_t) noexcept; //4c4f90
ItemInf* controller(std::int32_t index=0) noexcept; //498fd0->41cab0
ItemInf* create_controller(std::int32_t index=0); //4c5010->4c2130
Item* spawn(ItemInf&,int type,const sprite::Vec3&,std::uint32_t color,float angle,float speed,int delay,std::uint32_t extra,int sound,Environment&); //4c3c90
inline Item* spawn(ItemInf& owner,int type,const sprite::Vec3& position,std::uint32_t color,float angle,float speed,int delay,std::uint32_t extra,int sound){return spawn(owner,type,position,color,angle,speed,delay,extra,sound,environment());}
void spawn_many(ItemInf&,const sprite::Vec3&,int count,int type,Environment&); //4c45b0
inline void spawn_many(ItemInf& owner,const sprite::Vec3& position,int count,int type){spawn_many(owner,position,count,type,environment());}
void retire(Item&,Environment&); //4c3880
inline void retire(Item& item){retire(item,environment());}
int update(ItemInf&,Environment&); //4c25a0
inline int update(ItemInf& owner){return update(owner,environment());}
int draw(ItemInf&,int layer,Environment&); //4c38d0
inline int draw(ItemInf& owner,int layer){return draw(owner,layer,environment());}
int update_callback(ItemInf&); //4c46f0
int draw_callback(ItemInf&,int); //4c4740/4c4770
}
