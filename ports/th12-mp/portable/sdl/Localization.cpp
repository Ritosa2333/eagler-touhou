#include "BinaryReader.hpp"
#include <unordered_map>
namespace th12host {
namespace {
using Table=std::unordered_map<std::u32string,std::u32string>;
std::unordered_map<std::string,Table> translations;
std::string context;
}
std::u32string decode_utf8(const std::string& text){
    std::u32string out;
    for(size_t i=0;i<text.size();){const u8 first=u8(text[i++]);char32_t value=first;unsigned rest=0;
        if(first>=0xc2&&first<=0xdf){value=first&31;rest=1;}else if(first>=0xe0&&first<=0xef){value=first&15;rest=2;}else if(first>=0xf0&&first<=0xf4){value=first&7;rest=3;}
        else if(first>=0x80){out+=U'\ufffd';continue;}
        if(rest>text.size()-i){out+=U'\ufffd';break;}
        bool valid=true;for(unsigned j=0;j<rest;++j){const auto b=u8(text[i]);if((b&0xc0)!=0x80){valid=false;break;}++i;value=(value<<6)|(b&63);}
        if(!valid||value>0x10ffff||(value>=0xd800&&value<=0xdfff)||(rest==1&&value<0x80)||(rest==2&&value<0x800)||(rest==3&&value<0x10000))value=0xfffd;
        out+=value;
    }
    return out;
}
void load_translations(){
    translations.clear();context.clear();std::vector<u8> bytes;
    if(!host::read_file(host::paths().data_file("native-translations.bin"),bytes,16*1024*1024))return;
    try{BinaryReader r(bytes);r.magic("TH12LOC1",8);const u32 count=r.word();if(count>100000)throw std::runtime_error("Too many translated strings");
        for(u32 i=0;i<count;++i){const auto scope=r.text();const auto original=decode_utf8(r.text());const auto replacement=decode_utf8(r.text());translations[scope][original]=replacement;}
        if(!r.ended())throw std::runtime_error("Trailing translation data");
    }catch(const std::exception& e){translations.clear();host::log("localization: %s",e.what());}
}
void translation_context(const std::string& resource){context=resource;}
std::u32string translate_text(const std::u32string& text){
    for(const auto& scope:{context,std::string()})if(const auto table=translations.find(scope);table!=translations.end())if(const auto found=table->second.find(text);found!=table->second.end())return found->second;
    return text;
}
}
