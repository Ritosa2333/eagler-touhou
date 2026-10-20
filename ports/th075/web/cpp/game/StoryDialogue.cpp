#include "StoryDialogue.hpp"
namespace th075 {
void set_story_portrait(FighterActor& a,i8 image,u8 visible,u8 brightness){if(image>=0&&image<=a.portraitMaximum)a.noticePortrait=image;a.notice.visible=visible;a.notice.brightness=brightness;}
void show_story_portrait(FighterActor& a,u8 image,u8 direction){a.notice.mode=u8(2-u32(direction));set_story_portrait(a,i8(image),1,u8(u32(direction)*127+128));a.notice.duration=16;}
void advance_story_portrait(FighterActor& a){auto& p=a.notice;if(p.mode==0){if(p.duration<25)p.duration=i16(u16(p.duration)+1);}else if(p.mode==1||p.mode==2){if(p.duration>0)p.duration=i16(u16(p.duration)-1);if(p.mode==1){if(p.level>0)p.level=i16(u16(p.level)-1);}else if(p.level<5)p.level=i16(u16(p.level)+1);}}
void set_story_dialogue_visible(FighterActor& a,bool visible,u8 stage,DialoguePlatform& platform){if(visible)platform.origin(35,360,i8(stage)<8);a.notice.dialogueVisible=visible?1:0;}
void begin_story_dialogue(FighterActor& a,i16 label,u8 stage,DialoguePlatform& platform){a.dialogue={label,0,0,5};set_story_dialogue_visible(a,true,stage,platform);}
namespace {
void consume_branch(StoryDialogueCursor& cursor){i8 bit=1;cursor.part=1;while((i32(cursor.branches)&i32(bit))==0&&bit!=0){cursor.part=i16(u16(cursor.part)+1);bit=i8(u8(bit)<<1);}cursor.branches=i16(u16(cursor.branches)-u16(i16(bit)));}
}
bool advance_story_dialogue(FighterActor& a,FighterActor& other,const FighterScript& script,u8 stage,bool automatic,StoryDialogueCache& cache,DialoguePlatform& platform){
    auto& cursor=a.dialogue;const i16 block=cursor.label>=0&&u32(cursor.label)<script.labels.size()?script.labels[cursor.label]:-1;
    const i16 count=block>=0&&u32(block)<script.blocks.size()?i16(script.blocks[block].size()):0;
    const auto done=[&]{cache.record=cache.part=-1;return true;};
    if(count<=cursor.record||cursor.record<0)return done();
    const auto& command=script.blocks[block][cursor.record];const u8 kind=u8(command.kind);const i16 argument=command.argument;
    if(i32(cache.record)!=cursor.record||i32(cache.part)!=cursor.part){
        const auto display=[&](u8 r,u8 g,u8 b){platform.text(command.text,r,g,b,i8(stage)<8?24:16,u8(cursor.part),i32(cache.record)==cursor.record&&i32(cache.part)!=cursor.part);};
        if(kind<15)switch(command.kind){
        case StoryCommand::Player:case StoryCommand::Opponent:{
            FighterActor& active=command.kind==StoryCommand::Player?a:other;
            FighterActor& inactive=command.kind==StoryCommand::Player?other:a;
            if(argument<0)active.notice.mode=0;
            else{active.notice.mode=1;set_story_portrait(active,i8(argument),1,255);}
            if(command.kind==StoryCommand::Player)display(120,120,255);else display(255,120,120);
            if(argument>=0&&i8(inactive.notice.mode)>0){inactive.notice.mode=2;set_story_portrait(inactive,-1,0,128);}
            break;}
        case StoryCommand::Narrator:display(120,255,120);break;
        case StoryCommand::Face:a.scriptCursor.waiting=u8(argument);break;
        case StoryCommand::Music:platform.select_music(argument);platform.start_music();break;
        case StoryCommand::Wait:a.scriptCursor.musicDelay=argument;break;
        case StoryCommand::Voice:a.scriptCursor.musicId=argument;break;
        case StoryCommand::Route:cursor.branches=argument;if(cursor.branches<1)cursor.part=5;else consume_branch(cursor);break;
        }
        cache.record=i8(cursor.record);cache.part=i8(cursor.part);
    }
    if(i8(kind)<10){
        if(a.confirmCount==1||automatic){
            if(cursor.branches<1){cursor.part=5;cursor.record=i16(u16(cursor.record)+1);if(cursor.record==count)return done();}
            else{consume_branch(cursor);if(cursor.branches==0)cursor.part=5;}
        }
    }else{cursor.record=i16(u16(cursor.record)+1);if(cursor.record==count)return done();}
    return false;
}
}
