#include "shot_callbacks.hpp"
#include "../damage_regions/regions.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
#include <cmath>
#include <cstring>
#include <stdexcept>
namespace th20::source::player_entity {
namespace {
constexpr float pi=0x1.921fb6p+1f;
const ShotRecord& record(Shot& shot){return shot_record(*static_cast<Player*>(shot.context->objects_04[0]),shot.fields_b8[8]);}
float word_float(std::uint32_t word){return std::bit_cast<float>(word);}
float point_angle(sprite::Vec3 origin,sprite::Vec3 target){return ecl::math::arctangent(target.y-origin.y,target.x-origin.x);}
void set_angle(Shot& shot,float value){shot.motion.angle_1c=ecl::math::wrap_angle(value);}
void add_angle(Shot& shot,float value){set_angle(shot,ecl::math::wrap_angle(shot.motion.angle_1c+value));}
void laser_start(Shot& shot,int period,ShotCallbackEnvironment& env){
    shot.vector_b0.x=0;auto& host=env.firing();host.sound_at(20,static_cast<Player*>(shot.context->objects_04[0])->position_614.x);
    if(auto* region=host.damage(shot.fields_b8[9])){region->period=period;region->size.x=0;}shot.fields_3c[0]&=~1u;
}
bool contains_enemy(scheduler::List& list,std::uint32_t id){
    for(scheduler::Iterator iterator(list.sentinel.next);iterator.current;iterator.advance()){std::uint32_t value;std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(iterator.current->value)+0x88,4);if(value==id)return true;}return false;
}
int update_laser(Shot& shot,unsigned index,ShotCallbackEnvironment& env){ //501f50/502550
    auto& host=env.firing();auto& player=*static_cast<Player*>(shot.context->objects_04[0]);const auto& row=record(shot);
    auto* option=reinterpret_cast<Option*>(shot.field_110);
    sprite::Vec3 center=option?sprite::Vec3{float(option->vector_78.x)/128.0f,float(option->vector_78.y)/128.0f,0}:player.position_614;
    shot.motion.position=center;shot.motion.position.x+=row.offset.x;shot.motion.position.y+=row.offset.y;shot.motion.position.z+=0.0f;
    if(index==5)center=shot.motion.position;
    const float angle=shot.motion.angle_1c,desired=ecl::math::wrap_angle(row.angle);
    if(std::fabs(ecl::math::wrap_angle(ecl::math::angle_difference(angle,desired)))<=0.001f)set_angle(shot,desired);
    else{const float delta=ecl::math::wrap_angle(ecl::math::angle_difference(desired,angle));const float step=ecl::math::wrap_angle(delta*(index==4?0.4f:0.1f));set_angle(shot,ecl::math::wrap_angle(step+angle));}
    if(shot.fields_98[1]==2)return 0;
    env.sound_pan(20,player.position_614.x);
    if(shot.vector_b0.x<512.0f){shot.vector_b0.x+=18.0f;if(auto* region=host.damage(shot.fields_b8[9]))region->size.x=shot.vector_b0.x;}
    float dx,dy;ecl::math::polar(dx,dy,shot.motion.angle_1c,shot.vector_b0.x/2.0f);center.x+=dx;center.y+=dy;center.z=0;
    if(auto* animation=env.find_animation(shot.handle_18)){animation->base.vector_70.x=shot.vector_b0.x;animation->base.flags[1]|=4;animation->base.vector_68.x=shot.vector_b0.x/512.0f;animation->base.flags[1]|=8;}
    if(auto* region=host.damage(shot.fields_b8[9])){region->motion.position=center;if(index==4){region->damage=row.damage;region->motion.angle_1c=ecl::math::wrap_angle(shot.motion.angle_1c);region->angle=ecl::math::wrap_angle(shot.motion.angle_1c);}}
    if(!shot.fields_98[3]&&shot.fields_98[1]==1&&shot.fields_98[4]==1){env.interrupt(shot.handle_18,3);shot.fields_98[4]=0;}
    if(shot.fields_98[1]==1){
        const int duration=std::bit_cast<int>(row.fields_3c[0]);bool keep=duration!=0||shot.owner->timer_12400.current>=0;
        if(index==4)keep=keep&&player.state!=2&&player.state!=4&&!(shot.owner->field_1255c&1u);
        keep=keep&&!env.dialogue_active()&&shot.context->objects_04[1];
        if(index==4){
            if(keep&&duration>0&&shot.timer_1c.current>=duration)keep=false;
            if(keep&&option){const int pattern=env.weapon_pattern();const auto option_index=option->fields_f4[1]+1u;const auto& global_player=*static_cast<Player*>(host.session().contexts[0].objects_04[0]);keep=pattern_has_laser(global_player.shot_data,option_index,pattern);}
            if(keep)keep=!env.weapon_shooting();
        }else if(keep)keep=env.weapon_phase()!=0;
        if(!keep){if(auto* region=host.damage(shot.fields_b8[9]))env.retire_damage(*region);shot.fields_98[1]=2;env.interrupt(shot.handle_18,1);shot.owner->counters_12468[row.group]=0;env.stop_sound(20);}
    }
    shot.fields_98[3]=0;return 0;
}
}
bool pattern_has_laser(const void* data,std::uint32_t option,int pattern){
    const ShotRecord* row;std::memcpy(&row,static_cast<const std::uint8_t*>(data)+0x5d4+pattern*4,4);
    for(;row->period>=0;++row)if((static_cast<std::uint32_t>(row->source)&15u)==option&&row->type==2)return true;return false;
}
float shot_random_angle(std::uint32_t value,std::uint32_t modulus) noexcept{
    const auto numerator=_mm_set_ss(float(double(value)));const auto limit=_mm_set_ss(float(double(modulus)));
    const auto denominator=_mm_sub_ss(_mm_div_ss(limit,_mm_set_ss(pi*2.0f)),_mm_set_ss(pi));
    return _mm_cvtss_f32(_mm_sub_ss(_mm_div_ss(numerator,denominator),_mm_set_ss(pi)));
}
int initialize_shot_callback(Shot& shot,unsigned index,int frame,ShotCallbackEnvironment& env){
    auto& host=env.firing();
    switch(index){
    case 1:shot.fields_98[2]=0;return 0;                    //500270
    case 2:{                                             //500c50
        const auto& row=record(shot);const int time=static_cast<Player*>(host.session().contexts[0].objects_04[0])->shots.timer_12580.current;
        const auto phase=ecl::math::wrap_angle(((pi*2.0f)/120.0f)*float(time%120));
        float amount=ecl::math::wrap_angle(ecl::math::sine(phase)*word_float(row.fields_20[1]));
        if(row.angle<(-pi/2.0f-0.001f))amount=ecl::math::wrap_angle(amount*-1.0f);
        else if(row.angle<(-pi/2.0f+0.001f))return 0;
        amount=ecl::math::wrap_angle(amount+row.angle);set_angle(shot,amount);return 0;
    }
    case 3:case 6:{                                       //500290/500790
        auto& handle=shot.fields_98[2];handle=env.nearest_enemy(*shot.context,{shot.motion.position.x,shot.motion.position.y},index==3?400.0f:600.0f);
        if(!handle)set_angle(shot,-pi/2.0f);
        else{auto* enemy=env.find_enemy(handle);const auto target=env.enemy_position(enemy);
            if(index==3)set_angle(shot,point_angle(shot.motion.position,target));
            else{const float random=host.signed_random();const float offset=((random*pi)*5.0f)/180.0f;set_angle(shot,point_angle(shot.motion.position,target)+offset);}}
        return 0;
    }
    case 4:case 12:{                                      //500380/5009f0
        float closest=999.0f;auto& list=env.enemies(*shot.context);
        for(scheduler::Iterator iterator(list.sentinel.next);iterator.current;iterator.advance()){
            auto* enemy=iterator.current->value;if(env.excluded_enemy(enemy))continue;const auto target=env.enemy_position(enemy);
            if(index==4){if(target.y>=shot.motion.position.y-24.0f){const float distance=std::fabs(target.x-shot.motion.position.x);if(distance<closest){closest=distance;set_angle(shot,point_angle(shot.motion.position,target));}}}
            else{const float distance=target.y-shot.motion.position.y;if(distance<closest){closest=distance;const auto& row=record(shot);const float random=host.signed_random();set_angle(shot,point_angle(shot.motion.position,target)+random*word_float(row.fields_58[4]));}}
        }
        return 0;
    }
    case 5:add_angle(shot,((host.signed_random()*pi)*5.0f)/180.0f);return 0; //5006f0
    case 7:shot.fields_98[2]=0;shot.fields_3c[0]&=~0x3cu;return 0; //500ec0
    case 8:case 9:laser_start(shot,1,env);return 0;           //5005d0
    case 10:add_angle(shot,((host.signed_random()*pi)*2.0f)/90.0f);return 0; //5008c0
    case 11:{const auto& row=record(shot);add_angle(shot,host.signed_random()*word_float(row.fields_58[4]));return 0;} //500950
    case 13:case 18:return 0;                              //414b50 verified original empty callback
    case 14:{                                             //500dd0
        const auto& row=record(shot);const auto& position=reinterpret_cast<Option*>(shot.field_110)->vector_78;
        shot.motion.position={float(position.x)/128.0f,float(position.y)/128.0f,0};
        const float base=word_float(shot.owner->handle_12560),value=word_float(shot.owner->field_12564);
        set_angle(shot,(base+row.angle)+value*word_float(row.fields_58[4]));return 0;
    }
    case 15:{                                             //500ef0
        auto handle=env.nearest_enemy(*shot.context,{shot.motion.position.x,shot.motion.position.y},128.0f);
        if(!handle)set_angle(shot,-pi/2.0f);else set_angle(shot,point_angle(shot.motion.position,env.enemy_position(env.find_enemy(handle))));return 0;
    }
    case 16:case 23:case 31:{const float random=env.random_angle();set_angle(shot,recovered::add32(recovered::mul32(random,index==16?0.5f:index==23?0.2f:0.25f),-pi/2.0f));return 0;} //500fe0/501040/5017c0
    case 17:{                                             //501110, angle away from the player
        const auto& player=*static_cast<Player*>(shot.context->objects_04[0]);const float x=shot.motion.position.x-player.position_614.x,y=shot.motion.position.y-player.position_614.y;
        const float base=(x==0&&y==0)?pi/2.0f:ecl::math::arctangent(y,x);const float random=host.signed_random();
        set_angle(shot,base+(random*(pi/180.0f))*2.0f);shot.motion.field_18=16.0f;return 0;
    }
    case 19:shot.motion.position.y=480.0f;shot.motion.position.x+=host.signed_random()*32.0f;return 0; //5011d0
    case 20:case 22:{                                      //501240/5013b0
        auto& handle=shot.fields_98[2];if(!shot.context->objects_04[1]){handle=0;return 0;}
        if(!handle)handle=env.nearest_enemy(*shot.context,{shot.motion.position.x,shot.motion.position.y},640.0f);
        if(handle){if(!contains_enemy(env.enemies(*shot.context),handle))handle=0;
            else{auto* enemy=env.find_enemy(handle);if(env.excluded_enemy(enemy))handle=0;
                else{const auto target=env.enemy_position(enemy);if(index==20)set_angle(shot,point_angle(shot.motion.position,target));
                    else{const float random=host.signed_random();const float offset=((random*pi)*5.0f)/90.0f;set_angle(shot,point_angle(shot.motion.position,target)+offset);}}}}
        return 0;
    }
    case 21:case 28:case 30:shot.byte_114=1;return 0;        //501580
    case 24:{shot.motion.position.x+=host.signed_random()*32.0f;if(auto* visual=env.find_animation(shot.handle_18))visual->base.vector_50={2,2};return 0;} //5010a0
    case 25:{shot.byte_114=1;const float random=host.signed_random();set_angle(shot,recovered::add32(recovered::mul32(random,pi/18.0f),-pi/2.0f));shot.fields_b8[0]=shot.fields_b8[1]=0;return 0;} //5015a0
    case 26:{                                             //501650
        shot.byte_114=1;shot.fields_3c[0]&=~1u;const auto& player=*static_cast<Player*>(shot.context->objects_04[0]);const float x=player.vector_20c4.x;
        const float direction=x<-.1f?-(pi/180.0f)*6.0f-pi/2.0f:x<=.1f?-pi/2.0f:(pi/180.0f)*6.0f-pi/2.0f;
        set_angle(shot,direction);return 0;
    }
    case 27:laser_start(shot,4,env);return 0;                //500660
    case 29:set_angle(shot,shot.motion.angle_1c+host.signed_random()*(pi/18.0f));return 0; //501770
    default:(void)frame;throw std::out_of_range("Invalid nonnull SHT initialization callback index");
    }
}
int update_shot_callback(Shot& shot,unsigned index,ShotCallbackEnvironment& env){
    auto& host=env.firing();
    switch(index){
    case 1:{                                             //501820
        if(shot.fields_98[1]==2)return 0;auto& handle=shot.fields_98[2];
        if(!shot.context->objects_04[1])handle=0;
        else{
            if(!handle)handle=env.nearest_enemy(*shot.context,{shot.motion.position.x,shot.motion.position.y},256.0f);
            if(handle){if(!contains_enemy(env.enemies(*shot.context),handle))handle=0;
                else{auto* enemy=env.find_enemy(handle);if(env.excluded_enemy(enemy))handle=0;
                    else{const float desired=point_angle(shot.motion.position,env.enemy_position(enemy));const float delta=ecl::math::angle_difference(desired,shot.motion.angle_1c);float speed=shot.motion.field_18;
                        if(shot.timer_1c.current<60){if(std::fabs(delta)<pi/4.0f){if(std::fabs(delta)<pi/12.0f){speed+=0.2f;if(speed>16.0f)speed=16.0f;}}
                            else{speed-=0.2f;if(speed<4.0f)speed=4.0f;}add_angle(shot,delta*0.08f);shot.motion.field_18=speed;return 0;}
                        shot.motion.field_18=speed+0.2f;return 0;}}}
        }
        shot.motion.field_18+=0.1f;if(shot.motion.field_18>16.0f)shot.motion.field_18=16.0f;return 0;
    }
    case 2:case 6:if(shot.fields_98[1]==1)shot.motion.field_18+=0.3f;return 0; //501ba0
    case 3:{                                             //501be0
        if(shot.fields_98[1]==2)return 0;
        if(((shot.fields_3c[0]>>2)&15u)==0){
            if(!shot.context->objects_04[1])shot.fields_98[2]=0;
            else if(!shot.fields_98[2]){
                const auto position=shot.motion.position;auto& list=env.enemies(*shot.context);
                for(scheduler::Iterator iterator(list.sentinel.next);iterator.current;iterator.advance()){
                    auto* enemy=iterator.current->value;if(env.excluded_enemy(enemy))continue;const auto target=env.enemy_position(enemy);
                    if(target.y-16.0f<=position.y&&position.y<=target.y+16.0f&&!(target.x-16.0f<position.x&&position.x<target.x+16.0f)){
                        shot.fields_3c[0]=(shot.fields_3c[0]&~0x3cu)|4u;env.interrupt(shot.handle_18,2);recovered::timer_set(shot.timer_2c,0);shot.motion.field_18=0;shot.vector_100=target;break;
                    }
                }
            }
        }
        if(((shot.fields_3c[0]>>2)&15u)==1){
            if(shot.timer_2c.current==4){set_angle(shot,shot.motion.position.x<=shot.vector_100.x?0.0f:-pi);shot.motion.field_18=14.0f;shot.fields_3c[0]=(shot.fields_3c[0]&~0x3cu)|8u;}
            const auto rate=host.clock_rate();recovered::timer_add(shot.timer_2c,1.0f,&rate);
        }
        return 0;
    }
    case 7:return 0;                                     //501f30 verified original
    case 4:case 5:return update_laser(shot,index,env);
    case 8:if(shot.fields_98[1]==1&&(shot.motion.position.x< -192.0f||shot.motion.position.x>192.0f))set_angle(shot,-pi-shot.motion.angle_1c);return 0; //502a20
    case 9:case 14:if(shot.fields_98[1]==1)shot.motion.field_18+=0.1f;return 0; //502ad0
    case 10:if(shot.fields_98[1]==1)shot.motion.field_18+=-0.22f;return shot.timer_1c.current>=20?-1:0; //502b10
    case 11:                                             //502b70
        if(shot.fields_98[1]==1){if(shot.timer_1c.current<120){if(shot.motion.field_18>0){shot.motion.field_18+=-0.95f;if(shot.motion.field_18<=0.2f)shot.motion.field_18=0.2f;}}else shot.motion.field_18+=0.2f;}return 0;
    case 12:{                                            //502c50
        if(shot.timer_1c.current%30==0){shot.fields_b8[0]=std::bit_cast<std::uint32_t>(host.signed_random()*(pi/180.0f));shot.fields_b8[1]=std::bit_cast<std::uint32_t>(host.signed_random()*0.2f);}
        set_angle(shot,shot.motion.angle_1c+word_float(shot.fields_b8[0]));if(shot.motion.field_18<2.0f&&word_float(shot.fields_b8[1])<0)return 0;shot.motion.field_18+=word_float(shot.fields_b8[1]);return 0;
    }
    case 13:{                                            //502db0
        shot.motion.field_18+=0.1f;float width=16.0f;
        if(auto* animation=env.animation_child(shot.handle_18,9,0)){animation->base.vector_50.x=shot.motion.field_18+animation->base.vector_50.x;animation->base.flags[1]|=4;width=animation->base.vector_50.x;}
        if(auto* region=host.damage(shot.fields_b8[9])){
            const float factor=-width/2.0f;auto direction=shot.motion.vector_38;const auto& position=shot.motion.position;
            const float magnitude=ecl::math::square_root((direction.x*direction.x+direction.y*direction.y)+direction.z*direction.z);
            if(!(std::fabs(magnitude)<0.01f)){direction.x/=magnitude;direction.y/=magnitude;direction.z/=magnitude;}
            region->motion.position={direction.x*factor+position.x,direction.y*factor+position.y,direction.z*factor+position.z};
            region->angle=ecl::math::wrap_angle(shot.motion.angle_1c);region->size={width,28.0f};region->damage=std::bit_cast<int>(shot.fields_98[5]);
        }
        return shot.timer_1c.current>=130?1:0;
    }
    case 15:{                                            //502f70
        const int timeout=std::bit_cast<int>(record(shot).fields_3c[1]),time=shot.timer_1c.current;
        const int end=recovered::signed_bits(static_cast<std::uint32_t>(timeout)+10u),last=recovered::signed_bits(static_cast<std::uint32_t>(timeout)+20u);
        if(time==timeout){shot.motion.field_18=0;shot.motion.field_44|=32u;env.interrupt(shot.handle_18,2);host.sound_at(27,shot.view_index==0?-100.0f:100.0f);}
        else if(time>timeout&&time<=end){float x=4,y=0;ecl::math::rotate(x,y,shot.motion.angle_1c);shot.motion.position.x+=x;shot.motion.position.y+=y;shot.motion.position.z+=0.0f;}
        if(time>=end&&time<last)env.cancel_rectangles(*shot.context,shot.motion.position,{24.0f,56.0f,0},shot.motion.angle_1c);
        return 0;
    }
    case 16:if(shot.fields_98[1]==1){shot.motion.field_18+=0.1f;set_angle(shot,shot.motion.angle_1c+(-pi/2.0f-shot.motion.angle_1c)*0.05f);}return 0; //503240
    default:throw std::out_of_range("Invalid nonnull SHT update callback index");
    }
}
}
