#pragma once
#include "PortTitle.hpp"
#include "Types.hpp"

namespace th11 {

// Which shot file a character/subtype pair loads, and how the original computes the slot.
//
// Every table and every slot expression below was read out of the executables, not inferred from
// the file names. docs/porting/shot-array-refs.txt holds the disassembly and
// docs/porting/shot-files.json the arrays; this is that record as rows.
//
//   th12.exe .data 0x004b3190, 6 pointers, pl00a pl00b pl01a pl01b pl02a pl02b.
//   The site that indexes it, 0x00435b4a, reads the two selections as
//       mov edx, dword ptr [0x4b0c94]      ; the shot type
//       lea eax, [edx + ecx*2]            ; ecx is the character
//       mov eax, dword ptr [eax*4 + 0x4b3190]
//   so the slot is `character*2 + subtype`.
//
//   th13.exe .data 0x004c0b98, 4 pointers, pl00a pl01a pl02a pl03a - four characters, one file each, and no
//   `pl00b`/`pl01b`/`pl02b` anywhere in the image. The site that indexes it, 0x00441e39, reads both selections
//   from memory and adds them:
//       mov edx, dword ptr [0x4be7bc]      ; the same two globals the character screens fill
//       add edx, dword ptr [0x4be7b8]
//       mov eax, dword ptr [edx*4 + 0x4c0b98]
//   so the slot is `character + subtype`, which is `character*1 + subtype`.
//
//   th14.exe .data 0x004d9f20, 6 pointers, the TH12 names. At 0x0044c513:
//       mov ecx, dword ptr [0x4f5828]
//       mov eax, dword ptr [0x4f582c]
//       lea eax, [eax + ecx*2]
//       push dword ptr [eax*4 + 0x4d9f20]
//   so the slot is `character*2 + subtype` again, over two globals of its own.
//
//   th16.exe .rdata 0x00492ca8, 4 pointers, and NOT in file order: pl00 pl02 pl03 pl01.
//   The array is the character index order, so the row carries the permutation. At 0x00441062:
//       mov eax, dword ptr [0x4a57a8]
//       add eax, dword ptr [0x4a57a4]
//       push dword ptr [eax*4 + 0x492ca8]
//   so the slot is `character + subtype` over one file per character.
//
//   th17.exe .rdata 0x004a2128, 9 pointers, pl00a..pl02c: three characters, three loadouts.
//   At 0x0044644b:
//       mov ecx, dword ptr [0x4b59f4]
//       mov eax, dword ptr [0x4b59f8]
//       lea eax, [eax + ecx*2]
//       add eax, ecx
//       mov ecx, dword ptr [eax*4 + 0x4a2128]
//   the `ecx*2` and the `add ecx` make the multiplier three, so the slot is `character*3 + subtype`.
//
//   th18.exe .rdata 0x004b7088, 4 pointers, pl00..pl03 with no loadout letter. At 0x0045a7f3:
//       mov eax, dword ptr [0x4cccf8]
//       add eax, dword ptr [0x4cccf4]
//       mov ecx, dword ptr [eax*4 + 0x4b7088]
//   so the slot is `character + subtype`, one file per character.
//
// The multiplier is the chain's only per-title constant, so `shot_file_slot` is one expression with
// one number per title. Every name in every table was also checked against that title's own archive
// (docs/porting/shot-files-archive.json): 6/6, 4/4, 6/6, 4/4, 9/9 and 4/4 are present.
//
// `tools/th-sample-table.py --pattern 'pl[0-9]{2}[a-z]?\.sht'` prints any of these arrays,
// `tools/th-array-refs.py <exe> <array-va>` prints the indexing site, and
// `tools/th-shot-files-report.py` re-derives both tables per title.
inline constexpr const char* const th12_shot_files[6]={"pl00a.sht","pl00b.sht","pl01a.sht","pl01b.sht","pl02a.sht","pl02b.sht"};
inline constexpr const char* const th13_shot_files[4]={"pl00a.sht","pl01a.sht","pl02a.sht","pl03a.sht"};
inline constexpr const char* const th14_shot_files[6]={"pl00a.sht","pl00b.sht","pl01a.sht","pl01b.sht","pl02a.sht","pl02b.sht"};
inline constexpr const char* const th16_shot_files[4]={"pl00.sht","pl02.sht","pl03.sht","pl01.sht"};
inline constexpr const char* const th17_shot_files[9]={"pl00a.sht","pl00b.sht","pl00c.sht","pl01a.sht","pl01b.sht","pl01c.sht","pl02a.sht","pl02b.sht","pl02c.sht"};
inline constexpr const char* const th18_shot_files[4]={"pl00.sht","pl01.sht","pl02.sht","pl03.sht"};

// The measured multiplier of every site above: how many loadouts one character has.
inline constexpr u32 shot_loadout_count =
    kPortTitle==17?3u:((kPortTitle==13||kPortTitle==16||kPortTitle==18)?1u:2u);
inline constexpr u32 shot_file_count = kPortTitle==13?4u
    :(kPortTitle==14?6u:(kPortTitle==16?4u:(kPortTitle==17?9u:(kPortTitle==18?4u:6u))));

inline constexpr const char* shot_file_name(u32 slot) {
    if(kPortTitle==13)return th13_shot_files[slot];
    if(kPortTitle==14)return th14_shot_files[slot];
    if(kPortTitle==16)return th16_shot_files[slot];
    if(kPortTitle==17)return th17_shot_files[slot];
    if(kPortTitle==18)return th18_shot_files[slot];
    return th12_shot_files[slot];
}

// The slot for a selection, in the form each title's own indexing site has: the loadout count
// above times the character, plus the subtype. On th13, 16 and 18 the multiplier is one, which
// is the sum those sites compute.
inline constexpr u32 shot_file_slot(u32 character,u32 subtype) {
    return character*shot_loadout_count+subtype;
}


// The player side: how many characters and loadouts a title has, and which ANM files carry them.
//
// The counts follow from the two numbers above - the character count is the shot table divided by
// the loadouts - and the ANM set matches: pl00..pl02.anm for TH12 (55 ANM files), pl00..pl03.anm
// for TH13 and TH18, pl00 pl02 pl03 pl01 for TH16 (the same permutation its shot table has), and
// pl00..pl02 for TH17. Every one of them is present in its own archive.
//
// The bounds a selection has to satisfy follow from the same two numbers, so the five literal
// `character>2` checks (battle, session, recorder, player frame, dialogue) become one row.
inline constexpr const char* const th12_player_files[3]={"pl00.anm","pl01.anm","pl02.anm"};
inline constexpr const char* const th13_player_files[4]={"pl00.anm","pl01.anm","pl02.anm","pl03.anm"};
inline constexpr const char* const th14_player_files[3]={"pl00.anm","pl01.anm","pl02.anm"};
// Retail44c4a3..44c4c1 indexes4d9f38 by character*2+subtype and
// requests the chosen ANM at file9. B loadouts have their own resources.
inline constexpr const char* const th14_player_variants[6]={"pl00.anm","pl00b.anm","pl01.anm","pl01b.anm","pl02.anm","pl02b.anm"};
inline constexpr const char* const th16_player_files[4]={"pl00.anm","pl02.anm","pl03.anm","pl01.anm"};
inline constexpr const char* const th17_player_files[3]={"pl00.anm","pl01.anm","pl02.anm"};
inline constexpr const char* const th18_player_files[4]={"pl00.anm","pl01.anm","pl02.anm","pl03.anm"};

inline constexpr u32 player_file_count = shot_file_count/shot_loadout_count;
inline constexpr u32 player_character_max = player_file_count-1;
inline constexpr u32 player_subtype_max = shot_loadout_count-1;

inline constexpr const char* player_file_name(u32 index,u32 subtype=0) {
    if(kPortTitle==13)return th13_player_files[index];
    if(kPortTitle==14)return th14_player_variants[shot_file_slot(index,subtype)];
    if(kPortTitle==16)return th16_player_files[index];
    if(kPortTitle==17)return th17_player_files[index];
    if(kPortTitle==18)return th18_player_files[index];
    return th12_player_files[index];
}

inline constexpr bool player_selection_valid(i32 character,i32 subtype) {
    return character>=0 && u32(character)<=player_character_max && subtype>=0 && u32(subtype)<=player_subtype_max;
}
}
