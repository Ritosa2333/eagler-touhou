#include "MusicRoom.hpp"
#include <cstdio>
#include <cstring>
#include "Localization.hpp"
namespace th10 {
// 0x4349e0 / 0x434a20. Parsing consumes consecutive CR/LF bytes and replaces the
// first terminator with NUL. An unterminated final line is not copied.
char* skip_music_line(char* p,i32& remaining) noexcept {while(*p!='\n'&&*p!='\r'){if(!remaining)return p;++p;--remaining;}while(remaining&&(*p=='\n'||*p=='\r')){++p;--remaining;}return p;}
char* copy_music_line(char* p,char* destination,i32& remaining) noexcept {auto* start=p;while(*p!='\n'&&*p!='\r'){if(!remaining)return p;++p;--remaining;}if(remaining){*p=0;std::memcpy(destination,start,std::strlen(start)+1);--remaining;++p;while(remaining&&(*p=='\n'||*p=='\r')){++p;--remaining;}}return p;}
void MusicRoom::fill_comment(){
    auto& m=menu;auto& env=environment;if(m.elapsed.current%2||m.music_filled_comments>=8)return;
    auto* vm=env.registry->find_and_clear(m.music_comment_animations[m.music_filled_comments]);const bool locked=!env.unlocked[m.music_playing]&&m.music_warning;
    const char* line=nullptr;
    // The original eight-row warning leaves rows 0,2,5 blank and localizes the
    // message rows 1,3,4,6,7 with the five "Music Room spoiler N" records.
    // Mapping them in order would push the last two messages off the end and
    // surface the untouched Japanese rows as fallback.
    if(locked){static constexpr std::int8_t spoiler_row[8]={0,1,0,2,3,0,4,5};const i32 row=m.music_filled_comments;
        if(Localization::Active()&&spoiler_row[row]){char id[40];std::snprintf(id,sizeof(id),"th10 Music Room spoiler %d",static_cast<int>(spoiler_row[row]));line=Localization::StringById(id,env.locked_comments[row]);}
        else line=env.locked_comments[row];
    }
    else{
        line=Localization::MusicComment(u32(m.music_playing+1),u16(m.music_filled_comments),m.music_comments[m.music_playing][m.music_filled_comments]);
        // thcrap's music.cpp BP_music_cmt resolves a literal "@" comment line to
        // the track's Music Room note title instead of drawing the separator.
        if(line!=nullptr&&std::strcmp(line,"@")==0)line=Localization::MusicTitle(u32(m.music_playing+1),m.music_titles[m.music_playing]);
    }
    env.text(*vm,locked?0x8080ff:0xffffff,line);vm->pending_interrupt=2;++m.music_filled_comments;
}
void MusicRoom::layout(bool create){
    auto& m=menu;auto& env=environment;const i32 first=create?wrapping_add(m.elapsed.current*2,-2):0,last=create?m.elapsed.current*2:m.music_track_count;
    Vec3 position{64,0,0};const auto initial=number(96)-Extended::from_int(m.music_scroll)*number(20);position.y=(create?initial+Extended::from_int(static_cast<i32>(static_cast<u32>(m.elapsed.current)*40u-40u)):initial).to_float();
    for(i32 i=first;i<last&&i<m.music_track_count;++i){auto& id=m.music_title_animation(i);if(create)id=env.create(*m.animations,i+173);auto* vm=env.registry->find_and_clear(id);
        if(create){if(env.unlocked[i]){
            // The original musiccmt.txt title already carries its "No.%d  "
            // prefix, but localized titles do not. Add the prefix only for a
            // translated title; an untranslated fallback is used verbatim.
            const char* title=Localization::MusicTitle(u32(i+1),m.music_titles[i]);
            if(title==m.music_titles[i])env.text(*vm,0xffffff,title);else env.numbered_text(*vm,0xffffff,i+1,title);
        }else env.locked_text(*vm,0xffffff,i+1);}
        if(i<m.music_scroll||i>=m.music_scroll+10)vm->flags&=~2u;else vm->flags|=2;
        if(i==m.menu.selected)position.x=Scalar::sub(position.x,4);
        const auto distance=number(vm->script_position.y)-number(position.y);const auto absolute=distance<number(0)?-distance:distance;
        if(!create&&(absolute.is_nan()||!(absolute<number(40))))vm->script_position=position;else interpolate_menu_position(*vm,position,vm->script_position,4,0,*env.tangent,env.rate);
        if(i==m.menu.selected)position.x=Scalar::add(position.x,4);position.y=Scalar::add(position.y,20);vm->pending_interrupt=i==m.menu.selected?2:3;
    }
}
void MusicRoom::leave(){auto& m=menu;auto& env=environment;if(m.music_file){env.free_file(m.music_file);m.music_file=nullptr;}m.music_file=nullptr;m.menu.pop();for(i32 i=0;i<m.music_track_count;++i)env.registry->interrupt(m.music_title_animation(i),1);for(auto id:m.music_comment_animations)env.registry->interrupt(id,1);env.sound(11);m.set_phase(3,env.rate);}
// 0x433ef0. Song titles are created two per tick, comments one every two ticks.
// Selecting an unheard track first displays the original warning; selecting it
// again permits playback without marking that track as encountered in-game.
i32 MusicRoom::update(){
    auto& m=menu;auto& env=environment;
    switch(m.phase){
    case 0:
        if(m.elapsed.current==1){
            m.menu.item_count=6;m.menu.select(0);
            if(!env.registry->find(m.animation_ids[95])){m.create_script(95,env);m.animation_ids[195]=env.create(**env.effects,8);}
            m.create_script(103,env);i32 remaining=0;m.music_file=env.read_file(remaining);if(!m.music_file){leave();return 0;}
            i32 count=0;auto* cursor=m.music_file;while(remaining>0){if(*cursor!='@')cursor=skip_music_line(cursor,remaining);else{if(count>=32){leave();return 0;}cursor=copy_music_line(cursor+1,m.music_files[count],remaining);cursor=copy_music_line(cursor,m.music_titles[count],remaining);for(auto& line:m.music_comments[count])cursor=copy_music_line(cursor,line,remaining);++count;}}
            m.menu.item_count=count;m.menu.select(0);m.music_scroll=0;m.music_track_count=count;
            for(i32 i=0;i<8;++i)m.music_comment_animations[i]=env.create(**env.comment_file,i+39);
            m.music_filled_comments=m.music_playing=m.music_warning=0;
        }
        if(m.elapsed.current<10)layout(true);if(m.elapsed.current>=10)m.set_phase(1,env.rate);return 0;
    case 1:
        fill_comment();if(m.elapsed.current>=5)m.set_phase(2,env.rate);return 0;
    case 2:
        fill_comment();m.menu.reserved=m.menu.selected;if((*env.pressed|*env.repeated)&0x10)m.menu.move(-1);if((*env.pressed|*env.repeated)&0x20)m.menu.move(1);
        if(m.menu.reserved!=m.menu.selected){env.sound(12);if(m.menu.selected<m.music_scroll)m.music_scroll=m.menu.selected;else if(m.menu.selected>=m.music_scroll+10)m.music_scroll=m.menu.selected-9;layout(false);m.music_warning=0;}
        if(*env.pressed&0x1001){for(auto id:m.music_comment_animations)env.registry->interrupt(id,3);m.music_playing=m.menu.selected;m.music_filled_comments=0;m.reset_timer(env.rate);
            if(!env.unlocked[m.music_playing]&&!m.music_warning){env.music_command(*env.display_flags&16?4:3);m.music_warning=1;return 0;}
            env.load_music(m.music_files[m.menu.selected]);if(*env.display_flags&16)env.music_command(4);env.music_command(2);env.unlocked[0]=1;m.music_warning=0;return 0;
        }
        if(*env.pressed&10)leave();return 0;
    case 3:
        if(m.elapsed.current>=10){m.dismiss_script(103,env);m.signal_script(90,8,env);m.signal_script(91,8,env);m.dismiss_script(95,env);env.registry->interrupt(m.animation_ids[195],1);m.set_screen(2,env.rate);env.load_music("bgm/th10_02.wav");env.play_music();m.menu.pop();}return 0;
    default:return 0;
    }
}
}
