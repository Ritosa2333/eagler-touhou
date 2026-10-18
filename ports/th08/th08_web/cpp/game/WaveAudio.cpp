#include "WaveAudio.hpp"
#include <algorithm>
namespace th08 {
namespace {u32 word(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}}
bool PcmWave::valid(const WaveFormat& f) noexcept {return f.tag==1&&(f.channels==1||f.channels==2)&&(f.bits==8||f.bits==16)&&f.rate>0&&f.rate<=192000&&f.align==f.channels*(f.bits/8)&&f.byte_rate==f.rate*f.align;}
bool PcmWave::load(const u8* data,u32 size){
    if(!data||size<12||std::memcmp(data,"RIFF",4)||std::memcmp(data+8,"WAVE",4)||u64(word(data+4))+8>size)return false;
    const u32 end=word(data+4)+8;const u8* fmt=nullptr,*pcm=nullptr;u32 fmt_size=0,pcm_size=0;
    // The original chunk walker advances by the declared length plus eight;
    // do not insert a RIFF padding byte that the TH08 loader does not skip.
    for(u32 p=12;p+8<=end;){const u32 n=word(data+p+4);if(n>end-p-8)return false;
        if(!fmt&&!std::memcmp(data+p,"fmt ",4)){fmt=data+p+8;fmt_size=n;}
        if(!pcm&&!std::memcmp(data+p,"data",4)){pcm=data+p+8;pcm_size=n;}
        if(fmt&&pcm)break;
        p+=8+n;
    }
    if(!fmt||fmt_size<16||!pcm)return false;WaveFormat parsed{};std::memcpy(&parsed,fmt,std::min<u32>(18,fmt_size));
    if(!valid(parsed)||pcm_size%parsed.align)return false;format=parsed;samples.assign(pcm,pcm+pcm_size);return true;
}
bool BgmFormats::load(const u8* data,u32 size){
    if(!data||!size)return false;std::vector<BgmFormat> parsed;
    for(u32 p=0;p<size;){if(!data[p]){formats=std::move(parsed);return !formats.empty();}
        if(size-p<52)return false;BgmFormat entry;std::memcpy(&entry,data+p,52);
        if(!std::memchr(entry.name,0,16)||!PcmWave::valid(entry.format)||entry.start<0||entry.intro<0||entry.total<entry.intro||entry.preload<u32(entry.total))return false;
        parsed.push_back(entry);p+=52;
    }
    return false;
}
u32 BgmFormats::find(const char* name) const noexcept {
    if(!name)return 0;const char* last=std::strrchr(name,'/');if(!last)last=std::strrchr(name,'\\');const char* base=last?last+1:name;
    for(u32 i=0;i<formats.size();++i)if(!std::strcmp(base,formats[i].name))return i;return 0;
}
i32 WaveStream::open(AudioByteSource& input,const BgmFormat& f,u32 offset){close();source=&input;format=f;seek_offset=offset;const i32 status=reset(false);original_size=remaining;return status;}
i32 WaveStream::open_memory(const u8* data,u32 size,const BgmFormat& f){close();if(!data)return i32(0x80070057u);memory=data;memory_capacity=memory_size=size;format=f;return 0;}
i32 WaveStream::reopen(const BgmFormat& f){if(memory||!source)return i32(0x80004005u);format=f;const i32 status=reset(false);original_size=remaining;return status;}
i32 WaveStream::reset(bool loop){
    const u32 start=loop&&format.intro>0?u32(format.intro):0;
    if(memory){const u32 length=format.total>0?u32(format.total):memory_size;if(length>memory_capacity||start>length)return i32(0x80070057u);cursor=start;memory_size=length;return 0;}
    if(!source)return i32(0x800401f0u);
    if(format.total<0||start>u32(format.total))return i32(0x80070057u);cursor=seek_offset+u32(format.start)+start;remaining=u32(format.total)-start;return 0;
}
i32 WaveStream::read(u8* output,u32 request,u32& received){
    received=0;if(!memory&&!source)return i32(0x800401f0u);if(!output)return i32(0x80070057u);
    if(memory){const u32 n=std::min(request,memory_size-cursor);std::memcpy(output,memory+cursor,n);cursor+=n;received=n;}
    else{const u32 n=std::min(request,remaining);remaining-=n;received=source->read(cursor,output,n);cursor+=received;}
    return 0;
}
void WaveStream::close() noexcept {source=nullptr;memory=nullptr;format={};original_size=memory_capacity=memory_size=cursor=remaining=seek_offset=0;}
}
