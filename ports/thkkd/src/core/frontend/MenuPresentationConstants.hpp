// 原程序常量与GBK文本字节，由menu_presentation_extract.py生成。
#pragma once
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace kaikeidou::menu_presentation_detail {
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6b25b8:return 0x4059000000000000ull;
case 0x6d18e8:return 0x4020000000000000ull;
case 0x6d18f0:return 0x3feccccccccccccdull;
case 0x6d18f8:return 0x3fed70a3d70a3d71ull;
case 0x6d1900:return 0x3fc999999999999aull;
case 0x6d1908:return 0x4018000000000000ull;
case 0x6d1910:return 0x4014000000000000ull;
case 0x6d1918:return 0x3fa999999999999aull;
case 0x6d1920:return 0x3fd3333333333333ull;
case 0x6d1928:return 0x3f9999999999999aull;
case 0x6d1930:return 0x4082200000000000ull;
case 0x6d1938:return 0xc049000000000000ull;
case 0x6d1940:return 0x3fef5c28f5c28f5cull;
case 0x6d1aa8:return 0x404e000000000000ull;
case 0x6d1e20:return 0x4030000000000000ull;
case 0x6d1e48:return 0x4040000000000000ull;
case 0x6d1e50:return 0x408f400000000000ull;
case 0x6d1e58:return 0x4030aaaaaaaaaaabull;
default:throw std::out_of_range("menu presentation constant");}}
inline std::string_view text(std::uint32_t address){switch(address){
case 0x6cf830:return "\x25\x2e\x31\x66\x46\x50\x53";
case 0x6c7a88:return "\x4f\x70\x74\x69\x6f\x6e";
case 0x6c7a90:return "\xca\xb9\xd3\xc3\xd7\xf3\xd3\xd2\xd2\xc6\xb6\xaf\xbc\xfc\xb1\xe4\xb8\xfc";
case 0x6c7a3c:return "\x4b\x65\x79\x20\x43\x6f\x6e\x66\x69\x67";
case 0x6c7a48:return "\xca\xd6\xb1\xfa\xca\xe4\xc8\xeb";
case 0x6c7a58:return "\x4b\x65\x79\x20\x43\x6f\x6e\x66\x69\x67";
case 0x6c7a64:return "\xd1\xa1\xd4\xf1\xb1\xe4\xb8\xfc\xca\xe4\xc8\xeb";
case 0x6c79fc:return "\x4d\x75\x73\x69\x63\x20\x52\x6f\x6f\x6d";
case 0x6c7a08:return "\xce\xe5\xb7\xd6\xc7\xb0\xb5\xc4\xd2\xf4\xc0\xd6";
case 0xb411ce:return "\x52\x65\x70\x6c\x61\x79";
default:throw std::out_of_range("menu presentation text");}}
}
