// 原六面中Boss时间轴与敌弹使用的精确binary64常量。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace kaikeidou { namespace stage6_midboss_detail {
inline constexpr std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d19f0:return 0xc034000000000000ull;
case 0x6d1cc0:return 0x4062c00000000000ull;
case 0x6d1d50:return 0x4051800000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1a88:return 0x4049000000000000ull;
case 0x6d21b8:return 0xc018000000000000ull;
case 0x6d1af0:return 0x406c000000000000ull;
case 0x6d24a8:return 0x400d21fb54442d28ull;
case 0x6d1ad8:return 0x4008000000000000ull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1ae8:return 0x4044000000000000ull;
case 0x6d23e0:return 0x4075800000000000ull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1b38:return 0x4056800000000000ull;
case 0x6d21f0:return 0x4060400000000000ull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1858:return 0x4066800000000000ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d2400:return 0x402c000000000000ull;
case 0x6d1ba8:return 0x402e000000000000ull;
case 0x6d1e38:return 0x4089000000000000ull;
case 0x6d1c58:return 0x4058000000000000ull;
case 0x6b25f8:return 0x403e000000000000ull;
case 0x6d1868:return 0x3ff0000000000000ull;
case 0x6d2180:return 0x3ffe666666666666ull;
case 0x6d24a0:return 0xbff6666666666666ull;
case 0x6d1b28:return 0x3ff6666666666666ull;
case 0x6d19c0:return 0x3fe0000000000000ull;
case 0x6d1f08:return 0x4061800000000000ull;
case 0x6d2160:return 0x4076c00000000000ull;
case 0x6d2280:return 0xbffe666666666666ull;
case 0x6d1f98:return 0x407ac00000000000ull;
case 0x6d2498:return 0x40b1580000000000ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d1b58:return 0xc03e000000000000ull;
case 0x6d1a48:return 0x407c800000000000ull;
case 0x6d1c60:return 0x40b88b0000000000ull;
case 0x6d1950:return 0x3f50624dd2f1a9fcull;
case 0x6d1af8:return 0x4072c00000000000ull;
case 0x6d1ab0:return 0x4069000000000000ull;
case 0x6d22e0:return 0xbfd999999999999aull;
case 0x6d1a30:return 0xc020000000000000ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d21e8:return 0x3f86f0068db8bac7ull;
case 0x6d21c0:return 0x3ef4f8b588e368f1ull;
case 0x6d21e0:return 0x3f7cac083126e979ull;
case 0x6d1a28:return 0x4078800000000000ull;
case 0x6d1a78:return 0x4079000000000000ull;
case 0x6d21d8:return 0x4040800000000000ull;
case 0x6d21d0:return 0x403c000000000000ull;
case 0x6d21c8:return 0x3f826e978d4fdf3bull;
case 0x6b2620:return 0x3f747ae147ae147bull;
case 0x6d1908:return 0x4018000000000000ull;
case 0x6d1f38:return 0x4099000000000000ull;
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d1a80:return 0xc030000000000000ull;
case 0x6d1b88:return 0x4028000000000000ull;
case 0x6d21b0:return 0xc04d000000000000ull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d1c18:return 0x4010000000000000ull;
case 0x6d1a08:return 0x3f947ae147ae147bull;
case 0x6d21a8:return 0x3f0a36e2eb1c432dull;
case 0x6d21a0:return 0x3f6cac083126e979ull;
case 0x6d1b08:return 0x3fef0a3d70a3d70aull;
case 0x6d2140:return 0x3fe0c152382d7370ull;
case 0x6d2198:return 0x3fd0c152382d7370ull;
case 0x6d2190:return 0x40a05c0000000000ull;
case 0x6d2188:return 0x40af400000000000ull;
case 0x6d1eb0:return 0x4042000000000000ull;
case 0x6d1e50:return 0x408f400000000000ull;
case 0x6d1c80:return 0x3ffb333333333333ull;
case 0x6d1bc8:return 0x407fa00000000000ull;
case 0x6d1bc0:return 0x4073600000000000ull;
case 0x6d1bd0:return 0x3fefae147ae147aeull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1a20:return 0x3fee666666666666ull;
case 0x6d1a00:return 0x3ff3333333333333ull;
case 0x6d2208:return 0x401921fb54442d28ull;
case 0x6a7eb8:return 0x406fe00000000000ull;
case 0x6d1860:return 0x400921fb54442d28ull;
default:return 0;}}
inline const char* spell_name(std::uint32_t a){switch(a){
case 0x6c62b0:return "\xa1\xb8\xba\xec\xd3\xeb\xd7\xcf\xb5\xc4\xbe\xb3\xbd\xe7\xa1\xb9";
case 0x6c62c4:return "\xa1\xb8\xc0\xb6\xd3\xeb\xb3\xe0\xb5\xc4\xc6\xbd\xba\xe2\xa1\xb9";
case 0x6c62d8:return "\xa1\xb8\xc2\xcc\xd3\xeb\xd7\xcf\xb5\xc4\xb6\xd4\xc5\xbc\xa1\xb9";
case 0x6c62ec:return "\xa1\xb8\x53\x69\x6c\x65\x6e\x74\x20\x53\x69\x72\x65\x6e\xa1\xb9";
case 0x6c6304:return "\xa1\xb8\x53\x69\x6c\x65\x6e\x74\x20\x53\x69\x72\x65\x6e\xa1\xb9";
case 0x6c631c:return "\xa1\xb8\xce\xde\xd1\xd4\xb5\xc4\xb8\xe8\xbc\xa7\xa1\xb9";
case 0x6c632c:return "\xa1\xb8\xc3\xce\xbb\xc3\xb5\xc4\xc5\xdd\xc4\xad\xa1\xb9";
default:throw std::out_of_range("Stage6 spell string");}}
}}
