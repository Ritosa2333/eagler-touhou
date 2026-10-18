#include "enemy_opcode_state.hpp"
#include "enemy_opcode_data.hpp"
#include "enemy_drop.hpp"
#include "enemy_damage.hpp"
#include "enemy_variables.hpp"
#include "player_state.hpp"
#include "../card_system/card.hpp"
#include "../hud_system/dialogue.hpp"
#include "../bullet_system/bullet.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
namespace th20::source::gameplay {
namespace {
int add(int a,int b){return recovered::signed_bits(unsigned(a)+unsigned(b));}
template<class T>T get(const void* p,unsigned offset){T v;std::memcpy(&v,static_cast<const std::uint8_t*>(p)+offset,sizeof(v));return v;}
template<class T>void put(void* p,unsigned offset,T v){std::memcpy(static_cast<std::uint8_t*>(p)+offset,&v,sizeof(v));}
float divide(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float subtract(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void copy_phase_name(void* p,const char* name){if(!name){*static_cast<char*>(p)=0;return;}const auto length=std::strlen(name);if(length>=64)throw std::out_of_range("Enemy phase name exceeds original64-byte buffer");std::memcpy(p,name,length+1);}
EnemyAuxiliaryRecord& phase(EnemyState& s,unsigned index){if(index==0xffffffffu)throw std::out_of_range("Enemy phase index overflow");if(s.auxiliary.size()<index+1){const auto before=s.auxiliary.size();s.auxiliary.resize(index+1);for(auto i=before;i<s.auxiliary.size();++i)s.auxiliary[i].values[0]=s.auxiliary[i].values[1]=0xffffffffu;}return s.auxiliary.at(index);}
void bit(EnemyOpcodeReader& r,unsigned offset,unsigned mask,int value){r.put(offset,(r.get<unsigned>(offset)&~mask)|((value&1)?mask:0));}
card::CardInf& card_owner(EnemyOpcodeReader& r){return *static_cast<card::CardInf*>(r.context().objects_04[3]);}
std::uint32_t& drop_count(EnemyPatternState& p,int index,bool secondary){const auto slot=unsigned(index)-1;if(slot>=16)throw std::out_of_range("Enemy drop count index outside16 entries");return p.fields_00[(secondary?19:3)+slot];}
}
void add_enemy_reward_total(game_session::Player& p,int delta) noexcept {const int value=add(get<int>(&p,0xdc),delta);put(&p,0xdc,std::clamp(value,0,999999));}
void configure_enemy_phase(EnemyState& s,unsigned index,int life,int time,const char* name){auto& record=phase(s,index);record.values[0]=unsigned(life);if(life>=0){record.values[1]=unsigned(time);copy_phase_name(record.values.data()+2,name);copy_phase_name(record.values.data()+18,name);}}
void configure_enemy_timeout(EnemyState& s,unsigned index,const char* name){copy_phase_name(phase(s,index).values.data()+18,name);}
EnemyOpcodeResult execute_enemy_state_opcode(EnemyOpcodeReader& r,EnemyStateOpcodeServices& env){
    auto& s=r.state;auto& health=s.auxiliary_18c;auto& pattern=s.pattern_1a8;
    auto position=[&](){return r.get<sprite::Vec3>(0x110);};
    auto difficulty=[&](){return player_state::difficulty(env.session().player_table);};
    auto drop=[&](){env.drop(pattern,position(),(s.fields_2c8[2]&32u)!=0);};
    switch(r.opcode()){
    case 500:s.bounds_5c.x=r.real(0);s.bounds_5c.y=r.real(1);if(r.get<int>(0x284)==0){r.put(0x284,1);add_enemy_reward_total(*r.context().current_player,1);}break;
    case 501:s.bounds_64.x=r.real(0);s.bounds_64.y=r.real(1);break;
    case 502:s.fields_2c8[0]|=unsigned(r.integer(0))&0x3fffu;if(s.fields_2c8[0]&32u)for(auto& a:s.animations)env.visibility(a.handle,false);break;
    case 503:s.fields_2c8[0]&=~(unsigned(r.integer(0))&0x3fffu);if(!(s.fields_2c8[0]&32u))for(auto& a:s.animations)env.visibility(a.handle,true);break;
    case 504:s.fields_2c8[1]|=2u;for(int i=0;i<4;++i)r.put(0x178+4*i,r.real(i));break;
    case 505:s.fields_2c8[1]&=~2u;break;
    case 506:reset_enemy_drop_counts(pattern);break;
    case 507:{const int index=r.integer(0);drop_count(pattern,index,false)=unsigned(r.integer(1));break;}
    case 508:{const float y=r.real(1),x=r.real(0);pattern.field_a0=x;pattern.field_a4=y;break;}
    case 509:if(env.session().mode!=2)drop();break;
    case 510:pattern.fields_00[1]=unsigned(r.integer(0));break;
    case 511:health.words[0]=unsigned(r.integer(0));health.words[1]=health.words[0];health.words[2]=health.words[0];if(s.fields_2c8[1]&0x80u)s.fields_2c8[1]|=0x4000u;health.words[3]=health.words[0]*7u;break;
    case 512:{const int slot=r.integer(0);auto& c=r.controller();c.data.field_84&=~1u;if(slot<0){if(s.fields_2c8[1]&0x80u)c.data.handles_44[std::size_t(r.get<unsigned>(0x270))<16?r.get<unsigned>(0x270):throw std::out_of_range("Enemy boss slot")]=0;s.fields_2c8[1]&=~0x80u;}else{s.fields_2c8[1]|=0x80u;if(slot>=16)throw std::out_of_range("Enemy boss slot");c.data.handles_44[slot]=s.identifier;r.put(0x270,slot);}break;}
    case 513:recovered::timer_set(s.timer_a8,0);break;
    case 514:{
        const char* name=r.text(16,r.raw_word(12));
        if(env.session().mode==2&&(s.fields_2c8[1]&0x80u)){
            const bool extra=player_state::stage(env.session().player_table)==7&&enemy_phase_stage(env.session())<41;
            name=extra?(r.get<int>(0x270)==0?opcode_data::s_0056feb4:opcode_data::s_0056fedc):opcode_data::s_0056fec0;
            const int time=r.integer(2),index=r.integer(0);configure_enemy_phase(s,unsigned(index),0,time,name);
            configure_enemy_timeout(s,unsigned(r.integer(0)),opcode_data::s_0056fecc);
        }else{const int life=r.integer(1),time=r.integer(2),index=r.integer(0);configure_enemy_phase(s,unsigned(index),life,time,name);}break;
    }
    case 515:recovered::timer_set(s.timer_288,r.integer(0));break;
    case 516:{const float x=position().x;env.sound(r.integer(0),x);break;}
    case 517:{const int third=r.integer(2),second=r.integer(1),first=r.integer(0);env.shake(first,second,third);break;}
    case 518:env.dialogue(r.integer(0));env.cancel_bullets(r.context());env.erase_lasers(r.context(),false);env.clear(r.controller(),EnemyClearKind::ordinary,0);break;
    case 519:if(env.hud().collecting&&env.hud().collecting->field_100==0)return -1;break;
    case 520:for(unsigned i=0;i<3;++i)if(env.selected(r.controller(),i))return -1;break;
    case 521:{const char* name=r.text(8,r.raw_word(4));configure_enemy_timeout(s,unsigned(r.integer(0)),name);break;}
    case 522:case 528:case 531:case 532:case 533:{
        char name[128]{};const auto length=r.raw_word(12);if(length>128||16u+length>r.instruction.size()-16u)throw std::out_of_range("Enemy spell name outside original128-byte buffer");
        std::uint8_t key=0x77,step=7;for(unsigned i=0;i<length;++i){name[i]=char(r.instruction.bytes[32+i]^key);key=std::uint8_t(key+step);step=std::uint8_t(step+0x10);}
        if(!std::memchr(name,0,sizeof(name)))throw std::invalid_argument("Enemy spell name not terminated");
        int spell=r.integer(0);if(r.opcode()>=531)spell=add(add(spell,difficulty()),531-int(r.opcode()));
        const int portrait=r.integer(2),time=r.integer(1);env.card_start(r.context(),spell,name,time,portrait);
        health.words[6]|=1u;health.words[3]=health.words[0]*7u;recovered::timer_set(s.timer_a8,0);break;
    }
    case 523:env.card_finish(r.context());health.words[6]&=~1u;break;
    case 524:{const int stage=r.integer(0);player_state::write(env.session().player_table,0x1fc,std::clamp(stage,0,999));env.game().field_34=unsigned(stage);r.put(0x280,stage);r.put(0x284,0);break;}
    case 525:env.clear(r.controller(),EnemyClearKind::ordinary,0);break;
    case 526:{const float radius=r.real(0);r.put(0x274,recovered::mul32(radius,radius));break;}
    case 527:{const float amount=r.real(1);const int color=r.integer(2);const float ratio=divide(amount,recovered::int_float(recovered::signed_bits(health.words[1])));const int index=r.integer(0),boss=r.get<int>(0x270);const auto offset=0x1e0u+unsigned(boss)*0x54u+unsigned(index)*8u;if(boss<0||index<0||offset>sizeof(hud::FrontInf)-8)throw std::out_of_range("Enemy boss health segment index");put(&env.hud(),offset,ratio);put(&env.hud(),offset+4,color);break;}
    case 529:case 530:case 570:case 571:{const auto rank=r.get<unsigned>(0x50);const unsigned end=r.opcode()>=570?7:3;const int index=int(rank<end?rank+1:end+1);if(r.opcode()==529||r.opcode()==570){const int value=r.integer(index);r.integer_destination(0)=unsigned(value);}else{const float value=r.real(index);r.float_destination(0)=std::bit_cast<unsigned>(value);}break;}
    case 534:env.hud().fields_174[3]=unsigned(r.integer(0));break;
    case 535:recovered::timer_set(s.timer_298,r.integer(0));break;
    case 536:card_owner(r).flags|=8u;break;
    case 537:card_owner(r).flags|=16u;env.delete_animation(card_owner(r).effect_handle);break;
    case 538:bit(r,0x2cc,0x800u,r.integer(0));break;
    case 539:env.erase_lasers(r.context(),true);break;
    case 540:bit(r,0x2cc,0x1000u,r.integer(0));r.put(0x54,r.integer(1));r.put(0x58,r.get<unsigned>(0x28));s.fields_2c8[1]&=~0x2000u;s.fields_2c8[0]&=~1u;break;
    case 541:env.clock_scale(r.real(0));break;
    case 542:{int values[4];for(int i=0;i<4;++i)values[i]=r.integer(i);const int rank=difficulty();r.runtime.time=subtract(r.runtime.time,recovered::int_float(values[unsigned(rank)<3?rank:3]));break;}
    case 543:bit(r,0x2cc,0x8000u,r.integer(0));break;
    case 544:r.put(0x30,r.integer(0));break;
    case 545:env.clear(r.controller(),EnemyClearKind::group,r.integer(0));break;
    case 546:r.put(0x34,r.integer(0));break;
    case 547:r.put(0x264,r.integer(0));break;
    case 548:env.stage_title();break;
    case 549:{const auto identifier=unsigned(r.integer(1));const bool found=identifier&&find_enemy_in_list(r.controller().enemies,identifier);r.integer_destination(0)=found?1u:0u;break;}
    case 550:r.put(0x27c,r.entity.loader->find(r.text(4,r.raw_word(0))));break;
    case 551:{const int time=r.integer(0),mode=r.integer(1);const auto color=unsigned(r.integer(2));const float far_distance=r.real(4),near_distance=r.real(3);env.fog(time,mode,color,near_distance,far_distance);break;}
    case 552:bit(r,0x2cc,8u,r.integer(0));break;
    case 553:r.controller().data.field_88=unsigned(r.integer(0));break;
    case 554:{const float y=r.real(1),x=r.real(0);static_cast<bullet::Controller*>(r.context().primary_owner)->vector_4c={x,y};break;}
    case 555:{
        if(r.get<int>(0x250)>=0)env.sound(r.get<int>(0x250),position().x);
        if(r.get<int>(0x254)>=0){const auto current=position();const float dx=subtract(current.x,s.vector_6c.x),dy=subtract(current.y,s.vector_6c.y);const float distance=recovered::add32(recovered::mul32(dx,dx),recovered::mul32(dy,dy));const float angle=distance>=0.040000003f?ecl::math::arctangent(dy,dx):-1.5707964f;env.death_effect(r.controller(),r.get<unsigned>(0x258),r.get<int>(0x254),position(),angle);}break;
    }
    case 556:drop();break;
    case 557:bit(r,0x2c8,0x1000u,r.integer(0));break;
    case 558:r.put(0x38,r.real(0));break;
    case 559:r.put(0x4c,r.real(0));break;
    case 560:return env.defeat(r.entity)?1:0;
    case 561:{const int value=r.integer(0);auto& c=card_owner(r);c.flags=(c.flags&~0x100u)|((unsigned(value)&1u)<<8);break;}
    case 562:health.words[6]=(health.words[6]&~1u)|(unsigned(r.integer(0))&1u);break;
    case 563:{const int old=r.get<int>(0x284);if(old==0){const int value=r.integer(0);r.put(0x284,value);if(value>0)add_enemy_reward_total(*r.context().current_player,value);}else if(old==1){const int value=r.integer(0);r.put(0x284,value);add_enemy_reward_total(*r.context().current_player,add(value,-1));}break;}
    case 564:if(r.get<int>(0x284)!=0&&r.get<int>(0x280)==enemy_phase_stage(env.session())){add_phase_reward(*r.context().current_player,r.get<int>(0x284));r.put(0x284,0);}break;
    case 565:env.clear(r.controller(),EnemyClearKind::no_effect,0);break;
    case 566:health.words[0]=unsigned(r.integer(0));health.words[3]=health.words[0]*7u;break;
    case 567:{const int index=r.integer(0);drop_count(pattern,index,true)=unsigned(r.integer(1));break;}
    case 568:pattern.field_8c=unsigned(r.integer(0));recovered::timer_set(pattern.timer_90,0);break;
    case 572:pattern.fields_00[2]=unsigned(r.integer(0));break;
    case 573:env.clear(r.controller(),EnemyClearKind::exclude_special,0);break;
    case 574:if(!(s.fields_2c8[2]&0x40u))r.runtime.time=subtract(r.runtime.time,1);break;
    case 575:{auto* player=env.session().contexts[0].objects_04[0];const int value=r.integer(0);put(player,0x14,(get<unsigned>(player,0x14)&~0x400u)|((unsigned(value)&1u)<<10));break;}
    default:return std::nullopt;
    }
    return 0;
}
}
