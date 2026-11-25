#include "stage_background.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
std::vector<std::byte> read(const std::filesystem::path&p){std::ifstream f(p,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Missing stage resource");auto n=f.tellg();if(n<0||n>1000000)throw std::runtime_error("Invalid file size");std::vector<std::byte>b(static_cast<std::size_t>(n));f.seekg(0);if(!f.read(reinterpret_cast<char*>(b.data()),n))throw std::runtime_error("Read failure");return b;}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: soew-stage-background-tests <original-assets>");const std::filesystem::path root=argv[1];auto m=read(root/"STAGE0.MPN"),s=read(root/"STAGE0.MAP"),d=read(root/"STAGE0.DT1");auto bg=soew::StageBackground::decode(m,s,d);unsigned checks=0;auto check=[&](bool b){++checks;if(!b)throw std::runtime_error("Stage contract failure");};check(bg.tiles.size()==92);check(bg.map.size()==20);check(bg.script_offset==26);
    // Exact one-bit planar fixture: first tile pixel0 from B,R,G,E ->15.
    auto fixture=m;for(unsigned i=54;i<182;++i)fixture[i]=std::byte{0};for(unsigned plane=0;plane<4;++plane)fixture[54+plane*32]=std::byte{128};auto test=soew::StageBackground::decode(fixture,s,d);check(test.tiles[0][0]==15&&test.tiles[0][1]==0&&test.tiles[0][16]==0);
    for(unsigned offset=0;offset<6;++offset){auto bad=d;bad[offset]=std::byte{255};bool rejected=false;try{soew::StageBackground::decode(m,s,bad);}catch(const std::runtime_error&){rejected=true;}check(rejected);}
    for(unsigned n:{0u,53u,54u,11829u}){bool rejected=false;try{soew::StageBackground::decode(std::span(m).first(n),s,d);}catch(const std::runtime_error&){rejected=true;}check(rejected);}
    std::cout<<"stage0_tiles="<<bg.tiles.size()<<" sections=16 map_sections="<<bg.map.size()<<" script_offset="<<bg.script_offset<<" checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
