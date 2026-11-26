#pragma once
#include "DisplayFunctions.hpp"
#include "core/runtime/GameStartEntryMemory.hpp"
#include "core/frontend/MenuScenePlatform.hpp"
#include "core/math/Random.hpp"
#include "core/math/GameMath.hpp"
#include <array>
#include <cmath>
#include <cstring>
#include <functional>
#include <limits>
#include <stdexcept>

namespace kaikeidou {
enum class Alu {add,sub,xor_,and_,or_,shl,sar,shr};
// 显示在帧快照上计算，持久状态继续由已验证的游戏内核统一推进。
struct DisplayState {
    using U=std::uint32_t;using Q=std::uint64_t;
    std::vector<std::uint8_t> data;
    std::array<U,8> reg{};
    std::array<long double,16> floating_stack{};
    unsigned stack_size=0,precision=64;
    U pc=0,control=0x37f,fstatus=0;
    bool z=false,s=false,o=false,c=false,p=false;
    std::function<MenuScenePlatformResult(const MenuSceneRequest&)> platform;
    std::function<U(U)> input;
    DisplayState();
    void snapshot(GameStartEntryMemory& source);
    [[noreturn]] void fail(const char* reason)const;
    template<unsigned N>Q load(U address)const{
        if(Q(address)+N>data.size())fail("display read");Q value=0;std::memcpy(&value,data.data()+address,N);return value;
    }
    template<unsigned N>void store(U address,Q value){
        if(Q(address)+N>data.size())fail("display write");std::memcpy(data.data()+address,&value,N);
    }
    template<unsigned N>long double floating(U address)const{
        if constexpr(N==4){U bits=U(load<4>(address));float v;std::memcpy(&v,&bits,4);return v;}
        else{Q bits=load<8>(address);double v;std::memcpy(&v,&bits,8);return v;}
    }
    template<unsigned N>long double integer(U address)const{
        if constexpr(N==2)return std::int16_t(load<N>(address));
        else if constexpr(N==4)return std::int32_t(load<N>(address));
        else return static_cast<long double>(std::int64_t(load<N>(address)));
    }
    template<unsigned N>void fstore(U address,long double number){
        if constexpr(N==4){float v=float(number);U bits;std::memcpy(&bits,&v,4);store<4>(address,bits);}
        else{double v=double(number);Q bits;std::memcpy(&bits,&v,8);store<8>(address,bits);}
    }
    void push(U value){reg[4]-=4;store<4>(reg[4],value);}
    U pop(){U value=U(load<4>(reg[4]));reg[4]+=4;return value;}
    void fpush(long double value){if(stack_size==floating_stack.size())fail("display FPU overflow");floating_stack[stack_size++]=value;}
    long double& fp(unsigned index){if(index>=stack_size)fail("display FPU underflow");return floating_stack[stack_size-index-1];}
    long double fpop(){auto value=fp(0);--stack_size;return value;}
    long double round_operation(long double value)const{return precision==24?static_cast<long double>(float(value)):static_cast<long double>(double(value));}
    void compare_fpu(long double right){auto left=fp(0);fstatus=std::isnan(left)||std::isnan(right)?0x4500u:left<right?0x100u:left==right?0x4000u:0;}
    U round_integer();
    U alu(Alu operation,U left,U right,unsigned width);
    void divide(U divisor);
    std::string text(U address)const;
    std::string format(U pattern,U arguments);
    void external(U entry);
};
void dispatch_display(DisplayState&,std::uint32_t entry);
}
