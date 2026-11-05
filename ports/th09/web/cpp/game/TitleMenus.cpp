#include "TitleMenus.hpp"
#include "Localization.hpp"
#include <algorithm>
namespace th09 {
namespace {
#include "TitleData.inc"
}
bool TitleMenus::initialize(){
    std::memset(descriptions.data(),0,sizeof(descriptions));std::memset(music_comments.data(),0,sizeof(music_comments));
    if(!resources.load(AnimationFile::text,"text.anm")||!resources.load(AnimationFile::menu,"title01.anm")){error=resources.error;return false;}
    for(i32 i=0;i<14;++i){auto& a=descriptions[i];if(!resources.start(AnimationFile::text,a,5,false)||!resources.sprite(AnimationFile::text,a,a.activeSpriteIndex+i)){error="Title description animation";return false;}}
    ready=true;return true;
}
void TitleMenus::interrupt(i32 label){for(auto& a:animations)if(a.loadedSprite&&a.loadedSprite->anmIdx>=0)a.pendingInterrupt=i16(label);}
void TitleMenus::advance(){for(auto& a:animations)resources.advance(a);}
i32 TitleMenus::navigate(i32& cursor,i32 count,const InputFrame& input,bool horizontal){
    if(count<=0)return 0;const u16 keys=input.pressed|input.repeat;i32 step=0;
    if(horizontal){if(keys&0x40)step=-1;else if(keys&0x80)step=1;else if(keys&0x10)step=-1;else if(keys&0x20)step=1;}
    else {if(keys&0x10)step=-1;else if(keys&0x20)step=1;}
    if(!step)return 0;cursor+=step;if(cursor<0)cursor+=count;if(cursor>=count)cursor-=count;output.title_sound(12);return step;
}
void TitleMenus::select(i32 selected,i32 base,i32 count){
    if(base<0||count<0||u32(base+count)>animations.size()||selected<0||u32(base+selected)>=animations.size()){error="Title selection bounds";return;}
    for(i32 n=base;n<base+count;++n){auto& a=animations[n];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+1);a.pendingInterrupt=8;}
    auto& a=animations[base+selected];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex);a.pendingInterrupt=7;
}
bool TitleMenus::create_animations(){if(!animations.empty())return true;animations.resize(223);for(i32 n=0;n<223;++n){auto& a=animations[n];if(!resources.start(AnimationFile::menu,a,n)){error="Title animations";return false;}a.baseSpriteIndex=a.activeSpriteIndex;}return true;}
bool TitleMenus::main(){
    auto& s=state;const auto& in=input[2];
    if(s.state==0){
        if(s.load_frame==0){
            const auto previous=s.previous_screen;if(i32(previous)==0||previous==TitleScreen::score_name||previous==TitleScreen::music||previous==TitleScreen::replay_name)output.title_music(0);
            if(i32(previous)==0||previous==TitleScreen::rankings||previous==TitleScreen::score_name||previous==TitleScreen::replays||previous==TitleScreen::replay_name||previous==TitleScreen::music||previous==TitleScreen::story_difficulty||previous==TitleScreen::extra_difficulty||previous==TitleScreen::versus_difficulty||previous==TitleScreen::versus_type){if(!output.title_background("title00.png"))return false;}
            if(!create_animations())return false;
            if(settings.extra_unlocked[12]){auto& extra=animations[2];extra.color1.r=extra.color1.g=extra.color1.b=255;}
            interrupt(2);advance();if(!settings.extra_unlocked[12])animations[2].color1.d3dColor=0x30908040;
            if(s.return_to_versus){settings.game_flags&=~8u;s.selection=settings.versus;change(TitleScreen::versus_type);interrupt(5);return true;}
            s.selection_base=1;s.selection_count=8;select(s.selection,1,8);
            if(settings.game_flags&8){change(TitleScreen::replays);interrupt(5);return true;}settings.game_flags&=~2u;
        }
        if(s.load_frame<8){
            constexpr const char* ids[]={"th09 Menu Story Start","th09 Menu Extra Start","th09 Menu Match Start","th09 Menu Replay","th09 Menu Result","th09 Menu Music Room","th07 Menu Option","th07 Menu Quit"};
            output.title_text(descriptions[s.load_frame],Localization::StringById(ids[s.load_frame],title_help[s.load_frame]),0xfff0e0,0x300000);++s.load_frame;return true;
        }
        s.previous_selection=-1;s.load_frame=s.frames=0;s.state=1;s.idle_frames=0;
    }
    if(s.state==1){
        const i32 direction=navigate(s.selection,8,in);if(direction){if(!settings.extra_unlocked[12]&&s.selection==1)s.selection=1+direction;select(s.selection,s.selection_base,s.selection_count);}
        if(s.previous_selection!=s.selection){s.description=s.selection;descriptions[s.selection].pendingInterrupt=1;}s.previous_selection=s.selection;if(!settings.extra_unlocked[12])animations[2].color1.d3dColor=i32(0xb0b0b040u);
        if(s.load_frame>=10){
            if(in.held)s.idle_frames=0;if(++s.idle_frames>1500){settings.game_flags|=10;output.title_demo(settings.demo_index);settings.demo_index=(settings.demo_index+1)%3;leaving=true;return false;}
            if(in.pressed&0x1001){
                if(s.selection<=2){if(s.selection==1&&!settings.extra_unlocked[12]){output.title_sound(39);return true;}settings.game_flags&=~8u;output.title_sound(10);const auto next=s.selection==0?TitleScreen::story_difficulty:s.selection==1?TitleScreen::extra_difficulty:TitleScreen::versus_type;if(s.selection==2)s.selection=settings.versus;change(next);interrupt(5);settings.health[0]=settings.health[1]=10;return true;}
                if(s.selection>=3&&s.selection<=5){output.title_sound(10);const auto next=s.selection==3?TitleScreen::replays:s.selection==4?TitleScreen::rankings:TitleScreen::music;change(next);interrupt(5);if(next!=TitleScreen::rankings&&s.description>=0)descriptions[s.description].pendingInterrupt=2;return true;}
                if(s.selection==6){output.title_sound(10);s.state=s.selection=s.load_frame=s.frames=0;if(!options())return false;select(settings.extra_lives,18,3);select(settings.windowed,21,2);select(settings.music_mode,23,3);select(settings.screen_mode,34,2);s.state=3;s.frames=0;s.selection=0;}
                else if(s.selection==7){s.state=2;s.frames=0;interrupt(1);if(settings.music_mode==2)output.title_music_fade();}
            }
            if(in.pressed&10){auto& old=animations[s.selection+1];resources.sprite(AnimationFile::menu,old,old.baseSpriteIndex+1);s.selection=7;auto& a=animations[8];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex);output.title_sound(11);}
        }
    }else if(s.state==2&&s.frames>59){animations.clear();s.load_frame=0;output.title_exit();leaving=true;return false;}
    else if(s.state==3&&s.frames>29){change(TitleScreen::options);s.selection=0;return true;}
    tick();return true;
}
bool TitleMenus::difficulty(){
    auto& s=state;const auto& in=input[2];
    if(s.state==0){if(!s.load_frame){if(!output.title_background("select00.png"))return false;interrupt(s.screen==TitleScreen::extra_difficulty?25:12);advance();
        if(s.screen==TitleScreen::versus_difficulty){animations[131+settings.versus].pendingInterrupt=9;if(s.return_to_versus){change(TitleScreen::versus_character);if(!versus_character())return false;animations[136+settings.difficulty].pendingInterrupt=10;s.return_to_versus=0;return true;}}
        s.selection=s.screen==TitleScreen::extra_difficulty?4:settings.difficulty>3?1:settings.difficulty;s.selection_base=136;s.selection_count=4;select(s.selection,136,4);s.layout_changed=1;s.state=s.frames=0;s.description=-1;
    }if(s.load_frame==8)s.state=1;tick();return true;}
    if(s.state==1){
        if(s.screen==TitleScreen::extra_difficulty){if(in.pressed&0x30)output.title_sound(12);}else if(navigate(s.selection,4,in))select(s.selection,136,4);
        if(in.pressed&0x1001){settings.difficulty=u8(s.selection);output.title_sound(10);const bool vs=s.screen==TitleScreen::versus_difficulty;change(vs?TitleScreen::versus_character:s.screen==TitleScreen::story_difficulty?TitleScreen::story_character:TitleScreen::extra_character);if(!(vs?versus_character():character()))return false;animations[136+settings.difficulty].pendingInterrupt=vs?10:9;return true;}
        if(in.pressed&10){settings.difficulty=u8(s.selection);output.title_sound(11);s.frames=0;if(s.screen==TitleScreen::versus_difficulty){change(TitleScreen::versus_type);return true;}interrupt(6);s.state=3;}
    }else if(s.state==3&&s.frames>19){const auto previous=s.screen;change(TitleScreen::main);s.selection=previous!=TitleScreen::story_difficulty;return true;}
    tick();return true;
}
bool TitleMenus::versus_type(){
    auto& s=state;const auto& in=input[2];if(s.state==0){if(!s.load_frame){if(s.previous_screen!=TitleScreen::versus_difficulty&&!output.title_background("select00.png"))return false;interrupt(11);advance();if(s.return_to_versus){change(TitleScreen::versus_difficulty);if(!difficulty())return false;animations[131+settings.versus].pendingInterrupt=9;return true;}s.selection=settings.versus;s.selection_base=131;s.selection_count=5;select(s.selection,131,5);s.layout_changed=1;s.state=s.frames=0;s.description=-1;}if(s.load_frame==8)s.state=1;}
    else if(s.state==1){if(navigate(s.selection,5,in))select(s.selection,131,5);if(in.pressed&0x1001){settings.versus=s.selection;if(settings.versus==4){output.title_network();return true;}output.title_sound(10);change(TitleScreen::versus_difficulty);if(!difficulty())return false;animations[131+settings.versus].pendingInterrupt=9;return true;}if(in.pressed&10){settings.versus=s.selection;output.title_sound(11);s.state=3;s.frames=0;interrupt(6);}}
    else if(s.state==3&&s.frames>19){change(TitleScreen::main);s.selection=2;return true;}tick();return true;
}
bool TitleMenus::update_screen(){switch(state.screen){case TitleScreen::main:return main();case TitleScreen::story_difficulty:case TitleScreen::extra_difficulty:case TitleScreen::versus_difficulty:return difficulty();case TitleScreen::story_character:case TitleScreen::extra_character:return character();case TitleScreen::versus_type:return versus_type();case TitleScreen::versus_character:return versus_character();case TitleScreen::versus_stage:return versus_stage();case TitleScreen::options:return options();case TitleScreen::music:return music();case TitleScreen::key_config:return key_config();case TitleScreen::rankings:return rankings();case TitleScreen::score_name:return score_name();case TitleScreen::replays:return replays();case TitleScreen::replay_name:return replay_name();default:error="Title screen implementation pending";return false;}}
bool TitleMenus::update(const InputFrame& left,const InputFrame& right,const InputFrame& menu){if(!ready||!error.empty())return false;input[0]=left;input[1]=right;input[2]=menu;const bool result=update_screen();advance();if(state.description>=0&&state.description<i32(descriptions.size()))resources.advance(descriptions[state.description]);return result;}
void TitleMenus::draw(){output.title_begin_draw();for(auto& a:animations)if(a.visible){const auto position=a.pos;a.pos={a.pos.x+a.pos2.x,a.pos.y+a.pos2.y,a.pos.z+a.pos2.z};output.title_sprite(a,a.rotation.z!=0);a.pos=position;}if(state.description>=0&&state.description<i32(descriptions.size()))output.title_sprite(descriptions[state.description],false);if(state.screen==TitleScreen::music&&state.state==1){auto& a=animations[159+state.current_music];const auto p=a.pos;const auto color=a.color1;a.pos.x=208;a.pos.y=293;a.color1.d3dColor=-1;output.title_sprite(a,false);a.pos=p;a.color1=color;for(auto& comment:music_comments)output.title_sprite(comment,false);}if(state.screen==TitleScreen::rankings||state.screen==TitleScreen::score_name)draw_rankings();if(state.screen==TitleScreen::replays)draw_replays();if(state.screen==TitleScreen::replay_name)draw_replay_name();}
}
