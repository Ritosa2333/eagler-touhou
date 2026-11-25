#pragma once
#include "practice_advanced.hpp"
#include "menu.hpp"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <system_error>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
namespace soew {
inline std::wstring settings_warning_text(Language language,std::string_view warning){
    struct WarningText{std::string_view en;const wchar_t *ja,*hans,*hant;};
    constexpr WarningText messages[]={
        {"Malformed settings JSON; file protected",L"設定ファイルの形式が不正です。上書きしません。",L"设置文件格式有误，原文件未覆盖。",L"設定檔格式有誤，原檔未覆寫。"},
        {"Cannot read settings; file protected",L"設定ファイルを読めません。上書きしません。",L"无法读取设置，原文件未覆盖。",L"無法讀取設定，原檔未覆寫。"},
        {"Invalid settings size; file protected",L"設定ファイルのサイズが不正です。上書きしません。",L"设置文件大小有误，原文件未覆盖。",L"設定檔大小有誤，原檔未覆寫。"},
        {"Unsupported settings version; file protected",L"未対応の設定ファイルです。上書きしません。",L"不支持此设置版本，原文件未覆盖。",L"不支援此設定版本，原檔未覆寫。"},
        {"Unknown settings field; file protected",L"不明な設定項目があります。上書きしません。",L"设置中有未知选项，原文件未覆盖。",L"設定中有未知選項，原檔未覆寫。"},
        {"Some settings fields invalid; defaults used",L"不正な項目には初期設定を使用します。",L"部分设置无效，已使用默认值。",L"部分設定無效，已使用預設值。"},
        {"Cannot create temporary settings",L"設定の一時ファイルを作成できません。",L"无法创建临时设置文件。",L"無法建立暫存設定檔。"},
        {"Cannot flush settings",L"設定を書き込めません。",L"无法写入设置。",L"無法寫入設定。"},
        {"Cannot close settings",L"設定ファイルを閉じられません。",L"无法关闭设置文件。",L"無法關閉設定檔。"},
        {"Atomic settings replace failed",L"設定ファイルを更新できません。",L"无法更新设置文件。",L"無法更新設定檔。"},
        {"Could not switch fullscreen mode",L"全画面表示を切り替えられませんでした。",L"无法切换全屏。",L"無法切換全螢幕。"},
        {"Could not start in fullscreen mode",L"全画面表示で起動できませんでした。",L"无法以全屏启动。",L"無法以全螢幕啟動。"},
        {"Could not save the native clear record",L"クリア記録を保存できませんでした。",L"无法保存通关记录。",L"無法儲存通關紀錄。"},
        {"Could not save the native score; existing records retained",L"スコアを保存できません。既存の記録は保持されます。",L"无法保存分数，原记录已保留。",L"無法儲存分數，原紀錄已保留。"},
        {"Native scores could not be loaded; existing records retained",L"スコアを読めません。既存の記録は保持されます。",L"无法读取分数，原记录已保留。",L"無法讀取分數，原紀錄已保留。"},
    };
    if(warning.empty())return {};
    for(const auto &[en,ja,hans,hant]:messages)if(warning==en){
        if(language!=Language::English)return localized(language,ja,L"",hans,hant);
        return {en.begin(),en.end()};
    }
    return localized(language,L"設定ファイルに問題があります。",L"There is a problem with the settings file.",L"设置文件有误。",L"設定檔有誤。");
}
// Only user options, never live run inventory or original scores/config.
struct Settings {
    int rank=1,music=1,lives=2,bombs=3;
    bool reduced=false,autofire=false,show_more_info=false,fullscreen=false,fast_autofire=true;
    HitboxMode hitboxes=HitboxMode::Off;
    Language language=Language::Japanese;
    bool stage_intro=true,smooth_background=true;
    PracticeAdvanced practice_advanced;
    AudioLevels audio_levels;
    static Settings from(const Menu &m,PracticeAdvanced advanced={}){return {m.rank,m.music,m.lives,m.bombs,m.reduced,m.autofire,m.show_more_info,m.fullscreen,m.fast_autofire,m.hitboxes,m.language,m.stage_intro,m.smooth_background,advanced,m.audio_levels};}
    void apply(Menu &m)const{m.rank=rank;m.music=music==Menu::music_akyu&&!m.akyu_available?Menu::music_fm:music;m.lives=lives;m.bombs=bombs;m.reduced=reduced;m.autofire=autofire;m.show_more_info=show_more_info;m.fullscreen=fullscreen;m.fast_autofire=fast_autofire;m.hitboxes=hitboxes;m.language=language;m.stage_intro=stage_intro;m.smooth_background=smooth_background;m.audio_levels=audio_levels;}
    static constexpr const char* language_id(Language language){return localized(language,"ja","en","zh-Hans","zh-Hant");}
    static constexpr const wchar_t* wide_language_id(Language language){return localized(language,L"ja",L"en",L"zh-Hans",L"zh-Hant");}
    bool operator==(const Settings&)const=default;
};
struct SettingsStore {
    std::filesystem::path path;
    Settings saved;
    bool writable=true;
    std::string warning;
    bool needs_migration=false;
    explicit SettingsStore(std::filesystem::path p):path(std::move(p)){}
    struct Value {std::string text;bool integer=false,boolean=false,string=false;};
    // Validate all JSON values, including wrong-type and unknown fields, before applying any option.
    struct Reader {
        std::string_view s;
        std::size_t i=0;
        [[noreturn]] static void bad(){throw std::runtime_error("Malformed settings JSON; file protected");}
        void ws(){while(i<s.size()&&(s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]=='\t'))++i;}
        bool accept(char c){ws();if(i<s.size()&&s[i]==c){++i;return true;}return false;}
        void take(char c){if(!accept(c))bad();}
        unsigned hex(){unsigned value=0;for(int n=0;n<4;++n){if(i==s.size())bad();const char c=s[i++];value*=16;if(c>='0'&&c<='9')value+=c-'0';else if(c>='a'&&c<='f')value+=c-'a'+10;else if(c>='A'&&c<='F')value+=c-'A'+10;else bad();}return value;}
        std::string str(){
            take('"');std::string out;
            while(i<s.size()){
                const unsigned char c=s[i++];if(c=='"')return out;if(c<32)bad();
                if(c!='\\'){out+=char(c);continue;}
                if(i==s.size())bad();
                const char escape=s[i++];
                switch(escape){
                case '"':case '\\':case '/':out+=escape;break;
                case 'b':out+='\b';break;case 'f':out+='\f';break;case 'n':out+='\n';break;case 'r':out+='\r';break;case 't':out+='\t';break;
                case 'u':{
                    unsigned cp=hex();if(cp>=0xd800&&cp<=0xdbff){if(i+2>s.size()||s[i++]!='\\'||s[i++]!='u')bad();const unsigned low=hex();if(low<0xdc00||low>0xdfff)bad();cp=0x10000+((cp-0xd800)<<10)+(low-0xdc00);}else if(cp>=0xdc00&&cp<=0xdfff)bad();
                    if(cp<0x80)out+=char(cp);else if(cp<0x800){out+=char(0xc0|(cp>>6));out+=char(0x80|(cp&63));}else if(cp<0x10000){out+=char(0xe0|(cp>>12));out+=char(0x80|((cp>>6)&63));out+=char(0x80|(cp&63));}else{out+=char(0xf0|(cp>>18));out+=char(0x80|((cp>>12)&63));out+=char(0x80|((cp>>6)&63));out+=char(0x80|(cp&63));}break;
                }
                default:bad();
                }
            }bad();
        }
        Value value(unsigned depth=0){
            if(depth>32)bad();
            ws();if(i==s.size())bad();
            if(s[i]=='"')return {str(),false,false,true};
            if(accept('{')){if(!accept('}')){do{str();take(':');value(depth+1);}while(accept(','));take('}');}return {};}
            if(accept('[')){if(!accept(']')){do{value(depth+1);}while(accept(','));take(']');}return {};}
            const auto start=i;
            for(const auto literal:{std::string_view("true"),std::string_view("false"),std::string_view("null")})if(s.substr(i,literal.size())==literal){i+=literal.size();return {std::string(literal),false,literal!="null"};}
            if(s[i]=='-')++i;
            if(i==s.size())bad();
            if(s[i]=='0')++i;else{if(s[i]<'1'||s[i]>'9')bad();while(i<s.size()&&s[i]>='0'&&s[i]<='9')++i;}
            bool whole=true;
            if(i<s.size()&&s[i]=='.'){whole=false;++i;const auto digits=i;while(i<s.size()&&s[i]>='0'&&s[i]<='9')++i;if(i==digits)bad();}
            if(i<s.size()&&(s[i]=='e'||s[i]=='E')){whole=false;++i;if(i<s.size()&&(s[i]=='+'||s[i]=='-'))++i;const auto digits=i;while(i<s.size()&&s[i]>='0'&&s[i]<='9')++i;if(i==digits)bad();}
            Value out{std::string(s.substr(start,i-start)),false,false};int n=0;const auto r=std::from_chars(out.text.data(),out.text.data()+out.text.size(),n);out.integer=whole&&r.ec==std::errc{}&&r.ptr==out.text.data()+out.text.size();return out;
        }
    };
    static std::map<std::string,Value> parse(std::string_view s){
        Reader r{s};std::map<std::string,Value> result;r.take('{');
        if(!r.accept('}')){do{auto key=r.str();r.take(':');auto value=r.value();if(!result.emplace(std::move(key),std::move(value)).second)Reader::bad();}while(r.accept(','));r.take('}');}
        r.ws();if(r.i!=s.size())Reader::bad();return result;
    }
    Settings load(){
        warning.clear();writable=true;needs_migration=false;Settings out;
        try{
            if(!std::filesystem::exists(path)){saved=out;return out;}
            std::ifstream f(path,std::ios::binary|std::ios::ate);if(!f)throw std::runtime_error("Cannot read settings; file protected");const auto n=f.tellg();if(n<2||n>16384)throw std::runtime_error("Invalid settings size; file protected");std::string data(static_cast<std::size_t>(n),' ');f.seekg(0);if(!f.read(data.data(),static_cast<std::streamsize>(n)))throw std::runtime_error("Cannot read settings; file protected");const auto fields=parse(data);
            const auto version=fields.find("version");if(version==fields.end()||!version->second.integer||(version->second.text!="1"&&version->second.text!="2")){writable=false;warning="Unsupported settings version; file protected";saved=out;return out;}
            needs_migration=version->second.text=="1"||fields.contains("practice_language");
            constexpr std::array<std::string_view,21> known={"version","rank","music","lives","bombs","reduced","autofire","show_more_info","fullscreen","fast_autofire","hitboxes","language","stage_intro","smooth_background","practice_language","practice_speed_preset","practice_speed_fps","practice_free_fps","practice_flags","music_volume","effect_volume"};
            for(const auto &[key,value]:fields){(void)value;if(std::find(known.begin(),known.end(),key)==known.end()){writable=false;warning="Unknown settings field; file protected";}}
            const auto invalid=[&]{if(writable)warning="Some settings fields invalid; defaults used";};
            auto integer=[&](const char*key,int &v,int max){const auto it=fields.find(key);if(it==fields.end())return;int n=0;const auto &x=it->second;if(!x.integer){invalid();return;}std::from_chars(x.text.data(),x.text.data()+x.text.size(),n);if(n<0||n>max){invalid();return;}v=n;};
            auto boolean=[&](const char*key,bool &v){const auto it=fields.find(key);if(it==fields.end())return;if(!it->second.boolean){invalid();return;}v=it->second.text=="true";};
            integer("rank",out.rank,3);integer("music",out.music,Menu::music_akyu);integer("lives",out.lives,4);integer("bombs",out.bombs,3);boolean("reduced",out.reduced);boolean("autofire",out.autofire);boolean("show_more_info",out.show_more_info);boolean("fullscreen",out.fullscreen);boolean("fast_autofire",out.fast_autofire);boolean("stage_intro",out.stage_intro);if(!out.autofire)out.fast_autofire=true;
            boolean("smooth_background",out.smooth_background);
            const auto volume=[&](const char *key,int &value){
                int level=value;integer(key,level,AudioLevels::maximum);
                if(level%AudioLevels::step==0)value=level;else invalid();
            };
            volume("music_volume",out.audio_levels.music);volume("effect_volume",out.audio_levels.effects);
            int hitboxes=0;integer("hitboxes",hitboxes,2);out.hitboxes=static_cast<HitboxMode>(hitboxes);
            const auto language=[&](const char *key)->std::optional<Language>{
                const auto it=fields.find(key);
                if(it==fields.end())return std::nullopt;
                for(const auto lang:{Language::Japanese,Language::English,Language::ChineseSimplified,Language::ChineseTraditional})
                    if(it->second.string&&it->second.text==Settings::language_id(lang))return lang;
                invalid();return std::nullopt;
            };
            out.language=language("language").value_or(out.language);
            auto &advanced=out.practice_advanced;
            int preset=PracticeAdvanced::native_speed,fps=0,flags=0;
            integer("practice_speed_preset",preset,PracticeAdvanced::max_preset);
            integer("practice_speed_fps",fps,PracticeAdvanced::max_fps);
            integer("practice_flags",flags,(1u<<(PracticeAdvanced::FastRetry+1))-1);
            boolean("practice_free_fps",advanced.free_fps);
            advanced.flags=std::uint8_t(flags);advanced.speed_in_fps=fps>0;
            if(!preset)invalid();
            advanced.speed=unsigned(fps?fps:preset?preset:PracticeAdvanced::native_speed);
        }catch(const std::exception &e){warning=e.what();writable=false;out=Settings{};}
        saved=out;return out;
    }
    bool save(const Settings &value){
        if(!writable)return false;
        if(value==saved&&!needs_migration)return true;
        std::filesystem::path temp;
        try{
            if(!path.parent_path().empty())std::filesystem::create_directories(path.parent_path());
            static std::atomic<unsigned long long> sequence{0};temp=path;temp+=".tmp-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+"-"+std::to_string(sequence++);
            std::string practice;
            const auto &advanced=value.practice_advanced;
            if(advanced.speed_in_fps||advanced.speed!=PracticeAdvanced::native_speed)
                practice+=std::string(advanced.speed_in_fps?",\n  \"practice_speed_fps\": ":",\n  \"practice_speed_preset\": ")+std::to_string(advanced.speed);
            if(advanced.free_fps)practice+=",\n  \"practice_free_fps\": true";
            if(advanced.flags)practice+=",\n  \"practice_flags\": "+std::to_string(advanced.flags);
            std::string audio;
            if(value.audio_levels.music!=AudioLevels{}.music)audio+=",\n  \"music_volume\": "+std::to_string(value.audio_levels.music);
            if(value.audio_levels.effects!=AudioLevels{}.effects)audio+=",\n  \"effect_volume\": "+std::to_string(value.audio_levels.effects);
            const auto data=std::string("{\n  \"version\": 2,\n  \"rank\": ")+std::to_string(value.rank)+",\n  \"music\": "+std::to_string(value.music)+",\n  \"lives\": "+std::to_string(value.lives)+",\n  \"bombs\": "+std::to_string(value.bombs)+",\n  \"reduced\": "+(value.reduced?"true":"false")+",\n  \"autofire\": "+(value.autofire?"true":"false")+",\n  \"show_more_info\": "+(value.show_more_info?"true":"false")+(value.fullscreen?",\n  \"fullscreen\": true":"")+(value.stage_intro?"":",\n  \"stage_intro\": false")+(value.smooth_background?"":",\n  \"smooth_background\": false")+(value.autofire&&!value.fast_autofire?",\n  \"fast_autofire\": false":"")+(value.hitboxes==HitboxMode::Off?std::string{}:",\n  \"hitboxes\": "+std::to_string(static_cast<int>(value.hitboxes)))+",\n  \"language\": \""+Settings::language_id(value.language)+"\""+audio+practice+"\n}\n";
#ifdef _WIN32
            const auto file=CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);if(file==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot create temporary settings");
            DWORD written=0;const bool flushed=WriteFile(file,data.data(),static_cast<DWORD>(data.size()),&written,nullptr)&&written==data.size()&&FlushFileBuffers(file);const bool closed=CloseHandle(file)!=0;if(!flushed||!closed)throw std::runtime_error("Cannot flush settings");
            if(!MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("Atomic settings replace failed");
#else
            {std::ofstream f(temp,std::ios::binary|std::ios::trunc);if(!f)throw std::runtime_error("Cannot create temporary settings");f<<data;f.flush();if(!f)throw std::runtime_error("Cannot flush settings");f.close();if(!f)throw std::runtime_error("Cannot close settings");}
            std::filesystem::rename(temp,path);
#endif
            saved=value;needs_migration=false;warning.clear();return true;
        }catch(const std::exception &e){if(!temp.empty()){std::error_code ignored;std::filesystem::remove(temp,ignored);}warning=e.what();return false;}
    }
};
}
