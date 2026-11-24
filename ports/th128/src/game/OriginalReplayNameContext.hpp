#pragma once
#include "game/NativeReplaySlotPreview.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
namespace th128::record {
class OriginalReplayNameContext {
    unsigned epoch_=0;
    std::uint64_t timestamp_=0;
    NativeReplaySlotPreview preview_;
public:
    static OriginalReplayNameContext decode(std::span<const std::uint8_t> packet,unsigned epoch,
                                            std::span<const std::uint8_t> actualMetadata){
        if(packet.size()!=36||actualMetadata.size()!=112||epoch==0||NativeReplaySlotPreview::word(packet,0)!=0x31434e52u
           ||NativeReplaySlotPreview::word(packet,4)!=epoch)
            throw std::runtime_error("original replay name context packet or epoch rejected");
        const auto timestamp=(std::uint64_t(NativeReplaySlotPreview::word(packet,12))<<32)|NativeReplaySlotPreview::word(packet,8);
        const auto metadataTimestamp=(std::uint64_t(NativeReplaySlotPreview::word(actualMetadata,16))<<32)|NativeReplaySlotPreview::word(actualMetadata,12);
        if(timestamp>0x793406fffull||timestamp!=metadataTimestamp)
            throw std::runtime_error("original replay name entry clock differs from actual native metadata");
        std::array<std::uint8_t,3412> observed{};
        const auto putWord=[&](unsigned offset,unsigned value){for(unsigned index=0;index<4;++index)observed[offset+index]=std::uint8_t(value>>(8*index));};
        putWord(0,0x31565052u);putWord(4,25);putWord(8,136);putWord(12,1);
        std::copy(actualMetadata.begin(),actualMetadata.end(),observed.begin()+16);
        for(unsigned field=0;field<5;++field)putWord(128+4*field,NativeReplaySlotPreview::word(packet,16+4*field));
        OriginalReplayNameContext result;result.epoch_=epoch;result.timestamp_=timestamp;
        result.preview_=NativeReplaySlotPreview::decode(observed)[0];
        return result;
    }
    bool matches(unsigned epoch)const{return epoch_!=0&&epoch_==epoch;}
    std::uint64_t timestamp()const{return timestamp_;}
    std::string line(unsigned slot)const{
        if(!epoch_)throw std::runtime_error("original replay name heading has no actual entry context");
        auto display=preview_;std::fill_n(display.metadata.begin(),8,std::uint8_t(32));
        return display.line(slot);
    }
};
}
