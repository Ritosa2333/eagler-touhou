// 原角色名称编码及初始化用binary64存储常量。
#pragma once
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace kaikeidou::game_start_entry_detail {
inline std::string_view name(std::uint32_t selector){switch(selector){
case 0:return {"\xb3\xe0\xb7\xfb\xa1\xb8\xba\xec\xcd\xe2\xbd\xe1\xbd\xe7\xa1\xb9",16};
case 1:return {"\xd6\xec\xb7\xfb\xa1\xb8\xc3\xce\xcf\xeb\xc3\xee\xd6\xec\xa1\xb9",16};
case 2:return {"\xe7\xb3\xb7\xfb\xa1\xb8\xd2\xf5\xd1\xf4\xe7\xb3\xd3\xf1\xa1\xb9",16};
case 10:return {"\xc7\xe0\xb7\xfb\xa1\xb8\xcc\xec\xc0\xc7\xd0\xc7\xb5\xc4\xd4\xcb\xb6\xaf\xa1\xb9",20};
case 11:return {"\xc0\xb6\xb7\xfb\xa1\xb8\xd5\xbf\xc0\xb6\xbb\xf0\xbb\xa8\xa1\xb9",16};
case 12:return {"\xb2\xd4\xb7\xfb\xa1\xb8\xb2\xd4\xcc\xec\xc8\xd5\xd5\xd5\xa1\xb9",16};
case 20:return {"\xc2\xcc\xb7\xfb\xa1\xb8\xb4\xba\xd6\xae\xb6\xab\xb7\xe7\xa1\xb9",16};
case 21:return {"\xb4\xe4\xb7\xfb\xa1\xb8\xf4\xe4\xb4\xe4\xc9\xab\xd6\xae\xba\xa3\xa1\xb9",18};
case 22:return {"\xb1\xcc\xb7\xfb\xa1\xb8\xb1\xcc\xc2\xcc\xc9\xc1\xb9\xe2\xa1\xb9",16};
default:return {};}}
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6d1a60:return 0x4068000000000000ull;
case 0x6d1cb0:return 0x4078000000000000ull;
case 0x6d1960:return 0x3fb999999999999aull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6a7708:return 0x3f847ae147ae147bull;
case 0x6d1b60:return 0x3ff921fb54442d28ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1d18:return 0x4048000000000000ull;
case 0x6d1878:return 0x4050000000000000ull;
case 0x6d1a90:return 0x4054000000000000ull;
case 0x6d1a18:return 0x3fb47ae147ae147bull;
case 0x6d1918:return 0x3fa999999999999aull;
case 0x6d1968:return 0x3f9eb851eb851eb8ull;
case 0x6d1a08:return 0x3f947ae147ae147bull;
default:throw std::out_of_range("Game start constant address");}}
}
