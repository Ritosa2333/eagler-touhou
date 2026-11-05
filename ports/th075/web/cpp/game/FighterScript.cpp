#include "FighterScript.hpp"
namespace th075 {
static constexpr std::array<std::array<u8,2>,11> expressions{{
    {0x95,0x81},{0x8c,0x88},{0x8a,0xf0},{0x97,0x5d},{0x93,0x7b},
    {0x8a,0xbe},{0x98,0x66},{0x8b,0xc1},{0x95,0x89},{0x88,0xea},{0x93,0xf1}
}};
bool FighterScript::load(const void* source,std::size_t size){
    if(size>16*1024*1024){error="Story script exceeds resource limit";return false;}
    const auto* p=static_cast<const u8*>(source);std::vector<u8> text(p,p+size);u8 key=99,step=98;
    for(auto& byte:text){byte^=key;key=u8(key+step);step=u8(step+66);}return parse(text.data(),text.size());
}
bool FighterScript::parse(const void* source,std::size_t size){
    labels.fill(-1);blocks.clear();error.clear();const auto* text=static_cast<const u8*>(source);
    enum class State{Normal,Label,Expression,Text,Number,Comment};State state=State::Normal;
    u16 number=0;std::size_t cursor=0,start=0;
    const auto last=[&]()->StoryRecord*{return blocks.empty()||blocks.back().empty()?nullptr:&blocks.back().back();};
    while(cursor<size){const u8 byte=text[cursor];
        switch(state){
        case State::Normal:
            if(byte=='#'){state=State::Comment;number=0;}
            else if(byte=='B'){state=State::Label;number=0;}
            else if(byte=='P'||byte=='E'||byte=='N'||byte=='F'||byte=='M'||byte=='W'||byte=='V'||byte=='R'){
                if(blocks.empty()){error="Story command before label";return false;}
                const auto kind=byte=='P'?StoryCommand::Player:byte=='E'?StoryCommand::Opponent:byte=='N'?StoryCommand::Narrator:byte=='F'?StoryCommand::Face:byte=='M'?StoryCommand::Music:byte=='W'?StoryCommand::Wait:byte=='V'?StoryCommand::Voice:StoryCommand::Route;
                blocks.back().push_back({kind,-1,{}});if(byte!='P'&&byte!='E'&&byte!='N'){number=0;state=State::Number;}
            }else if(byte=='('){if(!last()){error="Expression without command";return false;}last()->argument=-1;state=State::Expression;}
            else if(byte=='"'){if(!last()){error="Text without command";return false;}state=State::Text;}
            ++cursor;start=cursor;break;
        case State::Label:
            if(byte>='0'&&byte<='9'){number=u16(number*10+byte-'0');++cursor;start=cursor;}
            else{if(number>=labels.size()||blocks.size()>=32767){error="Story label outside table";return false;}blocks.emplace_back();labels[number]=i16(blocks.size()-1);state=State::Normal;start=cursor;}
            break;
        case State::Number:
            if(byte>='0'&&byte<='9'){number=u16(number*10+byte-'0');++cursor;start=cursor;}
            else{std::memcpy(&last()->argument,&number,2);state=State::Normal;start=cursor;}
            break;
        case State::Expression:
            if(byte==')')state=State::Normal;
            else if(cursor+1<size){for(u32 i=0;i<expressions.size();i++)if(text[cursor]==expressions[i][0]&&text[cursor+1]==expressions[i][1]){last()->argument=i16(i);++cursor;break;}}
            ++cursor;break;
        case State::Text:
            if(byte=='"'){last()->text.assign(reinterpret_cast<const char*>(text+start),cursor-start);start=cursor;state=State::Normal;}
            ++cursor;break;
        case State::Comment:
            if(byte=='\n')state=State::Normal;++cursor;break;
        }
    }
    if(state==State::Text||state==State::Expression){error="Truncated story text or expression";return false;}return true;
}
const StoryRecord* FighterScript::get(u16 label,u32 index)const{
    if(label>=labels.size()||labels[label]<0||u32(labels[label])>=blocks.size())return nullptr;const auto& block=blocks[labels[label]];return index<block.size()?&block[index]:nullptr;
}
}
