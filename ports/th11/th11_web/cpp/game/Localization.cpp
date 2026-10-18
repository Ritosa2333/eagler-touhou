#include "Localization.hpp"
#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <vector>
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
#include <SDL3/SDL.h>
#endif
namespace th11::Localization {
namespace {
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
u16 u16le(const u8* p){return u16(p[0])|u16(p[1])<<8;}
u32 u32le(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
std::vector<u8> load(const char* name){
    const std::string path=std::string("/thcrap/th11/localization/")+name;
    size_t size=0;auto* p=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));
    if(!p)return {};
    std::vector<u8> bytes;if(size<=2u*1024u*1024u)bytes.assign(p,p+size);
    SDL_free(p);return bytes;
}
class Table {
    bool loaded=false,available=false;
    std::unordered_map<u64,std::string> entries;
public:
    const char* lookup(const char* name,u32 key,u16 line,const char* fallback,bool blank_missing=false){
        if(!loaded){
            loaded=true;const auto bytes=load(name);
            if(bytes.size()>=8&&!std::memcmp(bytes.data(),"ETL1",4)){
                const u32 count=u32le(bytes.data()+4);size_t at=8;
                if(count<=20000){
                    available=true;
                    for(u32 n=0;n<count;++n){
                        if(at+8>bytes.size()){available=false;break;}
                        const u32 id=u32le(bytes.data()+at);const u16 row=u16le(bytes.data()+at+4),length=u16le(bytes.data()+at+6);at+=8;
                        if(at+length>bytes.size()){available=false;break;}
                        entries[(u64(id)<<16)|row]=std::string(reinterpret_cast<const char*>(bytes.data()+at),length);
                        at+=length;
                    }
                    if(at!=bytes.size())available=false;
                    if(!available)entries.clear();
                }
            }
        }
        const auto found=entries.find((u64(key)<<16)|line);
        return found==entries.end()?(blank_missing&&available?"":fallback):found->second.c_str();
    }
};
class Strings {
    bool loaded=false;
    std::unordered_map<std::string,std::string> entries;
    std::unordered_map<std::string,std::string> formats;
    static bool signature(const std::string& format,std::string& out){
        for(size_t i=0;i<format.size();++i){
            if(format[i]!='%')continue;
            if(i+1<format.size()&&format[i+1]=='%'){++i;continue;}
            ++i;while(i<format.size()&&format[i]>='0'&&format[i]<='9')++i;
            if(i>=format.size()||(format[i]!='d'&&format[i]!='s'))return false;
            out+=format[i];
        }
        return true;
    }
    static bool strip_width_hints(const std::string& source,std::string& out){
        for(size_t i=0;i<source.size();){
            if(source.compare(i,3,"<r$")==0){
                const size_t split=source.find('$',i+3),end=split==std::string::npos?split:source.find('>',split+1);
                if(end==std::string::npos||split-i>100)return false;
                out.append(source,i+3,split-i-3);i=end+1;
            }else if(source.compare(i,3,"<c$")==0){
                const size_t end=source.find("$>",i+3);
                if(end==std::string::npos)return false;
                out.append(source,i+3,end-i-3);i=end+2;
            }else out+=source[i++];
        }
        return true;
    }
public:
    const char* lookup(const char* id,const char* fallback){
        if(!loaded){
            loaded=true;const auto bytes=load("strings.etl");
            if(bytes.size()>=8&&!std::memcmp(bytes.data(),"EST1",4)){
                const u32 count=u32le(bytes.data()+4);size_t at=8;
                if(count<=4096){
                    for(u32 n=0;n<count;++n){
                        if(at+8>bytes.size()){entries.clear();break;}
                        const u16 idlen=u16le(bytes.data()+at),length=u16le(bytes.data()+at+2),flags=u16le(bytes.data()+at+4);
                        at+=8;if(at+idlen+length>bytes.size()){entries.clear();break;}
                        std::string key(reinterpret_cast<const char*>(bytes.data()+at),idlen);at+=idlen;
                        std::string value(reinterpret_cast<const char*>(bytes.data()+at),length);at+=length;
                        if(flags&1)entries.emplace(std::move(key),std::move(value));
                    }
                    if(at!=bytes.size())entries.clear();
                }
            }
        }
        const auto found=entries.find(id?id:"");return found==entries.end()?fallback:found->second.c_str();
    }
    const char* format(const char* id,const char* fallback,bool layout=false){
        if(!id||!fallback)return fallback;
        const char* source=lookup(id,fallback);
        if(source==fallback)return fallback;
        const auto cached=formats.find(id);if(!layout&&cached!=formats.end())return cached->second.c_str();
        std::string normalized,original_signature,translated_signature;
        if(!strip_width_hints(source,normalized)||!signature(fallback,original_signature)||
           !signature(normalized,translated_signature)||original_signature!=translated_signature)return fallback;
        if(layout)return source;
        return formats.emplace(id,std::move(normalized)).first->second.c_str();
    }
};
Table spells,themes,comments;
Strings strings;
#endif
}
const char* SpellName(u32 id,const char* fallback,u32 rank){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    // thcrap_tsa/spells.cpp: BP_spell_id + BP_spell_name count down from
    // spell_id_real to spell_id (Result uses real - spell_rank).
    const u32 first=id-std::min(id,rank);
    for(u32 candidate=id;;--candidate){
        const char* value=spells.lookup("spells.etl",candidate,0,fallback);
        if(value!=fallback)return value;
        if(candidate==first)break;
    }
    return fallback;
#else
    (void)id;(void)rank;return fallback;
#endif
}
const char* MusicTitle(u32 track,const char* fallback){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    return themes.lookup("themes.etl",track,0,fallback);
#else
    (void)track;return fallback;
#endif
}
const char* MusicComment(u32 track,u16 line,const char* fallback){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    return comments.lookup("musiccmt.etl",track,line,fallback,true);
#else
    (void)track;(void)line;return fallback;
#endif
}
const char* StringById(const char* id,const char* fallback){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    return strings.lookup(id,fallback);
#else
    (void)id;return fallback;
#endif
}
const char* FormatStringById(const char* id,const char* fallback){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    return strings.format(id,fallback);
#else
    (void)id;return fallback;
#endif
}
const char* LayoutFormatStringById(const char* id,const char* fallback){
#if defined(TH_NATIVE_PLATFORM) && defined(TH_ENABLE_THCRAP)
    return strings.format(id,fallback,true);
#else
    (void)id;return fallback;
#endif
}
}
