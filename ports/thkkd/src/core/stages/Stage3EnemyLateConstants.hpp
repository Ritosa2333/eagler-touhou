// 原三面后半段敌行为使用的精确binary64常量。
#pragma once
#include <cstdint>
namespace kaikeidou { namespace stage3_enemy_late_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1948:return 0x4000000000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d19e8:return 0x4079400000000000ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6d1b18:return 0x4004000000000000ull;
case 0x6d1c80:return 0x3ffb333333333333ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d1940:return 0x3fef5c28f5c28f5cull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1de0:return 0xbff921fb54442d28ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d1f50:return 0x3ff199999999999aull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d19f8:return 0x407d400000000000ull;
case 0x6d1970:return 0x3fed1eb851eb851full;
case 0x6d1a98:return 0x4007333333333333ull;
case 0x6d2130:return 0x4000cccccccccccdull;
case 0x6d2548:return 0x4003333333333333ull;
case 0x6d1e40:return 0x3ff999999999999aull;
case 0x6d1f78:return 0x3fe199999999999aull;
case 0x6d2af0:return 0x3fef4bc6a7ef9db2ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d24f0:return 0x3f9ba5e353f7ced9ull;
case 0x6d2488:return 0x3ff4cccccccccccdull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
case 0x6d1858:return 0x4066800000000000ull;
default:return 0;}}
}}
