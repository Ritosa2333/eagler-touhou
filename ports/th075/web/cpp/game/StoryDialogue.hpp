#pragma once
#include "FighterActor.hpp"
#include "FighterScript.hpp"
namespace th075 {
struct DialoguePlatform {
    virtual ~DialoguePlatform()=default;
    virtual void text(const std::string&,u8 red,u8 green,u8 blue,u32 size,u8 part,bool continuation)=0;
    virtual void origin(float x,float y,bool battleLayout)=0;
    virtual void select_music(i16)=0;virtual void start_music()=0;
};
struct StoryDialogueCache {i8 record=-1,part=-1;};
void set_story_portrait(FighterActor&,i8 image,u8 visible,u8 brightness);
void show_story_portrait(FighterActor&,u8 image,u8 direction);
void advance_story_portrait(FighterActor&);
void set_story_dialogue_visible(FighterActor&,bool,u8 stage,DialoguePlatform&);
void begin_story_dialogue(FighterActor&,i16 label,u8 stage,DialoguePlatform&);
bool advance_story_dialogue(FighterActor&,FighterActor&,const FighterScript&,u8 stage,bool automatic,StoryDialogueCache&,DialoguePlatform&);
}
