// THA1 reader: the TH11 port's codec (game/Archive.cpp, ResourceCrypt.cpp,
// Lzss.cpp), reading entries from the file on demand instead of mapping the
// whole archive.
#include "Archive.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>

namespace th12host {
namespace {
u32 word(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
void put_word(std::vector<u8>& b,u32 v){for(int i=0;i<4;++i)b.push_back(u8(v>>(8*i)));}
// thtk thdat95.c th12_crypt_params, chosen by (sum of name bytes) & 7.
constexpr CryptParams ciphers[8]{{0x1b,0x73,0x40,0x3800},{0x51,0x9e,0x40,0x4000},{0xc1,0x15,0x400,0x2c00},{0x03,0x91,0x80,0x6400},
                                 {0xab,0xdc,0x80,0x6e00},{0x12,0x43,0x200,0x3c00},{0x35,0x79,0x400,0x3c00},{0x99,0x7d,0x80,0x2800}};
const CryptParams& cipher(const std::string& name){u8 sum=0;for(unsigned char c:name)sum=u8(sum+c);return ciphers[sum&7];}
std::string lower(std::string s){for(auto& c:s)c=char(std::tolower(static_cast<unsigned char>(c)));return s;}
}
void resource_encrypt(const u8* in,u8* out,u32 size,CryptParams p) noexcept {
    const u32 untouched=(size%p.block<p.block/4?size%p.block:0)+(size&1);
    i64 remaining=i64(size)-untouched,limit=p.limit;u32 cursor=0;
    while(remaining>0&&limit>0){
        const u32 block=std::min<u32>(p.block,u32(remaining));u32 linear=0;
        for(i32 parity=1;parity<=2;++parity)for(i32 pos=i32(block)-parity;pos>=0;pos-=2){out[cursor+linear]=in[cursor+pos]^p.key;p.key=u8(p.key+p.step);++linear;}
        cursor+=block;remaining-=block;limit-=block;
    }
    if(cursor<size)std::memcpy(out+cursor,in+cursor,size-cursor);
}
// Literal-only LZSS: a valid stream for the decoder (test archives, score files).
std::vector<u8> lzss_literals(const std::vector<u8>& in){
    std::vector<u8> out;u8 byte=0,mask=0x80;
    auto bit=[&](bool v){if(v)byte|=mask;mask>>=1;if(!mask){out.push_back(byte);byte=0;mask=0x80;}};
    for(u8 c:in){bit(true);for(int b=7;b>=0;--b)bit(c>>b&1);}
    bit(false);for(int b=0;b<13;++b)bit(false);if(mask!=0x80)out.push_back(byte);return out;
}

void resource_decrypt(const u8* in,u8* out,u32 size,CryptParams p) noexcept {
    if(!p.block)return;
    const u32 untouched=(size%p.block<p.block/4?size%p.block:0)+(size&1);
    // The original accepts signed sizes; an odd tiny input can make this -1.
    // Its loop then copies the entire input unchanged.
    i64 remaining=i64(size)-untouched,limit=p.limit;u32 cursor=0;
    while(remaining>0&&limit>0){
        const u32 block=std::min<u32>(p.block,u32(remaining));u32 linear=0;
        for(i32 parity=1;parity<=2;++parity)for(i32 pos=i32(block)-parity;pos>=0;pos-=2){out[cursor+pos]=in[cursor+linear]^p.key;p.key=u8(p.key+p.step);++linear;}
        cursor+=block;remaining-=block;limit-=block;
    }
    if(cursor<size)std::memcpy(out+cursor,in+cursor,size-cursor);
}
bool lzss_decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept {
    u8 dictionary[8192]{}; // zeroed per stream, like thtk's th_unlzss
    u32 cursor=0,head=1;u8 mask=0x80,byte=0;written=0;
    auto bit=[&](){if(mask==0x80)byte=cursor<size?input[cursor++]:0;u32 result=bool(byte&mask);mask>>=1;if(!mask)mask=0x80;return result;};
    auto bits=[&](u32 n){u32 value=0;while(n--)value=(value<<1)|bit();return value;};
    auto put=[&](u8 value){output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
    for(;;){
        if(bit()){if(written==capacity)return false;put(u8(bits(8)));}
        else {
            const u32 offset=bits(13);if(!offset)return true;
            const u32 length=bits(4)+3;if(length>capacity-written)return false;
            for(u32 i=0;i<length;++i)put(dictionary[(offset+i)&8191]);
        }
    }
}

Archive::~Archive(){close();}
void Archive::close(){if(file)std::fclose(file);file=nullptr;directory.clear();entries.clear();index.clear();}
bool Archive::open(const std::string& path){
    close();
    if(host::exists(path+"/sig.anm")||host::exists(path+"/default.ecl")){
        directory=path;
        for(const auto& name:host::list_files(path)){index.emplace(lower(name),entries.size());entries.push_back({name,0,0,0});}
        error.clear();return true;
    }
    file=host::open_file(path,"rb");if(!file){error="cannot open "+path;return false;}
    std::fseek(file,0,SEEK_END);const long length=std::ftell(file);std::fseek(file,0,SEEK_SET);
    const auto fail=[&](const char* why){error=path+": "+why;close();return false;};
    if(length<16)return fail("too small");const u32 size=u32(length);
    u8 raw[16],header[16];if(std::fread(raw,1,16,file)!=16)return fail("unreadable header");
    resource_decrypt(raw,header,16,{0x1b,0x37,16,16});
    if(std::memcmp(header,"THA1",4))return fail("not a THA1 archive");
    const u32 unpacked=word(header+4)-123456789u,packed=word(header+8)-987654321u,count=word(header+12)-135792468u;
    if(!count||count>100000||!packed||packed>size-16||unpacked>64*1024*1024||unpacked<count*16)return fail("bad header");
    const u32 table_at=size-packed;std::vector<u8> encoded(packed),decoded(packed),table(unpacked);
    std::fseek(file,long(table_at),SEEK_SET);if(std::fread(encoded.data(),1,packed,file)!=packed)return fail("unreadable index");
    resource_decrypt(encoded.data(),decoded.data(),packed,{0x3e,0x9b,0x80,packed});
    u32 written=0;if(!lzss_decode(decoded.data(),packed,table.data(),unpacked,written)||written!=unpacked)return fail("bad index");
    u32 cursor=0;std::vector<Entry> parsed;parsed.reserve(count);
    for(u32 i=0;i<count;++i){
        const u32 begin=cursor;while(cursor<unpacked&&table[cursor])++cursor;
        if(cursor==unpacked||cursor-begin>1024)return fail("bad index name");
        Entry e;e.name.assign(reinterpret_cast<const char*>(table.data()+begin),cursor-begin);
        cursor=begin+((cursor-begin+4)&~3u);if(cursor>unpacked||unpacked-cursor<12)return fail("bad index entry");
        e.offset=word(table.data()+cursor);e.size=word(table.data()+cursor+4);cursor+=12;
        if(e.offset<16||e.offset>table_at||e.size>64*1024*1024)return fail("bad entry offset");
        if(!parsed.empty()){if(e.offset<parsed.back().offset)return fail("unsorted entries");parsed.back().packed=e.offset-parsed.back().offset;}
        parsed.push_back(std::move(e));
    }
    parsed.back().packed=table_at-parsed.back().offset;entries.swap(parsed);
    for(size_t i=0;i<entries.size();++i)index.emplace(lower(entries[i].name),i);
    error.clear();return true;
}
const Archive::Entry* Archive::find(const std::string& name)const{
    std::string key=name;const auto slash=key.find_last_of("/\\");if(slash!=std::string::npos)key.erase(0,slash+1);
    const auto it=index.find(lower(key));return it==index.end()?nullptr:&entries[it->second];
}
bool Archive::read(const std::string& name,std::vector<u8>& output){
    output.clear();const Entry* e=find(name);
    if(!directory.empty()&&e)return host::read_file(directory+"/"+e->name,output,128*1024*1024);
    if(!file||!e){std::lock_guard<std::mutex> lock(mutex);error="missing "+name;return false;}
    std::vector<u8> raw(e->packed),decoded(e->packed);
    {std::lock_guard<std::mutex> lock(mutex);
        std::fseek(file,long(e->offset),SEEK_SET);if(e->packed&&std::fread(raw.data(),1,e->packed,file)!=e->packed){error="unreadable "+name;return false;}}
    resource_decrypt(raw.data(),decoded.data(),e->packed,cipher(e->name));
    if(e->packed==e->size){output.swap(decoded);return true;}
    output.resize(e->size);u32 written=0;
    if(!lzss_decode(decoded.data(),u32(decoded.size()),output.data(),u32(output.size()),written)||written!=e->size){output.clear();std::lock_guard<std::mutex> lock(mutex);error="corrupt "+name;return false;}
    return true;
}
std::vector<u8> Archive::build_for_tests(const std::vector<std::pair<std::string,std::vector<u8>>>& files,bool compress){
    std::vector<u8> out(16),table;
    for(const auto& [name,bytes]:files){
        const u32 at=u32(out.size());const std::vector<u8> stored=compress?lzss_literals(bytes):bytes;
        std::vector<u8> encoded(stored.size());resource_encrypt(stored.data(),encoded.data(),u32(stored.size()),cipher(name));
        out.insert(out.end(),encoded.begin(),encoded.end());
        table.insert(table.end(),name.begin(),name.end());const size_t pad=4-name.size()%4;table.insert(table.end(),pad,0);
        put_word(table,at);put_word(table,u32(bytes.size()));put_word(table,0);
    }
    table.insert(table.end(),16,0); // zero tail, as the original pads its index
    const auto packed_plain=lzss_literals(table);std::vector<u8> packed(packed_plain.size());
    resource_encrypt(packed_plain.data(),packed.data(),u32(packed.size()),{0x3e,0x9b,0x80,u32(packed.size())});
    out.insert(out.end(),packed.begin(),packed.end());
    std::vector<u8> header;header.insert(header.end(),{'T','H','A','1'});
    put_word(header,u32(table.size())+123456789u);put_word(header,u32(packed.size())+987654321u);put_word(header,u32(files.size())+135792468u);
    resource_encrypt(header.data(),out.data(),16,{0x1b,0x37,16,16});return out;
}
}
