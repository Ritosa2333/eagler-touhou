#include "Archive.hpp"
#include "ResourceCrypt.hpp"
#include <random>
namespace th11 {
namespace {
u32 word(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
}
bool Archive::open(const u8* bytes,u32 size,u32 archiveVersion){
    if(archiveVersion==75||archiveVersion==105||archiveVersion==123){
        if(!open_directory(bytes,size,size,archiveVersion))return false;
        source=bytes;return true;
    }
    entries.clear();source=nullptr;source_size=0;if(size<16)return false;
    version=archiveVersion;ciphers=archive_crypt_table(archiveVersion);
 // No cipher table means no generation was named, and the entries cannot be read without one. The
 // three-argument call to archive_open passed 0, which used to resolve to the TH14 rows: the index
 // parsed, every name and size looked right, and every read refused, which is a long way from the
 // mistake. Refusing here names it at the call.
 if(!ciphers)return false;
    u8 header[16];resource_crypt(bytes,header,16,{0x1b,0x37,16,16},false);
    if(std::memcmp(header,"THA1",4))return false;
    const u32 unpacked=word(header+4)-123456789u,packed=word(header+8)-987654321u,count=word(header+12)-135792468u;
    if(!count||count>100000||packed>size-16||!packed||unpacked>64*1024*1024||unpacked<count*16)return false;
    const u32 offset=size-packed;std::vector<u8> encoded(packed),table(unpacked);
    resource_crypt(bytes+offset,encoded.data(),packed,{0x3e,0x9b,128,packed},false);
    u32 written=0;if(!codec.decode(encoded.data(),packed,table.data(),unpacked,written)||written!=unpacked)return false;
    u32 cursor=0;std::vector<ArchiveEntry> parsed;parsed.reserve(count);
    for(u32 i=0;i<count;++i){
        const u32 begin=cursor;while(cursor<unpacked&&table[cursor])++cursor;
        if(cursor==unpacked||cursor-begin>1024)return false;
        ArchiveEntry e;e.name.assign(reinterpret_cast<const char*>(table.data()+begin),cursor-begin);
        cursor=begin+((cursor-begin+4)&~3u);if(cursor>unpacked||unpacked-cursor<12)return false;
        e.offset=word(table.data()+cursor);e.size=word(table.data()+cursor+4);e.reserved=word(table.data()+cursor+8);cursor+=12;
        if(e.offset<16||e.offset>offset||e.size>64*1024*1024)return false;
        if(!parsed.empty()){if(e.offset<parsed.back().offset)return false;parsed.back().compressed=e.offset-parsed.back().offset;}
        parsed.push_back(std::move(e));
    }
    parsed.back().compressed=offset-parsed.back().offset;entries.swap(parsed);source=bytes;source_size=size;return true;
}
bool Archive::read(u32 index,std::vector<u8>& output){
    output.clear();if(!source||index>=entries.size())return false;
    const auto& e=entries[index];u8 sum=0;for(unsigned char c:e.name)sum=u8(sum+c);
    if(version==75||version==105||version==123){
        if(e.offset>source_size||e.size>source_size-e.offset)return false;
        return decode_entry(index,source+e.offset,e.size,output);
    }
    // Every remaining step trusts offsets and sizes that came out of the decrypted table. When the source
    // bytes are not the archive the table was parsed from - a stale pointer after the session is rebuilt -
    // those numbers are whatever the old archive happened to hold, and the compressed read walks off the
    // buffer while `output.resize(e.size)` asks the allocator for an unrelated, enormous block. Both are
    // reported here as a failed read instead, because a refused file produces an error the caller can show
    // while an out-of-bounds access aborts the whole page.
    if(e.offset>source_size||e.compressed>source_size-e.offset||e.size>source_size||e.compressed>e.size)return false;
    std::vector<u8> decoded(e.compressed);resource_crypt(source+e.offset,decoded.data(),e.compressed,ciphers[sum&7],false);
    if(e.compressed==e.size){output.swap(decoded);return true;}
    output.resize(e.size);u32 written=0;
    if(!codec.decode(decoded.data(),decoded.size(),output.data(),output.size(),written)||written!=e.size){output.clear();return false;}
    return true;
}

// Format reference: thtk/thdat105.c and thcrypt105.c (see
// docs/porting/THTK-NOTICE.txt). std::mt19937 uses the same 2002 MT seed recurrence.
bool Archive::open_directory(const u8* bytes,u32 prefix,u32 total,u32 archiveVersion){
    entries.clear();source=nullptr;source_size=0;version=archiveVersion;ciphers=nullptr;
    if(!bytes||prefix>total||(version!=75&&version!=105&&version!=123))return false;
    const u32 fixed=version==75?2:6;
    if(prefix<fixed)return false;
    const u32 count=u32(bytes[0])|u32(bytes[1])<<8;
    const u32 length=version==75?count*108:word(bytes+2);
    if(!count||length>prefix-fixed||length>16*1024*1024||length<count*(version==75?108:10))return false;
    std::vector<u8> table(bytes+fixed,bytes+fixed+length);
    if(version!=75){std::mt19937 rng(fixed+length);for(auto& b:table)b^=u8(rng());}
    u8 key=version==75?0x64:0xc5,step=version==75?0x64:0x83;
    for(auto& b:table){b^=key;key=u8(key+step);step=u8(step+(version==75?0x4d:0x53));}
    u32 cursor=0;std::vector<ArchiveEntry> parsed;parsed.reserve(count);
    for(u32 i=0;i<count;++i){
        ArchiveEntry e;
        if(version==75){
            if(length-cursor<108)return false;
            u32 n=0;while(n<100&&table[cursor+n])++n;
            if(!n||n==100)return false;
            e.name.assign(reinterpret_cast<const char*>(table.data()+cursor),n);
            e.size=word(table.data()+cursor+100);e.offset=word(table.data()+cursor+104);cursor+=108;
        }else{
            if(length-cursor<9)return false;
            e.offset=word(table.data()+cursor);e.size=word(table.data()+cursor+4);
            const u32 n=table[cursor+8];cursor+=9;
            if(!n||n>length-cursor)return false;
            e.name.assign(reinterpret_cast<const char*>(table.data()+cursor),n);cursor+=n;
            if(e.name.find('\0')!=std::string::npos)return false;
        }
        // TH105/123 store slash-separated Shift-JIS names. A raw 0x5c can be
        // the second byte of a Japanese character, so do not rewrite it.
        if(version==75)for(auto& ch:e.name)if(ch=='\\')ch='/';
        if(e.offset<fixed+length||e.offset>total||e.size>total-e.offset)return false;
        e.compressed=e.size;parsed.push_back(std::move(e));
    }
    if(cursor!=length)return false;
    entries.swap(parsed);source_size=total;return true;
}
bool Archive::decode_entry(u32 index,const u8* bytes,u32 size,std::vector<u8>& output) const{
    output.clear();
    if(index>=entries.size()||(version!=75&&version!=105&&version!=123))return false;
    const auto& e=entries[index];
    if(size!=e.size||(!bytes&&size))return false;
    if(!size)return true;
    output.assign(bytes,bytes+size);
    if(version!=75){const u8 key=u8((e.offset>>1)|0x23);for(auto& b:output)b^=key;}
    return true;
}
}
