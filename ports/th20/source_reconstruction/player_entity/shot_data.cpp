#include "shot_data.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::player_entity {
void relocate_shot_data(std::span<std::uint8_t> bytes){
    if(bytes.size()<0x5d4)throw std::runtime_error("Truncated player SHT header");
    std::uint16_t count;std::memcpy(&count,bytes.data()+2,2);
    if(count>(bytes.size()-0x5d4)/4)throw std::runtime_error("Truncated player SHT offset table");
    for(unsigned i=0;i<count;++i){std::int32_t offset;std::memcpy(&offset,bytes.data()+0x5d4+i*4,4);if(offset>=0&&std::size_t(offset)>bytes.size())throw std::runtime_error("Player SHT pointer outside resource");}
    for(unsigned i=0;i<count;++i){std::int32_t offset;auto* entry=bytes.data()+0x5d4+i*4;std::memcpy(&offset,entry,4);if(offset>=0){const auto pointer=reinterpret_cast<std::uint32_t>(bytes.data())+std::uint32_t(offset);std::memcpy(entry,&pointer,4);}}
}
}
