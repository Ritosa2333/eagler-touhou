#include "TitleMenu.hpp"
#include "Localization.hpp"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <utility>
namespace th11 {
namespace {
const char* unheard_warning[]={
 "\x81\x40",
 "\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96\x81\x96",
 " ",
 "\x81\x40\x81\x40\x81\x40\x81\x40\x8b\xc8\x82\xcc\x83\x52\x83\x81\x83\x93\x83\x67\x82\xaa\x83\x6c\x83\x5e\x83\x6f\x83\x8c\x82\xc9\x82\xc8\x82\xe9\x8b\xb0\x82\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb7\x81\x42",
 "\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x82\xbb\x82\xea\x82\xc5\x82\xe0\x8d\xc4\x90\xb6\x82\xb5\x82\xdc\x82\xb7\x82\xa9\x81\x48",
 "\x81\x40",
 "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xa2\x95\xfb\x82\xcd\x82\xe0\x82\xa4\x88\xea\x93\x78\x8c\x88\x92\xe8\x83\x7b\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2",
 "\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xad\x82\xc8\x82\xa2\x95\xfb\x82\xcd\x81\x41\x83\x4a\x81\x5b\x83\x5c\x83\x8b\x82\xf0\x88\xda\x93\xae\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2"
};
}
bool TitleMenu::load_music_comments(const u8* data,u32 size){
    if(!data||size>1024*1024)return false;std::vector<MusicEntry> next;u32 p=0;
    auto line=[&](std::string& out,u32 limit){const u32 begin=p;while(p<size&&data[p]!='\n'&&data[p]!='\r'&&data[p])++p;if(p-begin>=limit)return false;out.assign(reinterpret_cast<const char*>(data+begin),p-begin);while(p<size&&(data[p]=='\n'||data[p]=='\r'))++p;return true;};
    while(p<size&&data[p]){
        if(data[p]!='@'){while(p<size&&data[p]!='\n'&&data[p]!='\r'&&data[p])++p;while(p<size&&(data[p]=='\n'||data[p]=='\r'))++p;continue;}
        ++p;MusicEntry e;if(next.size()>=20||!line(e.path,64)||!line(e.title,66))return false;
        for(auto& comment:e.comment)if(!line(comment,66))return false;
        next.push_back(std::move(e));
    }
    if(next.empty())return false;music_entries=std::move(next);return true;
}
void TitleMenu::music_position(i32 index,bool initial){
    auto* vm=animations.find(handles[210+index]);if(!vm){handles[210+index]=0;return;}
    const bool visible=index>=music_state.scroll&&index<music_state.scroll+10;
    vm->flags=visible?vm->flags|2:vm->flags&~2u;
    const Vec3 target{index==cursor.selected?60.f:64.f,float(96+(index-music_state.scroll)*20),0};
    if(!initial&&std::abs(float(double(vm->script_position.y)-target.y))>=40)vm->script_position=target;
    else {auto& v=vm->position_interpolation;v.duration=4;v.mode=InterpolationMode::Linear;
        std::memcpy(v.start,&vm->script_position,12);std::memcpy(v.end,&target,12);
        std::memcpy(v.initial_tangent,&animations.default_tangent,12);std::memcpy(v.final_tangent,&animations.default_tangent,12);v.timer.set(0,&animations.rate);}
    vm->pending_interrupt=index==cursor.selected?2:3;
}
void TitleMenu::music_comment(){
    auto& s=music_state;if(timer.current%2||s.line>=8)return;
    auto* vm=animations.find(handles[230+s.line]);if(!vm){error="music comment animation missing";return;}
    const bool prompt=!scores.settings[0x26+s.selected]&&s.prompt;
    std::string value;
    bool centered=false;
    if(prompt){
        static constexpr i32 spoiler_row[8]={0,1,0,2,3,0,4,5};
        const char* translated=unheard_warning[s.line];
        if(spoiler_row[s.line]){char id[48];std::snprintf(id,sizeof id,"th10 Music Room spoiler %d",spoiler_row[s.line]);
            translated=Localization::StringById(id,translated);}
        value=translated;
        // thcrap's <c$...$> carries centered-text markup rather than glyphs.
        if(value.size()>=5&&value.rfind("<c$",0)==0&&value.compare(value.size()-2,2,"$>")==0){
            value=value.substr(3,value.size()-5);centered=true;
        }
    }else{
        const auto& original=music_entries[s.selected].comment[s.line];
        const char* translated=Localization::MusicComment(u32(s.selected+1),u16(s.line),original.c_str());
        if(std::strcmp(translated,"@")==0)translated=Localization::MusicTitle(u32(s.selected+1),music_entries[s.selected].title.c_str());
        value=translated;
    }
    text_requests.push_back({vm->id,prompt?0x8080ffu:0xffffffu,0,0,0,std::move(value),false,centered});
    vm->pending_interrupt=2;++s.line;
}
void TitleMenu::music_room(u32 pressed,u32 repeat){
    auto& s=music_state;
    switch(substate){
    case 0:
        if(timer.current==1){
            cursor.count=6;cursor.select(0);if(!exists(93)){create(93);create(18,193,2);}create(101);
            if(music_entries.empty()){error="music comments missing";return;}
            cursor.count=i32(music_entries.size());cursor.select(0);s.count=cursor.count;s.scroll=0;
            for(i32 line=0;line<8;++line)create(41+line,230+line,3);s.line=s.selected=s.prompt=0;
        }
        if(timer.current>0&&timer.current<10)for(i32 i=timer.current*2-2;i<timer.current*2&&i<s.count;++i){
            create(171+i,210+i);const char* translated=Localization::MusicTitle(u32(i+1),music_entries[i].title.c_str());
            std::string name=translated;
            if(translated!=music_entries[i].title.c_str()){
                char prefix[16];std::snprintf(prefix,sizeof prefix,"No.%2d ",i+1);name=prefix+name;
            }
            if(!scores.settings[0x26+i]){
                std::string fallback="No.%2d ";for(i32 n=0;n<11;++n)fallback+="\x81\x48";
                const char* format=Localization::FormatStringById("Music Room Unknown Title",fallback.c_str());
                char line[256];std::snprintf(line,sizeof line,format,i+1);name=line;
            }
            text_requests.push_back({handles[210+i],0xffffff,0,0,0,std::move(name)});music_position(i,true);
        }
        if(timer.current>=10)step(1);break;
    case 1:music_comment();if(timer.current>4)step(2);break;
    case 2:
        music_comment();move(pressed|repeat,16,32);
        if(cursor.previous!=cursor.selected){sounds.push_back(12);
            if(cursor.selected<s.scroll)s.scroll=cursor.selected;else if(cursor.selected>=s.scroll+10)s.scroll=cursor.selected-9;
            for(i32 i=0;i<s.count;++i)music_position(i,false);if(s.line>7)s.prompt=0;
        }
        if(pressed&0x80001){
            for(i32 i=0;i<8;++i)family(230+i,3);
            s.selected=cursor.selected;s.line=0;timer.set(0,&animations.rate);
            if(!scores.settings[0x26+s.selected]&&!s.prompt){music_pause=true;s.prompt=1;return;}
            music_request=s.selected;scores.settings[0x26]=1;s.prompt=0;return;
        }
        if(pressed&0x102){cursor.pop();for(i32 i=0;i<s.count;++i)family(210+i,1);for(i32 i=0;i<8;++i)family(230+i,1);sounds.push_back(11);step(3);}break;
    case 3:
        if(timer.current>=10){close(101);close(93);family(193,1);change(TitleScreen::Main);music_request=0;cursor.pop();}break;
    }
}
}
