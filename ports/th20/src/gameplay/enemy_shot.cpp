#include "enemy_shot.hpp"
#include "../ecl_vm/math.hpp"
#include <bit>
#include <iterator>
namespace th20::source::gameplay {
EnemyShotValues shot_values(const EnemyQueuedRecord& record) noexcept{EnemyShotValues value;std::memcpy(&value,record.values.data(),sizeof(value));return value;}
void store_shot_values(EnemyQueuedRecord& record,const EnemyShotValues& value) noexcept{std::memcpy(record.values.data(),&value,sizeof(value));}
EnemyQueuedRecord& queued_shot(EnemyState& s,int index,EnemyShotOpcodeServices& env,bool unique){
    if(index<0)throw std::out_of_range("Negative Enemy shot index");const auto count=std::distance(s.queued.begin(),s.queued.end());if(count<=index)s.queued.resize(std::size_t(index)+1);
    auto at=s.queued.begin();std::advance(at,index);if(!at->owner||(unique&&at->owner.use_count()>1))at->owner=env.metadata(static_cast<const bullet::ShotMetadata*>(at->owner.get()));return *at;
}
sprite::Vec3 shot_origin(const EnemyState& state,const EnemyShotValues& value){
    sprite::Vec3 base;if(value.absolute.z<=.9f)std::memcpy(&base,state.motion_110.words,12);else base=value.absolute;
    sprite::Vec3 result{base.x+value.offset.x,base.y+value.offset.y,base.z+value.offset.z};if(!(value.absolute.z<=.9f))result.z=0;return result;
}
EnemyOpcodeResult execute_enemy_bullet_opcode(EnemyOpcodeReader& r,EnemyShotOpcodeServices& env){
    const auto op=r.opcode();if(op<600||op>633)return std::nullopt;auto& s=r.state;
    const auto at=[&](bool unique)->EnemyQueuedRecord&{return queued_shot(s,r.integer(0),env,unique);};
    const auto meta=[](EnemyQueuedRecord& q)->bullet::ShotMetadata&{return *static_cast<bullet::ShotMetadata*>(q.owner.get());};
    const auto position=[&](){return r.get<sprite::Vec3>(0x110);};
    switch(op){
    case 600:{auto& q=at(true);auto v=shot_values(q);v.parameters={};auto& metadata=meta(q);const auto padding0=metadata.padding_4a[0],padding1=metadata.padding_4a[1];auto defaults=env.metadata(nullptr);metadata=std::move(*static_cast<bullet::ShotMetadata*>(defaults.get()));metadata.padding_4a[0]=padding0;metadata.padding_4a[1]=padding1;v.command_cursor=0;v.offset.x=v.offset.y=0;v.absolute={};store_shot_values(q,v);break;} //offset.z(+3c) is deliberately retained
    case 601:{auto& q=at(false);auto v=shot_values(q);v.parameters.position=shot_origin(s,v);store_shot_values(q,v);env.fire(r.context(),v.parameters,q.owner,s.fields_250[9]);break;}
    case 602:{auto& q=at(false);auto v=shot_values(q);v.parameters.fields_00[0]=unsigned(r.integer(1));v.parameters.fields_00[1]=unsigned(r.integer(2));store_shot_values(q,v);break;}
    case 603:{auto& q=at(false);auto v=shot_values(q);v.offset.x=r.real(1);v.offset.y=r.real(2);store_shot_values(q,v);break;}
    case 604:{auto& q=at(false);auto v=shot_values(q);v.parameters.angle=ecl::math::wrap_angle(r.real(1));v.parameters.angle_step=ecl::math::wrap_angle(r.real(2));store_shot_values(q,v);break;}
    case 605:{auto& q=at(false);auto v=shot_values(q);v.parameters.speed=r.real(1);v.parameters.speed_step=r.real(2);store_shot_values(q,v);break;}
    case 606:{auto& q=at(false);auto v=shot_values(q);v.parameters.count=std::int16_t(r.integer(1));v.parameters.rows=std::int16_t(r.integer(2));store_shot_values(q,v);break;}
    case 607:{auto& q=at(true);meta(q).type=std::int16_t(r.integer(1));break;}
    case 608:{auto& q=at(true);meta(q).field_3c=r.integer(1);meta(q).field_40=r.integer(2);break;}
    case 609:case 610:case 611:case 612:{auto& q=at(true);auto v=shot_values(q);auto& metadata=meta(q);const bool append=op>=611,wide=op==610||op==612;int arg=append?1:2;const int index=append?v.command_cursor:r.integer(1);if(index<0)throw std::out_of_range("Negative Enemy ETEX index");if(metadata.commands.size()<std::size_t(index)+1)metadata.commands.resize(std::size_t(index)+1);auto& command=metadata.commands[unsigned(index)];command.words[9]=unsigned(r.integer(arg++));command.words[8]=unsigned(r.integer(arg++));command.words[4]=unsigned(r.integer(arg++));command.words[5]=unsigned(r.integer(arg++));if(wide){command.words[6]=unsigned(r.integer(arg++));command.words[7]=unsigned(r.integer(arg++));}command.words[0]=std::bit_cast<unsigned>(r.real(arg++));command.words[1]=std::bit_cast<unsigned>(r.real(arg++));if(wide){command.words[2]=std::bit_cast<unsigned>(r.real(arg++));command.words[3]=std::bit_cast<unsigned>(r.real(arg++));}v.command_cursor=recovered::signed_bits(unsigned(index)+1u);store_shot_values(q,v);break;}
    case 613:env.cancel_all(r.context());break;
    case 614:{const int destination=r.integer(0),source=r.integer(1);if(destination<0||source<0)throw std::out_of_range("Negative Enemy shot copy index");const auto count=std::distance(s.queued.begin(),s.queued.end());const auto size=std::size_t((std::max)(destination,source))+1;if(count<size)s.queued.resize(size);auto to=s.queued.begin(),from=s.queued.begin();std::advance(to,destination);std::advance(from,source);*to=*from;break;}
    case 615:case 616:{const float radius=r.real(0);env.cancel_circle(r.context(),position(),radius,true);break;}
    case 617:{const float x=env.player_position(r.context()).x-r.real(1);const float y=env.player_position(r.context()).y-r.real(2);r.float_destination(0)=std::bit_cast<unsigned>(ecl::math::arctangent(y,x));break;}
    case 618:{const int index=r.integer(0);const float angle=r.real(1),radius=r.real(2);auto& q=queued_shot(s,index,env,false);auto v=shot_values(q);ecl::math::polar(v.offset.x,v.offset.y,angle,radius);store_shot_values(q,v);break;}
    case 619:{auto& q=at(true);meta(q).field_00=r.real(1);break;}
    case 620:{auto& q=at(false);auto v=shot_values(q);v.absolute.x=r.real(1);v.absolute.y=r.real(2);v.absolute.z=v.absolute.x>=-990.f?1.f:0.f;store_shot_values(q,v);break;}
    case 621:{const float radius=r.real(0);const auto color=unsigned(r.integer(1));env.mesh(s,radius,color);break;}
    case 622:env.background_event(r.integer(0));break;
    case 623:{const auto value=unsigned(r.integer(0));auto& flags=r.controller().data.field_84;flags=(flags&~1u)|(value&1u);break;}
    case 624:case 631:s.fields_2c8[6]=env.callback(0,r.integer(0));s.fields_2c8[4]=op==631;break;
    case 625:s.fields_2c8[5]=env.callback(1,r.integer(0));break;
    case 626:s.fields_2c8[7]=env.callback(2,r.integer(0));break;
    case 627:case 628:{const float radius=r.real(0);env.cancel_circle(r.context(),position(),radius,false);break;}
    case 629:env.invoke_callback(s,r.integer(0));break;
    case 630:{const int score=r.integer(0);env.add_score(r.context(),position(),score);break;}
    case 632:{const int index=r.integer(0),command=r.integer(1);auto& q=queued_shot(s,index,env,true);auto& metadata=meta(q);if(command<0)throw std::out_of_range("Negative Enemy ETEX string index");if(metadata.commands.size()<std::size_t(command)+1)metadata.commands.resize(std::size_t(command)+1);metadata.commands[unsigned(command)].words[10]=reinterpret_cast<unsigned>(r.instruction.bytes+0x1c);break;}
    case 633:{auto& q=at(true);auto v=shot_values(q);v.command_cursor=recovered::signed_bits(unsigned(v.command_cursor)-1);if(v.command_cursor<0)v.command_cursor=0;store_shot_values(q,v);break;}
    }
    return 0;
}
}
