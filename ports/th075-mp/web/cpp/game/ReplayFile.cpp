#include "ReplayFile.hpp"
namespace th075 {
namespace {
u32 read_word(const std::array<u8,60>& h,u32 at){return u32(h[at])|(u32(h[at+1])<<8)|(u32(h[at+2])<<16)|(u32(h[at+3])<<24);}
void set_word(std::vector<u8>& h,u32 at,u32 v){for(u32 i=0;i<4;i++)h[at+i]=u8(v>>(i*8));}
void append_word(std::vector<u8>& out,u32 v,u32 width){for(u32 i=0;i<width;i++)out.push_back(u8(v>>(i*8)));}
template<class T> bool load_queue(ByteReader& reader,std::vector<T>& out,u32 width){
    const u32 count=reader.dword();if(!reader.valid||count>(reader.size-reader.offset)/width)return false;out.resize(count);
    for(auto& value:out){if(width==1)value=T(reader.byte());else if(width==2)value=T(reader.word());else value=T(reader.dword());}return reader.valid;
}
template<class T> bool encode_queue(std::vector<u8>& out,const std::vector<T>& values,u32 width){
    if(values.size()>std::numeric_limits<u32>::max())return false;append_word(out,u32(values.size()),4);for(const auto value:values)append_word(out,u32(value),width);return true;
}
}
bool ReplayFile::load(const void* source,std::size_t size){
    error.clear();rounds.clear();header.fill(0);if(size<60){error="Truncated replay header";return false;}std::memcpy(header.data(),source,60);
    const u32 count=header[14];if(count>8){error="Replay round count exceeds original header";return false;}
    const auto* bytes=static_cast<const u8*>(source);rounds.resize(count);
    for(u32 i=0;i<count;i++){
        const u32 offset=read_word(header,20+i*4);if(offset>size-60){error="Replay round offset outside file";rounds.clear();return false;}
        ByteReader reader(bytes+60+offset,size-60-offset);auto& round=rounds[i];
        for(auto& value:round.opponentOptions)value=reader.byte();round.stage=reader.byte();round.initialHealth=i16(reader.word());round.initialLives=reader.byte();round.initialScore=reader.dword();
        if(!load_queue(reader,round.playerInputs,1)||!load_queue(reader,round.randomValues,2)||!load_queue(reader,round.auxiliaryInputs,1)||!load_queue(reader,round.events,4)){error="Truncated replay data queue";rounds.clear();return false;}
    }return true;
}
bool ReplayFile::encode(std::vector<u8>& out)const{
    out.assign(header.begin(),header.end());if(rounds.size()>8)return false;out[14]=u8(rounds.size());
    for(u32 i=0;i<rounds.size();i++){
        if(out.size()-60>std::numeric_limits<u32>::max())return false;set_word(out,20+i*4,u32(out.size()-60));const auto& round=rounds[i];
        out.insert(out.end(),round.opponentOptions.begin(),round.opponentOptions.end());out.push_back(round.stage);append_word(out,u32(round.initialHealth),2);out.push_back(round.initialLives);append_word(out,round.initialScore,4);
        if(!encode_queue(out,round.playerInputs,1)||!encode_queue(out,round.randomValues,2)||!encode_queue(out,round.auxiliaryInputs,1)||!encode_queue(out,round.events,4))return false;
    }return true;
}
void ReplayStreams::bind(ReplayMode value,ReplayRound* data,u32 seed){mode=value;round=data;random.state=seed;inputCursor=randomCursor=auxiliaryCursor=eventCursor=0;exhausted=mode==ReplayMode::Playback&&(!round||round->playerInputs.empty());}
u8 ReplayStreams::input(u8 live){
    if(mode==ReplayMode::Playback){const u8 value=round&&inputCursor<round->playerInputs.size()?round->playerInputs[inputCursor++]:0;exhausted=!round||inputCursor>=round->playerInputs.size();return value;}
    if(mode==ReplayMode::Recording&&round)round->playerInputs.push_back(live);return live;
}
u8 ReplayStreams::auxiliary(u8 live){
    if(mode==ReplayMode::Playback){if(round&&auxiliaryCursor<round->auxiliaryInputs.size())return round->auxiliaryInputs[auxiliaryCursor++];return 0;}
    if(mode==ReplayMode::Recording&&round)round->auxiliaryInputs.push_back(live);return live;
}
i32 ReplayStreams::range(i32 low,i32 high){
    i16 value=0;
    if(mode==ReplayMode::Playback){if(round&&randomCursor<round->randomValues.size())value=round->randomValues[randomCursor++];}
    else{value=i16(random.next());if(mode==ReplayMode::Recording&&round)round->randomValues.push_back(value);}
    const u32 bits=(u32(high)-u32(low))*u32(i32(value));i32 product;std::memcpy(&product,&bits,4);return increment_wrapped(low,product/32768);
}
i16 ReplayStreams::control_timer(i16 live){
    if(mode==ReplayMode::Playback){if(round&&randomCursor<round->randomValues.size())return round->randomValues[randomCursor++];return live;}
    if(mode==ReplayMode::Recording&&round)round->randomValues.push_back(live);return live;
}
void ReplayStreams::append_event(u32 frame,u32 event){if(mode==ReplayMode::Recording&&round){round->events.push_back(frame);round->events.push_back(event);}}
}
