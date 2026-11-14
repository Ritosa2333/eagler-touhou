#include "ShtResource.hpp"
#include "PortTitle.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
namespace {template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
// A .sht file's layout is the title's, and the numbers below were read off the routines that parse them:
//
//   th12 0x00437680: `mov ecx, 0x268` - the group table starts there and has one u32 per group;
//                     `add dword ptr [ecx+eax], eax` adds the file pointer, so an entry is an absolute offset;
//                     shot specs are 0x34 bytes.
//   th13 0x00443e40: `mov ecx, 0xC8`, and `lea edi, [eax+0xF4]; add dword ptr [ecx+eax], edi` adds a fixed
//                     0xF4 instead of the file base, so an entry is relative to 0xF4 (which is where the spec
//                     region begins for the 11 groups these files hold); shot specs are 0x38 bytes.
//
// Both routines then replace four spec fields through lookup tables and take the loop's stride from the same
// constant, and on th13 those four fields are four bytes later (+0x28/+0x2c/+0x30/+0x34 against th12's
// +0x24/+0x28/+0x2c/+0x30). So the two layouts share their first 0x24 bytes and th13 carries one extra u32
// before the four fields - which is exactly the four-byte difference in the stride.
// TH14 44ecc0: header e0, group pointers relative to 108, shot stride 58.
// Where a title's sheets put the group table and their spec stride. TH12/13's numbers came from
// the routines that read the files (the note above); the four wide titles were fitted to each
// title's own sheet, one candidate at a time, with TH14 as the control: for TH14 itself the
// space has one solution and it is the row below, for TH17 two, and for TH16 and TH18 the table
// start is not where TH14 keeps it. docs/porting/sht-geometry.json has every fit with the spec
// ladder it walks, and tools/th-sht-fit.py re-runs the search.
constexpr u32 sht_header_bytes  = kPortTitle==16?0x190u
                                :(kPortTitle==14||kPortTitle==17||kPortTitle==18)?0xe0u
                                :kPortTitle==13?0xC8u:0x268u;
constexpr u32 sht_spec_bytes    = kPortTitle==18?0x5cu
                                :(kPortTitle==14||kPortTitle==16||kPortTitle==17)?0x58u
                                :kPortTitle==13?0x38u:0x34u;
constexpr u32 sht_entry_bytes   = kPortTitle==12?8u:4u;
constexpr u32 sht_offset_base   = kPortTitle==16?0x1b8u
                                :kPortTitle==17?0x130u
                                :kPortTitle==18?0x1dcu
                                :kPortTitle==14?0x108u
                                :kPortTitle==13?0xF4u:0u;
constexpr u32 sht_common_prefix=0x24u;                             // bytes the two spec layouts agree on
constexpr u32 sht_tail_bytes    = 16;   // the four callback selectors in the canonical prefix
// The version field the parser compares is per title and is not the TH12 value outside 12 and 13.
// Read out of each title's own sheets through the port's own archive decoder
// (docs/porting/sht-headers.json): first u16 = 3 for 12 and 13, 4 for 14 and 16, 5 for 17
// and 18, which is why a TH18 build was told "SHT resource pl00.sht" about a file it holds.
constexpr u32 sht_version = (kPortTitle==17||kPortTitle==18)?5u
                           :((kPortTitle==14||kPortTitle==16)?4u:3u);
// Which check turned a file away, so the message can carry it: a wrong layout, a wrong
// cipher and a file the title does not ship all arrive at the same caller otherwise.
}
// Why the last ShtResource::open turned a file away; GameResources carries it into the message.
const char* sht_parse_reason="";
bool ShtResource::open(const u8* bytes,u32 size){
    if(!bytes||size<sht_header_bytes){sht_parse_reason="header too short";return false;}
    ShtResource next;
#if PORT_TITLE == 14 || PORT_TITLE == 16 || PORT_TITLE == 17 || PORT_TITLE == 18
        std::memcpy(&next.header,bytes,0x28);
        std::memcpy(next.header.native_extra.data(),bytes+0x28,0x18);
        // TH16 4440e0 reads 21 pairs per formation at 40 / e8.
        // TH14 and the other wide sheets retain their existing 40 / 90 layout.
        std::memcpy(next.header.option_offsets.data(),bytes+0x40,kPortTitle==16?0x150:0xa0);
#else
    std::memcpy(&next.header,bytes,std::min<u32>(sizeof(ShtHeader),sht_header_bytes));
#endif
    const auto& h=next.header;
    if(h.version!=sht_version){sht_parse_reason="header version";return false;}
    if(!h.group_count){sht_parse_reason="no groups";return false;}
    if(h.group_count>(size-sht_header_bytes)/sht_entry_bytes){sht_parse_reason="group table over size";return false;}
    if(h.max_power_level<0){sht_parse_reason="power level negative";return false;}
    if(h.power_step<=0){sht_parse_reason="power step";return false;}
    const u32 table_end=sht_header_bytes+u32(h.group_count)*sht_entry_bytes;
    if(sht_offset_base&&table_end>sht_offset_base){sht_parse_reason="option table overlap";return false;}
    next.groups.reserve(h.group_count);
    for(u32 i=0;i<h.group_count;++i){
        const auto* entry=bytes+sht_header_bytes+i*sht_entry_bytes;const u64 absolute=u64(read<u32>(entry))+sht_offset_base;
        if(absolute>=size){sht_parse_reason="group offset over size";return false;}
        u32 offset=u32(absolute);ShotGroup group;group.metadata=sht_entry_bytes==8?read<u32>(entry+4):0;
        if(offset<table_end||offset>=size){sht_parse_reason="group offset before table";return false;}
        for(;;){
            if(offset>=size){sht_parse_reason="spec walk over size";return false;}
            if(read<i8>(bytes+offset)<0)break;
            if(size-offset<sht_spec_bytes){sht_parse_reason="spec over size";return false;}
            // The shared prefix, then the four trailing fields, skipping however many bytes the title's own layout
            // keeps between them (none on th12, one u32 on th13).
            ShotSpec s{};
#if PORT_TITLE == 14 || PORT_TITLE == 16 || PORT_TITLE == 17 || PORT_TITLE == 18
                std::memcpy(&s,bytes+offset,0x1c);
                s.option=read<i8>(bytes+offset+0x20);s.type=read<i8>(bytes+offset+0x21);
                s.animation=read<i16>(bytes+offset+0x22);s.hit_animation=read<i16>(bytes+offset+0x24);s.sound=read<i16>(bytes+offset+0x26);
                s.spawn=ShotSpawn(read<u32>(bytes+offset+0x28));s.update=ShotUpdate(read<u32>(bytes+offset+0x2c));
                s.hit_callback=read<u32>(bytes+offset+0x30);s.extra_callback=read<u32>(bytes+offset+0x34);
                s.native_at_1c=read<u32>(bytes+offset+0x1c);
                std::memcpy(s.native_tail.data(),bytes+offset+0x38,0x20);
#else
                std::memcpy(&s,bytes+offset,sht_common_prefix);
                const auto* tail=bytes+offset+sht_spec_bytes-sht_tail_bytes;
                s.spawn=ShotSpawn(read<u32>(tail));s.update=ShotUpdate(read<u32>(tail+4));
                s.hit_callback=read<u32>(tail+8);s.extra_callback=read<u32>(tail+12);
#endif
            // `hit_callback`/`extra_callback` are small mode selectors in TH12
            // (Sanae B sets extra_callback=1 on its spread), not code addresses,
            // so only bound them instead of demanding zero. On a title whose parser remaps
            // `option`/`spawn`/`update`/the two callbacks through lookup tables, the file's own values are raw
            // indices into those tables and the TH12 bounds do not apply. pl02a.sht on th13 carries option -37,
            // which is what first showed the bounds were a TH12 statement, not a structural one.
            const bool structural=s.interval>0&&std::isfinite(s.angle)&&std::isfinite(s.speed);
            const bool th12_enums=kPortTitle==12?
                (u32(s.option)<=8&&u32(s.spawn)<=3&&u32(s.update)<=3&&s.hit_callback<=3&&s.extra_callback<=3):true;
            if(!structural){sht_parse_reason="spec not structural";return false;}
            if(!th12_enums){sht_parse_reason="spec enum out of range";return false;}
            group.shots.push_back(s);offset+=sht_spec_bytes;
        }
        next.groups.push_back(std::move(group));
    }
    *this=std::move(next);return true;
}
i32 ShtResource::group_index(i32 power,i32 step,i32 character,i32 subtype,bool focused,i32 mode)const noexcept{
    (void)character;(void)subtype;(void)mode;
    if(step<=0)return -1;
    // Every TH12 loadout ships max_power_level+1 groups for the unfocused state
    // followed by the same count for the focused state; TH11's extra Marisa-B
    // "weapon mode" tier does not exist here.
    const i64 level=power/step,levels=i64(header.max_power_level)+1;
    const i64 index=level+(focused?levels:0);
    return index>=0&&index<i64(groups.size())?i32(index):-1;
}
}
