#include "ResourceManager.hpp"
#ifdef TH_ENABLE_THCRAP
#include "../game/RuntimeOverride.hpp"
#endif
namespace th08 {
std::string ResourceManager::path(const char* p){std::string s=p?p:"";for(auto& c:s){if(c=='\\')c='/';if(c>='A'&&c<='Z')c+=32;}while(s.size()>2&&s.substr(0,2)=="./")s.erase(0,2);return s;}
bool ResourceManager::put(const char* name,const u8* b,u32 size){if(!name||(!b&&size)||size>128*1024*1024)return false;files[path(name)]=std::vector<u8>(b,b+size);return true;}
bool ResourceManager::put_archive(const u8* b,u32 size){if(!b||size>128*1024*1024)return false;archive_bytes.assign(b,b+size);return archive.open(archive_bytes.data(),archive_bytes.size());}
std::vector<u8> ResourceManager::read(const char* name){
    const auto key=path(name);auto found=files.find(key);if(found!=files.end())return found->second;
    const auto slash=key.rfind('/');const auto base=slash==std::string::npos?key:key.substr(slash+1);found=files.find(base);if(found!=files.end())return found->second;
#ifdef TH_ENABLE_THCRAP
    // thcrap-style offline pack: /thcrap/th08/<relative> wins over the
    // original DAT (but never over user files above). Covers whole-file
    // replacements such as msg*.dat, end*.end, musiccmt.txt and PNGs.
    // RuntimeOverride::Read owns the path traversal guard.
    {std::vector<u8> override_bytes;
    if(RuntimeOverride::Read(key.c_str(),override_bytes))return override_bytes;
    if(base!=key&&RuntimeOverride::Read(base.c_str(),override_bytes))return override_bytes;}
#endif
#ifdef TH_NATIVE_PLATFORM
    auto cached=decoded.find(base);if(cached!=decoded.end()){cached->second.used=++cache_clock;return cached->second.bytes;}
#endif
    std::vector<u8> bytes;if(!archive.read(base.c_str(),bytes))return {};
#ifdef TH_NATIVE_PLATFORM
    constexpr u32 limit=32*1024*1024;
    if(bytes.size()<=limit){while(decoded_bytes+bytes.size()>limit&&!decoded.empty()){
        auto victim=decoded.begin();for(auto it=decoded.begin();it!=decoded.end();++it)if(it->second.used<victim->second.used)victim=it;
        decoded_bytes-=victim->second.bytes.size();decoded.erase(victim);
    }decoded_bytes+=bytes.size();decoded[base]={bytes,++cache_clock};}
#else
    files[base]=bytes;
#endif
    return bytes;
}
std::vector<u8> ResourceManager::read_prefix(const char* p,u32 size){auto b=read(p);if(b.size()>size)b.resize(size);return b;}
const std::vector<u8>& ResourceManager::file(const char* p){file_result=read(p);return file_result;}
std::vector<std::string> ResourceManager::user_replays(){std::vector<std::string> result;for(auto& f:files)if(f.first.find("replay/th8_ud")==0&&f.first.size()>4&&f.first.substr(f.first.size()-4)==".rpy")result.push_back(f.first.substr(7));return result;}
}
