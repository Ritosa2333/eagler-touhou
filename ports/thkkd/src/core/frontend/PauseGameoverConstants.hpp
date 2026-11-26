// 原暂停/gameover文本编码字节。
#pragma once
#include <cstdint>
#include <string_view>
#include <stdexcept>
namespace kaikeidou::pause_gameover_detail {
inline std::string_view text(std::uint32_t address){switch(address){
case 0x6c6d80:return {"\xd4\xdd\xcd\xa3",4};
case 0x6c6d8c:return {"\xbd\xe2\xb3\xfd\xd4\xdd\xcd\xa3",8};
case 0x6c6da0:return {"\xbb\xd8\xb5\xbd\xb1\xea\xcc\xe2",8};
case 0x6c6db0:return {"\xb4\xd3\xcd\xb7\xbf\xaa\xca\xbc",8};
case 0x6c6dc4:return {"\xc2\xfa\xc9\xed\xb4\xaf\xf0\xea",8};
case 0x6c6dd0:return {"\xbc\xcc\xd0\xf8\xcc\xf4\xd5\xbd",8};
case 0x6c6de4:return {"\xbb\xd8\xb5\xbd\xb1\xea\xcc\xe2",8};
case 0x6c6df4:return {"\xb4\xd3\xcd\xb7\xbf\xaa\xca\xbc",8};
case 0x6c6e08:return {"\xc2\xfa\xc9\xed\xb4\xaf\xf0\xea",8};
case 0x6c6e14:return {"\xc1\xb7\xcf\xb0\xbd\xe1\xca\xf8",8};
case 0x6c6e20:return {"\xbb\xd8\xb5\xbd\xb1\xea\xcc\xe2",8};
case 0x6c6e30:return {"\xb4\xd3\xcd\xb7\xbf\xaa\xca\xbc",8};
case 0x6c6e44:return {"\xc2\xfa\xc9\xed\xb4\xaf\xf0\xea",8};
case 0x6c6e50:return {"\xbd\xe1\xca\xf8",4};
case 0x6c6e58:return {"\xd4\xd9\xcc\xf4\xd5\xbd",6};
case 0x6c6e64:return {"\xbb\xd8\xb5\xbd\xb1\xea\xcc\xe2",8};
default:throw std::out_of_range("Pause text address");}}
}
