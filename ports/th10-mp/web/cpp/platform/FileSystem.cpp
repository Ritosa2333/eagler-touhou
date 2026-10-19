#include "FileSystem.hpp"
#include <cstdlib>
namespace th10::browser {
namespace {
constexpr ArchiveCipher resource_ciphers[]={
    {0x1b,0x37,0xaa,64,10240},{0x51,0xe9,0xbb,64,12288},
    {0xc1,0x51,0xcc,128,12800},{3,0x19,0xdd,1024,30720},
    {0xab,0xcd,0xee,512,10240},{0x12,0x34,0xff,128,12800},
    {0x35,0x97,0x11,128,10240},{0x99,0x37,0x77,1024,8192}
};
const char* read_mode="rb";
struct Stream {u32 handle;};
}
ArchiveEnvironment::ArchiveEnvironment(FileHost& h):host(h){ciphers=resource_ciphers;open_mode=&read_mode;}
void* ArchiveEnvironment::allocate_object(u32 bytes){return std::malloc(bytes);}
void ArchiveEnvironment::free_object(void* object){std::free(object);}
u8* ArchiveEnvironment::allocate_bytes(u32 bytes){return static_cast<u8*>(std::malloc(bytes));}
void ArchiveEnvironment::release_bytes(void* bytes){std::free(bytes);}
void* ArchiveEnvironment::create_stream(bool){auto* stream=static_cast<Stream*>(std::malloc(sizeof(Stream)));if(stream)stream->handle=0xffffffff;return stream;}
bool ArchiveEnvironment::open_stream(void* object,const char* name,const char*){auto& stream=*static_cast<Stream*>(object);if(stream.handle!=0xffffffff)host.close(stream.handle);stream.handle=host.open(name,false);return stream.handle!=0xffffffff;}
u32 ArchiveEnvironment::stream_length(void* object){return host.size(static_cast<Stream*>(object)->handle);}
void ArchiveEnvironment::destroy_stream(void* object){auto* stream=static_cast<Stream*>(object);if(stream->handle!=0xffffffff)host.close(stream->handle);std::free(stream);}
bool ArchiveEnvironment::seek(void* object,u32 offset){return host.seek(static_cast<Stream*>(object)->handle,static_cast<i32>(offset),0)!=0xffffffff;}
u32 ArchiveEnvironment::read(void* object,u8* output,u32 length){return host.read(static_cast<Stream*>(object)->handle,output,length);}
void ArchiveEnvironment::decrypt(u8* bytes,u32 length,const ArchiveCipher& cipher){transform_resource(bytes,length,cipher.key,cipher.step,cipher.block,cipher.limit,false,*this);}
u8* ArchiveEnvironment::decompress(u8* bytes,u32 length,u8* output,u32 capacity){const bool allocated=!output;if(allocated)output=allocate_bytes(capacity);if(output&&decode_lzss(bytes,length,output,capacity,dictionary)<0){if(allocated)release_bytes(output);return nullptr;}return output;}
FileSystem::FileSystem(FileHost& h):host(h),archives(h){archive=&resources;lock_depth=&depth;current_handle=&handle;}
FileSystem::~FileSystem(){if(handle!=0xffffffff)host.close(handle);resources.release(archives);}
bool FileSystem::attach_archive(const char* name){decoded.clear();cache_bytes=0;return resources.open(name,false,archives);}
u8* FileSystem::allocate_bytes(u32 bytes){return archives.allocate_bytes(bytes);}
void FileSystem::release_bytes(void* bytes){archives.release_bytes(bytes);}
u32 FileSystem::open(const char* name,bool write){return host.open(name,write);}
u32 FileSystem::length(u32 id){return host.size(id);}
void FileSystem::read(u32 id,u8* output,u32 count,u32* actual){*actual=host.read(id,output,count);}
void FileSystem::write(u32 id,const u8* input,u32 count,u32* actual){*actual=host.write(id,input,count);}
void FileSystem::close(u32 id){host.close(id);}
bool FileSystem::prewarm(const char* name){
    const auto* entry=resources.find(name);if(!entry||!entry->size||entry->size>cache_limit)return false;
    // The canonical archive name preserves its own case-insensitive lookup.
    const std::string key=entry->name;auto found=decoded.find(key);
    if(found!=decoded.end()){found->second.used=++cache_clock;return true;}
    while(cache_bytes+entry->size>cache_limit&&!decoded.empty()){
        auto victim=decoded.begin();for(auto it=decoded.begin();it!=decoded.end();++it)if(it->second.used<victim->second.used)victim=it;
        cache_bytes-=victim->second.bytes.size();decoded.erase(victim);
    }
    CachedResource value;value.bytes.resize(entry->size);value.used=++cache_clock;
    if(!resources.read(entry->name,value.bytes.data(),archives))return false;
    cache_bytes+=entry->size;decoded.emplace(key,std::move(value));return true;
}
void FileSystem::read_archive(const char* name,u8* output){
    const auto* entry=resources.find(name);if(!entry)return;
    const auto found=decoded.find(entry->name);if(found!=decoded.end()){++cache_hits;found->second.used=++cache_clock;std::memcpy(output,found->second.bytes.data(),entry->size);return;}
    ++cache_misses;if(prewarm(name)){std::memcpy(output,decoded.find(entry->name)->second.bytes.data(),entry->size);return;}
    resources.read(name,output,archives);
}
}
