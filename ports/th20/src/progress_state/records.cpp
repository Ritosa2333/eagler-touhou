#include "records.hpp"
#include "spell_defaults.hpp"
#include "../runtime_core/runtime_core.hpp"
namespace th20::source::progress {
void construct_profile(Profile& record) noexcept {
    auto* bytes=record.bytes;
    std::memset(bytes,0,0x14);
    // The first ten unrolled score constructors leave their final four padding
    // bytes intact. The next sixty are zero-filled before their constructors.
    for(unsigned i=0;i<10;++i)std::memset(bytes+0x18+i*0x28,0,0x24);
    std::memset(bytes+0x1a8,0,0x960);
    std::memset(bytes+0xb08,0,0x6ba4);
    std::memset(bytes+0x76b0,0,0x48);
    for(unsigned i=0;i<63;++i)std::memset(bytes+0x76f8+i*0x10,0,0xc);
}
void construct_metadata(Metadata& record) noexcept {
    auto* bytes=record.bytes;
    std::memset(bytes,0,12);
    std::memset(bytes+0x16,0,0x112);
    const std::uint32_t choices[16]={0,8,8,8,1,8,8,8,0,8,8,8,1,8,8,8};
    std::memcpy(bytes+0x128,choices,sizeof(choices));
    std::memset(bytes+0x168,0,0x8a);
    write<std::uint32_t>(bytes,0x188,9);
}
Snapshot::Snapshot() noexcept :file_size(0),file_buffer(nullptr),decoded_buffer(nullptr) {
    for(auto& profile:profiles)construct_profile(profile);
    construct_metadata(metadata);
}
void initialize_profile(Profile& profile) {
    auto* bytes=profile.bytes;
    write<std::uint16_t>(bytes,0,0x5243);write<std::uint16_t>(bytes,2,1);write<std::uint32_t>(bytes,8,sizeof(Profile));
    for(unsigned difficulty=0;difficulty<7;++difficulty)for(unsigned rank=0;rank<10;++rank){
        auto* entry=bytes+0x18+difficulty*400+rank*0x28;
        write<std::int64_t>(entry,0,100000-static_cast<std::int64_t>(rank)*10000);
        entry[8]=1;entry[9]=0;
        strcpy_s(reinterpret_cast<char*>(entry+0xa),10,"--------");
        write<std::uint32_t>(entry,0x18,0);write<std::uint32_t>(entry,0x1c,0);write<std::uint32_t>(entry,0x20,0);
    }
    for(unsigned spell=0;spell<113;++spell){auto* entry=bytes+0xb08+spell*0xe0;write<std::uint32_t>(entry,0xd0,spell);write<std::uint32_t>(entry,0xd4,spell_defaults[spell]);}
}
std::uint8_t metadata_checksum(const Metadata& metadata) {
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));
    std::uint8_t sum=0;for(unsigned offset=0x58;offset<0x1d0;++offset)sum=static_cast<std::uint8_t>(sum+metadata.bytes[offset]);return sum;
}
void update_metadata_checksum(Metadata& metadata,state::Random& random) {
    auto* bytes=metadata.bytes;bytes[0x1d1]=metadata_checksum(metadata);
    ++bytes[0x1b0+state::next(random)%32];++bytes[0x1d1];bytes[0x1d0]=static_cast<std::uint8_t>(bytes[0x1d0]+2);
    ++bytes[0x1d2+state::next(random)%32];
}
void initialize_metadata(Metadata& metadata,state::Random& random) {
    auto* bytes=metadata.bytes;write<std::uint16_t>(bytes,0,0x5453);write<std::uint16_t>(bytes,2,2);write<std::uint32_t>(bytes,8,sizeof(Metadata));
    strcpy_s(reinterpret_cast<char*>(bytes+12),10,"        ");
    for(unsigned i=0;i<32;++i)bytes[0x1b0+i]=static_cast<std::uint8_t>(state::next(random));
    for(unsigned i=0;i<32;++i)bytes[0x1d2+i]=static_cast<std::uint8_t>(state::next(random));
    write<std::uint32_t>(bytes,0x188,9);update_metadata_checksum(metadata,random);
}
std::uint32_t record_checksum(std::span<const std::uint8_t> bytes) noexcept {std::uint32_t sum=0;for(std::size_t i=8;i<bytes.size();++i)sum+=bytes[i];return sum;}
Profile* find_profile(Snapshot& snapshot,int character,int index) noexcept {return character>=0&&character<2&&index>=0&&index<9?&snapshot.profiles[character*9+index]:nullptr;}
std::int32_t selected_profile(const Metadata& metadata,int slot,int character,int mode) noexcept {return read<std::int32_t>(metadata.bytes,(mode==4?0x148:0x128)+character*16+slot*4);}
void copy_snapshot_records(Snapshot& destination,const Snapshot& source) noexcept {std::memcpy(destination.profiles,source.profiles,sizeof(source.profiles));destination.metadata=source.metadata;}
}
