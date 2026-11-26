#include "DisplayState.hpp"
#include <cstdio>
#include <sstream>

namespace kaikeidou {
namespace {
constexpr std::uint8_t constants[]={
#include "DisplayConstants.inc"
};
constexpr std::uint32_t jump_tables[][2]={
#include "DisplayJumpTables.inc"
};
}
DisplayState::DisplayState():data(0x1900000){std::copy(std::begin(constants),std::end(constants),data.begin()+0x6a0000);for(auto& entry:jump_tables)store<4>(entry[0],entry[1]);}
void DisplayState::snapshot(GameStartEntryMemory& source){
    source.reconcile();
    for(auto region:source.regions()){
        if(std::uint64_t(region[0])+region[1]>data.size())continue;
        auto bytes=source.read(region[0],region[1]);std::copy(bytes.begin(),bytes.end(),data.begin()+region[0]);
    }
    reg.fill(0);reg[4]=0x1808000;reg[5]=reg[4];stack_size=0;control=precision==24?0x7f:precision==53?0x27f:0x37f;fstatus=0;
}
[[noreturn]]void DisplayState::fail(const char* reason)const{
    std::ostringstream message;message<<reason<<" at "<<std::hex<<pc;throw std::runtime_error(message.str());
}
DisplayState::U DisplayState::alu(Alu operation,U left,U right,unsigned width){
    U mask=width==32?0xffffffffu:(1u<<width)-1u,sign=1u<<(width-1);left&=mask;right&=mask;U result=0;
    switch(operation){
    case Alu::add:{auto n=Q(left)+right;result=U(n)&mask;c=n>mask;o=(~(left^right)&(left^result)&sign)!=0;break;}
    case Alu::sub:result=(left-right)&mask;c=left<right;o=((left^right)&(left^result)&sign)!=0;break;
    case Alu::xor_:result=left^right;c=o=false;break;
    case Alu::and_:result=left&right;c=o=false;break;
    case Alu::or_:result=left|right;c=o=false;break;
    case Alu::shl:{auto shift=right&31;result=(left<<shift)&mask;c=shift?((left>>(width-shift))&1)!=0:c;o=((result&sign)!=0)!=c;break;}
    case Alu::sar:{auto shift=right&31;result=U(std::int32_t(left)>>shift)&mask;c=shift?((left>>(shift-1))&1)!=0:c;o=false;break;}
    case Alu::shr:{auto shift=right&31;result=left>>shift;c=shift?((left>>(shift-1))&1)!=0:c;o=(left&sign)!=0;break;}
    }
    z=result==0;s=(result&sign)!=0;U bits=result&255;bits^=bits>>4;bits^=bits>>2;bits^=bits>>1;p=(bits&1)==0;return result;
}
void DisplayState::divide(U divisor){
    auto denominator=std::int32_t(divisor);if(!denominator)fail("display division by zero");
    auto dividend=std::int64_t((Q(reg[2])<<32)|reg[0]);reg[0]=U(dividend/denominator);reg[2]=U(dividend%denominator);
}
DisplayState::U DisplayState::round_integer(){
    auto value=fp(0);switch((control>>10)&3){case 0:value=std::nearbyint(value);break;case 1:value=std::floor(value);break;case 2:value=std::ceil(value);break;case 3:value=std::trunc(value);break;}
    return !std::isfinite(value)||value< -2147483648.0L||value>=2147483648.0L?0x80000000u:U(std::int32_t(value));
}
std::string DisplayState::text(U address)const{
    std::string result;for(U i=0;i<65536;++i){auto byte=load<1>(address+i);if(!byte)return result;result.push_back(char(byte));}fail("display string length");
}
std::string DisplayState::format(U pattern,U arguments){
    std::string result,fmt=text(pattern);U at=arguments;
    for(std::size_t i=0;i<fmt.size();++i){
        if(fmt[i]!='%'){result+=fmt[i];continue;}
        if(i+1<fmt.size()&&fmt[i+1]=='%'){result+='%';++i;continue;}
        auto start=i;while(++i<fmt.size()&&std::string("diuxXofFeEgGsc").find(fmt[i])==std::string::npos){}
        if(i==fmt.size())fail("display format specification");
        auto spec=fmt.substr(start,i-start+1);char output[4096]{};char type=fmt[i];
        if(type=='s'){auto value=text(U(load<4>(at)));std::snprintf(output,sizeof(output),spec.c_str(),value.c_str());at+=4;}
        else if(std::string("fFeEgG").find(type)!=std::string::npos){std::snprintf(output,sizeof(output),spec.c_str(),double(floating<8>(at)));at+=8;}
        else{std::snprintf(output,sizeof(output),spec.c_str(),U(load<4>(at)));at+=4;}
        result+=output;
    }
    return result;
}
void DisplayState::external(U entry){
    const auto sp=reg[4];auto a=[&](unsigned i){return U(load<4>(sp+4+i*4));};
    auto f=[&](unsigned i){return floating<8>(sp+4+i*4);};
    switch(entry){
    case 0x401000:{auto base=f(0);auto exponent=std::int32_t(a(2));long double result=1;unsigned count=unsigned(exponent<0?-exponent:exponent);while(count){if(count&1)result=double(result*base);count>>=1;if(count)base=double(base*base);}fpush(exponent<0?1/result:result);break;}
    case 0x4f3690:break;
    case 0x4f36d4:fpush(std::sqrt(f(0)));break;
    case 0x4f3780:{auto bits=crt_sse2::cosine_bits(load<8>(sp+4));double v;std::memcpy(&v,&bits,8);fpush(v);break;}
    case 0x4f38b0:fpush(std::floor(f(0)));break;
    case 0x4f39d1:reg[0]=std::int32_t(a(0))<0?0u-a(0):a(0);break;
    case 0x4f39e1:fpush(std::fabs(f(0)));break;
    case 0x4f3aa0:{auto bits=crt_sse2::sine_bits(load<8>(sp+4));double v;std::memcpy(&v,&bits,8);fpush(v);break;}
    case 0x4f3f40:{auto value=text(a(1));for(U i=0;i<=value.size();++i)store<1>(a(0)+i,i<value.size()?std::uint8_t(value[i]):0);reg[0]=a(0);break;}
    case 0x4f4160:{for(U i=0;i<a(2);++i)store<1>(a(0)+i,a(1));reg[0]=a(0);break;}
    case 0x4f41da:{auto value=format(a(1),sp+12);for(U i=0;i<=value.size();++i)store<1>(a(0)+i,i<value.size()?std::uint8_t(value[i]):0);reg[0]=U(value.size());break;}
    case 0x5e3440:{Random random(U(load<4>(0xa74c08)));auto bits=random.real_bits(load<8>(sp+4),static_cast<RandomPrecision>(precision));double v;std::memcpy(&v,&bits,8);fpush(v);store<4>(0xa74c08,random.cursor());break;}
    case 0x5e3490:{Random random(U(load<4>(0xa74c08)));reg[0]=random.integer_bits(a(0),static_cast<RandomPrecision>(precision),true);store<4>(0xa74c08,random.cursor());break;}
    case 0x656c70:reg[0]=input(a(0));break;
    case 0x6968c0:{auto v=fpop();reg[0]=!std::isfinite(v)||v< -2147483648.0L||v>=2147483648.0L?0x80000000u:U(std::int32_t(v));break;}
    default:{
        MenuSceneRequest request{static_cast<MenuSceneApi>(entry),{},{},{}};unsigned count=0;std::string string;
        switch(entry){
        case 0x404830:count=2;break;case 0x4048a0:case 0x4049a0:count=3;break;
        case 0x406690:count=3;break;case 0x406770:count=2;break;case 0x4067e0:count=1;break;
        case 0x40dd90:case 0x41bb70:case 0x41dd00:count=4;break;
        case 0x41c3c0:case 0x41c910:count=9;break;
        case 0x41e460:count=5;break;case 0x41e5b0:count=8;break;case 0x421700:count=10;break;
        case 0x421760:case 0x42d190:count=6;break;
        case 0x43e010:case 0x43fd40:count=2;break;case 0x43e0d0:case 0x440240:count=1;break;case 0x4425f0:count=3;break;
        case 0x437c20:string=text(a(1));request.words={a(0)};request.texts={string};break;
        case 0x437c90:string=text(a(2));request.words={a(0),a(1),a(3),a(4),a(5),a(6)};request.texts={string};break;
        case 0x437f00:string=format(a(3),sp+20);request.words={a(0),a(1),a(2)};request.texts={string};break;
        case 0x4434d0:string=text(a(0));request.words={a(1),a(2)};request.texts={string};break;
        default:fail("unknown display platform function");
        }
        for(unsigned i=0;i<count;++i)request.words.push_back(a(i));
        if(entry==0x41dd00){auto size=Q(a(1))*3*24;if(Q(a(0))+size>data.size())fail("display vertices");request.bytes.assign(data.begin()+a(0),data.begin()+a(0)+size);}
        auto result=platform(request);reg[0]=result.eax;break;
    }
    }
    pop();
}
}
