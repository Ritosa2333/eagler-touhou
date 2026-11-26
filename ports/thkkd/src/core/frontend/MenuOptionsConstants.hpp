// 原设置和按键标签的字节；保存原空格与格式串。
#pragma once
#include <string_view>
#include <stdexcept>
#include <cstdint>
namespace kaikeidou::menu_options_detail {
inline std::string_view text(std::uint32_t address){switch(address){
case 0x6c732c:return {"\x20\x4c\x65\x66\x74",5};
case 0x6c7334:return {"\x52\x69\x67\x68\x74",5};
case 0x6c733c:return {"\x20\x20\x20\x55\x70",5};
case 0x6c7344:return {"\x20\x44\x6f\x77\x6e",5};
case 0x6c734c:return {"\x20\x53\x68\x6f\x74",5};
case 0x6c7354:return {"\x20\x42\x6f\x6d\x62",5};
case 0x6c735c:return {"\x20\x53\x6c\x6f\x77",5};
case 0x6c7364:return {"\x50\x61\x75\x73\x65",5};
case 0x6c736c:return {"\x20\x53\x6b\x69\x70",5};
case 0x6c7374:return {"\x65\x72\x72\x6f\x72",5};
case 0x6c7a78:return {"\x25\x73",2};
case 0x6c7a7c:return {"\x25\x64",2};
case 0x6c7a80:return {"\x51\x75\x69\x74",4};
case 0x6c7aa4:return {"\x20\x42\x47\x4d\x20\x56\x6f\x6c\x75\x6d\x65",11};
case 0x6c7ab0:return {"\x25\x64",2};
case 0x6c7ab4:return {"\x20\x20\x53\x45\x20\x56\x6f\x6c\x75\x6d\x65",11};
case 0x6c7ac0:return {"\x25\x64",2};
case 0x6c7ac4:return {"\x20\x20\x4d\x61\x74\x63\x68\x6c\x65\x73\x73",11};
case 0x6c7ad0:return {"\x4f\x4e",2};
case 0x6c7ad4:return {"\x4f\x46\x46",3};
case 0x6c7ad8:return {"\x20\x42\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64",11};
case 0x6c7ae4:return {"\x4f\x4e",2};
case 0x6c7ae8:return {"\x4f\x46\x46",3};
case 0x6c7aec:return {"\x51\x75\x69\x74",4};
default:throw std::out_of_range("Menu options string address");}}
inline std::uint64_t coordinate(std::uint32_t address){switch(address){
case 0x6d18d8:return 0x4074000000000000ull;
case 0x6d2100:return 0x406e400000000000ull;
case 0x6d1830:return 0x407b000000000000ull;
default:throw std::out_of_range("Menu options coordinate");}}
}
