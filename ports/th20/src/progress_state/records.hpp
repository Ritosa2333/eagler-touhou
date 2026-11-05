#pragma once
#include "profile.hpp"
#include "../runtime_state/state.hpp"
#include <cstddef>
#include <cstring>
#include <span>
namespace th20::source::progress {
struct Metadata {std::uint8_t bytes[0x1f8];};
struct alignas(8) Snapshot {
    std::uint32_t file_size;
    std::uint8_t* file_buffer;
    std::uint8_t* decoded_buffer;
    std::uint8_t padding_0c[4];
    Profile profiles[19]; // 18 selectable records followed by the fallback
    Metadata metadata;
    Snapshot() noexcept; // 50e540; owns buffers through SaveManager lifetime
};
static_assert(offsetof(Snapshot,profiles)==0x10 && offsetof(Snapshot,metadata)==0x91f48 && sizeof(Snapshot)==0x92140);
template<class T> T read(const void* base,std::size_t offset) noexcept {T value;std::memcpy(&value,static_cast<const std::uint8_t*>(base)+offset,sizeof(value));return value;}
template<class T> void write(void* base,std::size_t offset,T value) noexcept {std::memcpy(static_cast<std::uint8_t*>(base)+offset,&value,sizeof(value));}
void construct_profile(Profile&) noexcept;             // 50af20
void construct_metadata(Metadata&) noexcept;           // 50e6e0
void initialize_profile(Profile&);                     // 50ef00
void initialize_metadata(Metadata&,state::Random&);    // 50f090, stream1
std::uint8_t metadata_checksum(const Metadata&);       // 463f20
void update_metadata_checksum(Metadata&,state::Random&); // 4beb60
std::uint32_t record_checksum(std::span<const std::uint8_t>) noexcept; //50fc90
Profile* find_profile(Snapshot&,int character,int index) noexcept; //4bd460
std::int32_t selected_profile(const Metadata&,int slot,int character,int mode) noexcept; //464100 data branch
void copy_snapshot_records(Snapshot& destination,const Snapshot& source) noexcept; //50eeb0
}
