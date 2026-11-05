#include "ResourceArchive.hpp"
namespace th10 {
bool ResourceArchive::name_equal(const char* first,const char* second) noexcept {
    for(;;){u8 a=*first++,b=*second++;if(a>='A'&&a<='Z')a+=32;if(b>='A'&&b<='Z')b+=32;if(a!=b)return false;if(!a)return true;}
}
ArchiveEntry* ResourceArchive::find(const char* name) const noexcept {if(entries)for(i32 i=0;i<count;++i)if(name_equal(entries[i].name,name))return entries+i;return nullptr;}
u32 ResourceArchive::size(const char* name) const noexcept {const auto* entry=find(name);return entry?entry->size:0;}
u8* ResourceArchive::read(const char* name,u8* output,ArchiveEnvironment& env){
    if(!stream)return nullptr;auto* entry=find(name);if(!entry)return nullptr;
    const u32 length=entry[1].offset-entry->offset,capacity=entry->size;
    u8* bytes=length==capacity&&output?output:env.allocate_bytes(length);if(!bytes)return nullptr;
    if(!env.seek(stream,entry->offset)||!env.read(stream,bytes,length)){env.release_bytes(bytes);return nullptr;}
    u8 key=0;for(const char* p=entry->name;*p;++p)key=static_cast<u8>(key+static_cast<u8>(*p));
    env.decrypt(bytes,length,env.ciphers[key&7]);
    u8* result=length==capacity?bytes:env.decompress(bytes,length,output,capacity);
    if(bytes!=output)env.release_bytes(bytes);return result;
}
}
