#include "Archive.hpp"
#include "ResourceCrypt.hpp"
namespace th09 {
namespace {
u32 word(const u8* p) noexcept {return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
char lower(char c) noexcept {return c>='A'&&c<='Z'?c+('a'-'A'):c;}
bool equal(const std::string& a,const char* b) noexcept {
    for(char c:a){if(!*b||lower(c)!=lower(*b++))return false;}return !*b;
}
}
bool Archive::open(const u8* bytes,u32 size) {
    close();source=bytes;source_size=size;if(open_index())return true;close();return false;
}
bool Archive::open(ArchiveSource& input){close();stream=&input;source_size=input.size();if(open_index())return true;close();return false;}
bool Archive::read_source(u32 offset,u8* output,u32 count){if(offset>source_size||count>source_size-offset)return false;if(stream)return stream->read(offset,output,count);if(!source)return false;std::memcpy(output,source+offset,count);return true;}
bool Archive::open_index(){
    const auto size=source_size;u8 bytes[16];if(!read_source(0,bytes,16)||std::memcmp(bytes,"PBGZ",4))return false;
    u8 header[12];resource_crypt(bytes+4,header,12,{0x1b,0x37,12,0x400},false);
    const u32 count=word(header)-123456,offset=word(header+4)-345678,unpacked=word(header+8)-567891;
    // Validate lengths before allocating or accessing externally supplied files.
    if(!count||count>100000||offset<16||offset>=size||unpacked>64*1024*1024||unpacked<count*13)return false;
    std::vector<u8> encoded(size-offset),compressed(size-offset),table(unpacked);
    if(!read_source(offset,encoded.data(),encoded.size()))return false;
    resource_crypt(encoded.data(),compressed.data(),compressed.size(),{0x3e,0x9b,0x80,0x400},false);
    u32 written=0;if(!codec.decode(compressed.data(),compressed.size(),table.data(),table.size(),written))return false;
    std::vector<ArchiveEntry> parsed;parsed.reserve(count);
    u32 cursor=0;
    for(u32 i=0;i<count;++i){
        const u32 start=cursor;while(cursor<written&&table[cursor])++cursor;
        if(cursor==written||written-cursor<13)return false;
        ArchiveEntry entry;entry.name.assign(reinterpret_cast<const char*>(table.data()+start),cursor-start);++cursor;
        entry.offset=word(table.data()+cursor);entry.size=word(table.data()+cursor+4);entry.unknown=word(table.data()+cursor+8);cursor+=12;
        if(entry.offset<16||entry.offset>=offset||entry.size>64*1024*1024)return false;
        if(!parsed.empty()){if(entry.offset<parsed.back().offset)return false;parsed.back().compressed=entry.offset-parsed.back().offset;}
        parsed.push_back(std::move(entry));
    }
    parsed.back().compressed=offset-parsed.back().offset;
    entries.swap(parsed);return true;
}
const ArchiveEntry* Archive::find(const char* name) const noexcept {
    for(const auto& entry:entries)if(equal(entry.name,name))return &entry;return nullptr;
}
bool Archive::packed(const char* name,std::vector<u8>& bytes,u32& unpacked){const auto* entry=find(name);if(!entry)return false;bytes.resize(entry->compressed);unpacked=entry->size;return read_source(entry->offset,bytes.data(),u32(bytes.size()));}
bool Archive::read(const char* name,std::vector<u8>& output,bool decrypt) {
    const auto* entry=find(name);if(!entry)return false;
    output.assign(entry->size,0);u32 written=0;
    std::vector<u8> compressed;const u8* input=source?source+entry->offset:nullptr;
    if(stream){compressed.resize(entry->compressed);if(!read_source(entry->offset,compressed.data(),compressed.size())){output.clear();return false;}input=compressed.data();}
    if(!input||!codec.decode(input,entry->compressed,output.data(),output.size(),written)){output.clear();return false;}
    if(written!=entry->size){output.clear();return false;}
    return !decrypt||resource_unwrap(output);
}
}
