#pragma once
#include "EndingFrame.hpp"
#include "AnmManager.hpp"
#include "TextLayout.hpp"
#include "RecordStore.hpp"
namespace th17 {
struct EndingScenePlatform:EndingDestination {
 virtual bool read_message(const std::string&,MessageResource&)=0;
 virtual bool text(AnmVm&,const TextureText&)=0;
 virtual bool request_animation(i32 bank,const std::string&)=0;
 virtual bool prepare_animation(AnmResource&)=0;virtual bool cancel_animation_request()=0;
 virtual bool loading_overlay()=0;virtual bool end_loading_overlay()=0;
 virtual bool prepare_music(const std::string&)=0;virtual bool start_music(i32)=0;
 virtual bool fade_music(i32)=0;virtual bool sound(i32)=0;
 virtual bool shake(i32,i32)=0;virtual bool reset_caption_state()=0;
};
class EndingScene final:private EndingServices {
 AnmManager& animations;RecordStore& records;EndingScenePlatform& host;MessageResource program;i32 text_bank;
 bool create_text(i32,u32&)override;bool text(u32,const std::string&,u32)override;
 bool interrupt(u32,i32)override;bool retire(u32&)override;
 bool load_animation(i32,const std::string&)override;bool create_animation(i32,i32,u32&)override;
 bool loading_overlay()override;bool end_loading_overlay()override;bool read_staff(const std::string&,MessageResource&)override;
 bool prepare_music(const std::string&)override;bool start_music(i32)override;bool fade_music(i32)override;bool sound(i32)override;
 bool shake(i32,i32)override;bool reset_caption_state()override;bool check(bool,const std::string&);
public:
 std::string error;EndingScript script;EndingFrame frame;
 EndingScene(AnmManager&,RecordStore&,EndingScenePlatform&,FrameScheduler&,i32 text_bank=6);~EndingScene();
 bool initialize(const GameRun&,i32 preview_index=-1);
 static const char* filename(i32)noexcept;
 bool animation_ready(i32 bank,const u8*,u32);
 void controls(const EndingInput& input,u32 flags=0)noexcept{frame.controls(input,flags);}
};
}
