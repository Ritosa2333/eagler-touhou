#pragma once
#include <cstdint>
#include <stdexcept>
namespace th128::record {
inline std::uint16_t replayStageSeed(unsigned requestedSeed,unsigned actualSeed,unsigned actualCalls,bool receivedCarry){
    const bool requestMatches=receivedCarry?requestedSeed==65536:requestedSeed<=65535&&actualSeed==requestedSeed&&actualCalls==0;
    if(actualSeed>65535||!requestMatches)
        throw std::runtime_error("normal replay seed binding lacks an exact fresh or received-carry owner");
    return std::uint16_t(actualSeed);
}
}
