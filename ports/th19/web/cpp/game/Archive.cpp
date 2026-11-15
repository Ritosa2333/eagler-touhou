#include "Archive.hpp"
namespace th19 {
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
    // THA1 header (16 bytes), fully encrypted (even magic) with key 0x1b step 0x37.
    u8 enc[16];if(!read_source(0,enc,16))return false;
    u8 header[16];resource_crypt(enc,header,16,{0x1b,0x37,16,16},false);
    if(std::memcmp(header,"THA1",4))return false;
    const u32 size=word(header+4)-123456789;
    const u32 zsize=word(header+8)-987654321;
    const u32 count=word(header+12)-135792468;
    if(!count||count>100000||zsize>=source_size||size>64*1024*1024||size<count*13)return false;
    // Index table sits at the end of the archive; decrypt with 0x3e/0x9b/0x80, then LZSS.
    std::vector<u8> zdata(zsize),decrypted(zsize),table(size);
    if(!read_source(source_size-zsize,zdata.data(),zsize))return false;
    resource_crypt(zdata.data(),decrypted.data(),zsize,{0x3e,0x9b,0x80,zsize},false);
    u32 written=0;if(!codec.decode(decrypted.data(),zsize,table.data(),table.size(),written))return false;
    std::vector<ArchiveEntry> parsed;parsed.reserve(count);
    u32 cursor=0;
    for(u32 i=0;i<count;++i){
        const u32 start=cursor;while(cursor<written&&table[cursor])++cursor;
        if(cursor==written||written-cursor<13)return false;
        ArchiveEntry entry;entry.name.assign(reinterpret_cast<const char*>(table.data()+start),cursor-start);++cursor;
        // name is padded to 4-byte alignment after its NUL terminator
        cursor=(cursor+3)&~3u;
        entry.offset=word(table.data()+cursor);entry.size=word(table.data()+cursor+4);entry.unknown=word(table.data()+cursor+8);cursor+=12;
        if(entry.offset>=source_size-zsize||entry.size>64*1024*1024)return false;
        if(!parsed.empty()){if(entry.offset<parsed.back().offset)return false;parsed.back().compressed=entry.offset-parsed.back().offset;}
        parsed.push_back(std::move(entry));
    }
    parsed.back().compressed=(source_size-zsize)-parsed.back().offset;
    entries.swap(parsed);return true;
}
const ArchiveEntry* Archive::find(const char* name) const noexcept {
    for(const auto& entry:entries)if(equal(entry.name,name))return &entry;return nullptr;
}
bool Archive::packed(const char* name,std::vector<u8>& bytes,u32& unpacked){const auto* entry=find(name);if(!entry)return false;bytes.resize(entry->compressed);unpacked=entry->size;return read_source(entry->offset,bytes.data(),u32(bytes.size()));}
bool Archive::read(const char* name,std::vector<u8>& output,bool decrypt) {
    const auto* entry=find(name);if(!entry)return false;
    // Read the stored (possibly compressed + encrypted) bytes.
    std::vector<u8> compressed(entry->compressed);
    if(!read_source(entry->offset,compressed.data(),compressed.size()))return false;
    // Decrypt per-entry with the name-derived keyset (in/out must not overlap).
    std::vector<u8> plain(entry->compressed);
    const CryptParams params=crypt_params_for(entry->name.c_str());
    resource_crypt(compressed.data(),plain.data(),compressed.size(),params,false);
    if(entry->compressed==entry->size){output.swap(plain);return true;}
    output.assign(entry->size,0);u32 written=0;
    if(!codec.decode(plain.data(),plain.size(),output.data(),output.size(),written)){output.clear();return false;}
    if(written!=entry->size){output.clear();return false;}
    return true;
}
}
