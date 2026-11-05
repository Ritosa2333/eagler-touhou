#include "anm_vm.hpp"
#ifdef TH_SDL3
#include "pool.hpp" // AnimationCallback complete type for the virtual update() call
#endif
#include "../ecl_vm/math.hpp"
#include <cmath>
#include <cstring>
#include <string>
#include <stdexcept>

namespace th20::source::sprite {
namespace n=th20::recovered;
namespace m=th20::source::ecl::math;
namespace env=anm_environment;
namespace {
template<class T> T& at(Animation& a,std::size_t offset) {return *reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(&a)+offset);}
float as_float(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
std::uint32_t as_bits(float value){std::uint32_t bits;std::memcpy(&bits,&value,4);return bits;}
std::int32_t I(Animation& a,AnmInstruction& ins,unsigned i) {
    auto value=n::signed_bits(ins.arguments()[i]);
    return ins.mask&(1u<<(i&31)) ? anm_integer_variable(a,value):value;
}
float F(Animation& a,AnmInstruction& ins,unsigned i) {
    float value=as_float(ins.arguments()[i]);return ins.mask&(1u<<(i&31))?anm_float_variable(a,value):value;
}
void bit(Animation& a,std::size_t offset,unsigned mask,unsigned value) {auto& b=at<std::uint8_t>(a,offset);b=static_cast<std::uint8_t>((b&~mask)|(value&mask));}
void flag(Animation& a,unsigned mask,unsigned value){auto& f=a.base.flags[1];f=(f&~mask)|(value&mask);}
void raw_timer_add(n::Timer& timer,float delta) { // 0x454110, deliberately no initialization or rate
    timer.previous=timer.current;timer.current_f=n::add32(timer.current_f,delta);timer.current=n::truncate32(timer.current_f);
}
void snapshot_corners(Animation& a){
    Vec3 positions[4]{};env::calculate_corners(a,positions);
    for(unsigned i=0;i<4;++i)a.base.vectors_378[i]={positions[i].x,positions[i].y};
    a.base.vector_398={a.base.vectors_378[1].x-a.base.vectors_378[0].x,a.base.vectors_378[2].y-a.base.vectors_378[0].y};
}
void set_sprite(Animation& a,std::int32_t id){
    if(a.field_5e0)id=reinterpret_cast<std::int32_t(__cdecl*)(Animation*,std::int32_t)>(a.field_5e0)(&a,id);
    env::assign_sprite(a,id);a.base.field_43c=static_cast<std::uint32_t>(a.timer_4c8.current);
}
enum class Flow {advance,refetch,stop,remove,wait,interrupt};
template<class T>void initialize(Interpolation<T>& p,std::int32_t duration,std::int32_t mode,const T& start,const T& end){p.duration=duration;p.mode=mode;p.start=start;p.end=end;p.current=start;n::timer_set(p.timer,0);}
template<class T> bool compare(unsigned operation,T a,T b){switch(operation){case 0:return a==b;case 1:return a!=b;case 2:return a<b;case 3:return a<=b;case 4:return a>b;default:return a>=b;}}
Flow dispatch(Animation& a,AnmInstruction& ins){
    auto& b=a.base;auto* args=ins.arguments();const auto op=ins.opcode;
    auto id=[&](unsigned i=0)->std::uint32_t&{return anm_integer_destination(a,ins,i);};
    auto fd=[&](unsigned i=0)->float&{return anm_float_destination(a,ins,i);};
    auto jump=[&](unsigned p){n::timer_set(a.timer_4c8,n::signed_bits(args[p+1]));b.fields_10_28[6]=args[p];return Flow::refetch;};
    switch(op){
    case -1:case 1:bit(a,0x49a,1,0);b.fields_10_28[6]=0xffffffffu;return Flow::remove;
    case 0:case 5:break; // actual original no-operation cases / interrupt label
    case 2:b.fields_10_28[6]=0xffffffffu;return Flow::stop;
    case 4:bit(a,0x49a,1,0);[[fallthrough]];
    case 3:if(b.field_438)return Flow::interrupt;bit(a,0x4a1,1,1);n::timer_add(a.timer_4c8,-1.f,env::timer_rate());return Flow::wait;
    case 6:raw_timer_add(a.timer_4c8,n::int_float(n::signed_bits(0u-static_cast<std::uint32_t>(I(a,ins,0)))));break;
    case 7:n::timer_set(a.timer_4c8,b.timer.current);b.fields_10_28[6]=b.fields_10_28[0];return Flow::refetch;
    case 100:{const auto v=I(a,ins,1);id()=static_cast<std::uint32_t>(v);break;}
    case 101:{const auto v=F(a,ins,1);fd()=v;break;}
    case 102:case 104:case 106:case 108:case 110:{
        const auto v=I(a,ins,1);auto& d=id();
        switch(op){case 102:d+=v;break;case 104:d-=v;break;case 106:d*=v;break;
        case 108:d=static_cast<std::uint32_t>(n::signed_bits(d)/v);break;
        case 110:d=static_cast<std::uint32_t>(n::signed_bits(d)%v);break;}break;
    }
    case 103:case 105:case 107:case 109:{const float v=F(a,ins,1);auto& d=fd();switch(op){case 103:d+=v;break;case 105:d-=v;break;case 107:d*=v;break;case 109:d/=v;break;}break;}
    case 111:{const float v=F(a,ins,1),u=F(a,ins,0);const float r=static_cast<float>(std::fmod(static_cast<double>(u),static_cast<double>(v)));fd()=r;break;}
    case 112:case 114:case 116:case 118:case 120:{const auto u=I(a,ins,1),v=I(a,ins,2);std::uint32_t r=0;switch(op){case 112:r=std::uint32_t(u)+std::uint32_t(v);break;case 114:r=std::uint32_t(u)-std::uint32_t(v);break;case 116:r=std::uint32_t(u)*std::uint32_t(v);break;case 118:r=static_cast<std::uint32_t>(u/v);break;case 120:r=static_cast<std::uint32_t>(u%v);break;}id()=r;break;}
    case 113:case 115:case 117:case 119:{const float u=F(a,ins,1),v=F(a,ins,2);float r=0;switch(op){case 113:r=u+v;break;case 115:r=u-v;break;case 117:r=u*v;break;case 119:r=u/v;break;}fd()=r;break;}
    case 121:{const float v=F(a,ins,2),u=F(a,ins,1);const float r=static_cast<float>(std::fmod(static_cast<double>(u),static_cast<double>(v)));fd()=r;break;}
    case 122:{const auto v=I(a,ins,1);const auto r=env::random_bounded(v);id()=r;break;}
    case 123:{const float v=F(a,ins,1),r=env::random_unit()*v;fd()=r;break;}
    case 124:case 125:case 126:case 127:case 128:{const float v=F(a,ins,1);float r;switch(op){case 124:r=m::sine(v);break;case 125:r=m::cosine(v);break;case 126:r=static_cast<float>(std::tan(static_cast<double>(v)));break;case 127:r=static_cast<float>(std::acos(static_cast<double>(v)));break;default:r=static_cast<float>(std::atan(static_cast<double>(v)));break;}fd()=r;break;}
    case 129:{const float v=F(a,ins,0);const float r=m::wrap_angle(v+0.f);fd()=r;break;}
    case 130:{const float length=F(a,ins,3),angle=F(a,ins,2);float& y=fd(1);float& x=fd(0);m::polar(x,y,angle,length);break;}
    case 131:{const float low=F(a,ins,2),high=F(a,ins,3);const float span=high-low;const float radius_random=env::random_signed_unit();const float radius=n::add32(n::mul32(span,radius_random),low);const float angle_random=env::random_signed_unit();const float angle=n::mul32(angle_random,3.1415927410125732421875f);float x=0,y=0;m::polar(x,y,angle,radius);fd(0)=x;fd(1)=y;break;}
    case 200:return jump(0);
    case 201:--id();if(I(a,ins,0)>0)return jump(1);break;
    case 202:case 204:case 206:case 208:case 210:case 212:{const auto u=I(a,ins,0),v=I(a,ins,1);if(compare((op-202)/2,u,v))return jump(2);break;}
    case 203:case 205:case 207:case 209:case 211:case 213:{const float u=F(a,ins,0),v=F(a,ins,1);if(compare((op-203)/2,u,v))return jump(2);break;}
    case 300:bit(a,0x49a,1,1);set_sprite(a,I(a,ins,0));break;
    case 301:{bit(a,0x49a,1,1);const auto first=I(a,ins,0),span=I(a,ins,1);const auto v=env::random_next()%static_cast<std::uint32_t>(span)+static_cast<std::uint32_t>(first);set_sprite(a,n::signed_bits(v));break;}
    case 302:at<std::uint8_t>(a,0x498)=static_cast<std::uint8_t>(args[0]);bit(a,0x4a0,0x30,0);break;
    case 303:at<std::uint8_t>(a,0x499)=static_cast<std::uint8_t>(args[0]);break;
    case 304:env::set_layer(a,static_cast<std::uint8_t>(args[0]));break;
    case 305:flag(a,0x10,(args[0]&1)<<4);break;
    case 306:bit(a,0x4a1,2,(args[0]&1)<<1);break;
    case 307:flag(a,0x200,(args[0]&1)<<9);break;
    case 308:at<std::uint8_t>(a,0x4a0)^=0x40;b.vector_50.x*=-1.f;b.flags[1]|=4;break;
    case 309:at<std::uint8_t>(a,0x4a0)^=0x80;b.vector_50.y*=-1.f;b.flags[1]|=4;break;
    case 310:bit(a,0x49a,1,static_cast<std::uint8_t>(args[0])!=0);break;
    case 311:bit(a,0x4a0,0xc,(args[0]&3)<<2);break;
    case 312:bit(a,0x4a2,3,I(a,ins,0)&3);bit(a,0x4a1,0x60,(I(a,ins,1)&3)<<5);break;
    case 313:at<std::uint8_t>(a,0x4a4)=static_cast<std::uint8_t>(I(a,ins,0));b.flags[1]|=0x800000;break;
    case 314:flag(a,0x20,(I(a,ins,0)&1)<<5);break;
    case 315:flag(a,0x1000000,(args[0]&1)<<24);break;
    case 316:b.flags[1]|=1;break;
    case 317:b.flags[1]&=~1u;break;
    case 318:flag(a,0x400000,(I(a,ins,0)&1)<<22);break;
    case 319:{bit(a,0x49a,1,1);const float angle=b.vector_38.z;constexpr float pi=3.1415927410125732421875f;unsigned index;if(angle>-pi/4.f&&angle<pi/4.f)index=0;else if(angle>=pi/4.f&&angle<pi/2.f+pi/4.f)index=3;else if(angle<=-pi/4.f&&angle>-pi/2.f-pi/4.f)index=2;else index=1;set_sprite(a,I(a,ins,index));break;}
    case 400:case 441:{const float z=F(a,ins,2),y=F(a,ins,1),x=F(a,ins,0);auto& dest=(op==441||(b.flags[1]&0x40))?b.vector_484:b.vector_2c;dest={x,y,z};break;}
    case 401:b.vector_38.x=F(a,ins,0);b.vector_38.y=F(a,ins,1);b.vector_38.z=F(a,ins,2);b.flags[1]|=2;break;
    case 402:b.vector_50.x=F(a,ins,0);b.vector_50.y=F(a,ins,1);b.flags[1]|=4;break;
    case 403:at<std::uint8_t>(a,0x493)=static_cast<std::uint8_t>(I(a,ins,0));break;
    case 404:for(unsigned i=0;i<3;++i)at<std::uint8_t>(a,0x492-i)=static_cast<std::uint8_t>(I(a,ins,i));break;
    case 405:at<std::uint8_t>(a,0x497)=static_cast<std::uint8_t>(I(a,ins,0));break;
    case 406:for(unsigned i=0;i<3;++i)at<std::uint8_t>(a,0x496-i)=static_cast<std::uint8_t>(I(a,ins,i));break;
    case 407:case 410:case 433:{auto& p=op==410?b.interpolation_160:b.interpolation_8c;p.duration=I(a,ins,0);p.tangent_start=p.tangent_end={};p.mode=n::signed_bits(args[1]);p.start=op==410?b.vector_38:(b.flags[1]&0x40?b.vector_484:b.vector_2c);
        if(op==433){const float radius=F(a,ins,3),angle=F(a,ins,2);p.end={};m::polar(p.end.x,p.end.y,angle,radius);}else {const float x=F(a,ins,2),y=F(a,ins,3),z=F(a,ins,4);p.end={x,y,z};}n::timer_set(p.timer,0);if(op==410)b.flags[1]|=2;break;}
    case 408:case 413:{const unsigned color=op==408?0x490:0x494;Vec3i start{at<std::uint8_t>(a,color),at<std::uint8_t>(a,color+1),at<std::uint8_t>(a,color+2)};const auto blue=static_cast<std::uint8_t>(I(a,ins,4)),green=static_cast<std::uint8_t>(I(a,ins,3)),red=static_cast<std::uint8_t>(I(a,ins,2));const auto mode=static_cast<std::uint8_t>(args[1]);const auto duration=I(a,ins,0);initialize(op==408?b.interpolation_e0:b.interpolation_2a0,duration,mode,start,Vec3i{blue,green,red});if(op==413&&!(at<std::uint8_t>(a,0x4a1)&0x1c))bit(a,0x4a1,0x1c,4);break;}
    case 409:case 414:{const auto end=static_cast<std::uint8_t>(I(a,ins,2));const auto start=at<std::uint8_t>(a,op==409?0x493:0x497);const auto mode=static_cast<std::uint8_t>(args[1]);const auto duration=I(a,ins,0);initialize(op==409?b.interpolation_134:b.interpolation_2f4,duration,mode,static_cast<std::int32_t>(start),static_cast<std::int32_t>(end));if(op==414&&!(at<std::uint8_t>(a,0x4a1)&0x1c))bit(a,0x4a1,0x1c,4);break;}
    case 411:{auto& p=b.interpolation_1b4;p.duration=I(a,ins,0);p.tangent_start=p.tangent_end=m::wrap_angle(0.f);p.mode=n::signed_bits(args[1]);p.start=m::wrap_angle(b.vector_38.z);p.end=m::wrap_angle(F(a,ins,2));n::timer_set(p.timer,0);b.flags[1]|=2;break;}
    case 412:case 430:case 435:{const float x=F(a,ins,2),y=F(a,ins,3);const Vec2 end{x,y};const Vec2 start=op==412?b.vector_50:(op==430?b.vector_68:b.vector_58);const auto mode=static_cast<std::uint8_t>(args[1]);const auto duration=I(a,ins,0);initialize(op==412?b.interpolation_1e0:(op==430?b.interpolation_260:b.interpolation_220),duration,mode,start,end);b.flags[1]|=op==430?8:4;break;}
    case 415:{const float z=F(a,ins,2),y=F(a,ins,1),x=F(a,ins,0);b.vector_44={x,y,z};b.flags[1]|=0x80000;break;}
    case 416:{const float y=F(a,ins,1),x=F(a,ins,0);b.vector_60={x,y};b.flags[1]|=0x80000;break;}
    case 417:{const auto end=static_cast<std::uint8_t>(args[0]);const auto start=at<std::uint8_t>(a,0x493);const auto duration=I(a,ins,1);initialize(b.interpolation_134,duration,0,static_cast<std::int32_t>(start),static_cast<std::int32_t>(end));break;}
    case 418:snapshot_corners(a);break;
    case 419:flag(a,0x800,(I(a,ins,0)&1)<<11);break;
    case 420:{const float sx=F(a,ins,1),sy=F(a,ins,2),sz=F(a,ins,3),ex=F(a,ins,7),ey=F(a,ins,8),ez=F(a,ins,9);auto& p=b.interpolation_8c;p.duration=I(a,ins,0);p.tangent_start={sx,sy,sz};p.tangent_end={ex,ey,ez};p.mode=8;p.start=b.flags[1]&0x40?b.vector_484:b.vector_2c;const float x=F(a,ins,4),y=F(a,ins,5),z=F(a,ins,6);p.end={x,y,z};n::timer_set(p.timer,0);break;}
    case 421:b.flags[4]=args[0]&0xffff;b.flags[5]=args[0]>>16;break;
    case 422:b.vector_2c=a.vector_5bc;a.vector_5bc={};break;
    case 423:bit(a,0x4a1,0x1c,(args[0]&7)<<2);break;
    case 424:bit(a,0x4a2,0xe0,args[0]<<5);break;
    case 425:b.fields_3a0[0]=as_bits(F(a,ins,0));b.flags[1]|=0x80000;break;
    case 426:b.fields_3a0[1]=as_bits(F(a,ins,0));b.flags[1]|=0x80000;break;
    case 427:case 428:{auto& p=op==427?b.interpolation_320:b.interpolation_34c;p.duration=I(a,ins,0);p.tangent_start=p.tangent_end=0;p.mode=n::signed_bits(args[1]);p.start=as_float(b.fields_3a0[op-427]);p.end=F(a,ins,2);n::timer_set(p.timer,0);break;}
    case 429:b.vector_68.x=F(a,ins,0);b.vector_68.y=F(a,ins,1);b.flags[1]|=8;break;
    case 431:flag(a,0x80,(args[0]&1)<<7);break;
    case 432:flag(a,0x100,(I(a,ins,0)&1)<<8);break;
    case 434:b.vector_58.x=F(a,ins,0);b.vector_58.y=F(a,ins,1);b.flags[1]|=4;break;
    case 436:b.vector_80.x=F(a,ins,0);b.vector_80.y=F(a,ins,1);break;
    case 437:bit(a,0x4a2,0x1c,(I(a,ins,0)&7)<<2);break;
    case 438:bit(a,0x4a3,3,args[0]&3);break;
    case 439:flag(a,0x100000,(args[0]&1)<<20);b.field_4b8=args[1];b.field_4bc=args[2];break;
    case 440:if(at<std::uint8_t>(a,0x4a0)&0x40)b.vector_50.x*=-1.f;if(at<std::uint8_t>(a,0x4a0)&0x80)b.vector_50.y*=-1.f;bit(a,0x4a0,0xc0,0);b.flags[1]|=4;break;
    case 500:case 501:case 502:case 503:{const std::uint32_t flags=op==500?0:(op==501?4:(op==502?2:6));env::spawn_child(a,I(a,ins,0),flags);break;}
    case 504:env::spawn_detached(a,I(a,ins,0),0);break;
    case 505:case 510:{const std::uint32_t flags=(b.flags[1]&0x40000?4:0)+(op==510?2:0);const auto handle=env::spawn_child(a,I(a,ins,0),flags);auto& child=env::lookup_animation(handle);child.base.vector_484.x=F(a,ins,1);child.base.vector_484.y=F(a,ins,2);break;}
    case 506:{const auto handle=env::spawn_detached(a,I(a,ins,0),0);auto& child=env::lookup_animation(handle);const float x=F(a,ins,1),y=F(a,ins,2);Vec3 offset{x,y,0};transform_animation_offset(a,offset,true,true);child.base.vector_484.x=offset.x;child.base.vector_484.y=offset.y;break;}
    case 507:flag(a,0x1000,(I(a,ins,0)&1)<<12);break;
    case 508:env::spawn_effect(a,I(a,ins,0));break;
    case 509:if(a.fields_550[3])std::memcpy(b.fields_444,reinterpret_cast<Animation*>(a.fields_550[3])->base.fields_444,sizeof(b.fields_444));break;
    case 600:case 601:case 602:case 609:case 610:case 633:case 634:{
        const std::uint8_t kind=op==600?9:(op==601?13:(op==602?14:(op==609?24:(op==610?25:(op==633?47:48)))));at<std::uint8_t>(a,0x498)=kind;
        const auto bytes=op==634?0x54u:static_cast<std::uint32_t>(I(a,ins,0))*(op==609||op==610?0x30u:0x38u);a.fields_550[7]=bytes;a.fields_550[5]=reinterpret_cast<std::uint32_t>(env::allocate_geometry(bytes));
        if(op==633)b.fields_444[0]=static_cast<std::uint32_t>(I(a,ins,0));if(op==634)b.fields_444[4]=as_bits(F(a,ins,0));break;}
    case 603:case 606:case 607:case 608:case 612:case 613:case 614:{const std::uint8_t kind=op==603?16:(op==606?20:(op==607?21:(op==608?22:(op==612?27:(op==613?26:28)))));at<std::uint8_t>(a,0x498)=kind;b.vector_70.x=F(a,ins,0);b.vector_70.y=F(a,ins,1);break;}
    case 604:case 605:case 617:case 619:case 628:case 629:{const std::uint8_t kind=op==604?17:(op==605?18:(op==617?31:(op==619?33:(op==628?42:43))));at<std::uint8_t>(a,0x498)=kind;b.vector_70.x=F(a,ins,0);const auto value=static_cast<std::uint32_t>(I(a,ins,1));b.fields_444[0]=op>=628?value<<1:value;break;}
    case 611:case 615:case 616:case 620:case 621:case 622:case 630:case 631:case 632:{const auto kind=op==611?19:(op==615?29:(op==616?30:(op==620?34:(op==621?35:(op==622?36:op-586)))));at<std::uint8_t>(a,0x498)=static_cast<std::uint8_t>(kind);b.vector_70.x=F(a,ins,0);b.vector_70.y=F(a,ins,1);const auto value=static_cast<std::uint32_t>(I(a,ins,2));b.fields_444[0]=op>=630?value<<1:value;break;}
    case 618:at<std::uint8_t>(a,0x498)=32;break;
    case 623:case 624:case 625:case 626:case 627:at<std::uint8_t>(a,0x498)=static_cast<std::uint8_t>(op-586);b.vector_70.x=F(a,ins,0);b.vector_70.y=F(a,ins,1);b.fields_444[4]=as_bits(F(a,ins,2));if(op>=625){b.fields_444[5]=as_bits(F(a,ins,3));b.fields_444[0]=static_cast<std::uint32_t>(I(a,ins,4));}else b.fields_444[0]=static_cast<std::uint32_t>(I(a,ins,3));break;
    // All 161 original non-default cases are above. The original jump table
    // explicitly routes every other opcode to 0x42b8d3 -> 0x434dad (advance).
    // This is recovered invalid-opcode behavior, not a missing case fallback.
    default:return Flow::advance;
    }
    return Flow::advance;
}
}
std::int32_t anm_integer_variable(Animation& a,std::int32_t v){
    if(v>=10000&&v<=10003)return n::signed_bits(a.base.fields_444[v-10000]);
    if(v>=10004&&v<=10007)return n::truncate32(as_float(a.base.fields_444[v-10000]));
    if(v==10008||v==10009)return at<std::int32_t>(a,0x470+(v-10008)*4);
    if(v==10022)return n::signed_bits(env::random_bounded(a.base.fields_444[15]));
    if(v==10027||v==10028)return n::truncate32(at<float>(a,0x478+(v-10027)*4));
    if(v==10029)return n::signed_bits(a.base.fields_444[15]);
    if(v>=10033&&v<=10035)return n::truncate32(at<float>(a,0x464+(v-10033)*4));
    return v;
}
float anm_float_variable(Animation& a,float value){
    const auto v=n::truncate32(value);
    if(v>=10000&&v<=10003)return n::int_float(n::signed_bits(a.base.fields_444[v-10000]));
    if(v>=10004&&v<=10007)return as_float(a.base.fields_444[v-10000]);
    if(v==10008||v==10009)return n::int_float(at<std::int32_t>(a,0x470+(v-10008)*4));
    if(v==10010||v==10030)return env::random_signed_unit()*as_float(a.base.fields_444[14]);
    if(v==10011||v==10031)return env::random_unit()*as_float(a.base.fields_444[13]);
    if(v==10012||v==10032)return env::random_signed_unit()*as_float(a.base.fields_444[13]);
    if(v>=10013&&v<=10015)return at<float>(a,0x2c+(v-10013)*4);
    if(v>=10016&&v<=10021)return env::camera_component(v);
    if(v==10022)return static_cast<float>(static_cast<double>(env::random_next()));
    if(v>=10023&&v<=10025)return at<float>(a,0x38+(v-10023)*4);
    if(v==10026)return inherited_animation_rotation(a).z;
    if(v==10027||v==10028)return at<float>(a,0x478+(v-10027)*4);
    if(v==10029)return n::int_float(n::signed_bits(a.base.fields_444[15]));
    if(v>=10033&&v<=10035)return at<float>(a,0x464+(v-10033)*4);
    return value;
}
std::uint32_t& anm_integer_destination(Animation& a,AnmInstruction& ins,unsigned index){
    auto& word=ins.arguments()[index];if(!(ins.mask&(1u<<(index&31))))return word;
    if(word>=10000&&word<=10003)return a.base.fields_444[word-10000];
    if(word==10008||word==10009)return at<std::uint32_t>(a,0x470+(word-10008)*4);
    if(word==10029)return a.base.fields_444[15];return word;
}
float& anm_float_destination(Animation& a,AnmInstruction& ins,unsigned index){
    auto& word=ins.arguments()[index];if(ins.mask&(1u<<(index&31))){
        const auto v=n::truncate32(as_float(word));
        if(v>=10004&&v<=10007)return at<float>(a,0x454+(v-10004)*4);
        if(v>=10013&&v<=10015)return at<float>(a,0x2c+(v-10013)*4);
        if(v>=10023&&v<=10025)return at<float>(a,0x38+(v-10023)*4);
        if(v==10027||v==10028)return at<float>(a,0x478+(v-10027)*4);
        if(v>=10033&&v<=10035)return at<float>(a,0x464+(v-10033)*4);
    }return reinterpret_cast<float&>(word);
}
float animation_slowdown(Animation& a) noexcept{
    if(!a.fields_550[2]||(a.base.flags[1]&0x1000))return as_float(a.fields_550[4]);
    return animation_slowdown(*reinterpret_cast<Animation*>(a.fields_550[2]));
}
bool implemented_anm_opcode(std::int16_t op) noexcept{
    return (op>=-1&&op<=7)||(op>=100&&op<=131)||(op>=200&&op<=213)||(op>=300&&op<=319)||(op>=400&&op<=441)||(op>=500&&op<=510)||(op>=600&&op<=634);
}
std::int32_t execute_animation(Animation& a){
    auto& b=a.base;if(b.flags[7]&1)return 0;
    float& scale=env::clock_scale();const float saved_scale=scale;
    struct Restore {float& value;float saved;~Restore(){value=saved;}} restore{scale,saved_scale};
    if(b.flags[1]&0x100)scale=1.f;
    if(animation_slowdown(a)>0.f){scale=saved_scale-animation_slowdown(a)*saved_scale;if(scale<0.f)scale=0.f;}
    if(a.fields_550[6]){
#ifdef TH_SDL3
        // MSVC vtable order (deleting dtor, update, draw, ...) is not the
        // Itanium order clang emits for wasm (complete dtor, deleting dtor,
        // update, ...); the raw table[1] would call the deleting destructor.
        // A real virtual call lands on update() under both ABIs.
        if(reinterpret_cast<AnimationCallback*>(a.fields_550[6])->update())return 1;
#else
        struct Callback {void** table;};auto* cb=reinterpret_cast<Callback*>(a.fields_550[6]);
        if(reinterpret_cast<std::int32_t(__thiscall*)(Callback*)>(cb->table[1])(cb))return 1;
#endif
    }
    if(a.field_5dc&&reinterpret_cast<std::int32_t(__cdecl*)(Animation*)>(a.field_5dc)(&a))return 1;
    if(n::signed_bits(b.fields_10_28[6])>=0&&!(b.flags[1]&0x2000000)){
        n::timer_tick(a.timer_4d8,env::timer_rate());
        bool interrupt=b.field_438!=0;
        if(interrupt||(at<std::uint8_t>(a,0x4a0)&3)!=1||!env::gameplay_frozen()){
            for(;;){
                auto* start=reinterpret_cast<unsigned char*>(env::script(a));
                auto* ins=reinterpret_cast<AnmInstruction*>(start+b.fields_10_28[6]);
                if(interrupt){
                    AnmInstruction* fallback=nullptr;std::uint32_t offset=0,fallback_offset=0;
                    auto* label=reinterpret_cast<AnmInstruction*>(start);
                    while((label->opcode!=5||label->arguments()[0]!=b.field_438)&&label->opcode!=-1){
                        if(label->opcode==5&&label->arguments()[0]==0xffffffffu){fallback=label;fallback_offset=offset;}
                        offset+=label->size;label=reinterpret_cast<AnmInstruction*>(start+offset);
                    }
                    b.field_438=0;bit(a,0x4a1,1,0);interrupt=false;
                    if(label->opcode!=5&&fallback){label=fallback;offset=fallback_offset;}
                    if(label->opcode==5){n::timer_set(b.timer,a.timer_4c8.current);b.fields_10_28[0]=b.fields_10_28[6];n::timer_set(a.timer_4c8,label->time);b.fields_10_28[6]=offset+label->size;bit(a,0x49a,1,1);continue;}
                    if(ins->opcode==3){n::timer_add(a.timer_4c8,-1.f,env::timer_rate());break;}
                    continue;
                }
                if(a.timer_4c8.current<ins->time)break;
                const auto flow=dispatch(a,*ins);
                if(flow==Flow::remove)return 1;if(flow==Flow::stop)return 0;if(flow==Flow::wait)break;
                if(flow==Flow::interrupt){interrupt=true;continue;}
                if(flow==Flow::advance)b.fields_10_28[6]+=ins->size;
            }
        }else return 0;
    }else return 0;
    if(b.flags[1]&0x80000)update_animation_motion(a);
    if(at<std::uint8_t>(a,0x4a1)&2)env::add_camera_offset(a.vector_5bc);
    if(b.flags[1]&0x800)snapshot_corners(a);
    update_animation_interpolations(a);update_animation_geometry(a);n::timer_tick(a.timer_4c8,env::timer_rate());return 0;
}
void update_animation_motion(Animation& a){
    auto& b=a.base;
    for(unsigned i=0;i<3;++i){const float speed=at<float>(a,0x44+i*4);if(speed!=0.f){auto& v=at<float>(a,0x38+i*4);v=m::wrap_angle(n::add32(v,n::mul32(env::clock_scale(),speed)));b.flags[1]|=2;}}
    for(unsigned i:{1u,0u}){const float speed=at<float>(a,0x60+i*4);if(speed!=0.f){auto& v=at<float>(a,0x50+i*4);v=n::add32(v,n::mul32(env::clock_scale(),speed));b.flags[1]|=4;}}
    for(unsigned i=0;i<2;++i){const float speed=at<float>(a,0x3a0+i*4);if(speed!=0.f){auto& v=at<float>(a,0x78+i*4);v=n::add32(v,n::mul32(env::clock_scale(),speed));if(v<2.f){if(v<0.f)v=n::add32(v,2.f);}else v-=2.f;}}
}
void sample_animation_interpolation(void* storage,unsigned count,bool integer,bool angle,void* output,const float* rate){
    if(count<1||count>3||(angle&&(count!=1||integer)))throw std::invalid_argument("Invalid animation interpolation domain");
    auto* words=static_cast<std::uint32_t*>(storage);auto& timer=*reinterpret_cast<n::Timer*>(words+5*count);
    auto& duration=*reinterpret_cast<std::int32_t*>(words+5*count+4);const auto mode=n::signed_bits(words[5*count+5]);
    auto* result=static_cast<std::uint32_t*>(output);
    if(!integer&&!angle){
        const n::Timer initial_timer=timer;const auto initial_duration=duration;
        for(unsigned i=0;i<count;++i){m::Interpolator p{};p.start=as_float(words[i]);p.end=as_float(words[count+i]);p.tangent_start=as_float(words[2*count+i]);p.tangent_end=as_float(words[3*count+i]);p.current=as_float(words[4*count+i]);p.timer=initial_timer;p.duration=initial_duration;p.mode=mode;
            result[i]=as_bits(p.sample(rate));words[i]=as_bits(p.start);words[3*count+i]=as_bits(p.tangent_end);words[4*count+i]=as_bits(p.current);timer=p.timer;duration=p.duration;}
        return;
    }
    if(duration>0){n::timer_tick(timer,rate);if(timer.current>=duration){n::timer_set(timer,duration);duration=0;}}
    if(duration==0){std::memcpy(result,words+(mode==7||mode==17?0:count),count*4);return;}
    const auto sub=[](float x,float y){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(x),_mm_set_ss(y)));};
    const auto div=[](float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));};
    const auto mul=n::mul32;const auto add=n::add32;
    const float t=div(timer.current_f,n::int_float(duration));
    const float bases[4]={mul(mul(sub(t,1.f),sub(t,1.f)),add(mul(2.f,t),1.f)),mul(mul(t,t),sub(3.f,mul(2.f,t))),mul(mul(sub(1.f,t),sub(1.f,t)),t),mul(mul(sub(t,1.f),t),t)};
    if(angle){
        float start=as_float(words[0]),end=as_float(words[1]),tangent_end=as_float(words[3]),value=0;
        if(mode==7){start=m::wrap_angle(add(start,end));value=start;}
        else if(mode==17){start=m::wrap_angle(add(start,tangent_end));tangent_end=m::wrap_angle(add(tangent_end,end));value=start;}
        else if(mode==8){value=m::wrap_angle(mul(start,bases[0]));for(unsigned i=1;i<4;++i)value=m::wrap_angle(add(value,m::wrap_angle(mul(as_float(words[i]),bases[i]))));}
        else {const float difference=m::wrap_angle(m::angle_difference(end,start));const float delta=m::wrap_angle(mul(difference,m::easing(mode,timer.current_f,n::int_float(duration))));value=m::wrap_angle(add(delta,start));}
        words[0]=as_bits(start);words[3]=as_bits(tangent_end);words[4]=as_bits(value);result[0]=words[4];return;
    }
    for(unsigned i=0;i<count;++i){
        auto& start=words[i];const auto end=words[count+i];auto& tangent_end=words[3*count+i];auto& value=words[4*count+i];
        if(mode==7){start+=end;value=start;}
        else if(mode==17){start+=tangent_end;tangent_end+=end;value=start;}
        else if(mode==8){
            if(count==1){ // Scalar int multiplies each control into its basis before summing.
                const float term0=mul(mul(mul(n::int_float(n::signed_bits(start)),sub(t,1.f)),sub(t,1.f)),add(mul(2.f,t),1.f));
                const float term1=mul(mul(mul(n::int_float(n::signed_bits(end)),t),t),sub(3.f,mul(2.f,t)));
                const float term2=mul(mul(mul(n::int_float(n::signed_bits(words[2])),sub(1.f,t)),sub(1.f,t)),t);
                const float term3=mul(mul(mul(n::int_float(n::signed_bits(tangent_end)),sub(t,1.f)),t),t);
                value=static_cast<std::uint32_t>(n::truncate32(add(add(add(term0,term1),term2),term3)));
            }else{
                value=0;for(unsigned term=0;term<4;++term)value+=static_cast<std::uint32_t>(n::truncate32(mul(n::int_float(n::signed_bits(words[term*count+i])),bases[term])));
            }
        }else{
            const float v=mul(n::int_float(n::signed_bits(end-start)),m::easing(mode,timer.current_f,n::int_float(duration)));
            value=count==1?static_cast<std::uint32_t>(n::truncate32(add(v,n::int_float(n::signed_bits(start))))):static_cast<std::uint32_t>(n::truncate32(v))+start;
        }
        result[i]=value;
    }
}
void update_animation_interpolations(Animation& a){
    const float* rate=env::timer_rate();std::uint32_t out[3]{};
    auto sample=[&](unsigned offset,unsigned count,bool integer,bool angle=false){auto* bytes=reinterpret_cast<unsigned char*>(&a)+offset;auto duration=reinterpret_cast<std::int32_t*>(bytes)[count*5+4];if(!duration)return false;sample_animation_interpolation(bytes,count,integer,angle,out,rate);return true;};
    auto copy=[&](unsigned offset,unsigned count){std::memcpy(reinterpret_cast<unsigned char*>(&a)+offset,out,count*4);};
    if(sample(0x8c,3,false))copy(a.base.flags[1]&0x40?0x484:0x2c,3);
    if(sample(0xe0,3,true))for(unsigned i=0;i<3;++i)at<std::uint8_t>(a,0x490+i)=static_cast<std::uint8_t>(at<std::uint32_t>(a,0xe0+0x30+i*4)); // current, not returned endpoint
    if(sample(0x134,1,true))at<std::uint8_t>(a,0x493)=static_cast<std::uint8_t>(out[0]);
    if(sample(0x1e0,2,false)){copy(0x50,2);a.base.flags[1]|=4;}
    if(sample(0x220,2,false)){copy(0x58,2);a.base.flags[1]|=4;}
    if(sample(0x260,2,false)){copy(0x68,2);a.base.flags[1]|=8;}
    if(sample(0x160,3,false)){copy(0x38,3);a.base.flags[1]|=2;}
    if(sample(0x1b4,1,false,true)){copy(0x40,1);a.base.flags[1]|=2;}
    if(sample(0x2a0,3,true))for(unsigned i=0;i<3;++i)at<std::uint8_t>(a,0x494+i)=static_cast<std::uint8_t>(at<std::uint32_t>(a,0x2a0+0x30+i*4));
    if(sample(0x2f4,1,true))at<std::uint8_t>(a,0x497)=static_cast<std::uint8_t>(out[0]);
    if(sample(0x320,1,false))copy(0x3a0,1);
    if(sample(0x34c,1,false))copy(0x3a4,1);
}
Vec3& inherited_animation_rotation(Animation& a){
    a.vector_5d0=a.base.vector_38;
    if(a.fields_550[2]&&!(a.base.flags[1]&0x1000)){
        const Vec3& inherited=inherited_animation_rotation(*reinterpret_cast<Animation*>(a.fields_550[2]));
        a.vector_5d0.x=n::add32(a.vector_5d0.x,inherited.x);a.vector_5d0.y=n::add32(a.vector_5d0.y,inherited.y);a.vector_5d0.z=n::add32(a.vector_5d0.z,inherited.z);
        a.base.vector_38.x=m::wrap_angle(a.base.vector_38.x);a.base.vector_38.y=m::wrap_angle(a.base.vector_38.y);a.base.vector_38.z=m::wrap_angle(a.base.vector_38.z);
    }return a.vector_5d0;
}
Vec3 animation_position(Animation& a){
    const auto& b=a.base;Vec3 result{n::add32(n::add32(a.vector_5bc.x,b.vector_2c.x),b.vector_484.x),n::add32(n::add32(a.vector_5bc.y,b.vector_2c.y),b.vector_484.y),n::add32(n::add32(a.vector_5bc.z,b.vector_2c.z),b.vector_484.z)};
    const auto mode=at<std::uint8_t>(a,0x4a4);
    if(mode>=1&&mode<=4){float factor=env::screen_scale();if(mode==2||mode==4)factor=n::mul32(factor,.5f);result.x=n::mul32(result.x,factor);result.y=n::mul32(result.y,factor);result.z=n::mul32(result.z,factor);}
    if(a.fields_550[2]&&!(b.flags[1]&0x1000)){
        auto& parent=*reinterpret_cast<Animation*>(a.fields_550[2]);
        if(b.flags[1]&0x20)m::rotate(result.x,result.y,parent.base.vector_38.z);
        if(b.flags[1]&0x400000){result.x=n::mul32(result.x,parent.base.vector_50.x);result.y=n::mul32(result.y,parent.base.vector_50.y);}
        const auto inherited=animation_position(parent);result.x=n::add32(result.x,inherited.x);result.y=n::add32(result.y,inherited.y);result.z=n::add32(result.z,inherited.z);
    }else{
        const unsigned preset=at<std::uint8_t>(a,0x4a3)&3;if(preset){result.x=n::add32(result.x,n::int_float(env::screen_offset(preset==1?0:1,0)));result.y=n::add32(result.y,n::int_float(env::screen_offset(preset==1?0:1,1)));}
    }return result;
}
void transform_animation_offset(Animation& a,Vec3& v,bool rotate,bool scale){
    if(a.fields_550[2]&&!(a.base.flags[1]&0x1000))transform_animation_offset(*reinterpret_cast<Animation*>(a.fields_550[2]),v,(a.base.flags[1]&0x20)!=0,(a.base.flags[1]&0x400000)!=0);
    if(rotate)m::rotate(v.x,v.y,a.base.vector_38.z);
    if(scale){v.x=n::mul32(v.x,a.base.vector_50.x);v.y=n::mul32(v.y,a.base.vector_50.y);}
}
void update_animation_geometry(Animation& a){
    auto& b=a.base;const auto type=at<std::uint8_t>(a,0x498);
    if(type!=9&&type!=13&&type!=14&&type!=24&&type!=25&&type!=47&&type!=48)return; // original switch default
    auto* vertices=reinterpret_cast<float*>(a.fields_550[5]);const auto count=n::signed_bits(b.fields_444[0]);
    constexpr float pi=3.1415927410125732421875f;
    const float u_offset=as_float(b.field_78),v_offset=as_float(b.field_7c);
    const std::uint32_t first_color=b.field_490,second_color=(at<std::uint8_t>(a,0x4a1)&0x1c)?b.field_494:b.field_490;
    b.flags[1]&=~0x200000u;
    auto write_color=[](float* address,std::uint32_t color){std::memcpy(address,&color,4);};
    auto scaled_radii=[&](float& first,float& second){
        if(a.fields_550[3]&&!(b.flags[1]&0x1000)){auto& parent=*reinterpret_cast<Animation*>(a.fields_550[3]);first=n::mul32(parent.base.vector_50.x,first);second=n::mul32(parent.base.vector_50.y,second);}
        const auto mode=at<std::uint8_t>(a,0x4a4);if(mode==1){first=n::mul32(first,env::screen_scale());second=n::mul32(second,env::screen_scale());}else if(mode==2){first=n::mul32(n::mul32(env::screen_scale(),.5f),first);second=n::mul32(n::mul32(env::screen_scale(),.5f),second);}
    };
    if(type==9||type==13||type==14||type==47){
        const bool closed=type==9||type==47;const unsigned stride=type==47?6:7;const unsigned color_slot=stride==7?4:3,uv_slot=color_slot+1;
        float angle=closed?b.vector_38.z:m::wrap_angle(b.vector_38.z-b.vector_38.x/2.f);
        const float angle_step=closed?(pi*2.f)/n::int_float(count-1):b.vector_38.x/n::int_float(count-1);
        const float uv_step=n::int_float(n::signed_bits(b.fields_444[1]))/n::int_float(count-1);float uv_progress=0.f;
        Vec3 center{};if(type!=47)center=animation_position(a);if(type==14)angle=m::wrap_angle(b.vector_38.z);
        const float thickness=type==47?as_float(b.fields_444[4]):b.vector_50.x,average=type==47?as_float(b.fields_444[5]):b.vector_50.y;
        float radius_first=n::add32(n::mul32(thickness,.5f),average),radius_second=average-n::mul32(thickness,.5f);scaled_radii(radius_first,radius_second);
        float* cursor=vertices;const auto iterations=closed?count-1:count;
        for(std::int32_t i=0;i<iterations;++i){
            for(unsigned side=0;side<2;++side){
                if(stride==7)cursor[3]=1.f;write_color(cursor+color_slot,closed?(side?second_color:first_color):second_color);
                cursor[uv_slot]=n::add32(b.vectors_378[side].x,u_offset);cursor[uv_slot+1]=n::add32(uv_progress,v_offset);
                m::polar(cursor[0],cursor[1],angle,side?radius_second:radius_first);cursor[2]=0.f;
                cursor[0]=n::add32(cursor[0],center.x);cursor[1]=n::add32(cursor[1],center.y);cursor[2]=n::add32(cursor[2],center.z);cursor+=stride;
            }
            uv_progress=n::add32(uv_progress,uv_step);angle=m::wrap_angle(n::add32(angle,angle_step));
        }
        if(closed){std::memcpy(cursor,vertices,stride*4);cursor[uv_slot+1]=n::add32(uv_progress,v_offset);std::memcpy(cursor+stride,vertices+stride,stride*4);cursor[stride+uv_slot+1]=n::add32(uv_progress,v_offset);}
    }else if(type==24||type==25){
        const float sweep=as_float(b.fields_444[4]);float angle=m::wrap_angle(as_float(b.fields_444[7])-sweep/2.f);
        const float angle_step=sweep/n::int_float(count-1),uv_step=n::int_float(n::signed_bits(b.fields_444[1]))/n::int_float(count-1);float uv_progress=0.f;
        const float width=as_float(b.fields_444[5]),radius=as_float(b.fields_444[6]);float height=width/2.f,first_radius=radius,second_radius=radius;
        if(type==25){height=0;first_radius-=width/2.f;second_radius=n::add32(width/2.f,second_radius);}
        float* cursor=vertices;
        for(std::int32_t i=0;i<count;++i){for(unsigned side=0;side<2;++side){float x=0,y=0;m::polar(x,y,angle,side?second_radius:first_radius);write_color(cursor+3,second_color);cursor[4]=n::add32(b.vectors_378[side].x,u_offset);cursor[5]=n::add32(uv_progress,v_offset);cursor[0]=x;cursor[1]=side?-height:height;cursor[2]=y;cursor+=6;}uv_progress=n::add32(uv_progress,uv_step);angle=m::wrap_angle(n::add32(angle,angle_step));}
    }else{
        float radius=as_float(b.fields_444[4]);if(a.fields_550[3]&&!(b.flags[1]&0x1000))radius=n::mul32(reinterpret_cast<Animation*>(a.fields_550[3])->base.vector_50.x,radius);
        const auto mode=at<std::uint8_t>(a,0x4a4);if(mode==1)radius=n::mul32(radius,env::screen_scale());else if(mode==2)radius=n::mul32(n::mul32(env::screen_scale(),.5f),radius);
        write_color(vertices+3,first_color);write_color(vertices+9,second_color);write_color(vertices+15,second_color);
        vertices[4]=n::add32(n::add32((b.vectors_378[1].x-b.vectors_378[0].x)/2.f,b.vectors_378[0].x),u_offset);vertices[5]=n::add32(b.vectors_378[0].y,v_offset);
        vertices[10]=n::add32(b.vectors_378[2].x,u_offset);vertices[11]=n::add32(b.vectors_378[2].y,v_offset);vertices[16]=n::add32(b.vectors_378[3].x,u_offset);vertices[17]=n::add32(b.vectors_378[3].y,v_offset);
        m::polar(vertices[0],vertices[1],-pi/2.f,radius);m::polar(vertices[6],vertices[7],n::add32(-pi/2.f,n::mul32((pi*2.f)/3.f,2.f)),radius);m::polar(vertices[12],vertices[13],n::add32(-pi/2.f,(pi*2.f)/3.f),radius);
        vertices[2]=vertices[8]=vertices[14]=0.f;for(unsigned base:{6u,12u})for(unsigned component=0;component<3;++component)vertices[base+component]-=vertices[component];vertices[0]=vertices[1]=vertices[2]=0.f;
    }
}
}
