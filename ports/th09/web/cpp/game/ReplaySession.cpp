#include "ReplaySession.hpp"
#include "Binary.hpp"
#include <algorithm>
namespace th09 {
bool ReplayPlayback::begin(const ReplayFile& file,u32 round,ReplayRoundSettings& settings){
    bytes.clear();frames=frame_number=samples=0;frame_rate=slowdown=0;
    if(round>=10||file.data().size()<ReplayFile::metadata_end)return false;
    const auto& data=file.data();const u32 end=ReplayFile::header_size+read32(data.data()+28);
    if(end>data.size())return false;
    for(u32 group=0;group<4;++group){
        offsets[group]=file.stream_offset(group,round);
        if(!offsets[group]||offsets[group]>=end)return false;
        if(group==3)continue;
        u32 limit=end;
        for(u32 g=0;g<4;++g)for(u32 r=0;r<10;++r){
            const u32 next=file.stream_offset(g,r);
            if(next>offsets[group])limit=std::min(limit,next);
        }
        if(limit-offsets[group]<32||(limit-offsets[group]-32)%2)return false;
        const u32 count=(limit-offsets[group]-32)/2;
        if(group&&count!=frames)return false;
        frames=count;
    }
    for(u32 side=0;side<2;++side){
        const u8* p=data.data()+offsets[side];auto& s=settings.players[side];
        s.points=read32(p);s.seed=read16(p+4);s.character=p[6];s.cpu=p[7];s.lives=p[8];
        s.selector=i8(p[9]);s.cpu_level=read16(p+10);s.background=p[12];s.extends=read32(p+16);
        if(s.character>=16)return false;
    }
    std::memcpy(settings.config.data(),data.data()+0xdc,204);settings.difficulty=data[0xd7];
    std::copy_n(data.data()+0x1e6,4,settings.settings);
    bytes=data;return true;
}
ReplayPlayback::Step ReplayPlayback::advance(u32 flags,GameInput (&inputs)[3],const bool (&auto_focus)[2]){
    if(!(flags&4)||(flags&0x200))return Step::inactive;
    if(bytes.empty()||frame_number>=frames)return Step::finished;
    for(u32 side=0;side<3;++side)inputs[side].advance(read16(bytes.data()+offsets[side]+32+frame_number*2));
    for(u32 side=0;side<2;++side)inputs[side].update_auto_focus(auto_focus[side]);
    if(frame_number%30==0){
        // Original playback reads one byte ahead, including a file trailer if present.
        // Preserve that behavior within the validated file, but never read past it.
        const u32 at=offsets[3]+samples+1;const u8 rate=at<bytes.size()?bytes[at]:0;
        frame_rate=rate&0x7f;slowdown=(rate&0x80)?-1:0;++samples;
    }
    ++frame_number;return Step::advanced;
}
u32 ReplayPlayback::after_update(u32 flags,i32 dialogue_state) const noexcept {
    return (flags&4)&&!(flags&0x200)&&(dialogue_state>=0||dialogue_state==-2)&&frame_number%3!=2?6:1;
}
void InputCapture::sample(GameInput (&inputs)[3],const bool (&auto_focus)[2],Rng& rng,i32& external_event) noexcept {
    seed=rng.seed;flags=external_event?0x100:0;rng.calls=0;external_event=0;
    for(auto& input:inputs)input.advance(input.device.held);
    for(u32 side=0;side<2;++side)inputs[side].update_auto_focus(auto_focus[side]);
}
void ReplayRecording::begin(u32 prior_ending_frames){parts.clear();parts.emplace_back();frame_number=0;ending=prior_ending_frames;}
bool ReplayRecording::advance(u32 flags,bool paused,const GameInput (&inputs)[3],const bool (&cpu)[2],u8 rate){
    if(!(flags&4)||paused)return false;
    if(flags&0x200){if(ending>=3)return false;++ending;}
    if(parts.empty())parts.emplace_back();auto& part=parts.back();
    for(u32 side=0;side<3;++side)part.inputs[side].push_back(side<2&&cpu[side]?0:inputs[side].held);
    if(frame_number%30==0)part.rates.push_back(rate);
    if(part.inputs[0].size()>=chunk_frames)parts.emplace_back();
    ++frame_number;return true;
}
std::vector<u8> ReplayRecording::stream(u32 group) const {
    std::vector<u8> result;if(group>3)return result;
    for(const auto& part:parts){
        if(group==3){
            // The original writer truncates the FPS count separately for each chunk.
            const auto count=std::min(part.rates.size(),part.inputs[0].size()/30);
            result.insert(result.end(),part.rates.begin(),part.rates.begin()+count);
        }else for(u16 value:part.inputs[group]){result.push_back(u8(value));result.push_back(u8(value>>8));}
    }
    return result;
}
}
