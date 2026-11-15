#pragma once
#include "PortTitle.hpp"
#include "Types.hpp"
namespace th11 {
// The cue table is the title's, like its sound list, its shot set and its instruction numbering.
//
// TH12 (measured from its files): `musiccmt.txt` lists 17 `@bgm/*.wav` lines and `thbgm.fmt`
// holds 18 records of 52 bytes, `thbgm.fmt` record n naming the file in the comment file's line n
// for n in 0..16, so the cue is the room's index into that order. Record 17 (`th10_17.wav`) is
// the TH10 leftover TH12 still ships, with no room comment, exactly as in TH11.
//
// TH13 (same measurement, `tools/th-music-table.py`): `thbgm.fmt` holds **31** records, and
// `musiccmt.txt` lists 17 of them as `@bgm/th13_00` - the file writes the room's names without
// the `.wav` suffix, so a reader that insists on the suffix sees no comments at all. Those 17
// comments are the room's own order, and it is **not** the layout's: the layout pairs each
// stage theme with its boss variant (`th13_01`, `th13_01b`), the room names only the themes,
// and the room puts `th13_20` and `th13_22` before `th13_18` and `th13_19`. The first 17
// rows below are therefore that measured order; the remaining 14 records (the boss variants
// and the `th128_08` leftover) follow as cues 17..30, which is enough to reach every file
// while TH13's exe stage table - the one that says which cue a stage and its boss ask for -
// is still unread. TH12's rows above are unaffected: there the comments *are* the layout
// order, 17 of 17.
//
// Entries are file stems: AudioDevice::music() appends ".wav" to look the track up in thbgm.fmt.
struct MusicTrack {i32 cue;const char* file;};
inline constexpr MusicTrack th12_music_tracks[]={
 {0,"th12_01"},{1,"th12_00"},{2,"th12_02"},{3,"th12_04"},{4,"th12_05"},
 {5,"th12_07"},{6,"th12_08"},{7,"th12_09"},{8,"th12_10"},{9,"th12_13"},
 {10,"th12_14"},{11,"th12_16"},{12,"th12_17"},{13,"th12_18"},{14,"th12_19"},
 {15,"th12_20"},{16,"th12_21"},{17,"th10_17"}
};
inline constexpr MusicTrack th13_music_tracks[]={
 // the room's 17 comments, in the comment file's order
 {0,"th13_00"},{1,"th13_01"},{2,"th13_03"},{3,"th13_04"},{4,"th13_06"},
 {5,"th13_07"},{6,"th13_08"},{7,"th13_10"},{8,"th13_11"},{9,"th13_12"},
 {10,"th13_15"},{11,"th13_16"},{12,"th13_17"},{13,"th13_20"},{14,"th13_22"},
 {15,"th13_18"},{16,"th13_19"},
 // the records no comment names (boss variants and the TH128 leftover), in layout order
 {17,"th13_01b"},{18,"th13_03b"},{19,"th13_04b"},{20,"th13_06b"},{21,"th13_07b"},
 {22,"th13_08b"},{23,"th13_10b"},{24,"th13_11b"},{25,"th13_12b"},{26,"th13_15b"},
 {27,"th13_16b"},{28,"th13_20b"},{29,"th13_22b"},{30,"th128_08"}
};
// TH14's music room names 17 tracks; thbgm.fmt additionally holds th128_08.
inline constexpr MusicTrack th14_music_tracks[]={
 {0,"th14_01"},{1,"th14_02"},{2,"th14_03"},{3,"th14_04"},{4,"th14_05"},
 {5,"th14_06"},{6,"th14_07"},{7,"th14_09"},{8,"th14_10"},{9,"th14_11"},
 {10,"th14_12"},{11,"th14_14"},{12,"th14_15"},{13,"th14_18"},{14,"th14_19"},
 {15,"th14_16"},{16,"th14_17"},{17,"th128_08"}
};
// TH16 ships 18 records and 17 comments. The two readings agree for the first 13 and then
// differ: the layout's records 13 and 14 are `th16_14` and `th16_15` where the room writes
// `th16_16` and `th16_17`, and the prepared pair before the theme pair. The rows below are the
// comments' order, which is the order the runtime's cue table already follows on TH12, TH13 and
// TH14, and the one record no comment names (`th128_08`) follows after. tools/th-music-table.py
// prints both readings from the title's own files (the six-space block is the comments, --cpp the
// layout), and the same tool's layout reading of TH14 is not the order this file has used there
// either - the comments are.
inline constexpr MusicTrack th16_music_tracks[]={
 {0,"th16_01"},{1,"th16_02"},{2,"th16_03"},{3,"th16_04"},{4,"th16_05"},
 {5,"th16_06"},{6,"th16_07"},{7,"th16_08"},{8,"th16_09"},{9,"th16_10"},
 {10,"th16_11"},{11,"th16_13"},{12,"th16_12"},{13,"th16_16"},{14,"th16_17"},
 {15,"th16_14"},{16,"th16_15"},{17,"th128_08"}
};
// TH17's own set: 18 records, 17 comments, and here the comments agree with the records for
// all 17, so the two readings coincide.
inline constexpr MusicTrack th17_music_tracks[]={
 {0,"th17_01"},{1,"th17_03"},{2,"th17_04"},{3,"th17_05"},{4,"th17_06"},
 {5,"th17_07"},{6,"th17_08"},{7,"th17_09"},{8,"th17_10"},{9,"th17_11"},
 {10,"th17_12"},{11,"th17_14"},{12,"th17_13"},{13,"th17_17"},{14,"th17_18"},
 {15,"th17_15"},{16,"th17_16"},{17,"th128_08"}
};
// TH18: 18 records and 17 comments, 13 of which line up before the same divergence - the room
// writes the pair 17 and 18 where the layout has 15 and 16. Same reading as TH16's.
inline constexpr MusicTrack th18_music_tracks[]={
 {0,"th18_01"},{1,"th18_03"},{2,"th18_04"},{3,"th18_05"},{4,"th18_06"},
 {5,"th18_07"},{6,"th18_08"},{7,"th18_09"},{8,"th18_10"},{9,"th18_11"},
 {10,"th18_12"},{11,"th18_13"},{12,"th18_14"},{13,"th18_17"},{14,"th18_18"},
 {15,"th18_15"},{16,"th18_16"},{17,"th128_08"}
};
// One view over the current title's rows, so the loops that walk the table stay unchanged.
struct MusicTables {
    const MusicTrack* rows;u32 count;
    const MusicTrack* begin()const{return rows;}
    const MusicTrack* end()const{return rows+count;}
};
inline constexpr MusicTables music_tracks = kPortTitle==18
    ? MusicTables{th18_music_tracks,u32(sizeof(th18_music_tracks)/sizeof(MusicTrack))}
    : (kPortTitle==17)
    ? MusicTables{th17_music_tracks,u32(sizeof(th17_music_tracks)/sizeof(MusicTrack))}
    : (kPortTitle==16)
    ? MusicTables{th16_music_tracks,u32(sizeof(th16_music_tracks)/sizeof(MusicTrack))}
    : (kPortTitle==14)
    ? MusicTables{th14_music_tracks,u32(sizeof(th14_music_tracks)/sizeof(MusicTrack))}
    : (kPortTitle==13)
    ? MusicTables{th13_music_tracks,u32(sizeof(th13_music_tracks)/sizeof(MusicTrack))}
    : MusicTables{th12_music_tracks,u32(sizeof(th12_music_tracks)/sizeof(MusicTrack))};
// Original stage descriptor (4a3828 + stage*64), music-room unlock indices.
inline i32 stage_music(i32 stage,bool boss=false){return stage>=1&&stage<=7?(stage-1)*2+(boss?2:1):0;}
}
