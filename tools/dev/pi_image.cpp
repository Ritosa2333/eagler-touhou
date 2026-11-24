#include "th03/assets/decode.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
int main(int argc,char**argv){if(argc!=3)return 2;std::ifstream f(argv[1],std::ios::binary|std::ios::ate);if(!f)return 3;std::vector<std::byte>b(static_cast<std::size_t>(f.tellg()));f.seekg(0);f.read(reinterpret_cast<char*>(b.data()),b.size());const auto p=th03::assets::decode_pi(b);if(!p||!p->palette)return 4;const auto &im=p->image;std::filesystem::path out=argv[2];if(std::filesystem::exists(out))return 5;std::ofstream o(out,std::ios::binary);auto u16=[&](unsigned v){o.put(char(v));o.put(char(v>>8));};auto u32=[&](unsigned v){u16(v);u16(v>>16);};unsigned stride=(im.width*3+3)&~3u;o.put('B');o.put('M');u32(54+stride*im.height);u32(0);u32(54);u32(40);u32(im.width);u32(im.height);u16(1);u16(24);u32(0);u32(stride*im.height);u32(0);u32(0);u32(0);u32(0);for(unsigned row=0;row<im.height;++row){for(unsigned x=0;x<im.width;++x){const auto c=p->palette->colors[im.indices[(im.height-row-1)*im.width+x]];o.put(char(c.b*17));o.put(char(c.g*17));o.put(char(c.r*17));}for(unsigned i=im.width*3;i<stride;++i)o.put(0);}std::cout<<im.width<<'x'<<im.height<<'\n';return o?0:6;}
