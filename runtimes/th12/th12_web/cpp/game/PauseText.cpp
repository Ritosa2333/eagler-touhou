#include "PauseMenu.hpp"
#include <cstdio>
#include <ctime>
namespace th11 {
namespace {
i32 integer(const u8* p){i32 n;std::memcpy(&n,p,4);return n;}
std::tm calendar(u64 stamp){std::time_t t=std::time_t(stamp);const auto* tm=std::localtime(&t);return tm?*tm:std::tm{};}
const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  "};
const char* brief[]={"tst","St1","St2","St3","St4","St5","St6","Ex ","All"};
const char* players[]={"ReimuA ","ReimuB ","MarisaA","MarisaB","SanaeA","SanaeB"};
const char* difficulties[]={"E","N","H","L","X"};
#if PORT_TITLE == 14
const char* players14[]={"ReimuA ","ReimuB ","MarisaA","MarisaB","SakuyaA","SakuyaB"};
const char* difficulties14[]={"E ","N ","H ","L ","EX"};
#endif
const char* label(i32 stage,bool short_name=false){return stage>=0&&stage<9?(short_name?brief[stage]:stages[stage]):"???";}
}
void PauseMenu::queue_ascii(AsciiText& out)const{
    AsciiStyle style;style.shadow=true;char line[256];
    auto add=[&](const char* s,float x,float y){out.add(s,{x,y,0},style);};
#if PORT_TITLE == 16
    // Own43edc0 normal game-over draw requests; font rasterization is separate.
    if(state==2){
        if(th16_substate==15){
            add("            Score Ranking!!",48,64);
            if(!unranked){
                const float y=96.f+float(cursor.selected)*18.f;add(entered_name.data(),75,y);
                style.color=0xffffff00;add("_",75.f+9.f*float(name_length)-(name_length==8?9.f:0.f),y);
                constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*";
                for(i32 i=0;i<91;++i){char glyph[2]={i<88?letters[i]:i==88?char(0x81):i==89?char(0x7f):char(0x80),0};style.color=name_cursor.selected==i?0xffffff00:0xff808080;add(glyph,112.f+float(i%13)*18.f,320.f+float(i/13)*16.f);}style.color=0xffffffff;
            }
            if(selection>=0&&selection<5&&difficulty>=0&&difficulty<6)for(i32 rank=0;rank<10;++rank){
                style.color=!unranked&&cursor.selected==rank?0xffffff00:0xff808080;
                const auto* p=scores.th16_characters[selection].data()+0x10+(rank+difficulty*10)*32;u64 stamp;std::memcpy(&stamp,p+16,8);
                if(!stamp)std::snprintf(line,sizeof line,"%2d %s %9d%d --/--/-- Stage -",rank+1,reinterpret_cast<const char*>(p+6),integer(p),i8(p[5]));
                else{const auto t=calendar(stamp);const i32 reached=i8(p[4]);const char* stage_name=reached==9?"ExClear":label(reached);std::snprintf(line,sizeof line,"%2d %s %9d%d %.2d/%.2d/%.2d %s",rank+1,reinterpret_cast<const char*>(p+6),integer(p),i8(p[5]),t.tm_year%100,t.tm_mon+1,t.tm_mday,stage_name);}
                add(line,48,96.f+float(rank)*18.f);
            }style.color=0xffffffff;
        }
        if(th16_substate!=14){std::snprintf(line,sizeof line,"Credit %d",th16_continues_remaining);add(line,184,448);}
    }return;
#endif
#if PORT_TITLE == 14
    // Retail4485d0 state15 draws the remaining credits below the ANM choices.
    if(state==14){constexpr i32 limits[]={3,3,4,5,0};const i32 credit=difficulty>=0&&difficulty<5?limits[difficulty]-continues:0;
        std::snprintf(line,sizeof line,"Credit %d",credit);add(line,184,448);return;}
#endif
    auto name=[&](float x,float y){
        add(entered_name.data(),x,y);style.color=0xffffff00;add("_",x+9*(name_length==8?7:name_length),y);
        constexpr char letters[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
        for(i32 i=0;i<91;++i){char glyph[2]={i<88?letters[i]:i==88?char(0x81):i==89?char(0x7f):char(0x80),0};style.color=name_cursor.selected==i?0xffffff00:0xff808080;add(glyph,float(112+i%13*18),float(320+i/13*16));}style.color=0xffffffff;
    };
    if(state==9||state==16||state==23){
        for(i32 row=0;row<25;++row){style.color=cursor.selected==row?0xffffff00:0xff808080;
            if(!replay_files[row])std::snprintf(line,sizeof(line),kPortTitle==14?"No.%.2d -------- --/--/-- ------- -- St-":"No.%.2d -------- --/--/-- ------- - St-",row+1);
            else{const auto& r=replay_files[row]->replay;const auto* p=r.decoded().data();u64 stamp;std::memcpy(&stamp,p+12,8);const auto t=calendar(stamp);
#if PORT_TITLE == 14
                if(p[10]&2)std::snprintf(line,sizeof(line),"No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d",row+1,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,players14[r.character()*2+r.subtype()],signed_bits(u32(integer(p+0x90))+1));
                else std::snprintf(line,sizeof(line),"No.%.2d %s %.2d/%.2d/%.2d %s %s %s",row+1,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,players14[r.character()*2+r.subtype()],difficulties14[r.difficulty()],label(integer(p+0x88),true));
#else
                std::snprintf(line,sizeof(line),"No.%.2d %.8s %.2d/%.2d/%.2d %s %s %s",row+1,reinterpret_cast<const char*>(p),t.tm_year%100,t.tm_mon+1,t.tm_mday,players[r.character()*2+r.subtype()],difficulties[r.difficulty()],label(integer(p+0x68),true));
#endif
            }
            add(line,48,float(64+row*15));
        }
    }else if(state==10||state==17||state==24){
        const float initial=float(cursor.selected*15+64);
#if PORT_TITLE == 14
        const float y=timer.current<10?initial+(224.f-initial)*float(timer.current)/10.f:224.f;
#else
        const float y=timer.current<10?float(double(initial)+(224-double(initial))*timer.current/10):224;
#endif
        name(102,y);const auto t=calendar(timestamp);
#if PORT_TITLE == 14
        if(spell_practice_id>=0)std::snprintf(line,sizeof(line),"No.%.2d          %.2d/%.2d/%.2d %s Sp %3d",cursor.selected+1,t.tm_year%100,t.tm_mon+1,t.tm_mday,players14[selection],signed_bits(u32(spell_practice_id)+1));
        else std::snprintf(line,sizeof(line),"No.%.2d          %.2d/%.2d/%.2d %s %s %s",cursor.selected+1,t.tm_year%100,t.tm_mon+1,t.tm_mday,players14[selection],difficulties14[difficulty],label(stage,true));add(line,48,y);
#else
        std::snprintf(line,sizeof(line),"No.%.2d          %.2d/%.2d/%.2d %s %s %s",cursor.selected+1,t.tm_year%100,t.tm_mon+1,t.tm_mday,players[selection],difficulties[difficulty],label(stage,true));add(line,48,y);
#endif
    }else if(state==18||state==25){
        add("            Score Ranking!!",48,64);if(!unranked)name(75,float(96+cursor.selected*18));
        for(i32 rank=0;rank<10;++rank){style.color=!unranked&&cursor.selected==rank?0xffffff00:0xff808080;const auto* p=scores.characters[selection].data()+0x10+(rank+difficulty*10)*28;
            u64 stamp;std::memcpy(&stamp,p+16,8);
            if(!stamp)std::snprintf(line,sizeof(line),kPortTitle==14?"%2d %s %9d%d --/--/-- Stage -":"%2d %.8s %.9d%d --/--/-- Stage -",rank+1,reinterpret_cast<const char*>(p+6),integer(p),i8(p[5]));
            else{const auto t=calendar(stamp);std::snprintf(line,sizeof(line),kPortTitle==14?"%2d %s %9d%d %.2d/%.2d/%.2d %s":"%2d %.8s %.9d%d %.2d/%.2d/%.2d %s",rank+1,reinterpret_cast<const char*>(p+6),integer(p),i8(p[5]),t.tm_year%100,t.tm_mon+1,t.tm_mday,label(i8(p[4])));}add(line,48,float(96+rank*18));
        }
    }
}
}
