#ifdef TH_ENABLE_THCRAP
#include "Localization.hpp"

#include <cctype>
#include <cstring>
#include <string>
#include <unordered_map>

#include "RuntimeOverride.hpp"
#include "Types.hpp"
#ifdef TH_NATIVE_PLATFORM
#include <SDL3/SDL.h>
#define TH09_LOCALIZATION_LOG(...) SDL_Log(__VA_ARGS__)
#else
#define TH09_LOCALIZATION_LOG(...) ((void)0)
#endif

namespace th09 {
namespace {
// CP932 -> Unicode map used to re-encode untranslated original bytes when a
// pack substitutes them into UTF-8 text. TH09 ships this table as
// /fonts/cp932.bin, the same table used by its original-text rasterizer.
struct Cp932
{
    std::vector<std::uint16_t> mapping;
    bool loaded()const{return !mapping.empty();}
    bool load(const unsigned char* data,std::size_t size)
    {
        if(data==nullptr||size<131072)
            return false;
        mapping.resize(65536);
        for(std::size_t index=0;index<65536;index++)
            mapping[index]=static_cast<std::uint16_t>(data[index*2]|(static_cast<std::uint16_t>(data[index*2+1])<<8));
        return true;
    }
    std::uint16_t next(const unsigned char*& cursor,const unsigned char* end)const noexcept
    {
        if(cursor>=end)
            return 0;
        std::uint32_t value=*cursor++;
        if((value>=0x81&&value<=0x9f)||(value>=0xe0&&value<=0xfc))
        {
            if(cursor>=end||!*cursor)
                return 0x30fb;
            value=(value<<8)|*cursor++;
        }
        return loaded()?mapping[value]:(value<=0x80?static_cast<std::uint16_t>(value):0x30fb);
    }
};
std::uint16_t ReadU16(const unsigned char* data)
{
    return static_cast<std::uint16_t>(data[0])|
           (static_cast<std::uint16_t>(data[1])<<8);
}

std::int16_t ReadI16(const unsigned char* data)
{
    return static_cast<std::int16_t>(ReadU16(data));
}

std::uint32_t ReadU32(const unsigned char* data)
{
    return static_cast<std::uint32_t>(data[0])|
           (static_cast<std::uint32_t>(data[1])<<8)|
           (static_cast<std::uint32_t>(data[2])<<16)|
           (static_cast<std::uint32_t>(data[3])<<24);
}

std::size_t Utf8SequenceLength(const unsigned char* text)
{
    const unsigned char first=text[0];
    std::size_t length=1;
    if((first&0xe0)==0xc0)
        length=2;
    else if((first&0xf0)==0xe0)
        length=3;
    else if((first&0xf8)==0xf0)
        length=4;
    for(std::size_t index=1;index<length;index++)
        if(text[index]==0||(text[index]&0xc0)!=0x80)
            return 1;
    return length;
}

bool IsValidUtf8(const unsigned char* text)
{
    while(*text!=0)
    {
        const unsigned char first=*text;
        std::size_t length;
        if(first<0x80)
            length=1;
        else if(first>=0xc2&&first<=0xdf)
            length=2;
        else if(first>=0xe0&&first<=0xef)
            length=3;
        else if(first>=0xf0&&first<=0xf4)
            length=4;
        else
            return false;
        for(std::size_t index=1;index<length;index++)
            if(text[index]==0||(text[index]&0xc0)!=0x80)
                return false;
        if((length==3&&first==0xe0&&text[1]<0xa0)||
           (length==3&&first==0xed&&text[1]>=0xa0)||
           (length==4&&first==0xf0&&text[1]<0x90)||
           (length==4&&first==0xf4&&text[1]>=0x90))
            return false;
        text+=length;
    }
    return true;
}

std::size_t TextUnitLength(const unsigned char* text,bool utf8)
{
    if(utf8)
        return Utf8SequenceLength(text);
    const unsigned char first=text[0];
    const bool shiftJisLead=(first>=0x81&&first<=0x9f)||(first>=0xe0&&first<=0xfc);
    return shiftJisLead&&text[1]!=0?2:1;
}

class Table
{
public:
    const char* Lookup(const char* path,std::uint32_t key,std::uint16_t line,const char* fallback,
                       bool blankMissing=false)
    {
        Load(path);
        const std::uint64_t packed=(static_cast<std::uint64_t>(key)<<16)|line;
        const auto found=entries.find(packed);
        return found==entries.end()?(blankMissing&&available?"":fallback):found->second.c_str();
    }

    std::size_t Count()
    {
        return entries.size();
    }

private:
    void Load(const char* path)
    {
        if(loaded)
            return;
        loaded=true;
        std::vector<u8> storage;
        if(!RuntimeOverride::Read(path,storage))
            return;
        const unsigned char* data=storage.data();
        const std::size_t size=storage.size();
        if(size<8||std::memcmp(data,"ETL1",4)!=0)
            return;
        available=true;
        const std::uint32_t count=ReadU32(data+4);
        std::size_t offset=8;
        for(std::uint32_t index=0;index<count;index++)
        {
            if(offset>size||size-offset<8)
                break;
            const std::uint32_t key=ReadU32(data+offset);
            const std::uint16_t line=ReadU16(data+offset+4);
            const std::uint16_t length=ReadU16(data+offset+6);
            offset+=8;
            if(offset>size||size-offset<length)
                break;
            const std::uint64_t packed=(static_cast<std::uint64_t>(key)<<16)|line;
            entries[packed]=std::string(reinterpret_cast<const char*>(data+offset),length);
            offset+=length;
        }
    }

    bool loaded=false;
    bool available=false;
    std::unordered_map<std::uint64_t,std::string> entries;
};

struct AsciiRecord
{
    std::string id;
    std::string translation;
    std::string baseline;
    float extraX=0.0f;
    bool hasTranslation=false;
    bool hasAlignment=false;
};

struct AsciiIdValue
{
    std::string translation;
    bool hasTranslation=false;
};

class AsciiTable
{
public:
    const AsciiRecord* Lookup(const char* fallback)
    {
        Load();
        if(!available||fallback==nullptr)
            return nullptr;
        const auto found=aliases.find(fallback);
        return found==aliases.end()?nullptr:&found->second;
    }

    const AsciiIdValue* LookupId(const char* id)
    {
        Load();
        if(!available||id==nullptr)
            return nullptr;
        const auto found=ids.find(id);
        return found==ids.end()?nullptr:&found->second;
    }

    std::size_t Count()
    {
        return aliases.size();
    }

private:
    static bool DecodeString(const unsigned char* data,std::size_t size,std::size_t& offset,
                             std::uint16_t length,std::string& value,bool allowEmpty)
    {
        if(offset>size||size-offset<length)
            return false;
        if(!allowEmpty&&length==0)
            return false;
        if(length!=0&&std::memchr(data+offset,0,length)!=nullptr)
            return false;
        value.assign(reinterpret_cast<const char*>(data+offset),length);
        offset+=length;
        return IsValidUtf8(reinterpret_cast<const unsigned char*>(value.c_str()));
    }

    void Load()
    {
        if(loaded)
            return;
        loaded=true;

        std::vector<u8> storage;
        if(!RuntimeOverride::Read("localization/ascii.etl",storage))
            return;
        const unsigned char* data=storage.data();
        const std::size_t size=storage.size();
        bool valid=size>=8&&std::memcmp(data,"EAS1",4)==0;
        std::unordered_map<std::string,AsciiRecord> nextAliases;
        std::unordered_map<std::string,AsciiIdValue> nextIds;
        std::size_t offset=8;
        std::uint32_t count=valid?ReadU32(data+4):0;
        if(count>4096)
            valid=false;

        for(std::uint32_t index=0;valid&&index<count;index++)
        {
            if(offset>size||size-offset<12)
            {
                valid=false;
                break;
            }
            const std::uint16_t aliasLength=ReadU16(data+offset);
            const std::uint16_t idLength=ReadU16(data+offset+2);
            const std::uint16_t translationLength=ReadU16(data+offset+4);
            const std::uint16_t baselineLength=ReadU16(data+offset+6);
            const std::int16_t extraHalf=ReadI16(data+offset+8);
            const std::uint16_t flags=ReadU16(data+offset+10);
            offset+=12;
            if((flags&~0x3u)!=0)
            {
                valid=false;
                break;
            }

            std::string alias;
            AsciiRecord record;
            if(!DecodeString(data,size,offset,aliasLength,alias,true)||
               !DecodeString(data,size,offset,idLength,record.id,false)||
               !DecodeString(data,size,offset,translationLength,record.translation,true)||
               !DecodeString(data,size,offset,baselineLength,record.baseline,true))
            {
                valid=false;
                break;
            }
            record.hasTranslation=(flags&1u)!=0;
            record.hasAlignment=(flags&2u)!=0;
            record.extraX=static_cast<float>(extraHalf)*0.5f;
            if((!record.hasTranslation&&!record.translation.empty())||
               (!record.hasAlignment&&(!record.baseline.empty()||extraHalf!=0))||
               (record.hasAlignment&&record.baseline.empty()))
            {
                valid=false;
                break;
            }

            const auto idFound=nextIds.find(record.id);
            if(idFound==nextIds.end())
            {
                nextIds.emplace(record.id,AsciiIdValue{record.translation,record.hasTranslation});
            }
            else if(idFound->second.hasTranslation!=record.hasTranslation||
                    idFound->second.translation!=record.translation)
            {
                valid=false;
                break;
            }

            // Empty aliases are EAS1 lookup-only records used by helper IDs.
            // They intentionally populate only the ID map.
            if(!alias.empty()&&!nextAliases.emplace(alias,std::move(record)).second)
            {
                valid=false;
                break;
            }
        }
        if(valid&&offset!=size)
            valid=false;

        if(!valid)
            return;
        aliases.swap(nextAliases);
        ids.swap(nextIds);
        available=true;
    }

    bool loaded=false;
    bool available=false;
    std::unordered_map<std::string,AsciiRecord> aliases;
    std::unordered_map<std::string,AsciiIdValue> ids;
};

struct StringRecord
{
    std::string translation;
    bool hasTranslation=false;
};

class StringTable
{
public:
    const StringRecord* Lookup(const char* id)
    {
        Load();
        if(!available||id==nullptr)
            return nullptr;
        const auto found=entries.find(id);
        return found==entries.end()?nullptr:&found->second;
    }

    std::size_t Count()
    {
        return entries.size();
    }

private:
    static bool DecodeString(const unsigned char* data,std::size_t size,std::size_t& offset,
                             std::uint16_t length,std::string& value,bool allowEmpty)
    {
        if(offset>size||size-offset<length)
            return false;
        if(!allowEmpty&&length==0)
            return false;
        if(length!=0&&std::memchr(data+offset,0,length)!=nullptr)
            return false;
        value.assign(reinterpret_cast<const char*>(data+offset),length);
        offset+=length;
        return IsValidUtf8(reinterpret_cast<const unsigned char*>(value.c_str()));
    }

    void Load()
    {
        if(loaded)
            return;
        loaded=true;

        std::vector<u8> storage;
        if(!RuntimeOverride::Read("localization/strings.etl",storage))
            return;
        const unsigned char* data=storage.data();
        const std::size_t size=storage.size();
        bool valid=size>=8&&std::memcmp(data,"EST1",4)==0;
        std::unordered_map<std::string,StringRecord> parsed;
        std::size_t offset=8;
        const std::uint32_t count=valid?ReadU32(data+4):0;
        if(count>4096)
            valid=false;

        for(std::uint32_t index=0;valid&&index<count;index++)
        {
            if(offset>size||size-offset<8)
            {
                valid=false;
                break;
            }
            const std::uint16_t idLength=ReadU16(data+offset);
            const std::uint16_t translationLength=ReadU16(data+offset+2);
            const std::uint16_t flags=ReadU16(data+offset+4);
            const std::uint16_t reserved=ReadU16(data+offset+6);
            offset+=8;
            if((flags&~0x1u)!=0||reserved!=0)
            {
                valid=false;
                break;
            }

            std::string id;
            StringRecord record;
            if(!DecodeString(data,size,offset,idLength,id,false)||
               !DecodeString(data,size,offset,translationLength,record.translation,true))
            {
                valid=false;
                break;
            }
            record.hasTranslation=(flags&1u)!=0;
            if((!record.hasTranslation&&!record.translation.empty())||
               !parsed.emplace(std::move(id),std::move(record)).second)
            {
                valid=false;
                break;
            }
        }
        if(valid&&offset!=size)
            valid=false;

        if(!valid)
            return;
        entries.swap(parsed);
        available=true;
    }

    bool loaded=false;
    bool available=false;
    std::unordered_map<std::string,StringRecord> entries;
};

static bool ParsePrintfSignature(const char* format,std::string& signature)
{
    signature.clear();
    if(format==nullptr)
        return false;
    for(const char* cursor=format;*cursor!='\0';++cursor)
    {
        if(*cursor!='%')
            continue;
        ++cursor;
        if(*cursor=='%')
            continue;
        if(*cursor=='\0')
            return false;

        while(*cursor=='-'||*cursor=='+'||*cursor==' '||*cursor=='#'||
              *cursor=='0'||*cursor=='\'')
            ++cursor;
        if(*cursor=='*')
            return false;
        while(std::isdigit(static_cast<unsigned char>(*cursor)))
            ++cursor;
        if(*cursor=='.')
        {
            ++cursor;
            if(*cursor=='*')
                return false;
            while(std::isdigit(static_cast<unsigned char>(*cursor)))
                ++cursor;
        }
        // None of the format contracts use length modifiers. Reject them
        // instead of guessing a va_list type.
        if(*cursor=='h'||*cursor=='l'||*cursor=='j'||*cursor=='z'||
           *cursor=='t'||*cursor=='L')
            return false;

        switch(*cursor)
        {
        case 'd':
        case 'i':
            signature.push_back('i');
            break;
        case 'u':
        case 'o':
        case 'x':
        case 'X':
            signature.push_back('u');
            break;
        case 'c':
            signature.push_back('c');
            break;
        case 's':
            signature.push_back('s');
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            signature.push_back('f');
            break;
        default:
            return false;
        }
    }
    return true;
}

Table g_Spells;
Table g_Stages;
Table g_Themes;
Table g_MusicComments;
Table g_SpellComments;
AsciiTable g_Ascii;
StringTable g_Strings;

// Matches SPELL_COMMENT_OWNER_LINE in the thcrap pack compiler: comment_N
// lines occupy (N-1)*0x100+line, leaving 0x200 for the owner override.
constexpr std::uint16_t kSpellCommentOwnerLine=0x200;

struct Options
{
    static bool ReadString(const std::string& json,const char* name,std::string& value)
    {
        const std::string key=std::string("\"")+name+"\"";
        std::size_t cursor=json.find(key);
        if(cursor==std::string::npos)
            return false;
        cursor=json.find(':',cursor+key.size());
        if(cursor==std::string::npos)
            return false;
        cursor++;
        while(cursor<json.size()&&
              std::isspace(static_cast<unsigned char>(json[cursor])))
            cursor++;
        if(cursor>=json.size()||json[cursor++]!='\"')
            return false;
        const std::size_t end=json.find('\"',cursor);
        if(end==std::string::npos)
            return false;
        value.assign(json,cursor,end-cursor);
        return !value.empty();
    }

    void Load()
    {
        if(loaded)
            return;
        loaded=true;

        std::vector<u8> storage;
        if(!RuntimeOverride::Read("localization/options.json",storage))
            return;
        const std::string json(reinterpret_cast<const char*>(storage.data()),storage.size());

        if(!ReadString(json,"fontFile",fontFile))
            return;
        if(fontFile.find("..")!=std::string::npos||
           fontFile.find('/')!=std::string::npos||
           fontFile.find('\\')!=std::string::npos||
           fontFile.find(':')!=std::string::npos)
        {
            fontFile.clear();
            return;
        }
        ReadString(json,"font",fontName);
        available=true;
    }

    bool loaded=false;
    bool available=false;
    std::string fontFile;
    std::string fontName;
};

Options g_Options;

// One-shot activation report. This is the in-wasm self check for the core
// mechanism: it forces every table through its strict parser and verifies the
// UTF-8 unit walker on a known sample, so a malformed pack shows up in the
// console instead of failing silently channel by channel.
void ReportActivation()
{
    static bool reported=false;
    if(reported)
        return;
    reported=true;

    static const char sample[]="\xe6\xb0\xb8\xe5\xa4\x9c\xe6\x8a\x84";
    const unsigned char* cursor=reinterpret_cast<const unsigned char*>(sample);
    std::size_t units=0;
    while(*cursor!=0)
    {
        cursor+=Utf8SequenceLength(cursor);
        units++;
    }
    const bool utf8Ok=units==3&&IsValidUtf8(reinterpret_cast<const unsigned char*>(sample));

    const char* missing="";
    g_Spells.Lookup("localization/spells.etl",0,0,missing);
    g_Stages.Lookup("localization/stages.etl",0,0,missing);
    g_Themes.Lookup("localization/themes.etl",0,0,missing);
    g_MusicComments.Lookup("localization/musiccmt.etl",0,0,missing);
    g_SpellComments.Lookup("localization/spellcomments.etl",0,0,missing);
    Localization::AsciiEntryView view{};
    g_Ascii.Lookup("");
    g_Strings.Lookup("");

    TH09_LOCALIZATION_LOG("th09 thcrap localization active: font=%s (%s) utf8=%s "
                          "spells=%zu stages=%zu themes=%zu musiccmt=%zu spellcomments=%zu ascii=%zu strings=%zu",
                          g_Options.fontFile.c_str(),
                          g_Options.fontName.empty()?"(unnamed)":g_Options.fontName.c_str(),
                          utf8Ok?"ok":"FAILED",
                          g_Spells.Count(),g_Stages.Count(),g_Themes.Count(),
                          g_MusicComments.Count(),g_SpellComments.Count(),g_Ascii.Count(),g_Strings.Count());
}
} // namespace

bool Localization::Active()
{
    g_Options.Load();
    if(!g_Options.available)
        return false;
    ReportActivation();
    return true;
}

const char* Localization::FontFile()
{
    g_Options.Load();
    return g_Options.available?g_Options.fontFile.c_str():nullptr;
}

const char* Localization::FontName()
{
    g_Options.Load();
    return g_Options.available&&!g_Options.fontName.empty()?g_Options.fontName.c_str():nullptr;
}

const char* Localization::SpellName(std::uint32_t id,const char* fallback)
{
    return g_Spells.Lookup("localization/spells.etl",id,0,fallback);
}

const char* Localization::StageName(std::uint32_t id,const char* fallback)
{
    return g_Stages.Lookup("localization/stages.etl",id,0,fallback);
}

const char* Localization::MusicTitle(std::uint32_t track,const char* fallback)
{
    return g_Themes.Lookup("localization/themes.etl",track,0,fallback);
}

const char* Localization::MusicComment(std::uint32_t track,std::uint16_t line,const char* fallback)
{
    return g_MusicComments.Lookup("localization/musiccmt.etl",track,line,fallback,true);
}

const char* Localization::SpellComment(std::uint32_t number,std::uint16_t line,const char* fallback)
{
    return g_SpellComments.Lookup("localization/spellcomments.etl",number,line,fallback);
}

const char* Localization::SpellCommentOwner(std::uint32_t number,const char* fallback)
{
    return g_SpellComments.Lookup("localization/spellcomments.etl",number,kSpellCommentOwnerLine,fallback);
}

bool Localization::LookupAscii(const char* fallback,AsciiEntryView& view)
{
    const AsciiRecord* record=g_Ascii.Lookup(fallback);
    if(record==nullptr)
        return false;
    view.id=record->id.c_str();
    view.text=record->hasTranslation?record->translation.c_str():fallback;
    view.baseline=record->baseline.c_str();
    view.extraX=record->extraX;
    view.hasTranslation=record->hasTranslation;
    view.hasAlignment=record->hasAlignment;
    return true;
}

const char* Localization::AsciiString(const char* fallback)
{
    AsciiEntryView view{};
    return LookupAscii(fallback,view)?view.text:fallback;
}

const char* Localization::AsciiStringById(const char* id,const char* fallback)
{
    const AsciiIdValue* value=g_Ascii.LookupId(id);
    return value!=nullptr&&value->hasTranslation?value->translation.c_str():fallback;
}

const char* Localization::StringById(const char* id,const char* fallback)
{
    const StringRecord* record=g_Strings.Lookup(id);
    return record!=nullptr&&record->hasTranslation?record->translation.c_str():fallback;
}

const char* Localization::FormatStringById(const char* id,const char* fallback)
{
    const StringRecord* record=g_Strings.Lookup(id);
    if(record==nullptr||!record->hasTranslation||fallback==nullptr)
        return fallback;

    std::string fallbackSignature;
    std::string translatedSignature;
    if(!ParsePrintfSignature(fallback,fallbackSignature)||
       !ParsePrintfSignature(record->translation.c_str(),translatedSignature)||
       fallbackSignature!=translatedSignature)
        return fallback;

    return record->translation.c_str();
}

const char* Localization::Utf8(const char* text)
{
    if(text==nullptr)
        return nullptr;
    // FontDevice only UTF-8-decodes while a pack is active, and falls
    // back to the CP932 path otherwise. Without a pack the original bytes must
    // pass through untouched, or original Japanese text would be decoded twice.
    if(!Active())
        return text;
    if(IsValidUtf8(reinterpret_cast<const unsigned char*>(text)))
        return text;
#ifdef TH_NATIVE_PLATFORM
    // Mixed localized strings happen when an untranslated CP932 format or
    // argument is substituted into a translated UTF-8 line. Re-encode the
    // CP932 piece so the whole formatted result stays valid UTF-8 and the
    // rasterizer never falls back to the wrong code page for either half.
    static Cp932 table;
    static bool ready=false;
    if(!ready)
    {
        ready=true;
        std::size_t size=0;
        if(void* bytes=SDL_LoadFile("/fonts/cp932.bin",&size))
        {
            table.load(static_cast<const u8*>(bytes),u32(size));
            SDL_free(bytes);
        }
    }
    if(!table.loaded())
        return text;
    static std::string pool[8];
    static unsigned slot=0;
    std::string& out=pool[(slot++)&7u];
    out.clear();
    const u8* cursor=reinterpret_cast<const u8*>(text);
    const u8* end=cursor+std::strlen(text);
    while(cursor<end)
    {
        const u16 code=table.next(cursor,end);
        if(code==0)
            break;
        if(code<0x80)
            out.push_back(char(code));
        else if(code<0x800)
        {
            out.push_back(char(0xc0|(code>>6)));
            out.push_back(char(0x80|(code&0x3f)));
        }
        else
        {
            out.push_back(char(0xe0|(code>>12)));
            out.push_back(char(0x80|((code>>6)&0x3f)));
            out.push_back(char(0x80|(code&0x3f)));
        }
    }
    return out.c_str();
#else
    return text;
#endif
}

void Localization::CopyCodepointChunk(char* destination,std::size_t capacity,const char* source,
                                      std::size_t firstCodepoint,std::size_t maximumCodepoints)
{
    if(destination==nullptr||capacity==0)
        return;
    destination[0]='\0';
    if(source==nullptr)
        return;
    const unsigned char* cursor=reinterpret_cast<const unsigned char*>(source);
    const bool utf8=IsValidUtf8(cursor);
    for(std::size_t index=0;index<firstCodepoint&&*cursor!=0;index++)
        cursor+=TextUnitLength(cursor,utf8);
    std::size_t written=0;
    for(std::size_t index=0;index<maximumCodepoints&&*cursor!=0;index++)
    {
        const std::size_t length=TextUnitLength(cursor,utf8);
        if(written+length>=capacity)
            break;
        std::memcpy(destination+written,cursor,length);
        written+=length;
        cursor+=length;
    }
    destination[written]='\0';
}

void Localization::CopyText(char* destination,std::size_t capacity,const char* source)
{
    if(destination==source)
        return;
    CopyCodepointChunk(destination,capacity,source,0,static_cast<std::size_t>(-1));
}

void Localization::CopyDisplayColumnChunk(char* destination,std::size_t capacity,const char* source,
                                          std::size_t firstColumn,std::size_t maximumColumns)
{
    if(destination==nullptr||capacity==0)
        return;
    destination[0]='\0';
    if(source==nullptr)
        return;
    const unsigned char* cursor=reinterpret_cast<const unsigned char*>(source);
    const bool utf8=IsValidUtf8(cursor);
    std::size_t column=0;
    while(*cursor!=0&&column<firstColumn)
    {
        const std::size_t length=TextUnitLength(cursor,utf8);
        column+=length==1&&*cursor<0x80?1:2;
        cursor+=length;
    }
    std::size_t written=0;
    const std::size_t endColumn=firstColumn+maximumColumns;
    while(*cursor!=0)
    {
        const std::size_t length=TextUnitLength(cursor,utf8);
        const std::size_t columns=length==1&&*cursor<0x80?1:2;
        if(column+columns>endColumn||written+length>=capacity)
            break;
        std::memcpy(destination+written,cursor,length);
        written+=length;
        cursor+=length;
        column+=columns;
    }
    destination[written]='\0';
}
} // namespace th09

#endif // TH_ENABLE_THCRAP
