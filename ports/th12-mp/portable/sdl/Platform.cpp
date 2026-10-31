// SDL native files, UTF-8 paths and atomic profile/replay replacement.
// Adapted from saekaze/th12-switch (CC0-1.0); see source.json.
#include "Platform.hpp"
#include <SDL3/SDL.h>
#include <cstdarg>
#include <ctime>
#include <filesystem>
#include <algorithm>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#ifdef __SWITCH__
#include <switch.h>
#endif

namespace th12host::host {
namespace fs=std::filesystem;
namespace {
FILE* log_file=nullptr;
std::string utf8(const fs::path& path){const auto s=path.u8string();return {reinterpret_cast<const char*>(s.data()),s.size()};}
fs::path native_path(const std::string& value){return fs::path(std::u8string(reinterpret_cast<const char8_t*>(value.data()),value.size()));}
std::string sdl_path(char* value){if(!value)return {};std::string out=value;SDL_free(value);return out;}
}
Paths& paths(){static Paths p;return p;}
FILE* open_file(const std::string& path,const char* mode){
#ifdef _WIN32
    std::wstring wide_mode;for(const char* p=mode;*p;++p)wide_mode+=wchar_t(*p);
    return _wfopen(native_path(path).c_str(),wide_mode.c_str());
#else
    return std::fopen(path.c_str(),mode);
#endif
}
void locate_data(int argc,char** argv){
    std::vector<std::string> candidates;std::string save;
    for(int i=1;i<argc;++i){
        const std::string arg=argv[i];
        if(arg=="--data-dir"&&i+1<argc)candidates.push_back(argv[++i]);
        else if(arg=="--save-dir"&&i+1<argc)save=argv[++i];
        else if(!arg.empty()&&arg[0]!='-')candidates.push_back(arg);
    }
    if(const char* value=SDL_getenv("TH12_DATA_ROOT"))candidates.emplace_back(value);
#ifdef __SWITCH__
    if(argc>0&&argv[0])candidates.push_back(utf8(native_path(argv[0]).parent_path()));
    for(const char* root:{"sdmc:/switch/","sdmc:/","sdmc:/touhou/","sdmc:/switch/touhou/","sdmc:/games/","sdmc:/roms/"})
        for(const char* name:{"th12","touhou12","touhou 12"})candidates.push_back(std::string(root)+name);
#else
    if(const char* base=SDL_GetBasePath())candidates.emplace_back(base);candidates.push_back(".");
#endif
    paths().data=candidates.empty()?".":candidates.front();
    for(const auto& c:candidates)if(!c.empty()&&exists(c+"/th12.dat")){paths().data=c;break;}
#ifdef __SWITCH__
    paths().save=save.empty()?paths().data:save;
#else
    paths().save=save.empty()?sdl_path(SDL_GetPrefPath("EaglerTouhou","th12")):save;
    if(paths().save.empty())paths().save=".";
#endif
    make_directory(paths().save);
    if(log_file)std::fclose(log_file);
    log_file=open_file(paths().save_file("th12-native.log"),"w");
}
void log(const char* fmt,...){
    va_list ap;va_start(ap,fmt);char line[2048];std::vsnprintf(line,sizeof line,fmt,ap);va_end(ap);
    std::fprintf(stderr,"%s\n",line);
    if(log_file){std::fprintf(log_file,"%s\n",line);std::fflush(log_file);}
}
bool exists(const std::string& path){std::error_code ec;return fs::exists(native_path(path),ec);}
bool make_directory(const std::string& path){std::error_code ec;const auto p=native_path(path);return fs::create_directories(p,ec)||fs::is_directory(p,ec);}
std::vector<std::string> list_files(const std::string& folder){
    std::vector<std::string> names;std::error_code ec;
    for(fs::directory_iterator it(native_path(folder),ec),end;!ec&&it!=end;it.increment(ec)){
        const auto name=utf8(it->path().filename());if(!name.empty()&&name[0]!='.')names.push_back(name);
    }
    std::sort(names.begin(),names.end());return names;
}
bool read_file(const std::string& path,std::vector<u8>& data,u64 limit){
    FILE* f=open_file(path,"rb");if(!f)return false;
    if(std::fseek(f,0,SEEK_END)!=0){std::fclose(f);return false;}
    const long size=std::ftell(f);std::rewind(f);
    if(size<0||u64(size)>limit){std::fclose(f);return false;}
    data.resize(size_t(size));const bool ok=data.empty()||std::fread(data.data(),1,data.size(),f)==data.size();std::fclose(f);return ok;
}
bool write_file(const std::string& path,const u8* data,size_t size){
    const auto temporary=path+".tmp";FILE* f=open_file(temporary,"wb");if(!f)return false;
    const bool written=!size||std::fwrite(data,1,size,f)==size;
    const bool flushed=std::fflush(f)==0;const bool closed=std::fclose(f)==0;
    std::error_code ec;
    if(!written||!flushed||!closed){fs::remove(native_path(temporary),ec);return false;}
#ifdef _WIN32
    const bool ok=MoveFileExW(native_path(temporary).c_str(),native_path(path).c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
    if(!ok)fs::remove(native_path(temporary),ec);return ok;
#else
    fs::rename(native_path(temporary),native_path(path),ec);
#ifdef __SWITCH__
    if(ec&&exists(path)){
        const auto backup=path+".bak";ec.clear();fs::rename(native_path(path),native_path(backup),ec);
        if(ec)return false;
        fs::rename(native_path(temporary),native_path(path),ec);
        if(ec){std::error_code restore;fs::rename(native_path(backup),native_path(path),restore);return false;}
        fs::remove(native_path(backup),ec);return true;
    }
#endif
    return !ec;
#endif
}
double seconds(){return double(SDL_GetPerformanceCounter())/double(SDL_GetPerformanceFrequency());}
i64 unix_time(){return i64(std::time(nullptr));}
LocalTime local_time(i64 seconds){
    LocalTime t;
#ifdef __SWITCH__
    TimeCalendarTime c{};TimeCalendarAdditionalInfo info{};
    if(R_SUCCEEDED(timeToCalendarTimeWithMyRule(u64(seconds),&c,&info))){t.year=c.year;t.month=c.month;t.day=c.day;t.hour=c.hour;t.minute=c.minute;return t;}
#endif
    const std::time_t v=std::time_t(seconds);std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm,&v);
#else
    localtime_r(&v,&tm);
#endif
    t.year=tm.tm_year+1900;t.month=tm.tm_mon+1;t.day=tm.tm_mday;t.hour=tm.tm_hour;t.minute=tm.tm_min;return t;
}
}
