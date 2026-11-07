#include "PracticeReplay.hpp"
#include <array>
#include <charconv>
#include <string_view>
#include <type_traits>

namespace th17 {
namespace {
class Reader {
    std::string_view remaining;
    void space(){
        const auto first=remaining.find_first_not_of(" \t\r\n");
        remaining.remove_prefix(first==std::string_view::npos?remaining.size():first);
    }
public:
    explicit Reader(std::string_view value):remaining(value){}
    bool take(std::string_view token){
        space();
        if(remaining.substr(0,token.size())!=token)return false;
        remaining.remove_prefix(token.size());
        return true;
    }
    bool text(std::string_view& value){
        if(!take("\""))return false;
        const auto end=remaining.find('"');
        if(end==std::string_view::npos)return false;
        value=remaining.substr(0,end);
        for(char c:value)if(u8(c)<32||c=='\\')return false;
        remaining.remove_prefix(end+1);
        return true;
    }
    template<class T>bool number(T& value){
        space();
        if(remaining.empty()||remaining.front()<'0'||remaining.front()>'9')return false;
        if(remaining.size()>1&&remaining[0]=='0'&&remaining[1]>='0'&&remaining[1]<='9')return false;
        const auto start=remaining.data();
        const auto parsed=std::from_chars(start,start+remaining.size(),value);
        if(parsed.ec!=std::errc())return false;
        remaining.remove_prefix(size_t(parsed.ptr-start));
        return true;
    }
    bool flag(bool& value){
        if(take("true")){value=true;return true;}
        if(take("false")){value=false;return true;}
        return false;
    }
    bool done(){space();return remaining.empty();}
};
}

bool read_native_practice(const u8* data,u32 size,i32 stage,i32 rank,i32 spell,PracticeSettings& output){
    if(!size||size>4096)return false;
    const auto* text=reinterpret_cast<const char*>(data);
    size_t length=0;
    while(length<size&&text[length])++length;
    if(length==size)return false;
    for(size_t i=length;i<size;++i)if(text[i])return false;

    PracticeSettings settings;
    // THPracParam is zero-initialized when reading a replay.
    settings.lives=settings.bombs=settings.power=settings.value=0;
    auto& patch=settings.patch;
    i32 mode=0;
    bool game=false,offsets=false,score=false,dialogue=false,drops=false,version=false;
    constexpr std::array names{"mode","stage","section","phase","life","life_fragment","bomb",
        "bomb_fragment","power","value","graze","goast_1","goast_2","goast_3","goast_4","goast_5"};
    const std::array<i32*,names.size()> values{&mode,&patch.stage,&patch.section,&patch.phase,
        &settings.lives,&settings.life_pieces,&settings.bombs,&settings.bomb_pieces,
        &settings.power,&settings.value,&settings.graze,&patch.spirits[0],&patch.spirits[1],
        &patch.spirits[2],&patch.spirits[3],&patch.spirits[4]};
    Reader reader({text,length});
    if(!reader.take("{"))return false;
    u32 seen=0;
    do{
        std::string_view key,value;
        if(!reader.text(key)||!reader.take(":"))return false;
        if(key=="game"){
            if(game||!reader.text(value)||value!="th17")return false;
            game=true;
        }else if(key=="version"){
            if(version||!reader.text(value))return false;
            version=true;
        }else if(key=="score"){
            if(score||!reader.number(settings.score))return false;
            score=true;
        }else if(key=="dlg"){
            if(dialogue||!reader.flag(patch.dialogue))return false;
            dialogue=true;
        }else if(key=="keepSpellDrops"){
            if(drops||!reader.flag(patch.keep_drops))return false;
            drops=true;
        }else if(key=="keikiFinalPhaseOfs"){
            if(offsets||!reader.take("[")||!reader.number(patch.keiki_offsets[0])||!reader.take(",")||
               !reader.number(patch.keiki_offsets[1])||!reader.take(",")||!reader.number(patch.keiki_offsets[2])||
               !reader.take("]"))return false;
            offsets=true;
        }else{
            size_t index=0;
            while(index<names.size()&&key!=names[index])++index;
            if(index==names.size()||(seen&(1u<<index))||!reader.number(*values[index]))return false;
            seen|=1u<<index;
        }
    }while(reader.take(","));
    if(!reader.take("}")||!reader.done()||!game||mode!=(spell<0?1:2))return false;
    if(mode==2&&patch.stage==0)patch.stage=stage;
    if(patch.stage!=stage||!patch.valid(rank,spell))return false;
    if(mode==1&&(settings.score>9999999990ll||settings.lives>8||settings.life_pieces>2||
                 settings.bombs>8||settings.bomb_pieces>2||settings.power>400||
                 settings.value>999990||settings.graze>999999))return false;
    output=settings;
    return true;
}
}
