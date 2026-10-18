#include "Item.hpp"
#include "GameMath.hpp"
namespace th10 {
// The original rectangle comparisons treat unordered coordinates as inside.
bool ItemRegion::contains(const Vec3& point) const noexcept {
    return !(number(point.x)<number(minimum.x)||number(point.y)<number(minimum.y)||
             number(maximum.x)<number(point.x)||number(maximum.y)<number(point.y));
}
namespace {
void advance_position(Item& item,float rate){
    const auto x=number(rate)*number(item.velocity.x),y=number(rate)*number(item.velocity.y);
    const float z=Scalar::mul(rate,item.velocity.z);
    item.position.x=(x+number(item.position.x)).to_float();item.position.y=(y+number(item.position.y)).to_float();
    item.position.z=Scalar::add(z,item.position.z);
}
void attract(Item& item,ItemFrameEnvironment& env){
    const auto x=number(env.player_position->x)-number(item.position.x),y=number(env.player_position->y)-number(item.position.y);
    const float angle=x==number(0.0f)&&y==number(0.0f)?1.5707963705062866f:angle_to(y,x).to_float();
    const auto velocity=polar(angle,item.attraction_speed);item.velocity.x=velocity.x;item.velocity.y=velocity.y;
    advance_position(item,*env.default_rate);
    if(number(item.attraction_speed)<number(12.0f))item.attraction_speed=Scalar::add(item.attraction_speed,.2f);
    if(*env.player_state==4){item.state=1;item.velocity.x=item.velocity.y=0;}
}
bool collect(Item& item,ItemFrameEnvironment& env){
    auto& economy=*env.economy;bool full_power=false;
    switch(item.kind){
    case 1:case 10:{
        const bool power_up=economy.add_power(1,env);env.update_power_display(economy.power/20,(economy.power%20)*100/20);
        if(power_up){
            env.refresh_player_power();env.popup(item.position,-1,0xffffff40);env.play_sound(0x1d,item.position.x);
            if(economy.power>=100)full_power=true;
            economy.add_rank(12);
        }else env.popup(item.position,economy.power/2,0xffff4040);
        economy.extend_faith_timer(60,env.default_rate);break;
    }
    case 2:case 5:{
        const auto total=static_cast<i32>(static_cast<u32>(economy.item_value)*10u);i32 points=total,rank=8;u32 color=0xffffff00;
        if(item.kind==2){
            if(number(env.player_position->y)<number(144.0f))points=wrapping_add(points,-(points%10));
            else{
                const auto factor=(number(env.player_position->y)-number(144.0f))*number(.003289473708719015f);
                const auto reduction=(factor*(Extended::from_int(economy.item_value)*number(.5f)-number(5000.0f))).truncate_int();
                points=wrapping_add(total/2,static_cast<i32>(0u-static_cast<u32>(reduction)));points=wrapping_add(points,-(points%10));
                color=0xffffffff;rank=1;
            }
        }
#ifdef TH_ENABLE_THPRAC
        env.point_collected(color==0xffffff00);
#endif
        env.popup(item.position,points,color);economy.add_rank(rank);economy.add_score(points);economy.extend_faith_timer(100,env.default_rate);break;
    }
    case 3:{
        const i32 value=economy.difficulty==2?8000:economy.difficulty==3||economy.difficulty==4?10000:5000;
        env.popup(item.position,value,0xff00ff00);economy.add_item_value(value);economy.extend_faith_timer(120,env.default_rate);break;
    }
    case 4:case 11:{
        const bool power_up=economy.add_power(20,env);env.update_power_display(economy.power/20,(economy.power%20)*100/20);
        if(power_up){
            env.refresh_player_power();env.play_sound(0x1d,item.position.x);env.popup(item.position,-1,0xffffff40);economy.add_rank(24);
            if(economy.power>=100)full_power=true;
        }else env.popup(item.position,economy.power/2,0xffff4040);
        economy.extend_faith_timer(20,env.default_rate);break;
    }
    case 7:economy.add_lives(1,env);economy.add_rank(256);break;
    case 8:economy.add_item_value(10);economy.extend_faith_timer(3,env.default_rate);economy.add_score(10);break;
    case 9:env.popup(item.position,100,0xff00ff00);economy.add_item_value(100);economy.extend_faith_timer(60,env.default_rate);break;
    }
    item.state=0;env.play_sound(0x14,item.position.x);return full_power;
}
}
// 0x41afd0: one item within the manager's ordered regular/faith pool traversal.
ItemUpdate Item::update(ItemFrameEnvironment& env){
    if(state==0)return ItemUpdate::Skipped;
    if(state==5){
        value_variant=wrapping_add(value_variant,-1);
        if(value_variant<0){state=2;env.initialize_animation(*this,wrapping_add(kind,0x176));}
        return ItemUpdate::Skipped;
    }
    if(state==1){
        if(((*env.player_state!=2&&*env.player_state!=4)&&number(env.player_position->y)<number(128.0f))||*env.auto_collect){
            attraction_speed=*env.player_attraction_speed;state=3;attract(*this,env);
        }else{
            advance_position(*this,*env.default_rate);
            const auto vertical=number(*env.default_rate)*number(.03f)+number(velocity.y);velocity.y=vertical.to_float();
            if(number(0.0f)<vertical||number(0.0f)==vertical)velocity.x=0;
            if(number(2.0f)<number(velocity.y))velocity.y=2;
            if(number(472.0f)<number(position.y)){state=0;return ItemUpdate::Skipped;}
        }
    }else if(state==2){
        advance_position(*this,*env.default_rate);
        const auto vertical=number(*env.default_rate)*number(.03f)+number(velocity.y);velocity.y=vertical.to_float();
        if(number(0.0f)<vertical||number(0.0f)==vertical){attraction_speed=*env.player_attraction_speed;state=3;attract(*this,env);}
        else if(number(472.0f)<number(position.y)){state=0;env.economy->add_rank(-4);return ItemUpdate::Skipped;}
    }else if(state==3||state==4)attract(*this,env);
    if(*env.player_state!=2){
        if(env.pickup_region->contains(position))return collect(*this,env)?ItemUpdate::FullPower:ItemUpdate::Skipped;
        if(state!=3&&state!=4&&((*env.input_keys&4)?env.slow_region:env.fast_region)->contains(position)){
            state=4;attraction_speed=Scalar::mul(*env.player_attraction_speed,.3333333432674408f);
        }
    }
    animation.update(*env.animations);timer.tick();return ItemUpdate::Active;
}
i32 ItemManager::update(ItemFrameEnvironment& env){
    faith_count=active_count=0;bool full_power=false;
    auto update_item=[&](Item& item){const auto result=item.update(env);if(result==ItemUpdate::FullPower)full_power=true;else if(result==ItemUpdate::Active)++active_count;};
    for(auto& item:regular)update_item(item);
    for(auto& item:faith)update_item(item);
    if(full_power)convert_power(env);return 1;
}
// 0x41ba50. Only kinds 1 and 4 pass the original outer condition.
i32 ItemManager::convert_power(ItemEnvironment& env){
    for(auto& item:regular)if(item.state&&(item.kind==1||item.kind==4)){
        item.state=0;spawn(item.position,9,0xffffffff,-1.5707963705062866f,2.2f,env);env.spawn_effect(item.position,0x189);
    }
    return 0;
}
}
