#pragma once
#include "GameConfiguration.hpp"
#include "Lzss.hpp"
namespace th08 {
struct ReplayHeader {
    u32 magic=0x50523854;u16 version=6;u8 unknown6=0,unknown7=0;
    u32 unknown8=0,file_size=0,checksum=0;u8 unknown14=0,key=0,unknown16=0,unknown17=0;
    u32 compressed_size=0,payload_size=0,stage_offsets[9]{},timing_offsets[9]{};
};
struct ReplayStage {
    u32 end_score=0;i32 points=0,graze=0,extends=0,next_extend=0,point_value=0;
    i16 humanity=0;u16 seed=0;u8 power=0,lives=0,bombs=0,rank=0,power_item_counter=0,captured_spells=0,clock=0,reserved23=0;
};
static_assert(sizeof(ReplayHeader)==0x68&&sizeof(ReplayStage)==0x24);
struct ReplayMetadata {
    ReplayHeader header;
    u8 unknown68=0,minor_version=0,shot_type=0,difficulty=0;
    char date[6]{},player_name[8]{};
    u8 unknown7a=0,practice=0;i16 spell_number=0;char spell_name[48]{};
    u16 major_version=0;u32 spell_score=0;GameConfiguration configuration;
    u8 reservedf0[40]{};float lag=0;i8 clear_state=0;u8 padding11d[3]{};
    i32 unknown120=0,exe_size=0,exe_checksum=0;char exe_version[6]{};u8 padding132[2]{};
};
static_assert(sizeof(ReplayMetadata)==0x134&&offsetof(ReplayMetadata,configuration)==0xb4);
class ReplayFile {
public:
    bool decode(const u8* data,u32 size);
    bool assign_decoded(const u8* data,u32 size);
    std::vector<u8> encode();
    const std::vector<u8>& decoded() const noexcept {return bytes;}
    const char* error() const noexcept {return failure;}
private:
    std::vector<u8> bytes;Lzss codec;const char* failure="";
};
}
