#include "replay.hpp"
#include "../program_entry/program_entry.hpp"
#include "../archive/resource_manager.hpp"
#include <cstring>
#include <filesystem>
#include <new>
#include <stdexcept>
namespace th20::source::replay {
int load(ReplayInf& o,const char* filename){
    strcpy_s(o.filename,sizeof(o.filename),filename);
    const bool embedded=(game_session::flags()&0x20u)!=0;
    const auto full_path=(std::filesystem::path(program_entry::window_state.user_data_directory)/"replay"/filename).string();
    auto input=resources::read(embedded?filename:full_path.c_str(),!embedded);if(!input)return -1;
    if(input->size()<sizeof(FileHeader))return -1;
    const auto storage_size=embedded?input->size():sizeof(FileHeader);o.header=static_cast<FileHeader*>(runtime::allocate_bytes(storage_size));if(!o.header)throw std::bad_alloc();std::memcpy(o.header,input->data(),storage_size);
    auto& header=*o.header;
    if(!embedded&&(header.magic!=0x72303274u||header.version!=1||header.header_size!=0x30||header.user_size!=0x100||header.stage_size!=0x2a0))return -1;
    if(header.packed_size>input->size()-sizeof(FileHeader))return -1;
    Bytes packed(input->begin()+sizeof(FileHeader),input->begin()+sizeof(FileHeader)+header.packed_size);
    decrypt(packed,{0x5c,0xe1,0x400,header.packed_size});decrypt(packed,{0x7d,0x3a,0x100,header.packed_size});
    auto decoded=resources::decode_shared(packed,header.unpacked_size);if(decoded.size()<sizeof(UserHeader))return -1;
    o.decoded=static_cast<std::uint8_t*>(runtime::allocate_bytes(decoded.size()));if(!o.decoded)throw std::bad_alloc();std::memcpy(o.decoded,decoded.data(),decoded.size());o.user=reinterpret_cast<UserHeader*>(o.decoded);
    const int count=static_cast<int>(o.user->fields_d0[1])<8?static_cast<int>(o.user->fields_d0[1]):6;std::size_t offset=0x100;
    for(int i=0;i<count;++i){
        if(offset>decoded.size()||sizeof(StageRecord)>decoded.size()-offset)return -1;
        auto* record=reinterpret_cast<StageRecord*>(o.decoded+offset);if(record->stage<0||record->stage>=8)return -1;
        if(record->data_bytes>decoded.size()-offset-sizeof(StageRecord)||std::uint64_t(record->frame_count)*6>record->data_bytes)return -1;
        auto& cursor=o.playback[record->stage];cursor.stage=record;cursor.inputs=reinterpret_cast<InputFrame*>(record+1);cursor.fps=reinterpret_cast<std::uint8_t*>(cursor.inputs)+record->frame_count*6;
        offset+=sizeof(StageRecord)+record->data_bytes;
    }
    return 0;
}
}
