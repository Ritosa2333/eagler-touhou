#pragma once
#include "MessageProgram.hpp"
#include "Timer.hpp"
#include <array>
namespace th15 {
struct DialogueInput {float rate=1;u32 pressed=0,held=0;u32 shoot_frames=0,skip_frames=0;i32 stage=1;};
struct DialogueState {
    Timer age{},clock{},wait{};u32 cursor=0;std::array<u32,12> handles{};
    std::array<Vec3,3> anchors{{{16,0,0},{16,0,0},{16,0,0}}};
    i32 complete=0;u32 flags=0;i32 line=0,box_visible=0,speaker=0;std::array<u32,3> colors{};
    Vec3 position{384,640,0};float width=320;i32 portrait_state=0,balloon_style=0;
    bool bubble_position_pending=false,bubble_right=false;
};
struct DialoguePortrait {i32 resource=-1,script=-1;};
struct DialogueResources {i32 text=-1,player=-1,logo=-1,balloons=-1;std::array<DialoguePortrait,4> enemies{},names{};i32 character=0;};
struct DialogueText {u32 handle=0;std::string bytes;i32 font=0,style=0;u32 color=0;Vec3 position{};bool move=false;std::array<i32,2> ruby{};u32 secondary_color=0;i32 offset=0;bool right_aligned=false;bool offset_pixels=false;bool music_detail=false;};
// Text rasterization, BGM and the graphics backend are separate named services.
struct DialogueHost {
    virtual ~DialogueHost()=default;
    virtual bool create(i32 resource,i32 script,u32&)=0;
    virtual bool interrupt(u32 handle,i32 label,bool immediate=false)=0;
    virtual bool retire(u32&)=0;virtual bool text_initialize(u32)=0;
    virtual bool text(const DialogueText&)=0;virtual bool position(u32,const Vec3&)=0;
    virtual i32 text_extent(const std::string&,i32){return -1;}
    virtual bool depth(u32,float)=0;virtual bool balloon_width(u32,i32,float)=0;
    virtual bool create_balloon(i32 resource,i32 script,const Vec3& at,float width,i32 style,u32& handle){return create(resource,script,handle)&&position(handle,at)&&balloon_width(handle,style,width);}
    virtual bool follow_balloon(DialogueState&)=0;
    virtual bool music(bool boss)=0;virtual bool fade_music(float)=0;
    virtual bool stage_complete()=0;virtual bool name_banner()=0;virtual bool sound(i32)=0;
    virtual bool clear_field()=0;
};
class Dialogue {
    const MessageScript& script;DialogueHost& host;DialogueResources resources;i32 input_stage_=1;
    bool reset_lines();bool reset_depth();bool remove_balloon();bool select_speaker(i32);
    bool draw_line(const MessageInstruction&);bool execute(const MessageInstruction&);bool check(bool,const char*);
public:
    DialogueState state;std::string error;bool ended=false;
    Dialogue(const MessageScript& script,DialogueHost& host,DialogueResources resources):script(script),host(host),resources(resources){}
    bool initialize();int update(const DialogueInput&);
};
}
