#include "TitleMenu.hpp"
#include "Localization.hpp"
#include <cstdio>
#include <cstring>
namespace th11 {
namespace {
i32 spell_difficulty(i32 id){return kPortTitle==14?(id<26?(id+2)%4:id<108?id%4:4):(id<162?(id+2)%4:4);}
i32 spell_pages(i32 difficulty){i32 n=0;for(i32 id=0;id<ScoreFile::spell_count;++id)if(spell_difficulty(id)==difficulty)++n;return(n+9)/10+1;}
i32 integer(const u8* p){u32 v;std::memcpy(&v,p,4);return signed_bits(v);}
constexpr i32 record_text_base=kPortTitle==14?262:194;
}
void TitleMenu::record_spells(){
    i32 skip=(page.selected-1)*10;record_rows=0;
    for(i32 id=0;id<ScoreFile::spell_count&&record_rows<10;++id){
        if(spell_difficulty(id)!=secondary.selected)continue;
        if(skip){--skip;continue;}
        const auto* aggregate=scores.characters[6].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride;
        const auto* own=scores.characters[cursor.selected].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride;
        char line[512];u32 color=0x808080;
        if(integer(aggregate+(kPortTitle==14?0x88:0x84))){
            size_t n=0;while(n<(kPortTitle==14?128:64)&&aggregate[n])++n;
            std::string name(reinterpret_cast<const char*>(aggregate),n);
            const char* translated=Localization::SpellName(u32(id),name.c_str());
            const bool localized=translated!=name.c_str();
            if(localized)name=translated;
            else if(name.size()<42)name.resize(42,' ');
            const char* fallback="No.%3d %s %4d/%4d";
            const char* format=localized?Localization::FormatStringById("th10 Result Known Spell",fallback):fallback;
            std::snprintf(line,sizeof(line),format,id+1,name.c_str(),integer(own+0x80),integer(own+(kPortTitle==14?0x88:0x84)));
            color=integer(own+0x80)?0xffff80:0xefefef;
        }else {std::string fallback="No.%3d ";for(i32 i=0;i<21;++i)fallback+="\x81\x48";
            fallback+=" %4d/%4d";
            const char* format=Localization::FormatStringById("th10 Result Unknown Spell",fallback.c_str());
            std::snprintf(line,sizeof(line),format,id+1,integer(own+0x80),integer(own+(kPortTitle==14?0x88:0x84)));}
        if(!exists(record_text_base+record_rows))handles[record_text_base+record_rows]=0;
        text_requests.push_back({handles[record_text_base+record_rows],color,0,0,0,line,false,true});++record_rows;
    }
    for(i32 i=record_rows;i<10;++i){if(!exists(record_text_base+i))handles[record_text_base+i]=0;text_requests.push_back({handles[record_text_base+i],0xffffffff,0,0,0," ",false,true});}
}
void TitleMenu::records(u32 pressed,u32 repeat){
    switch(substate){
    case 0:
        cursor.count=6;cursor.select(0);secondary.count=5;secondary.select(1);secondary.wrap=1;
        page.count=spell_pages(secondary.selected);page.select(0);page.wrap=1;
        if constexpr(kPortTitle==14){if(!exists(261))create(19,261,2);create(98);}
        else{if(!exists(92)){create(92);create(18,193,2);}create(100);create(cursor.selected/3+150);}
        step(1);create(cursor.selected+152);create(secondary.selected+158);
        for(const i32 script:{166,167,168,169,163,164,165,170})create(script);
        [[fallthrough]];
    case 1:if(timer.current>6)step(2);break;
    case 2:
        cursor.previous=cursor.selected;secondary.previous=secondary.selected;page.previous=page.selected;
        if((pressed|repeat)&16){secondary.move(-1);family(168,2,true);}
        if((pressed|repeat)&32){secondary.move(1);family(169,2,true);}
        if(secondary.previous!=secondary.selected){
            sounds.push_back(12);close(secondary.previous+158);create(secondary.selected+158);
            if(page.selected>0){page.select(1);record_spells();}page.count=spell_pages(secondary.selected);
        }
        if((pressed|repeat)&64){cursor.move(-1);family(166,2,true);}
        if((pressed|repeat)&128){cursor.move(1);family(167,2,true);}
        if(cursor.previous!=cursor.selected){
            sounds.push_back(12);if constexpr(kPortTitle!=14)if(cursor.previous/3!=cursor.selected/3){close(cursor.previous/3+150);create(cursor.selected/3+150);}
            close(cursor.previous+152);create(cursor.selected+152);if(page.selected>0)record_spells();
        }
        if(pressed&0x80001){
            if(page.selected==0)for(i32 i=0;i<10;++i)create((kPortTitle==14?3:25)+i,record_text_base+i,3);
            page.move(1);if(page.selected==0)for(i32 i=0;i<10;++i)family(record_text_base+i,1);else record_spells();sounds.push_back(10);
        }
        if(secondary.selected==4&&cursor.selected==(kPortTitle==14?4:3)){
            constexpr u8 legacy_code[]={31,30,21,23,38,24,47,18,48,18,18,19};
            constexpr u8 th14_code[]={24,19,23,24,49,48,18,18,19};
            const auto* code=kPortTitle==14?th14_code:legacy_code;
            constexpr i32 code_length=kPortTitle==14?9:12;
            if(pressed&0x80103)unlock_progress=unlock_timeout=0;
            if(unlock_progress<code_length){
                if(key_edges[code[unlock_progress]]&128){++unlock_progress;unlock_timeout=0;}
                else for(i32 i=0;i<57;++i)if(key_edges[i]&128){unlock_progress=0;break;}
            }else{
                // Retail459300 is reached only after the original hidden code.
                std::memset(scores.settings.data()+0x16,0x11,16);
                if constexpr(kPortTitle==14){
                    for(i32 id=0;id<ScoreFile::spell_count;++id){auto* p=scores.characters[6].data()+ScoreFile::spell_offset+id*ScoreFile::spell_stride+0x88;const i32 seen=integer(p);if(seen<99999){const u32 value=u32(wrapping_add(seen,1));std::memcpy(p,&value,4);}}
                    for(i32 c=0;c<6;++c){auto* p=scores.characters[c].data()+0x7dc;const u32 value=u32(wrapping_add(integer(p),1));std::memcpy(p,&value,4);for(i32 i=0;i<ScoreFile::practice_slots;++i)scores.characters[c][ScoreFile::practice_flags_offset+9+i*8]=1;}
                }else for(i32 c=0;c<6;++c)for(i32 i=0;i<ScoreFile::practice_slots;++i)scores.characters[c][ScoreFile::practice_flags_offset+9+i*8]=1;
                sounds.push_back(kPortTitle==14?17:44);unlock_progress=0;
            }
            if(++unlock_timeout>300)unlock_progress=unlock_timeout=0;
        }
        if(pressed&0x102){
            step(3);sounds.push_back(11);close(secondary.selected+158);close(cursor.selected+152);if constexpr(kPortTitle!=14)close(cursor.selected/3+150);
            for(const i32 script:{166,167,168,169,163,164,165,170})close(script);
            for(i32 i=0;i<10;++i)family(record_text_base+i,1);
        }break;
    case 3:
        if(timer.current>=6){if constexpr(kPortTitle==14){close(98);close(261);}else{close(100);close(92);family(193,1);}change(TitleScreen::Main);cursor.pop();}break;
    }
}
}
