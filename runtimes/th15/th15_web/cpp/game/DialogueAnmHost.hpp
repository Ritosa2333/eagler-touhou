#pragma once
#include "Dialogue.hpp"
#include "AnmManager.hpp"
#include "AnmCoordinates.hpp"
namespace th15 {
struct DialogueSceneServices {
 virtual ~DialogueSceneServices()=default;
 virtual bool initialize_text(AnmVm&,i32 width,i32 height)=0;
 virtual bool paint_text(AnmVm&,const DialogueText&)=0;
 virtual i32 dialogue_text_extent(const std::string&,i32){return -1;}
 virtual bool dialogue_music(bool boss)=0;
 virtual bool fade_dialogue_music(float)=0;
 virtual bool dialogue_stage_complete()=0;
 virtual bool dialogue_name_banner()=0;
 virtual bool dialogue_sound(i32)=0;
 virtual bool clear_dialogue_field()=0;
};
// The dialogue runner owns handles; this bridge uses actual animation objects
// and balloon children. Font pixels, music and session transitions have named
// scene interfaces rather than interpreter addresses.
class DialogueAnmHost final:public DialogueHost {
 AnmManager& animations;AnmEnvironment& environment;DialogueSceneServices& scene;
 bool fail(const char*);
public:
 std::string error;
 DialogueAnmHost(AnmManager& a,AnmEnvironment& e,DialogueSceneServices& s):animations(a),environment(e),scene(s){}
 bool create(i32 resource,i32 script,u32&)override;
 bool interrupt(u32 handle,i32 label,bool immediate=false)override;
 bool retire(u32&)override;
 bool text_initialize(u32)override;
 bool text(const DialogueText&)override;
 i32 text_extent(const std::string& value,i32 font)override{return scene.dialogue_text_extent(value,font);}
 bool position(u32,const Vec3&)override;
 bool depth(u32,float)override;
 bool balloon_width(u32,i32 style,float)override;
 bool create_balloon(i32 resource,i32 script,const Vec3&,float width,i32 style,u32&)override;
 bool follow_balloon(DialogueState&)override;
 bool music(bool boss)override{return scene.dialogue_music(boss);}
 bool fade_music(float duration)override{return scene.fade_dialogue_music(duration);}
 bool stage_complete()override{return scene.dialogue_stage_complete();}
 bool name_banner()override{return scene.dialogue_name_banner();}
 bool sound(i32 id)override{return scene.dialogue_sound(id);}
 bool clear_field()override{return scene.clear_dialogue_field();}
 bool release(DialogueState&);
};
}
