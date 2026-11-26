// 原文本与浮点常量的存储字节。
#pragma once
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace kaikeidou::menu_start_detail {
inline std::string_view text(std::uint32_t address){switch(address){
case 0x6c7468:return {"\x47\x61\x6d\x65\x20\x53\x74\x61\x72\x74",10};
case 0x6c7474:return {"\x45\x78\x74\x72\x61\x20\x53\x74\x61\x72\x74",11};
case 0x6c7480:return {"\x4f\x6d\x6e\x69\x76\x65\x72\x73\x65\x20\x53\x74\x61\x72\x74",15};
case 0x6c7490:return {"\x3f\x3f\x3f\x20\x53\x74\x61\x72\x74",9};
case 0x6c749c:return {"\x50\x72\x61\x63\x74\x69\x63\x65\x20\x53\x74\x61\x72\x74",14};
case 0x6c74ac:return {"\x4d\x75\x73\x69\x63\x20\x52\x6f\x6f\x6d",10};
case 0x6c74b8:return {"\x4b\x65\x79\x20\x43\x6f\x6e\x66\x69\x67",10};
case 0x6c74c4:return {"\x4f\x70\x74\x69\x6f\x6e",6};
case 0x6c74cc:return {"\x51\x75\x69\x74",4};
case 0x6c74d4:return {"\x65\x72\x72\x6f\x72",5};
case 0x6c74dc:return {"\x52\x61\x6e\x6b\x20\x53\x65\x6c\x65\x63\x74",11};
case 0x6c74e8:return {"\xc8\xa5\xc4\xc4\xc0\xef\xd3\xce\xd3\xbe\xa3\xbf",12};
case 0x6c79c4:return {"\x52\x61\x6e\x6b\x20\x53\x65\x6c\x65\x63\x74",11};
case 0x6c79d0:return {"\xc8\xa5\xc4\xc4\xc0\xef\xd3\xce\xd3\xbe\xa3\xbf",12};
case 0x6c79e0:return {"\x52\x61\x6e\x6b\x20\x53\x65\x6c\x65\x63\x74",11};
case 0x6c79ec:return {"\xc8\xa5\xc4\xc4\xc0\xef\xd3\xce\xd3\xbe\xa3\xbf",12};
case 0xb411ce:return {"\x52\x65\x70\x6c\x61\x79",6};
default:throw std::out_of_range("Menu start text address");}}
inline std::uint64_t constant(std::uint32_t address){switch(address){
case 0x6a7eb8:return 0x406fe00000000000ull;
case 0x6d1878:return 0x4050000000000000ull;
case 0x6d1880:return 0x3fe999999999999aull;
case 0x6d1888:return 0x4070400000000000ull;
case 0x6d1890:return 0x3ff2666666666666ull;
case 0x6d1898:return 0x4024000000000000ull;
case 0x6d18a0:return 0x4041000000000000ull;
case 0x6d18a8:return 0xc041000000000000ull;
case 0x6d18b0:return 0x3fe3333333333333ull;
case 0x6d18b8:return 0x3fed70a3d70a3d70ull;
case 0x6d18c0:return 0x4064000000000000ull;
case 0x6d18c8:return 0x407e000000000000ull;
case 0x6d18d0:return 0x4077700000000000ull;
case 0x6d18d8:return 0x4074000000000000ull;
case 0x6d18e0:return 0x3fd999999999999aull;
case 0x6d1a40:return 0x4034000000000000ull;
case 0x6d1dd0:return 0xc064000000000000ull;
case 0x6d1edc:return 0x0000000043200000ull;
case 0x6d1ee0:return 0x0000000043a00000ull;
case 0x6d2ab0:return 0xc065400000000000ull;
default:throw std::out_of_range("Menu start constant address");}}
}
