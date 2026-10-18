#include "MusicLayout.hpp"
#include "Timer.hpp"
namespace th15 {
namespace {u32 word(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}u16 half(const u8* p){u16 n;std::memcpy(&n,p,2);return n;}}
bool MusicLayout::open(const u8* bytes,u32 size){
 error.clear();std::vector<MusicTrack> result;u32 at=0;while(at+52<=size&&bytes[at]){const auto* p=bytes+at;u32 n=0;while(n<16&&p[n])n++;if(n==16){error="Unterminated music filename";return false;}MusicTrack t;t.filename.assign(reinterpret_cast<const char*>(p),n);t.offset=word(p+16);t.length=word(p+20);t.loop_start=word(p+24);t.loop_end=word(p+28);t.channels=half(p+34);t.rate=word(p+36);t.alignment=half(p+44);t.bits=half(p+46);
 if(half(p+32)!=1||!t.channels||!t.rate||t.bits!=16||t.alignment!=t.channels*2||word(p+40)!=u64(t.rate)*t.alignment||t.loop_start>=t.loop_end||t.loop_end>t.length||t.offset<16||t.length%t.alignment||t.loop_start%t.alignment||t.loop_end%t.alignment){error="Invalid original PCM music layout";return false;}
 for(const auto& other:result)if(other.filename==t.filename){error="Duplicate music filename";return false;}result.push_back(std::move(t));at+=52;
 }for(;at<size;at++)if(bytes[at]){error="Nonzero data after music table";return false;}if(result.empty()){error="Empty music table";return false;}tracks=std::move(result);return true;
}
i32 MusicLayout::original_index(const std::string& value)const noexcept{auto slash=value.find_last_of('/');if(slash==std::string::npos)slash=value.find_last_of('\\');const u32 begin=slash==std::string::npos?0:slash+1;for(u32 n=0;n<tracks.size();n++)if(value.compare(begin,std::string::npos,tracks[n].filename)==0)return n;return 0;}
i32 adjusted_music_volume(i32 base,i32 master)noexcept{if(!master)return -10000;const float inverse=float(1.f-float(float(master)/100.f));return wrapping_sub(truncate_int(float(float(1.f-float(inverse*inverse))*float(wrapping_add(base,5000)))),5000);}
}
