#include "TitleMenu.hpp"
#include <cstdio>
#include <ctime>
#include <cstring>
namespace th11 {
namespace {
i32 integer(const u8* p){u32 v;std::memcpy(&v,p,4);return signed_bits(v);}
float real(const u8* p){float v;std::memcpy(&v,p,4);return v;}
std::tm calendar(const u8* p){i64 stamp;std::memcpy(&stamp,p,8);std::time_t t=std::time_t(stamp);auto* tm=std::localtime(&t);return tm?*tm:std::tm{};}
const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  "};
const char* brief_stages[]={"tst","St1","St2","St3","St4","St5","St6","Ex ","All"};
const char* players[]={"ReimuA ","ReimuB ","MarisaA","MarisaB","SanaeA","SanaeB"};
const char* players14[]={"ReimuA ","ReimuB ","MarisaA","MarisaB","SakuyaA","SakuyaB"};
const char* difficulties[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
const char* stage_label(i32 stage,bool brief=false){return stage>=0&&stage<9?(brief?brief_stages[stage]:stages[stage]):"???";}
std::string replay_line(const TitleMenu::ReplayEntry* entry,i32 index){
    char line[256];
    if(!entry){if(index<25)std::snprintf(line,sizeof(line),"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%",index+1);else std::snprintf(line,sizeof(line),"User  -------- --/--/-- --:-- ------- ------- --- ---%%");return line;}
    const auto& r=entry->replay;const auto* p=r.decoded().data();const auto t=calendar(p+12);
    char prefix[16];if(index<25)std::snprintf(prefix,sizeof(prefix),"No.%.2d",index+1);
    else{const auto slash=entry->path.find_last_of("/\\");const auto base=entry->path.substr(slash==std::string::npos?0:slash+1);std::snprintf(prefix,sizeof(prefix),"%.4s ",base.size()>=11?base.c_str()+7:"????");}
    if(kPortTitle==14&&index<25&&(p[10]&2))
        std::snprintf(line,sizeof(line),"%s %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%",prefix,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,players14[r.character()*2+r.subtype()],signed_bits(u32(integer(p+0x90))+1),double(real(p+0x74)));
    else std::snprintf(line,sizeof(line),"%s %.8s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",prefix,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,(kPortTitle==14?players14:players)[r.character()*2+r.subtype()],difficulties[r.difficulty()],stage_label(integer(p+(kPortTitle==14?0x88:0x68)),true),double(real(p+(kPortTitle==14?0x74:0x54))));
    return line;
}
}
void TitleMenu::queue_ascii(AsciiText& out)const{
    char line[256];AsciiStyle style;style.shadow=true;
    auto add=[&](float x,float y){out.add(line,{x,y,0},style);};
    if(screen==TitleScreen::Practice&&(substate==2||substate==3)&&(timer.current>=10||substate==3)){
        const auto* p=scores.characters[selection.character*2+selection.partner].data();
        for(i32 st=1;st<=6;++st){const i32 index=st+selection.difficulty*ScoreFile::practice_stage_stride;const bool available=p[ScoreFile::practice_flags_offset+1+index*8];
            style.color=cursor.selected!=st-1?0xff808080:!available?0xffdfdfdf:substate==3&&timer.current%4>=2?0xff000000:0xffffff00;
            if(available)std::snprintf(line,sizeof(line),"%s  %.8d0",stages[st],integer(p+ScoreFile::practice_score_offset+index*8));else std::snprintf(line,sizeof(line),"%s  ---------",stages[st]);
            add(kPortTitle==14?320:selection.character?296:168,float((kPortTitle==14?192:216)+(st-1)*18));
        }
    }else if((screen==TitleScreen::Records||screen==TitleScreen::Results)&&substate==2){
        const bool result=screen==TitleScreen::Results;
        const auto* p=scores.characters[result?selection.character*2+selection.partner:cursor.selected].data();
        if(result||!page.selected)for(i32 rank=0;rank<10;++rank){const auto* row=p+0x10+((result?selection.difficulty:secondary.selected)*10+rank)*28;style.color=result&&!result_unranked?(cursor.selected==rank?0xffffffff:0xff404040):0xff0000ff|u32(255-rank*16)*0x10100;
            u64 stamp;std::memcpy(&stamp,row+16,8);
            if(stamp){const auto t=calendar(row+16);std::snprintf(line,sizeof(line),"%2d  %.8s  %9d%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",rank+1,reinterpret_cast<const char*>(row+6),integer(row),i8(row[5]),t.tm_year+1900,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,stage_label(i8(row[4])),double(real(row+24)));}
            else std::snprintf(line,sizeof(line),"%2d  %.8s  %9d%d  ----/--/-- --:--  Stage -  ---%%",rank+1,reinterpret_cast<const char*>(row+6),integer(row),i8(row[5]));add(48,float(160+rank*18));
        }
        if(!result){style.color=0xffffffff;std::snprintf(line,sizeof(line),"    %5d",integer(p+ScoreFile::play_count_offset));add(328,378);
        const i32 time=integer(p+ScoreFile::play_time_offset);std::snprintf(line,sizeof(line),"%3d:%.2d:%.2d",time/216000,time/3600%60,time/60%60);add(328,396);
        std::snprintf(line,sizeof(line),"    %5d",integer(p+ScoreFile::clear_count_offset+secondary.selected*4));add(328,414);}
    }else if(screen==TitleScreen::Replays||screen==TitleScreen::ReplaySave){
        if(substate==2){for(i32 row=0;row<25;++row){const i32 index=(screen==TitleScreen::ReplaySave?0:page.selected*25)+row;style.color=cursor.selected==row?0xffffff00:0xff808080;out.add(replay_line(replay_files[index].get(),index).c_str(),{58,float(80+row*15),0},style);}}
        else if(substate==4&&replay_files[replay_file]){
            const auto& r=replay_files[replay_file]->replay;const auto* p=r.decoded().data();
            const float y=timer.current<10?(kPortTitle==14?80.f+((10.f-timer.fractional)*float((replay_file%25)*15))/10.f:float(80+double((replay_file%25)*15)*(10-double(timer.fractional))/10)):80;
            out.add(replay_line(replay_files[replay_file].get(),replay_file).c_str(),{58,y,0},style);
            if(timer.current>=10)for(i32 st=1;st<=7;++st){style.color=cursor.selected==st-1?0xffffff00:0xff808080;
                if(!r.stage(st))std::snprintf(line,sizeof(line),"%s  ---------",stages[st]);
                else{const auto* next=st<6?r.header(st+1):nullptr;std::snprintf(line,sizeof(line),"%s  %.8d%d",stages[st],integer(next?next+(kPortTitle==14?0x1c:12):p+0x14),integer(next?next+(kPortTitle==14?0x24:40):p+(kPortTitle==14?0x8c:0x6c)));}
                add(220,float(128+(st-1)*18));
            }
        }
    }
    if(kPortTitle==14&&(screen==TitleScreen::SpellRecords||screen==TitleScreen::SpellDifficulty)&&substate>0&&(substate<=2||(substate==3&&screen==TitleScreen::SpellRecords))){
        style.font=2;style.shadow=true;
        for(i32 row=0;row<5;++row){const i32 id=spell_choices[row];if(id<-1)continue;
            const auto* aggregate=scores.characters[6].data()+ScoreFile::spell_offset+id*i32(ScoreFile::spell_stride);
            const auto* own=scores.characters[secondary.selected].data()+ScoreFile::spell_offset+id*i32(ScoreFile::spell_stride);
            const bool selected=screen==TitleScreen::SpellDifficulty&&cursor.selected==row;
            style.color=integer(own+0x84)?selected?0xff90d0ff:0xff60a0c0:selected?0xffb0b0b0:0xff404040;
            const float y=191.f+row*44.f;
            if(!integer(aggregate+0x88)&&!integer(aggregate+0x8c)){std::snprintf(line,sizeof(line),"SCORE        00  ----/----");add(330,y);}
            else{std::snprintf(line,sizeof(line),"SCORE %8d0  %4d/%4d",integer(own+0x98),integer(own+0x84),integer(own+0x8c));add(330,y);
                style.color=integer(own+0x80)?0xff206060:0xff404040;std::snprintf(line,sizeof(line),"GAME MODE        %4d/%4d",integer(own+0x80),integer(own+0x88));add(330,y+10.f);}
        }
    }
    const bool result_name=screen==TitleScreen::Results&&substate==2&&!result_unranked;
    const bool replay_name=screen==TitleScreen::ReplaySave&&substate==3;
    if(replay_name&&pending_replay){
        auto entry=*pending_replay;auto title=replay_line(&entry,replay_file);if(title.size()>=14)title.replace(6,8,"        ");
        const float y=timer.current<10?float(240+double(replay_file*15+80-240)*(10-double(timer.fractional))/10):240;
        style.color=0xffffffff;style.shadow=false;out.add(title.c_str(),{58,y,0},style);
    }
    if(result_name||(replay_name&&timer.current>=10)){
        const float x=result_name?84:112,y=result_name?float(160+cursor.selected*18):240;
        style.color=0xffffffff;style.shadow=result_name;out.add(entered_name.data(),{x,y,0},style);
        style.color=0xffffff00;out.add("_",{x+float((name_length==8?7:name_length)*9),y,0},style);
        constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
        for(i32 i=0;i<91;++i){char glyph[2]={i<88?letters[i]:i==88?char(0x81):i==89?char(0x7f):char(0x80),0};style.color=name_cursor.selected==i?0xffffff00:0xff808080;out.add(glyph,{float(212+(i%13)*18),float(360+(i/13)*16),0},style);}
    }
}
}
