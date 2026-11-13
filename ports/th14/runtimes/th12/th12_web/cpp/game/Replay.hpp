#pragma once
#include "PortTitle.hpp"
#include "Types.hpp"
#include <array>
#include <string>
#include <vector>

namespace th11 {
// The on-disk replay layout is the title's. TH12's file header is 0x70 bytes and each stage
// record's fixed part is 0xA0 (TH11's was 0x90); both were read out of th12.exe's replay
// loader, which computes the payload as `lea edx,[eax+0xa0]` (0x43c49c) and the next record as
// `lea eax,[eax+ecx+0xa0]` (0x43c4cc).
//
// TH13's pairs are larger, and its stage size comes out of its own loader the same way:
// `lea edx,[eax+0xc4]` (0x48db8) and `lea eax,[eax+edx+0xc4]` (0x48deb) are the same two
// shapes with 0xC4. The file header is 0x74, which is where the two disagree; it is measured
// by closing the arithmetic on every attract demo TH13 ships: 116 + 196 + each record's payload
// equals that demo's declared unpacked size for all four, and 0x70 does for none.
//
// The selection fields keep their offsets (+0x58 stages, +0x5c character, +0x60 shot type,
// +0x64 difficulty), but their bounds are the title's too: TH13 has four playable characters
// (`ShotFiles.hpp`, read out of th13.exe at 0x4c0b98) where TH12 has two, and two of its four
// demos carry character 2 and 3.
// Every later title's number was measured the same way TH13's was, and the measurement is one
// arithmetic demand: a replay's decoded body is a header and `n` records of equal stride
// (`stage_header + payload`), and the strides have to tile the declared length exactly, on every
// demo the title ships, with stage numbers 1..7 and no repeats. The solve
// (`tools/th-replay-geometry.py`) recovers TH12's 0x70/0xA0 on all four of its demos (stages
// 4, 3, 1, 1) and TH13's 0x74/0xC4 on all four (3, 2, 1, 5) before it is asked about the rest,
// and then leaves exactly one shape per title, on every demo it ships:
//   TH14 0x94 + 0xDC, TH16 0xA0 + 0x294, TH17 0xA0 + 0x158, TH18 0xC8 + 0x126C.
// TH12 therefore has the only 1-byte-shorter header and the only 0x40 first block; the four
// newer titles' records carry a much larger fixed part than TH13's 0xC4, and TH18's is larger
// again, which is the same 4-byte tail difference its version word showed.
// TH11 and TH12 share the 0x70 file header and differ in the record: the four TH11 files at
// `reference/assets` close on 0x70 plus a 0x90 fixed part on all of them (the same stride solve
// as the titles above, on that set), where TH12's close on 0xA0. Both keep the selection at
// +0x58, which is the offset the TH11 files themselves put the four in-range words at.
inline constexpr u32 replay_file_header = kPortTitle==18?0xC8u:kPortTitle==16?0xA0u:kPortTitle==17?0xA0u
    :kPortTitle==14?0x94u:kPortTitle==13?0x74u:0x70u;
inline constexpr u32 replay_stage_header = kPortTitle==18?0x126Cu:kPortTitle==16?0x294u
    :kPortTitle==17?0x158u:kPortTitle==14?0xDCu:kPortTitle==13?0xC4u:kPortTitle==12?0xA0u:0x90u;

// Where the four selection words live in the decoded body (stage count, character, shot type,
// difficulty). TH12's and TH13's are the +0x58 row the port already carried. The other four
// were found with the same demand the walk uses for shape: the four words are in range on every
// demo the title ships and the character word varies between demos. The search
// (`tools/tmp/replay-selection-search.py`) reports exactly one offset for TH12 and one for TH13 -
// the known +0x58 - before it is trusted, and then one each:
//   TH14 +0x78 (characters 0,1,2), TH16 +0x80 (0,1,2,3), TH17 +0x84 (0,1,2), TH18 +0xA8 (0,1,2,3).
// Each title's character maximum is the one its own shot table implies
// (`player_character_max`, ShotFiles.hpp: 2, 3, 2, 3, 2, 3 for 12/13/14/16/17/18), and the
// demos hit exactly that maximum in six of six titles - which is the second reading of the same
// offsets.
inline constexpr u32 replay_selection = kPortTitle==18?0xA8u:kPortTitle==17?0x84u:kPortTitle==16?0x80u
    :kPortTitle==14?0x78u:0x58u;

// The entry pair sits at a different place, and TH13's restore says where. Its own playback start
// hands the shared restore the record plus 0xC rather than the record:
//   th13.exe 0x048fe4  mov edi, dword ptr [ebx + ecx*8 + 0xd8]   ; this stage's record
//   th13.exe 0x048feb  lea ecx, [edi + 0xc]
//   th13.exe 0x048ff4  call 0x47850
// and 0x47850 reads the pair from that pointer's first two words and nothing else:
//   th13.exe 0x047855  mov edx, dword ptr [ecx]                  ; +0x0C: x
//   th13.exe 0x047857  mov dword ptr [eax + 0x5c4], edx          ; the live 1/128 coordinate
//   th13.exe 0x04785d  mov ecx, dword ptr [ecx + 4]              ; +0x10: y
//   th13.exe 0x047860  mov dword ptr [eax + 0x5c8], ecx
//   th13.exe 0x04786c  fld qword ptr [0x4ae8c0]                  ; the same 1/128 as TH12's
// Both words are the stage's start in all four attract demos - x = 0, y = 51200, which 1/128
// turns into the 400 pixels a stage puts the player at - and that is the pair TH12 keeps at
// +0x30/+0x34. TH13's +0x10 is where the same arithmetic says `PlayerMotion::last_error 10` came
// from: reading TH12's power offset there measured the height as a power level, 51200/40 = 1280.
// The pair is the generation's, and every title says where it is the same way: the stage start
// it stores is (0, 51200) in every attract demo, and the scan looks for exactly that pair inside
// the record's own fixed part and reports the offsets where it holds in all of a title's demos.
// TH12 answers +0x30 and nothing else; TH13, TH14, TH16, TH17 and TH18 all answer +0xC and
// nothing else, across their seventeen demos together. TH12 is the outlier here exactly as it is
// in the container and the selection.
inline constexpr u32 replay_state_x = kPortTitle==12?0x30u:0x0Cu;
inline constexpr u32 replay_state_y = kPortTitle==12?0x34u:0x10u;

// The same restore hands the pair over and leaves the live economy alone (`0x47850` for TH13), so
// TH12's fixed part carries the older scalar layout. TH14 has a distinct 28-word
// snapshot at +14 restored by454a51/456133; its own branch is in Replay.cpp.
// Reading TH12's score,
// lives, rank and weapon offsets out of a newer record is what the option rebuild then measured:
// TH16's +0x30 holds its character, 1, 3, 0 and 2 across its four demos, and the reset failed
// with `initial player stage reset (110)` - power divided by the power step came out past the
// eight formation levels the tables have.
inline constexpr bool replay_state_scalars = kPortTitle==12||kPortTitle==14;
inline constexpr u32 kReplayFileHeader=replay_file_header;
inline constexpr u32 kReplayStageHeader=replay_stage_header;

struct GameEconomy;struct Rng;struct PlayerMotionState;
struct ReplayStageState {
    u32 seed=0;i32 score=0,power=0,points=0,lives=0,fragments=0,rank=0;
    i32 x=0,y=0,continues=0,extends=0,focused=0,graze=0,weapon=0;
    std::array<u32,20> spell_times{};
#if PORT_TITLE == 14
    // 454a51 / 456133 restore this complete snapshot at 4f5828.
    // Preserve unnamed words while projecting known fields into the economy.
    std::array<u32,28> th14_globals{};
    i32 th14_practice_mode=-1;
    u32 th14_stage_flags=0;
#endif
    // False when the title restores only the entry pair, so `restore` must leave the live
    // economy alone instead of overwriting it with words read at another title's offsets.
    bool scalars=replay_state_scalars;
    void read(const u8* header);
    void restore(GameEconomy&,Rng&,bool first,Rng* visual=nullptr)const;
#if PORT_TITLE == 14
    void restore_economy_th14(GameEconomy&)const;
#endif
    void restore_position(PlayerMotionState&)const;
};
// TH11 1.00a t11r v4. Entries are six bytes PER TICK, not timestamped
// change records: held, pressed and released keys, each a u16. Playback
// restores the stored edges; it must not derive them from the previous tick.
struct ReplayInput {
    u32 held=0,previous=0,pressed=0,released=0;
    u32 fps=0;bool end_marker=false;
    i32 touch_mode=0;float touch_x=0,touch_y=0;
};
struct ReplayTouch {u32 frame=0;i32 mode=0;float x=0,y=0;};
struct ReplayStage {
    u32 number=0,offset=0,frames=0,payload_size=0;
};
class Replay {
public:
    bool open(const u8*,u32);
    bool select(u32 stage);
    ReplayInput tick(bool active=true);
    const ReplayStage* stage(u32 number) const;
    const u8* header(u32 number) const;
    const std::vector<u8>& decoded() const {return bytes;}
    u32 stage_count() const {return count;}
    u32 character() const;u32 subtype() const;u32 difficulty() const;
    u32 frame() const {return cursor;}
#if PORT_TITLE == 14
    // Playback+210 starts at zero;455ed0 resets it at stage activation.
    u32 callback_frame() const {return callback_cursor;}
#endif
    // The input the last tick consumed. Read-only, so a diagnostic can line the recording's own bits up against
    // what the player did with them without advancing the stream to ask.
    const ReplayInput& current() const {return input;}
    u32 selected_stage()const{return selected;}
    const std::string& error() const {return failure;}
    bool uses_touch()const;
    void retain_metadata();
private:
    std::vector<u8> bytes;std::array<ReplayStage,8> stages{};
    std::array<std::vector<ReplayTouch>,8> touches{};u32 touch_cursor=0;
    u32 count=0,selected=0,cursor=0;ReplayInput input;std::string failure;
#if PORT_TITLE == 14
    u32 callback_cursor=0;
#endif
    bool metadata_only=false;
};
}
