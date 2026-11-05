#pragma once
#include "ExactFloat.hpp"
namespace touhou::numeric {
// Plain float values cross the game API. Unusual exponent/rounding cases use
// the original number implementation instead of silently changing a replay.
template<class Extended,bool (*normal_mode)()> struct ScalarMath {
    template<int operation> static float binary(float a,float b) noexcept{
        float out;if(normal_mode()&&scalar_binary(a,b,out,operation))return out;
        const auto x=Extended::from_float(a),y=Extended::from_float(b);
        if constexpr(operation==0)return (x+y).to_float();
        if constexpr(operation==1)return (x-y).to_float();
        if constexpr(operation==2)return (x*y).to_float();
        return (x/y).to_float();
    }
    static float add(float a,float b)noexcept{return binary<0>(a,b);}
    static float sub(float a,float b)noexcept{return binary<1>(a,b);}
    static float mul(float a,float b)noexcept{return binary<2>(a,b);}
    static float div(float a,float b)noexcept{return binary<3>(a,b);}
    static bool multiply_integer(std::int32_t count,float step,float& out)noexcept{
        return count>=-16777216&&count<=16777216&&normal_mode()&&scalar_binary(static_cast<float>(count),step,out,2);
    }
    static float mul_int(std::int32_t count,float step)noexcept{
        float out;if(multiply_integer(count,step,out))return out;
        return (Extended::from_int(count)*Extended::from_float(step)).to_float();
    }
    static std::int32_t mul_int_truncate(std::int32_t count,float step)noexcept{
        float out;if(multiply_integer(count,step,out))return truncate(out);
        return (Extended::from_int(count)*Extended::from_float(step)).truncate_int();
    }
    template<bool subtract> static float scaled(float base,std::int32_t count,float step)noexcept{
        float delta,out;
        if(multiply_integer(count,step,delta)&&(subtract?scalar_binary(base,delta,out,1):scalar_binary(delta,base,out,0)))return out;
        const auto product=Extended::from_int(count)*Extended::from_float(step);
        return (subtract?Extended::from_float(base)-product:product+Extended::from_float(base)).to_float();
    }
    static float add_scaled(float base,std::int32_t count,float step)noexcept{return scaled<false>(base,count,step);}
    static float sub_scaled(float base,std::int32_t count,float step)noexcept{return scaled<true>(base,count,step);}
    static std::int32_t truncate(float x)noexcept{
        if(x>=-2147483648.f&&x<2147483648.f)return static_cast<std::int32_t>(x);
        return Extended::from_float(x).truncate_int();
    }
};
}
