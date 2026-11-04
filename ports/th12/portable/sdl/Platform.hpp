// Switch host: paths, files and logging shared by the devices.
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace th12host {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using u64=std::uint64_t;
using i8=std::int8_t; using i16=std::int16_t; using i32=std::int32_t; using i64=std::int64_t;
}
namespace th12host::host {
// Game data folder (th12.exe, th12.dat, thbgm.dat, msgothic.ttc) and save
// folder. On Switch both are the folder the NRO lives in
// (sdmc:/switch/touhou/touhou12 by default), the same layout as the other
// Saekaze ports.
struct Paths {
    std::string data=".";
    std::string save=".";
    std::string data_file(const char* name)const{return data+"/"+name;}
    std::string save_file(const char* name)const{return save+"/"+name;}
};
Paths& paths();
void locate_data(int argc,char** argv);
FILE* open_file(const std::string& path,const char* mode);

void log(const char* fmt,...);
bool exists(const std::string& path);
bool make_directory(const std::string& path);
// File names in a folder (no '.' entries), in directory order.
std::vector<std::string> list_files(const std::string& folder);
bool read_file(const std::string& path,std::vector<u8>& data,u64 limit);
// Writes through a temporary file and renames (FAT rename does not replace).
bool write_file(const std::string& path,const u8* data,size_t size);
inline bool write_file(const std::string& path,const std::vector<u8>& data){return write_file(path,data.data(),data.size());}
double seconds();
// Wall clock (Unix seconds) and its local calendar fields, as the original
// stores and shows replay and ranking dates.
i64 unix_time();
struct LocalTime {i32 year=1970,month=1,day=1,hour=0,minute=0;};
LocalTime local_time(i64 seconds);
}
