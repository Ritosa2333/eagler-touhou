#include "TitleMenus.hpp"
#include "Localization.hpp"
#include <algorithm>
#include <cstdio>
namespace th09 {
namespace {
#include "TitleData.inc"
// The pinned original locked_music_name is a "%5s " format fed the first five
// columns of the untranslated title, which start with "No.x". A localized
// title has no such prefix, so locked rows format the track number directly
// with the same placeholder instead of slicing the real title (TH10
// MusicRoom::locked_text).
constexpr const char* locked_title="\x4e\x6f\x2e\x25\x64\x20\x20\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48";
}
void TitleMenus::music_list(){
    auto& s=state;for(i32 n=0;n<s.music_count;++n)animations[159+n].pendingInterrupt=n==s.selection?7:8;
    for(i32 n=0;n<30;++n){auto& a=animations[159+n];a.flag1=n>=s.music_scroll&&n<std::min(s.music_count,s.music_scroll+10);if(a.flag1)a.pos.y=float((n-s.music_scroll+1)*18)+104.f-20.f;}
}
bool TitleMenus::music(){
    auto& s=state;const auto& in=input[2];
    if(!s.state){if(!s.load_frame){
        if(!output.title_background("music00.png"))return false;interrupt(16);advance();s.selection=s.music_scroll=s.state=s.frames=0;s.description=-1;
        std::vector<u8> data;if(!resources.read("musiccmt.txt",data)){error="Music commentary resource";return false;}music_entries.clear();
        size_t at=0;const auto line=[&](){const size_t first=at;while(at<data.size()&&data[at]!=10&&data[at]!=13)++at;std::string text(reinterpret_cast<const char*>(data.data()+first),at-first);while(at<data.size()&&(data[at]==10||data[at]==13))++at;return text;};
        while(at<data.size()){if(data[at]!='@'){++at;continue;}++at;MusicEntry entry;entry.file=line();entry.title=line();for(u32 n=0;n<7&&at<data.size()&&data[at]!='@';++n)entry.comments[n]=line();music_entries.push_back(std::move(entry));}
        if(music_entries.empty()||music_entries.size()>30){error="Music commentary count";return false;}s.music_count=i32(music_entries.size());
        for(i32 n=0;n<s.music_count;++n){auto& entry=music_entries[n];
            // The original musiccmt.txt title already carries its "No.x  "
            // prefix, but localized titles do not. Add the prefix only for a
            // translated title; an untranslated fallback is used verbatim.
            const std::string original_title=entry.title;
            const char* translated=Localization::MusicTitle(u32(n+1),original_title.c_str());
            if(translated==original_title.c_str())entry.title=original_title;
            else{char numbered[192];std::snprintf(numbered,sizeof(numbered),"No.%d  %s",n+1,translated);entry.title=numbered;}
            for(u32 line=0;line<entry.comments.size();++line)entry.comments[line]=Localization::MusicComment(u32(n+1),u16(line),entry.comments[line].c_str());
        }
        for(i32 n=0;n<s.music_count;++n){auto& a=animations[159+n];if(!resources.start(AnimationFile::menu,a,159+n,false))return false;const auto& entry=music_entries[n];
            if(settings.music_unlocked[n])output.title_text(a,entry.title.c_str(),0xd0e0ff,0x302080);
            else{char text[160]{};std::snprintf(text,sizeof(text),Localization::Utf8(locked_title),n+1);output.title_text(a,text,0x80a0a0,0x100040);}
            a.pos={93,float((n+1)*18)+104.f-20.f,0};a.anchor=3;
        }
        music_list();s.current_music=s.selection;for(i32 n=0;n<8;++n)resources.start(AnimationFile::text,music_comments[n],n+6,false);s.music_paused=0;
    }if(s.load_frame==8){s.state=1;s.load_frame=0;}}
    else if(s.state==1){
        if(s.load_frame>=2&&s.load_frame<=16&&s.load_frame%2==0){const i32 n=s.load_frame/2-1;auto& a=music_comments[n];a.pendingInterrupt=1;
            constexpr const char* spoilerIds[]={"th09 Music Room spoiler 1","th09 Music Room spoiler 2","th09 Music Room spoiler 3"};
            const char* text=settings.music_unlocked[s.current_music]?music_entries[s.current_music].comments[n].c_str():n<3?Localization::StringById(spoilerIds[n],locked_music_help[n]):locked_music_help[n];output.title_text(a,text,0xe0e0ff,0x302080);}
        if(navigate(s.selection,s.music_count,in)){if(s.selection<s.music_scroll)s.music_scroll=s.selection;else if(s.selection>=s.music_scroll+10)s.music_scroll=std::max(0,s.selection-9);music_list();}
        if(in.pressed&4)output.title_music_fade();if(in.pressed&0x200){output.title_music_pause(!s.music_paused);s.music_paused=1-s.music_paused;}
        if(in.pressed&0x4000){if(s.music_paused)output.title_music_pause(false);s.music_paused=0;output.title_music_file(music_entries[s.current_music].file.c_str());}
        if(in.pressed&0x1001){if(s.music_paused)output.title_music_pause(false);s.current_music=s.selection;s.music_paused=0;output.title_music_file(music_entries[s.selection].file.c_str());s.load_frame=0;for(i32 n=0;n<8;++n)resources.start(AnimationFile::text,music_comments[n],6+n,false);}
        else if(in.pressed&10){output.title_sound(11);change(TitleScreen::main);s.selection=5;return true;}
    }
    for(auto& a:music_comments)resources.advance(a);tick();return true;
}
}
