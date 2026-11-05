#include "file_codec.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <new>
namespace th20::source::progress {
namespace {
std::uint8_t* allocate(std::size_t count){auto* result=static_cast<std::uint8_t*>(runtime::allocate_bytes(count));if(!result)throw std::bad_alloc();return result;}
bool valid_header(const Snapshot& snapshot){auto* header=snapshot.file_buffer;
    return header&&snapshot.file_size>=44&&read<std::uint32_t>(header,0)==0x32304854&&read<std::uint32_t>(header,8)==4&&
        read<std::uint32_t>(header,0x24)==snapshot.file_size-44&&read<std::uint32_t>(header,0xc)==0xe0&&
        read<std::uint32_t>(header,0x10)==sizeof(Profile)&&read<std::uint32_t>(header,0x14)==sizeof(Metadata)&&read<std::uint32_t>(header,0x18)==sizeof(Snapshot);
}
}
void release_snapshot_buffers(Snapshot& snapshot) noexcept {runtime::release_bytes(snapshot.file_buffer);snapshot.file_buffer=nullptr;runtime::release_bytes(snapshot.decoded_buffer);snapshot.decoded_buffer=nullptr;}
int parse_snapshot(Snapshot& snapshot,const Decode& decode) {
    if(!valid_header(snapshot)){
        runtime::release_bytes(snapshot.file_buffer);snapshot.file_buffer=allocate(44);std::memset(snapshot.file_buffer,0,44);
        write<std::uint32_t>(snapshot.file_buffer,0,0x32304854);write<std::uint32_t>(snapshot.file_buffer,8,4);write<std::uint32_t>(snapshot.file_buffer,0x20,0x100);
        write<std::uint32_t>(snapshot.file_buffer,0xc,0xe0);write<std::uint32_t>(snapshot.file_buffer,0x10,sizeof(Profile));write<std::uint32_t>(snapshot.file_buffer,0x14,sizeof(Metadata));write<std::uint32_t>(snapshot.file_buffer,0x18,sizeof(Snapshot));return 0;
    }
    const auto compressed_size=read<std::uint32_t>(snapshot.file_buffer,0x24),decoded_size=read<std::uint32_t>(snapshot.file_buffer,0x28);
    Bytes compressed(snapshot.file_buffer+44,snapshot.file_buffer+44+compressed_size);decrypt(compressed,{0xac,0x35,16,compressed_size});std::memcpy(snapshot.file_buffer+44,compressed.data(),compressed.size());
    if(decoded_size>0x3fffffffu)throw std::length_error("Score decode allocation overflows original 32-bit size");
    snapshot.decoded_buffer=allocate(std::size_t(decoded_size)*4);const auto decoded=decode(compressed,decoded_size);std::memcpy(snapshot.decoded_buffer,decoded.data(),decoded.size());
    std::size_t offset=0;
    while(offset<decoded.size()){
        // Original malformed/truncated records can overread or loop forever.
        // Stop at that boundary; valid record behavior and checksum order match.
        const auto remaining=decoded.size()-offset;if(remaining<12)return 0;const auto* record=decoded.data()+offset;
        const auto magic=read<std::uint16_t>(record,0),version=read<std::uint16_t>(record,2);const auto checksum=read<std::uint32_t>(record,4),size=read<std::uint32_t>(record,8);
        if(magic==0x5243){
            if(remaining<sizeof(Profile))return 0;
            if(version==1&&record_checksum({record,sizeof(Profile)})==checksum&&size==sizeof(Profile)){
                const auto character=read<std::int32_t>(record,12),index=read<std::int32_t>(record,16);
                auto* destination=character==2?&snapshot.profiles[18]:find_profile(snapshot,character,index);
                if(!destination)return 0;std::memcpy(destination,record,sizeof(Profile));
            }
        }else if(magic==0x5453){
            if(remaining<sizeof(Metadata))return 0;
            if(version==2&&record_checksum({record,sizeof(Metadata)})==checksum&&size==sizeof(Metadata))std::memcpy(&snapshot.metadata,record,sizeof(Metadata));
        }else return 0;
        if(!size||size>remaining)return 0;offset+=size;
    }
    return 0;
}
Bytes serialize_snapshot(Snapshot& snapshot,const Encode& encode) {
    if(!snapshot.file_buffer)return {};
    Bytes body;body.reserve(sizeof(snapshot.profiles)+sizeof(Metadata));
    auto append=[&](const void* pointer,std::size_t size){auto* bytes=static_cast<const std::uint8_t*>(pointer);body.insert(body.end(),bytes,bytes+size);};
    for(unsigned character=0;character<2;++character)for(unsigned index=0;index<9;++index){auto& profile=snapshot.profiles[character*9+index];
        if(read<std::uint16_t>(profile.bytes,0)!=0x5243)continue;
        write<std::uint32_t>(profile.bytes,12,character);write<std::uint32_t>(profile.bytes,16,index);write<std::uint32_t>(profile.bytes,4,record_checksum(profile.bytes));append(&profile,sizeof(profile));
    }
    auto& fallback=snapshot.profiles[18];write<std::uint32_t>(fallback.bytes,12,2);write<std::uint32_t>(fallback.bytes,4,record_checksum(fallback.bytes));append(&fallback,sizeof(fallback));
    write<std::uint32_t>(snapshot.metadata.bytes,4,record_checksum(snapshot.metadata.bytes));append(&snapshot.metadata,sizeof(Metadata));
    write<std::uint32_t>(snapshot.file_buffer,0x28,static_cast<std::uint32_t>(body.size()));auto compressed=encode(body);const auto compressed_size=static_cast<std::uint32_t>(compressed.size());
    write<std::uint32_t>(snapshot.file_buffer,0x24,compressed_size);write<std::uint32_t>(snapshot.file_buffer,4,compressed_size+44);encrypt(compressed,{0xac,0x35,16,compressed_size});
    Bytes output(snapshot.file_buffer,snapshot.file_buffer+44);output.insert(output.end(),compressed.begin(),compressed.end());return output;
}
}
