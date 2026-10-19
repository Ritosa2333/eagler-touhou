#include "DataArchive.hpp"
#include <algorithm>
namespace th075 {
static std::string canonical(std::string s){std::replace(s.begin(),s.end(),'\\','/');return s;}
bool DataArchive::read_index(const void* source,std::size_t bytes,std::uint64_t archiveBytes){
    entries.clear();error.clear();ByteReader file(source,bytes);const u32 count=file.word();
    const std::size_t tableBytes=std::size_t(count)*108;
    const auto* encrypted=file.take(tableBytes);
    if(!encrypted||archiveBytes<2+tableBytes){error="Truncated archive index";return false;}
    std::vector<u8> table(encrypted,encrypted+tableBytes);u8 key=0x64,step=0x64;
    for(auto& value:table){value^=key;key=u8(key+step);step=u8(step+0x4d);}
    std::vector<ArchiveEntry> parsed;parsed.reserve(count);ByteReader directory(table.data(),table.size());
    for(u32 i=0;i<count;++i){const auto* name=directory.take(100);const u32 size=directory.dword(),offset=directory.dword();
        const auto* end=static_cast<const u8*>(std::memchr(name,0,100));
        if(!end){error="Unterminated archive path";return false;}
        std::string path=canonical(std::string(reinterpret_cast<const char*>(name),std::size_t(end-name)));
        if(path.empty()||path[0]=='/'||path.find(':')!=std::string::npos||std::uint64_t(offset)<2+tableBytes||std::uint64_t(offset)+size>archiveBytes){error="Invalid archive entry";return false;}
        std::size_t start=0;while(start<=path.size()){auto finish=path.find('/',start);if(finish==std::string::npos)finish=path.size();if(path.substr(start,finish-start)==".."){error="Invalid archive path";return false;}start=finish+1;}
        parsed.push_back({std::move(path),size,offset});
    }
    entries=std::move(parsed);return true;
}
const ArchiveEntry* DataArchive::find(const std::string& name)const{const auto path=canonical(name);for(const auto& e:entries)if(e.name==path)return &e;return nullptr;}
}
