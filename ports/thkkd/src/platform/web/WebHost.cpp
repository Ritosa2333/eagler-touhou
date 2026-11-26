#include "WebHost.hpp"
#include "LegacyDisplayData.hpp"
#include "core/frontend/MenuReplayFormat.hpp"
#include <emscripten.h>
#include <filesystem>
#include <cstring>
#include <sstream>

EM_JS(int,thkkd_platform,(unsigned api,const unsigned* args,unsigned count,const char* const* texts,const unsigned* lengths,unsigned text_count,const unsigned char* bytes,unsigned byte_count),{
    const words=Array.from(HEAPU32.subarray(args>>>2,(args>>>2)+count));
    const strings=[];
    const decoder=Module.thkkdDecoder||(Module.thkkdDecoder=new TextDecoder('gbk'));
    for(let i=0;i<text_count;i++){const p=HEAPU32[(texts>>>2)+i],n=HEAPU32[(lengths>>>2)+i];strings.push(decoder.decode(HEAPU8.subarray(p,p+n)));}
    return Module.thkkdPlatform.call(api,words,strings,HEAPU8.subarray(bytes,bytes+byte_count))|0;
});
EM_JS(void,thkkd_keys,(unsigned char* keys),{Module.thkkdPlatform.keys(HEAPU8.subarray(keys,keys+256));});
EM_JS(void,thkkd_div_handles,(unsigned* output,unsigned count),{HEAPU32.set(Module.thkkdPlatform.divHandles.slice(0,count),output>>>2);});
EM_JS(int,thkkd_motion,(double x,double y,double old_x,double old_y,double speed,unsigned state,unsigned frame,unsigned cursor,unsigned replay,double* result),{
    const value=Module.thkkdPlatform.input.motion?.({x,y,oldX:old_x,oldY:old_y,speed,blocked:!!state,frame,cursor,replay:!!replay});
    if(!value)return 0;HEAPF64[result>>>3]=value.x;HEAPF64[(result>>>3)+1]=value.y;return 1;
});
EM_JS(void,thkkd_motion_record,(double x,double y,unsigned cursor,unsigned replay),{Module.thkkdPlatform.input.recordMotion?.({x,y,cursor,replay:!!replay});});
EM_JS(void,thkkd_file_saved,(const char* path),{Module.thkkdPlatform.onFileSaved?.(UTF8ToString(path));});
EM_JS(void,thkkd_draw_identity,(unsigned id,unsigned birth,unsigned variant,unsigned pc,unsigned frame,unsigned scene,unsigned stage),{
    const r=Module.thkkdPlatform.renderer;r.identity=id?[id,birth,variant,pc].join(':'):null;r.tick=frame;r.scene=[scene,stage].join(':');
});

namespace kaikeidou {
bool apply_web_movement(std::uint64_t& x,std::uint64_t& y,std::uint64_t old_x,std::uint64_t old_y,std::uint64_t speed,std::uint32_t state,std::uint32_t frame,std::uint32_t cursor,bool replay){
    auto real=[](std::uint64_t bits){double value;std::memcpy(&value,&bits,8);return value;};double result[2]{};
    if(!thkkd_motion(real(x),real(y),real(old_x),real(old_y),real(speed),state,frame,cursor,replay,result))return false;
    if(!std::isfinite(result[0])||!std::isfinite(result[1]))throw std::runtime_error("Invalid extended movement");
    for(unsigned i=0;i<2;++i){std::uint64_t bits;std::memcpy(&bits,&result[i],8);auto value=binary_float::binary64(bits);value=binary_float::rounded(value,0,24);const auto rounded=binary_float::to_binary64(value);if(i==0)x=rounded;else y=rounded;}thkkd_motion_record(real(x),real(y),cursor,replay);return true;
}
namespace {
using U=std::uint32_t;using Bytes=std::vector<std::uint8_t>;
Bytes word(U n){Bytes b(4);std::memcpy(b.data(),&n,4);return b;}
bool drawing(U n){switch(n){case 0x404830:case 0x4048a0:case 0x40dd90:case 0x41bb70:case 0x41c3c0:case 0x41c910:case 0x41dd00:case 0x41e460:case 0x41e5b0:case 0x41e7b0:case 0x421700:case 0x421760:case 0x42d190:case 0x437c90:case 0x437f00:return true;default:return false;}}
int browser(const MenuSceneRequest& r){
    std::vector<const char*> texts;std::vector<U> lengths;
    for(auto t:r.texts){texts.push_back(t.data());lengths.push_back(U(t.size()));}
    return thkkd_platform(U(r.api),r.words.data(),U(r.words.size()),texts.data(),lengths.data(),U(texts.size()),r.bytes.data(),U(r.bytes.size()));
}
}
std::string WebHost::data_path(std::string_view name)const{
    std::string normalized(name);std::replace(normalized.begin(),normalized.end(),'\\','/');
    auto p=std::filesystem::path(normalized).lexically_normal();
    if(p.is_absolute())throw std::runtime_error("User file path must be relative");
    for(const auto& part:p)if(part=="..")throw std::runtime_error("User file path escapes save root");
    std::filesystem::path resolved="/savesthkkd";
    for(const auto& part:p){
        auto next=resolved/part;
        if(!std::filesystem::exists(next)&&std::filesystem::is_directory(resolved)){
            auto folded=[](std::string text){for(auto& c:text)if(c>='A'&&c<='Z')c=char(c-'A'+'a');return text;};
            const auto wanted=folded(part.string());
            for(const auto& entry:std::filesystem::directory_iterator(resolved))if(folded(entry.path().filename().string())==wanted){next=entry.path();break;}
        }
        resolved=next;
    }
    return resolved.string();
}
std::string WebHost::memory_string(U address)const{
    std::string text;for(U i=0;i<16384;++i){auto c=memory->byte(address+i);if(!c)return text;text.push_back(char(c));}
    throw std::runtime_error("Unterminated game string");
}
void WebHost::draw_scene(U entry){
    if(!display_){display_=std::make_unique<DisplayState>();install_legacy_display_data(*display_);}
    display_->precision=precision_;display_->snapshot(*memory);
    display_->platform=[this](const MenuSceneRequest& r){return invoke(r);};
    display_->input=[this](U binding){return U(input_count(binding));};
    const auto registers=world_.frame.registers;const bool old=suppress_drawing_;
    rendering_snapshot_=true;suppress_drawing_=false;
    try{display_->push(0);dispatch_display(*display_,entry);}
    catch(...){world_.frame.registers=registers;rendering_snapshot_=false;suppress_drawing_=old;throw;}
    world_.frame.registers=registers;rendering_snapshot_=false;suppress_drawing_=old;
}
void WebHost::present_gameplay(){draw_scene(0x61d690);suppress_drawing_=true;}
void WebHost::draw_identity(){
    U id=0,birth=0,variant=0,pc=0;const U frame=world_.patterns.game_frame;
    if(rendering_snapshot_&&display_){
        auto& d=*display_;pc=d.pc;const auto bp=d.reg[5];
        const auto stack=[&](int offset){const auto address=std::int64_t(bp)+offset;return address>=0&&std::uint64_t(address)+4<=d.data.size()?U(d.load<4>(U(address))):0xffffffffu;};
        if(pc>=0x614280&&pc<0x618c30){const auto index=stack(8);if(index<world_.projectiles.bullets.size()){
            const auto& object=world_.projectiles.bullets[index];id=200000+index;birth=frame-object.word(object.word(0)==1?0xc:0x10);variant=object.word(0)*256+object.word(4);
        }}else if(pc>=0x619b80&&pc<0x61d0f0){const auto index=stack(-4);if(index<world_.enemies.enemies.size()){
            const auto& object=world_.enemies.enemies[index];id=300000+index;birth=object.word(0x24);variant=object.word(0);
        }}else if(pc>=0x612480&&pc<0x612b80){const auto index=stack(-4);if(index<world_.effects.effects.size()){
            const auto& object=world_.effects.effects[index];id=400000+index;birth=frame-object.word(4);variant=object.word(0xc);
        }}else if((pc>=0x612b80&&pc<0x613560)||(pc>=0x619a80&&pc<0x619b80)){
            id=100000;birth=world_.player.word(0x14);variant=world_.player.word(0);
        }
    }
    thkkd_draw_identity(id,birth,variant,pc,frame,world_.menu.outer_scene,world_.lifecycle.stage);
}
MenuScenePlatformResult WebHost::invoke(const MenuSceneRequest& r,MenuReplayFormatMemory* local){
    U api=U(r.api);const auto& a=r.words;auto arg=[&](unsigned i){return a.at(i);};
    MenuScenePlatformResult out{0,world_.frame.registers.ecx,world_.frame.registers.edx,{},{}};
    if(suppress_drawing_&&drawing(api))return out;
    if(rendering_snapshot_&&(api==0x43e010||api==0x43e0d0||api==0x43fd40||api==0x440240||api==0x4425f0||api==0x4434d0))return out;
    if(drawing(api)||api==0x41b8e0)draw_identity();
    switch(api){
    case 0x443e20:out.eax=exiting?U(-1):0;break;
    case 0x443db0:break;
    case 0x41b8e0:suppress_drawing_=false;out.eax=browser(r);break;
    case 0x40e6b0:out.eax=browser(r);++frames;break;
    case 0x43d9f0:out.bytes.resize(256);thkkd_keys(out.bytes.data());break;
    case 0x43da40:out.eax=browser(r);break;
    case 0x437fd0:out.eax=U(std::uint64_t(frames)*1000/60);break;
    case 0x69722c:break;
    case 0x443d40:out.eax=1;break;
    case 0x4049a0:out.eax=(arg(0)<<16)|(arg(1)<<8)|arg(2);break;
    case 0x43c2c0:{
        out.eax=browser(r);std::vector<U> handles(arg(0));thkkd_div_handles(handles.data(),U(handles.size()));
        Bytes bytes(handles.size()*4);std::memcpy(bytes.data(),handles.data(),bytes.size());out.writes.push_back({arg(5),std::move(bytes)});break;
    }
    case 0x432c20:out.eax=browser(r);out.writes.push_back({0x8b63f4,word(out.eax)});break;
    case 0x437f00:{
        std::string formatted;const auto format=r.texts.at(0);unsigned argument=3,text_argument=1;
        for(std::size_t i=0;i<format.size();++i){
            if(format[i]!='%'){formatted+=format[i];continue;}
            if(i+1<format.size()&&format[i+1]=='%'){formatted+='%';++i;continue;}
            auto start=i;while(++i<format.size()&&std::string("diuxXofFeEgGsc").find(format[i])==std::string::npos){}
            if(i==format.size())throw std::runtime_error("Incomplete display format");
            std::string spec(format.substr(start,i-start+1));char buffer[4096]{};
            if(format[i]=='s'){
                std::string value;if(text_argument<r.texts.size()){value=std::string(r.texts.at(text_argument++));++argument;}else value=memory_string(arg(argument++));
                std::snprintf(buffer,sizeof(buffer),spec.c_str(),value.c_str());
            }else if(std::string("fFeEgG").find(format[i])!=std::string::npos){
                std::uint64_t bits=arg(argument)|(std::uint64_t(arg(argument+1))<<32);argument+=2;double v;std::memcpy(&v,&bits,8);std::snprintf(buffer,sizeof(buffer),spec.c_str(),v);
            }else std::snprintf(buffer,sizeof(buffer),spec.c_str(),arg(argument++));
            formatted+=buffer;
        }
        MenuSceneRequest text{MenuSceneApi::draw_font_text,{arg(0),arg(1),arg(2),0,0,0},{formatted},{}};out.eax=browser(text);break;
    }
    case 0x455740:if(!local)throw std::runtime_error("Formatting requires argument memory");out.eax=format_menu_replay_integer(*local,{arg(0),arg(1),arg(2),arg(3)});break;
    case 0x697128:out.eax=next_allocation_;next_allocation_+=0x110000;break;
    case 0x69712c:out.eax=0;break;
    case 0x697044:{
        std::string name;try{name=memory_string(arg(0));}catch(const GameStartEntryAccessFault&){out.eax=U(-1);break;}
        if(!replay_override.empty()&&memory->word(0xb4310c)!=0&&arg(4)==3)name=replay_override;
        auto path=data_path(name);std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        auto mode=std::ios::binary|std::ios::in;if(arg(1)&0x40000000u)mode|=std::ios::out;if(arg(4)==2)mode|=std::ios::trunc;
        std::fstream f(path,mode);if(!f){out.eax=U(-1);break;}out.eax=++next_handle_;files_.emplace(out.eax,std::move(f));if(arg(4)==2)recording_paths_[out.eax]=path;break;
    }
    case 0x697048:{auto it=files_.find(arg(0));if(it==files_.end()){out.eax=U(-1);break;}auto& f=it->second;f.clear();auto mode=arg(3)==2?std::ios::end:arg(3)==1?std::ios::cur:std::ios::beg;f.seekg(std::int32_t(arg(1)),mode);out.eax=U(f.tellg());break;}
    case 0x6970e0:{auto it=files_.find(arg(0));U count=0;if(it!=files_.end()){Bytes bytes(arg(2));it->second.read(reinterpret_cast<char*>(bytes.data()),bytes.size());count=U(it->second.gcount());bytes.resize(count);out.writes.push_back({arg(1),std::move(bytes)});out.eax=1;}out.writes.push_back({arg(3),word(count)});break;}
    case 0x697040:{auto it=files_.find(arg(0));U count=0;if(it!=files_.end()){auto bytes=memory->read(arg(1),arg(2));it->second.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());if(it->second){count=arg(2);out.eax=1;}}out.writes.push_back({arg(3),word(count)});break;}
    case 0x6971fc:{out.eax=files_.erase(arg(0))?1:0;auto found=recording_paths_.find(arg(0));if(found!=recording_paths_.end()){thkkd_file_saved(found->second.c_str());recording_paths_.erase(found);}break;}
    case 0x438f70:break;
    default:if(api>=0x600000&&api<0x690000)draw_scene(api);else out.eax=browser(r);break;
    }
    return out;
}
ApplicationStartupPlatformResult WebHost::invoke(const ApplicationStartupRequest& r){
    ApplicationStartupPlatformResult out;const auto& a=r.words;
    switch(r.api){
    case 0x4f4572:{auto path=data_path(r.texts.at(0));const bool writing=r.texts.at(1).find('w')!=std::string::npos;auto mode=writing?std::ios::out|std::ios::trunc:std::ios::in;std::fstream f(path,mode|std::ios::binary);if(f){out.eax=++next_handle_;config_files_.emplace(out.eax,std::move(f));if(writing)config_write_paths_[out.eax]=path;}break;}
    case 0x4f4366:{auto it=config_files_.find(a.at(2));if(it==config_files_.end())break;Bytes bytes;char c;while(bytes.size()+1<a.at(1)&&it->second.get(c)){bytes.push_back(std::uint8_t(c));if(c=='\n')break;}if(!bytes.empty()){bytes.push_back(0);out.writes.push_back({a.at(0),std::move(bytes)});out.eax=a.at(0);}break;}
    case 0x4f4257:{auto it=config_files_.find(a.at(1));if(it!=config_files_.end())it->second<<r.texts.at(0);break;}
    case 0x4f4600:{const auto handle=a.at(0);config_files_.erase(handle);const auto found=config_write_paths_.find(handle);if(found!=config_write_paths_.end()){thkkd_file_saved(found->second.c_str());config_write_paths_.erase(found);}break;}
    default:{std::vector<std::string_view> texts(r.texts.begin(),r.texts.end());out.eax=browser(MenuSceneRequest{static_cast<MenuSceneApi>(r.api),r.words,std::move(texts),{}});break;}
    }
    return out;
}
bool WebHost::open(std::string_view name,bool writing){close();auto path=data_path(name);if(writing)std::filesystem::create_directories(std::filesystem::path(path).parent_path());save_file_.open(path,std::ios::binary|(writing?std::ios::out|std::ios::trunc:std::ios::in));if(save_file_&&writing)save_write_path_=path;return bool(save_file_);}
std::size_t WebHost::read(std::uint8_t* data,std::size_t n){save_file_.read(reinterpret_cast<char*>(data),n);return std::size_t(save_file_.gcount());}
void WebHost::write(const std::uint8_t* data,std::size_t n){save_file_.write(reinterpret_cast<const char*>(data),n);if(!save_file_)throw std::runtime_error("Cannot write save data");}
void WebHost::close(){if(save_file_.is_open())save_file_.close();save_file_.clear();if(!save_write_path_.empty()){auto path=std::move(save_write_path_);save_write_path_.clear();thkkd_file_saved(path.c_str());}}
}
