#pragma once
#include "MessageResource.hpp"
#include "Timer.hpp"
#include <array>
namespace th17 {
struct EndingInput {u32 held=0,pressed=0,held_frames=0;float rate=1;};
inline i32 ending_index(i32 character,i32 animal,i32 continues)noexcept{return continues?character+13:character*3+animal;}
struct EndingRun {i32 difficulty=0,continues=0;u32 first_seen_flags=0;bool preview=false;};
struct EndingState {
 Timer age,clock,wait;std::array<u32,5> text_handles{};
 u32 instruction_offset=0,flags=1;i32 line=0;u32 color=0xffffff;
 std::array<i32,4> banks{{-1,-1,-1,-1}};std::array<u32,16> sprites{};
 i32 loading_bank=-1;std::string loading_name;
};
struct EndingServices {
 virtual ~EndingServices()=default;
 virtual bool create_text(i32 script,u32&)=0;virtual bool text(u32,const std::string&,u32)=0;
 virtual bool interrupt(u32,i32)=0;virtual bool retire(u32&)=0;
 virtual bool load_animation(i32 bank,const std::string&)=0;
 virtual bool create_animation(i32 bank,i32 script,u32&)=0;
 virtual bool loading_overlay()=0;virtual bool end_loading_overlay()=0;
 virtual bool read_staff(const std::string&,MessageResource&)=0;
 virtual bool prepare_music(const std::string& stem)=0;virtual bool start_music(i32 track)=0;
 virtual bool fade_music(i32 seconds)=0;virtual bool sound(i32)=0;
 virtual bool shake(i32 kind,i32 amount)=0;virtual bool reset_caption_state()=0;
};
// The original ending and staff MSG interpreter. Instructions refer to named
// resources; file preparation, glyph upload and audio belong to the platform.
class EndingScript {
 EndingServices& host;const MessageScript* script=nullptr;MessageResource staff;
 u32 instruction=0;bool check(bool,const char*);bool advance();bool reset(const MessageScript&,bool create_text);
public:
 EndingState state;EndingRun run;std::string error;bool finished=false;
 explicit EndingScript(EndingServices& services):host(services){}
 bool initialize(const MessageScript&);bool animation_ready(i32 bank);
 bool step(const EndingInput&);u32 position()const noexcept{return instruction;}
 static const char* staff_filename(const EndingRun&)noexcept;
};
}
