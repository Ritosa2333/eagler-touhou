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
const char* players[]={"ReimuA ","ReimuB ","ReimuC ","MarisaA","MarisaB","MarisaC"};
const char* difficulties[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
const char* stage_label(i32 stage,bool brief=false){return stage>=0&&stage<9?(brief?brief_stages[stage]:stages[stage]):"???";}
std::string replay_line(const TitleMenu::ReplayEntry* entry,i32 index){
    char line[256];
#ifdef TH11_MULTIPLAYER
    if(!entry){if(index<99)std::snprintf(line,sizeof(line),"No.%.2d -------- --/--/-- --:-- ------- ------- --- -----",index+1);else std::snprintf(line,sizeof(line),"User  -------- --/--/-- --:-- ------- ------- --- -----");return line;}
    if(entry->is_multiplayer){
        const auto& r=entry->multiplayer;const auto t=calendar(reinterpret_cast<const u8*>(&r.timestamp));
        char date[32];if(r.timestamp)std::snprintf(date,sizeof(date),"%.2d/%.2d/%.2d %.2d:%.2d",t.tm_year%100,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min);else std::snprintf(date,sizeof(date),"--/--/-- --:--");
        char prefix[16];if(index<99)std::snprintf(prefix,sizeof(prefix),"No.%.2d",index+1);
        else{const auto slash=entry->path.find_last_of("/\\");const auto base=entry->path.substr(slash==std::string::npos?0:slash+1);std::snprintf(prefix,sizeof(prefix),"%.4s ",base.size()>=11?base.c_str()+7:"????");}
        std::snprintf(line,sizeof(line),"%s %-8.8s %s %s %s %s %uP/P%u",prefix,r.name.c_str(),date,players[r.selection],difficulties[r.difficulty],stage_label(r.completed?8:r.last_stage,true),r.player_count,r.recorded_player+1);
        return line;
    }
#endif
    if(!entry){if(index<25)std::snprintf(line,sizeof(line),"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%",index+1);else std::snprintf(line,sizeof(line),"User  -------- --/--/-- --:-- ------- ------- --- ---%%");return line;}
    const auto& r=entry->replay;const auto* p=r.decoded().data();const auto t=calendar(p+12);
    char prefix[16];if(index<25)std::snprintf(prefix,sizeof(prefix),"No.%.2d",index+1);
    else{const auto slash=entry->path.find_last_of("/\\");const auto base=entry->path.substr(slash==std::string::npos?0:slash+1);std::snprintf(prefix,sizeof(prefix),"%.4s ",base.size()>=11?base.c_str()+7:"????");}
    std::snprintf(line,sizeof(line),"%s %.8s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",prefix,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,players[r.character()*3+r.subtype()],difficulties[r.difficulty()],stage_label(integer(p+0x68),true),double(real(p+0x54)));
    return line;
}
}
void TitleMenu::queue_ascii(AsciiText& out)const{
    char line[256];AsciiStyle style;style.shadow=true;
    auto add=[&](float x,float y){out.add(line,{x,y,0},style);};
    if(screen==TitleScreen::Practice&&(substate==2||substate==3)&&(timer.current>=10||substate==3)){
        const auto* p=scores.characters[selection.character*3+selection.partner].data();
        for(i32 st=1;st<=6;++st){const i32 index=st+selection.difficulty*6;const bool available=p[0x5a1+index*8];
            style.color=cursor.selected!=st-1?0xff808080:!available?0xffdfdfdf:substate==3&&timer.current%4>=2?0xff000000:0xffffff00;
            if(available)std::snprintf(line,sizeof(line),"%s  %.8d0",stages[st],integer(p+0x59c+index*8));else std::snprintf(line,sizeof(line),"%s  ---------",stages[st]);
            add(selection.character?296:168,float(216+(st-1)*18));
        }
    }else if((screen==TitleScreen::Records||screen==TitleScreen::Results)&&substate==2){
        const bool result=screen==TitleScreen::Results;
        const auto* p=scores.characters[result?selection.character*3+selection.partner:cursor.selected].data();
        if(result||!page.selected)for(i32 rank=0;rank<10;++rank){const auto* row=p+0x10+((result?selection.difficulty:secondary.selected)*10+rank)*28;style.color=result&&!result_unranked?(cursor.selected==rank?0xffffffff:0xff404040):0xff0000ff|u32(255-rank*16)*0x10100;
            u64 stamp;std::memcpy(&stamp,row+16,8);
            if(stamp){const auto t=calendar(row+16);std::snprintf(line,sizeof(line),"%2d  %.8s  %9d%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%",rank+1,reinterpret_cast<const char*>(row+6),integer(row),i8(row[5]),t.tm_year+1900,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,stage_label(i8(row[4])),double(real(row+24)));}
            else std::snprintf(line,sizeof(line),"%2d  %.8s  %9d%d  ----/--/-- --:--  Stage -  ---%%",rank+1,reinterpret_cast<const char*>(row+6),integer(row),i8(row[5]));add(48,float(160+rank*18));
        }
        if(!result){style.color=0xffffffff;std::snprintf(line,sizeof(line),"    %5d",integer(p+0x588));add(328,378);
        const i32 time=integer(p+0x58c);std::snprintf(line,sizeof(line),"%3d:%.2d:%.2d",time/216000,time/3600%60,time/60%60);add(328,396);
        std::snprintf(line,sizeof(line),"    %5d",integer(p+0x590+secondary.selected*4));add(328,414);}
    }else if(screen==TitleScreen::Replays||screen==TitleScreen::ReplaySave){
        if(substate==2){for(i32 row=0;row<25;++row){const i32 index=(screen==TitleScreen::ReplaySave?0:page.selected*25)+row;
#ifdef TH11_MULTIPLAYER
            if(multiplayer_replay_menu&&index==99)continue;
#endif
            style.color=cursor.selected==row?0xffffff00:0xff808080;out.add(replay_line(replay_files[index].get(),index).c_str(),{58,float(80+row*15),0},style);}}
        else if(substate==4&&replay_files[replay_file]){
            const auto& entry=*replay_files[replay_file];const auto& r=entry.replay;const auto* p=r.decoded().data();
            const float y=timer.current<10?float(80+double((replay_file%25)*15)*(10-double(timer.fractional))/10):80;
            out.add(replay_line(replay_files[replay_file].get(),replay_file).c_str(),{58,y,0},style);
            if(timer.current>=10)for(i32 st=1;st<=7;++st){style.color=cursor.selected==st-1?0xffffff00:0xff808080;
                if(!entry.has_stage(st))std::snprintf(line,sizeof(line),"%s  ---------",stages[st]);
#ifdef TH11_MULTIPLAYER
                // Like TH10MP, chapters name a confirmed input frame; the
                // archive does not contain retail per-stage score snapshots.
                else if(entry.is_multiplayer)std::snprintf(line,sizeof(line),"%s",stages[st]);
#endif
                else{const auto* next=st<6?r.header(st+1):nullptr;std::snprintf(line,sizeof(line),"%s  %.8d%d",stages[st],integer(next?next+12:p+0x14),integer(next?next+40:p+0x6c));}
                add(220,float(128+(st-1)*18));
            }
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
