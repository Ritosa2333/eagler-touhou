#include "ResourceArchive.hpp"
namespace th10 {
char* ResourceArchive::duplicate_name(const char* name,CodecMemory& memory){
    auto* result=reinterpret_cast<char*>(memory.allocate_bytes(std::strlen(name)+1));
    if(result){char* out=result;do{*out++=*name;}while(*name++);}return result;
}
void ResourceArchive::release_entry(ArchiveEntry& entry,CodecMemory& memory){if(entry.name){memory.release_bytes(entry.name);entry.name=nullptr;}}
void ResourceArchive::release_entries(ArchiveEntry* value,ArchiveLifecycleEnvironment& env){
    auto* allocation=reinterpret_cast<u32*>(value)-1;u32 remaining=*allocation;
    while(remaining)release_entry(value[--remaining],env);env.free_object(allocation);
}
ArchiveEntry* ResourceArchive::parse_index(const u8* bytes,i32 count,u32 offset,ArchiveLifecycleEnvironment& env){
    const u32 elements=static_cast<u32>(count)+1;
    auto* allocation=static_cast<u32*>(env.allocate_object(elements*16+4));if(!allocation)return nullptr;
    *allocation=elements;auto* result=reinterpret_cast<ArchiveEntry*>(allocation+1);
    // The original entry constructor initializes only the owning name pointer.
    for(u32 i=0;i<elements;++i)result[i].name=nullptr;
    for(i32 i=0;i<count;++i){
        result[i].name=duplicate_name(reinterpret_cast<const char*>(bytes),env);
        bytes+=(std::strlen(reinterpret_cast<const char*>(bytes))+4)&~u32(3);
        std::memcpy(&result[i].offset,bytes,4);std::memcpy(&result[i].size,bytes+4,4);std::memcpy(&result[i].reserved,bytes+8,4);bytes+=12;
    }
    result[count].offset=offset;result[count].size=0;return result;
}
void ResourceArchive::release(ArchiveLifecycleEnvironment& env){
    if(filename)env.release_bytes(filename);filename=nullptr;
    if(entries)release_entries(entries,env);entries=nullptr;
    if(stream)env.destroy_stream(stream);stream=nullptr;count=0;
}
bool ResourceArchive::load_index(const char* name,ArchiveLifecycleEnvironment& env){
    if(!stream)return false;
    u8* packed=nullptr;u8* decoded=nullptr;
    const auto fail=[&](){if(packed)env.release_bytes(packed);if(decoded)env.release_bytes(decoded);if(stream)env.destroy_stream(stream);stream=nullptr;return false;};
    if(!env.open_stream(stream,name,*env.open_mode))return fail();
    u32 header[4]{};if(!env.read(stream,reinterpret_cast<u8*>(header),16))return fail();
    env.decrypt(reinterpret_cast<u8*>(header),16,ArchiveCipher{0x1b,0x37,0,16,16});
    if(header[0]!=0x31414854)return fail();
    const u32 capacity=header[1]-0x075bcd15,length=header[2]-0x3ade68b1;count=static_cast<i32>(header[3]+0xf7e7f8acu);
    const u32 offset=env.stream_length(stream)-length;env.seek(stream,offset);
    packed=env.allocate_bytes(length);if(!packed)return fail();
    if(!env.read(stream,packed,length))return fail();
    env.decrypt(packed,length,ArchiveCipher{0x3e,0x9b,0,128,static_cast<i32>(length)});
    decoded=env.decompress(packed,length,nullptr,capacity);if(!decoded)return fail();
    entries=env.build_index(decoded,count,offset);if(!entries)return fail();
    env.release_bytes(packed);env.release_bytes(decoded);return true;
}
bool ResourceArchive::open(const char* name,bool memory,ArchiveLifecycleEnvironment& env){
    release(env);stream=env.create_stream(memory);if(!stream)return false;
    if(load_index(name,env)){filename=duplicate_name(name,env);if(filename){if(!memory)env.open_stream(stream,filename,*env.open_mode);return true;}}
    release(env);return false;
}
}
