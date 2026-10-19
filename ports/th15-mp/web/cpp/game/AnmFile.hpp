#pragma once
#include "AnmManager.hpp"
#include <array>
namespace th15 {
// Logical ANM properties are mapped to the original 0x608-byte autosave
// record. Resource identifiers and script offsets survive process restarts;
// persisted pointer values are never dereferenced by the portable engine.
class AnmFile {
public:
    using Block=std::array<u8,0x608>;
    std::string error;
    bool write(const AnmVm&,const AnmManager&,Block&,bool allow_unbound=false,bool initialized_unbound=false);
    bool read(const u8*,u32,AnmVm&,AnmManager&,bool allow_unbound=false,bool initialized_unbound=false);
    using TreeWriter=std::function<bool(u32,std::vector<u8>&)>;
    using TreeReader=std::function<u32(const u8*,u32,u32&)>;
    bool write_geometry(const AnmVm&,const AnmManager&,std::vector<u8>&,const TreeWriter&);
    bool read_geometry(const u8* record,const u8* data,u32 size,AnmVm&,AnmManager&,u32& consumed,const TreeReader&);
};
}
