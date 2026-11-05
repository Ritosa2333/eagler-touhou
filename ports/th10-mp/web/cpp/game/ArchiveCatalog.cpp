#include "ArchiveCatalog.hpp"
namespace th10 {
const char* ArchiveCatalog::basename(const char* name) noexcept {const auto* slash=std::strchr(name,'/');return slash?slash+1:name;}
ResourceArchive* ArchiveCatalog::find(const char* name) const noexcept {
    char filename[264];const auto length=std::strlen(name);if(length>=260)return nullptr;std::memcpy(filename,name,length+1);
    if(const auto* slash=std::strchr(name,'/'))std::memcpy(filename+(slash-name),".dat",5);
    const auto limit=count;for(i32 i=0;i<limit;++i)if(std::strcmp(archives[i].filename,filename)==0)return archives+i;return nullptr;
}
bool ArchiveCatalog::add(const char* name){if(!environment.open_archive(archives[count],name))return false;count=wrapping_add(count,1);return true;}
u8* ArchiveCatalog::read(const char* name,u8* output){
    auto& env=environment;if(auto* archive=find(name))return env.read_archive(*archive,basename(name),output);
    ArchiveFileStream stream;stream.initialize();stream.open(name,*env.file_mode,*env.streams);const u32 length=stream.size(*env.streams);auto* bytes=stream.read_all(length,*env.streams);stream.destroy(*env.streams);return bytes;
}
u32 ArchiveCatalog::size(const char* name){auto& env=environment;if(auto* archive=find(name))return env.archive_size(*archive,basename(name));ArchiveFileStream stream;stream.initialize();stream.open(name,*env.file_mode,*env.streams);const auto length=stream.size(*env.streams);stream.destroy(*env.streams);return length;}
bool ArchiveCatalog::open_memory(ArchiveMemoryStream& stream,const char* name){stream.bytes=read(name,nullptr);stream.length=size(name);stream.cursor=stream.bytes;return stream.bytes!=nullptr;}
}
