// 原binary64常量按位冻结。
#pragma once
#include <cstdint>
#include <stdexcept>
namespace kaikeidou::ex_boss_opening_projectile_detail {
inline std::uint64_t constant(std::uint32_t a){switch(a){
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1ba8:return 0x402e000000000000ull;
default:throw std::out_of_range("EX opening projectile constant");}}
}
