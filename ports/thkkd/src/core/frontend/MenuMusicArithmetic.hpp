// 菜单X坐标按原x87精度加减1；6968C0使用已验证的SSE截断路径。
#pragma once
#include "core/math/BinaryFloat.hpp"
#include <cstring>
namespace kaikeidou::menu_music_detail {
inline std::int32_t signed_word(std::uint32_t value){std::int32_t result;std::memcpy(&result,&value,4);return result;}
inline bool nan(std::uint64_t bits){return (bits&0x7fffffffffffffffull)>0x7ff0000000000000ull;}
inline bool above60(std::uint64_t bits){return !nan(bits)&&!(bits>>63)&&bits>0x404e000000000000ull;}
inline bool below64(std::uint64_t bits){return !nan(bits)&&((bits>>63)||bits<0x4050000000000000ull);}
inline std::uint64_t step(std::uint64_t bits,bool subtract,unsigned precision){
    if((bits&0x7fffffffffffffffull)==0x7ff0000000000000ull)return bits;
    return binary_float::to_binary64(binary_float::add(binary_float::binary64(bits),
        binary_float::integer(subtract?0xffffffffu:1u),precision));
}
inline std::int32_t coordinate(std::uint64_t bits){
    if((bits&0x7ff0000000000000ull)==0x7ff0000000000000ull)return signed_word(0x80000000u);
    const auto value=binary_float::binary64(bits);
    if(value.exponent>=31)return signed_word(0x80000000u);
    return signed_word(binary_float::truncate(value));
}
}
