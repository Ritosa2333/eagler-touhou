#pragma once
#include <map>
#include <string>
#include <array>
#include <cstring>
#include <algorithm>
namespace th5w { namespace locale {
struct Translation {const char *source,*english,*japanese;};
inline const Translation* Table(){static const Translation table[]={
#include "LocalizationTable.inc"
{nullptr,nullptr,nullptr}};return table;}
inline std::string& PracticeLanguage(){static std::string value="zh-CN";return value;}
inline std::string& GameLanguage(){static std::string value="ja";return value;}
inline std::map<std::string,std::array<char,2048>>& Buffers(){static std::map<std::string,std::array<char,2048>> values;return values;}
inline const char* Lookup(const char* source,const std::string& language){if(language=="zh-CN"||language=="lang_zh-hans")return source;for(const auto* t=Table();t->source;t++)if(!strcmp(t->source,source))return language=="en-US"?t->english:t->japanese;return source;}
// Static widget label arrays retain these stable addresses. A Launcher locale
// change updates every registered label before a game scene is constructed.
inline const char* Text(const char* source){if(strlen(source)>=2048)return source;auto& buffer=Buffers()[source];const char* text=Lookup(source,PracticeLanguage());strncpy(buffer.data(),text,buffer.size()-1);buffer.back()=0;return buffer.data();}
inline void Configure(const char* game,const char* practice){GameLanguage()=game;PracticeLanguage()=practice;for(auto& pair:Buffers()){const char* value=Lookup(pair.first.c_str(),PracticeLanguage());strncpy(pair.second.data(),value,pair.second.size()-1);pair.second.back()=0;}}
inline std::wstring GameText(const wchar_t* wide){std::string utf8;for(const wchar_t* p=wide;*p;p++){unsigned c=*p;if(c<128)utf8+=char(c);else if(c<2048){utf8+=char(192|(c>>6));utf8+=char(128|(c&63));}else{utf8+=char(224|(c>>12));utf8+=char(128|((c>>6)&63));utf8+=char(128|(c&63));}}
 const auto* text=reinterpret_cast<const unsigned char*>(Lookup(utf8.c_str(),GameLanguage()));std::wstring result;while(*text){unsigned c=*text++;if(c>=224){c=(c&15)<<12;c|=(*text++&63)<<6;c|=*text++&63;}else if(c>=192){c=(c&31)<<6;c|=*text++&63;}result+=wchar_t(c);}return result;
}
}}
