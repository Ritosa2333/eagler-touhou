#include "WindowsHost.hpp"
#include "core/frontend/MenuReplayFormat.hpp"
#include "render/DisplayState.hpp"
#include "LegacyDisplayData.hpp"
#include <DxLib.h>
#include <windows.h>
#include <algorithm>
#include <cstring>
#include <sstream>

namespace kaikeidou {
namespace {
using U = std::uint32_t;
using Bytes = std::vector<std::uint8_t>;
std::wstring wide(std::string_view text) {
    if (text.empty()) return {};
    int n = MultiByteToWideChar(936, 0, text.data(), int(text.size()), nullptr, 0);
    std::wstring out(n, 0);
    MultiByteToWideChar(936, 0, text.data(), int(text.size()), out.data(), n);
    return out;
}
double real(U lo, U hi) { std::uint64_t q = lo | (std::uint64_t(hi) << 32); double v; std::memcpy(&v,&q,8); return v; }
float single(U bits) { float value; std::memcpy(&value,&bits,4); return value; }
Bytes word(U value) { Bytes b(4); std::memcpy(b.data(),&value,4); return b; }
bool intrinsic_alpha(const std::filesystem::path& path) {
    auto mask=path.parent_path()/(path.stem().wstring()+L"_a"+path.extension().wstring());
    if(std::filesystem::is_regular_file(mask))return true;
    std::ifstream file(path,std::ios::binary);std::array<unsigned char,8> signature{};
    file.read(reinterpret_cast<char*>(signature.data()),signature.size());
    constexpr std::array<unsigned char,8> png{{137,80,78,71,13,10,26,10}};
    if(signature!=png)return false;
    std::array<unsigned char,8> chunk{};
    while(file.read(reinterpret_cast<char*>(chunk.data()),chunk.size())){
        const auto length=(U(chunk[0])<<24)|(U(chunk[1])<<16)|(U(chunk[2])<<8)|chunk[3];
        if(std::memcmp(chunk.data()+4,"tRNS",4)==0)return true;
        if(std::memcmp(chunk.data()+4,"IHDR",4)==0){
            if(length!=13)return false;
            std::array<unsigned char,13> header{};file.read(reinterpret_cast<char*>(header.data()),header.size());
            if(header[9]==4||header[9]==6)return true;
            file.seekg(4,std::ios::cur);
        }else{
            if(std::memcmp(chunk.data()+4,"IDAT",4)==0||std::memcmp(chunk.data()+4,"IEND",4)==0)return false;
            file.seekg(std::streamoff(length)+4,std::ios::cur);
        }
    }
    return false;
}
bool drawing(U api) {
    switch (api) {
    case 0x404830: case 0x4048a0: case 0x40dd90: case 0x41bb70: case 0x41c3c0:
    case 0x41c910: case 0x41dd00: case 0x41e460: case 0x41e5b0: case 0x41e7b0:
    case 0x421700: case 0x421760: case 0x42d190: case 0x437c90: case 0x437f00: return true;
    default: return false;
    }
}
}

WindowsHost::WindowsHost(GameState& state, unsigned bits, bool variant, DesktopOptions options)
    : GamePlatformServices(state,bits,variant), options_(std::move(options)) {
    DxLib::SetOutApplicationLogValidFlag(FALSE);
    std::filesystem::create_directories(options_.userdata / "replay");
    if (!std::filesystem::exists(options_.userdata / "config.txt")) {
        DisplaySettings{}.save(options_.userdata/"config.txt");
    }
    settings_=DisplaySettings::read(options_.userdata/"config.txt");
    windowed_=settings_.windowed();window_scale_=settings_.window_scale();
    output_width_=settings_.width();output_height_=settings_.height();color_depth_=settings_.color_depth();
    software_rendering_=settings_.software_rendering();
    render_component=[this](U entry){draw_scene(entry);};
}
WindowsHost::~WindowsHost() { shutdown(); }
void WindowsHost::shutdown() noexcept {
    if(closing_)return;closing_=true;
    close();files_.clear();config_files_.clear();
    if(initialized_){
        initialized_=false;
        if(DxLib::DxLib_IsInit()){
            auto note=[&](const char* s){std::ofstream(options_.userdata/"thkkd.log",std::ios::app)<<s<<'\n';};
            note("shutdown sounds");DxLib::InitSoundMem();
            note("shutdown graphics");DxLib::InitGraph();canvas_=-1;
            note("shutdown platform");DxLib::DxLib_End();note("shutdown complete");
        }
    }
}
void WindowsHost::draw_scene(U entry){
    if(!display_){display_=std::make_unique<DisplayState>();install_legacy_display_data(*display_);}
    display_->precision=precision_;display_->snapshot(*memory);
    display_->platform=[this](const MenuSceneRequest& r){return invoke(r);};
    display_->input=[this](U binding){return U(input_count(binding));};
    const auto registers=world_.frame.registers;
    rendering_snapshot_=true;const bool old=suppress_drawing;suppress_drawing=false;
    try{display_->push(0);dispatch_display(*display_,entry);}
    catch(...){world_.frame.registers=registers;rendering_snapshot_=false;suppress_drawing=old;throw;}
    world_.frame.registers=registers;rendering_snapshot_=false;suppress_drawing=old;
}
void WindowsHost::present_gameplay(){draw_scene(0x61d690);suppress_drawing=true;}
std::filesystem::path WindowsHost::data_path(std::string_view name) const {
    auto relative = std::filesystem::path(wide(name)).lexically_normal();
    if (relative.is_absolute()) throw std::runtime_error("Game data path must be relative");
    for (const auto& part : relative) if (part == L"..") throw std::runtime_error("Game data path escapes userdata");
    return options_.userdata / relative;
}
std::filesystem::path WindowsHost::asset_path(std::string_view name) const {
    return options_.assets / std::filesystem::path(wide(name));
}
std::string WindowsHost::memory_string(U address) const {
    std::string text;
    for (U i=0;i<16384;++i) { auto c=memory->byte(address+i); if (!c) return text; text.push_back(char(c)); }
    throw std::runtime_error("Unterminated game string");
}
void WindowsHost::initialize_window() {
    if (initialized_) return;
    DxLib::SetOutApplicationLogValidFlag(FALSE);
    DxLib::SetMainWindowText(L"東方海恵堂 — thkkd");
    if(options_.window_mode_override>=0)windowed_=options_.window_mode_override!=0;
    DxLib::ChangeWindowMode(windowed_?TRUE:FALSE);
    DxLib::SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_BORDERLESS_WINDOW);
    DxLib::SetFullScreenScalingMode(DX_FSSCALINGMODE_BILINEAR);
    DxLib::SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
    DxLib::SetGraphMode(output_width_,output_height_,color_depth_);
    DxLib::SetWindowSizeExtendRate(640.0*window_scale_/output_width_,480.0*window_scale_/output_height_);
    DxLib::SetUseSoftwareRenderModeFlag(software_rendering_?TRUE:FALSE);
    DxLib::SetAlwaysRunFlag(TRUE);
    DxLib::SetWindowUserCloseEnableFlag(FALSE);
    DxLib::SetWaitVSyncFlag(FALSE);
    DxLib::SetUseMediaFoundationFlag(FALSE);
    // 沿用原作的绘图后端，保持加算图元和透明纹理的混合结果。
    DxLib::SetUseDirect3DVersion(DX_DIRECT3D_9);
    DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_GB2312);
    if (options_.secondary) {
        struct MonitorPosition { RECT rect{}; bool found=false; } position;
        EnumDisplayMonitors(nullptr,nullptr,[](HMONITOR monitor,HDC,LPRECT,LPARAM value)->BOOL {
            MONITORINFO info{sizeof(MONITORINFO)}; GetMonitorInfoW(monitor,&info);
            if (!(info.dwFlags & MONITORINFOF_PRIMARY)) {
                auto& p=*reinterpret_cast<MonitorPosition*>(value); p.rect=info.rcWork; p.found=true; return FALSE;
            }
            return TRUE;
        },reinterpret_cast<LPARAM>(&position));
        if (position.found) DxLib::SetWindowPosition(position.rect.left+40,position.rect.top+40);
        for(int i=0;i<DxLib::GetDisplayNum();++i){int x=0,y=0,w=0,h=0,primary=0;DxLib::GetDisplayInfo(i,&x,&y,&w,&h,&primary);if(!primary){DxLib::SetUseDirectDrawDeviceIndex(i);break;}}
    }
    if (DxLib::DxLib_Init() < 0) throw std::runtime_error("DxLib initialization failed");
    initialized_=true;
    DxLib::SetUseSetDrawScreenSettingReset(FALSE);
    DxLib::SetUseTransColorGraphCreateFlag(FALSE);
    DxLib::SetUseAlphaImageLoadFlag(TRUE);
    canvas_=DxLib::MakeScreen(640,480,FALSE);
    if(canvas_<0)throw std::runtime_error("Cannot create the logical game screen");
    DxLib::SetDrawScreen(canvas_);
    DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
    DxLib::SetFontSize(16);
    config_files_.clear();
    std::ofstream(options_.userdata/"thkkd.log",std::ios::app)<<"display windowed="<<windowed_<<" output="<<output_width_<<'x'<<output_height_<<" depth="<<color_depth_<<" scale="<<window_scale_<<" software="<<software_rendering_<<" filter=bilinear alpha=legacy\n";
}
int WindowsHost::legacy_transparency(int graph,int requested)const{
    // 原版在混合绘制时保留 PNG 和独立遮罩的透明度，即使调用参数为 FALSE。
    const auto found=graph_alpha_.find(graph);
    const bool blended=blend_mode_==DX_BLENDMODE_ALPHA||blend_mode_==DX_BLENDMODE_ADD||blend_mode_==DX_BLENDMODE_SUB;
    return requested||(blended&&found!=graph_alpha_.end()&&found->second);
}
void WindowsHost::toggle_fullscreen(){
    const bool requested=!windowed_;
    if(DxLib::ChangeWindowMode(requested?TRUE:FALSE)!=0)return;
    windowed_=requested;settings_.values[0]=windowed_?0:1;
    if(windowed_)DxLib::SetWindowSize(int(640*window_scale_),int(480*window_scale_));
    DxLib::SetDrawScreen(canvas_);DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
    DxLib::SetDrawArea(draw_area_[0],draw_area_[1],draw_area_[2],draw_area_[3]);
    settings_.save(options_.userdata/"config.txt");
    std::ofstream(options_.userdata/"thkkd.log",std::ios::app)<<"display toggled windowed="<<windowed_<<'\n';
}
int WindowsHost::process_messages(){
    if(options_.close_frame && frames>=options_.close_frame && !close_posted_){PostMessageW(DxLib::GetMainWindowHandle(),WM_CLOSE,0,0);close_posted_=true;}
    const auto result=DxLib::ProcessMessage();
    if(result!=0 || DxLib::GetWindowUserCloseFlag(TRUE)){
        std::ofstream(options_.userdata/"thkkd.log",std::ios::app)<<"window close requested frame="<<frames<<'\n';return -1;
    }
    const bool hotkey=DxLib::CheckHitKey(KEY_INPUT_F11)||((DxLib::CheckHitKey(KEY_INPUT_LALT)||DxLib::CheckHitKey(KEY_INPUT_RALT))&&DxLib::CheckHitKey(KEY_INPUT_RETURN));
    if((hotkey&&!hotkey_down_)||options_.toggle_frames.erase(frames))toggle_fullscreen();
    hotkey_down_=hotkey;
    return options_.frame_limit&&frames>=options_.frame_limit?-1:0;
}
int WindowsHost::present_screen(){
    int blend=0,alpha=255,r=255,g=255,b=255;DxLib::GetDrawBlendMode(&blend,&alpha);DxLib::GetDrawBright(&r,&g,&b);
    const int mode=DxLib::GetDrawMode();DxLib::SetDrawScreen(DX_SCREEN_BACK);
    int width=0,height=0;DxLib::GetDrawScreenSize(&width,&height);
    DxLib::SetDrawArea(0,0,width,height);DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND,255);DxLib::SetDrawBright(255,255,255);DxLib::ClearDrawScreen();
    DxLib::SetDrawMode(DX_DRAWMODE_BILINEAR);
    const double scale=std::min(width/640.0,height/480.0);const float w=float(640*scale),h=float(480*scale),x=(width-w)*0.5f,y=(height-h)*0.5f;
    DxLib::DrawExtendGraphF(x,y,x+w,y+h,canvas_,FALSE);const int result=DxLib::ScreenFlip();
    DxLib::SetDrawScreen(canvas_);DxLib::SetDrawBlendMode(blend,alpha);DxLib::SetDrawBright(r,g,b);DxLib::SetDrawMode(mode);
    DxLib::SetDrawArea(draw_area_[0],draw_area_[1],draw_area_[2],draw_area_[3]);return result;
}
void WindowsHost::screenshot(const std::filesystem::path& path) {
    if (path.empty()) return;
    std::filesystem::create_directories(path.parent_path());
    DxLib::SaveDrawScreenToPNG(0,0,640,480,path.c_str());
}

MenuScenePlatformResult WindowsHost::invoke(const MenuSceneRequest& request, MenuReplayFormatMemory* local) {
    const auto api=static_cast<U>(request.api); const auto& a=request.words;
    if(options_.trace && (frames<3 || frames+1==options_.frame_limit || options_.capture_frames.count(frames+1) || api==0x43c090 || api==0x43c2c0)) { std::ofstream trace(options_.userdata/"platform.log",std::ios::app);trace<<"frame="<<frames<<" snapshot="<<rendering_snapshot_<<" api="<<std::hex<<api;for(auto v:a)trace<<' '<<v;for(auto value:request.texts)trace<<" text="<<value;trace<<'\n'; }
    MenuScenePlatformResult out{0,world_.frame.registers.ecx,world_.frame.registers.edx,{},{}};
    auto arg=[&](std::size_t i)->U { return a.at(i); };
    auto text=[&](std::size_t i=0) { return wide(request.texts.at(i)); };
    if (suppress_drawing && drawing(api)) return out;
    if(rendering_snapshot_ && (api==0x43e010||api==0x43e0d0||api==0x43fd40||api==0x440240||api==0x4425f0||api==0x4434d0))return out;
    switch (api) {
    case 0x443e20:
        out.eax=U(process_messages()); break;
    case 0x443db0: break;
    case 0x41b8e0: suppress_drawing=false; out.eax=DxLib::ClearDrawScreen(); break;
    case 0x40e6b0:
        if(!options_.screenshot.empty()&&options_.capture_frames.count(frames+1)){
            const auto path=options_.screenshot.parent_path()/(options_.screenshot.stem().wstring()+L"-"+std::to_wstring(frames+1)+options_.screenshot.extension().wstring());screenshot(path);
        }
        if (!options_.screenshot.empty() && options_.frame_limit && frames+1==options_.frame_limit) screenshot(options_.screenshot);
        out.eax=present_screen(); ++frames; break;
    case 0x43d9f0: {
        std::array<char,256> keys{}; DxLib::GetHitKeyStateAll(keys.data());
        if (!options_.scripted_keys.empty()) {
            keys.fill(0); auto it=options_.scripted_keys.find(frames);
            if (it!=options_.scripted_keys.end()) for (auto key:it->second) if (key<keys.size()) keys[key]=1;
        }
        out.bytes.assign(keys.begin(),keys.end()); break;
    }
    case 0x43da40: out.eax=options_.scripted_keys.empty()?DxLib::GetJoypadInputState(DX_INPUT_PAD1):0; break;
    case 0x437fd0: out.eax=DxLib::GetNowCount(); break;
    case 0x69722c: if (!options_.fast) Sleep(std::min<U>(arg(0),100)); break;
    case 0x466760: out.eax=DxLib::GetRand(int(arg(0))); break;
    case 0x443d40: out.eax=DxLib::GetWindowActiveFlag(); break;
    case 0x4049a0: out.eax=DxLib::GetColor(int(arg(0)),int(arg(1)),int(arg(2))); break;
    case 0x404830: blend_mode_=int(arg(0));out.eax=DxLib::SetDrawBlendMode(blend_mode_,int(arg(1))); break;
    case 0x4048a0: out.eax=DxLib::SetDrawBright(int(arg(0)),int(arg(1)),int(arg(2))); break;
    case 0x40dd90: draw_area_={{int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3))}};out.eax=DxLib::SetDrawArea(draw_area_[0],draw_area_[1],draw_area_[2],draw_area_[3]); break;
    case 0x43c090: {
        auto path=asset_path(request.texts.at(0)); out.eax=DxLib::LoadGraph(path.c_str(),int(arg(0)));
        if (out.eax==U(-1)) throw std::runtime_error("Cannot load resource: "+path.u8string());
        graph_alpha_[int(out.eax)]=intrinsic_alpha(path);
        if(options_.trace)std::ofstream(options_.userdata/"platform.log",std::ios::app)<<"resource handle="<<std::hex<<out.eax<<" file="<<request.texts.at(0)<<" alpha="<<graph_alpha_[int(out.eax)]<<'\n';break;
    }
    case 0x43c2c0: {
        std::vector<int> handles(arg(0)); auto path=asset_path(request.texts.at(0));
        out.eax=DxLib::LoadDivGraph(path.c_str(),int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3)),int(arg(4)),handles.data(),int(arg(6)));
        if (out.eax==U(-1)) throw std::runtime_error("Cannot load sprite sheet: "+path.u8string());
        const bool alpha=intrinsic_alpha(path);for(auto handle:handles)graph_alpha_[handle]=alpha;
        if(options_.trace){std::ofstream log(options_.userdata/"platform.log",std::ios::app);for(std::size_t i=0;i<handles.size();++i)log<<"resource handle="<<std::hex<<handles[i]<<" file="<<request.texts.at(0)<<" tile="<<std::dec<<i<<" alpha="<<alpha<<'\n';}
        Bytes values(handles.size()*4); std::memcpy(values.data(),handles.data(),values.size());
        out.writes.push_back({arg(5),std::move(values)}); break;
    }
    case 0x403e20: out.eax=DxLib::DeleteGraph(int(arg(0)));if(out.eax!=U(-1))graph_alpha_.erase(int(arg(0)));break;
    case 0x41bb70: out.eax=DxLib::DrawGraph(int(arg(0)),int(arg(1)),int(arg(2)),legacy_transparency(int(arg(2)),int(arg(3)))); break;
    case 0x41c910: out.eax=DxLib::DrawRotaGraphF(single(arg(0)),single(arg(1)),real(arg(2),arg(3)),real(arg(4),arg(5)),int(arg(6)),legacy_transparency(int(arg(6)),int(arg(7))),int(arg(8))); break;
    case 0x41c3c0: out.eax=DxLib::DrawRotaGraph(int(arg(0)),int(arg(1)),real(arg(2),arg(3)),real(arg(4),arg(5)),int(arg(6)),legacy_transparency(int(arg(6)),int(arg(7))),int(arg(8))); break;
    case 0x421700: out.eax=DxLib::DrawModiGraphF(single(arg(0)),single(arg(1)),single(arg(2)),single(arg(3)),single(arg(4)),single(arg(5)),single(arg(6)),single(arg(7)),int(arg(8)),legacy_transparency(int(arg(8)),int(arg(9)))); break;
    case 0x421760: out.eax=DxLib::DrawBox(int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3)),arg(4),int(arg(5))); break;
    case 0x41e7b0: out.eax=DxLib::DrawPixel(int(arg(0)),int(arg(1)),arg(2)); break;
    case 0x41e460: out.eax=DxLib::DrawCircle(int(arg(0)),int(arg(1)),int(arg(2)),arg(3),a.size()>4?int(arg(4)):TRUE);break;
    case 0x41e5b0: out.eax=DxLib::DrawTriangle(int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3)),int(arg(4)),int(arg(5)),arg(6),int(arg(7))); break;
    case 0x41dd00: {
        auto count=arg(1)*3;std::vector<VERTEX3D> vertices(count);
        if(request.bytes.size()<count*24)throw std::runtime_error("Incomplete mesh vertices");
        for(U i=0;i<count;++i){auto p=request.bytes.data()+i*24;auto& v=vertices[i];std::memcpy(&v.pos,p,12);std::memcpy(&v.dif,p+12,4);std::memcpy(&v.u,p+16,8);v.norm=DxLib::VGet(0,0,-1);}
        DxLib::SetUseLighting(FALSE);out.eax=DxLib::DrawPolygon3D(vertices.data(),int(arg(1)),int(arg(2)),legacy_transparency(int(arg(2)),int(arg(3))));break;
    }
    case 0x42d190: out.eax=DxLib::DrawLine(int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3)),arg(4),int(arg(5))); break;
    case 0x432800: {
        auto name=text();out.eax=DxLib::CreateFontToHandle(name.c_str(),int(arg(0)),int(arg(1)),int(arg(2)),int(arg(3)),int(arg(4)),int(arg(5)));break;
    }
    case 0x432c20: {
        const auto font=U(DxLib::GetDefaultFontHandle());out.eax=font;
        out.writes.push_back({0x8b63f4,word(font)});break;
    }
    case 0x4331b0: out.eax=DxLib::SetFontSize(int(arg(0))); break;
    case 0x437bd0: { auto s=text(); out.eax=DxLib::GetDrawStringWidth(s.c_str(),int(s.size())); break; }
    case 0x437c20: { auto s=text(); out.eax=DxLib::GetDrawStringWidthToHandle(s.c_str(),int(s.size()),int(arg(0))); break; }
    case 0x437c90: { auto s=text(); out.eax=DxLib::DrawStringToHandle(int(arg(0)),int(arg(1)),s.c_str(),arg(2),int(arg(3)),arg(4),int(arg(5)));break; }
    case 0x437f00: {
        std::string formatted;const auto format=request.texts.at(0);std::size_t argument=3,text_argument=1;
        for(std::size_t i=0;i<format.size();++i){
            if(format[i]!='%'){formatted+=format[i];continue;}
            if(i+1<format.size()&&format[i+1]=='%'){formatted+='%';++i;continue;}
            auto start=i;while(++i<format.size()&&std::string("diuxXofFeEgGsc").find(format[i])==std::string::npos){}
            if(i==format.size())throw std::runtime_error("Incomplete display format");
            std::string spec(format.substr(start,i-start+1));char buffer[4096]{};
            if(format[i]=='s'){
                std::string value;
                if(text_argument<request.texts.size()){value=std::string(request.texts.at(text_argument++));++argument;}
                else value=memory_string(arg(argument++));
                std::snprintf(buffer,sizeof(buffer),spec.c_str(),value.c_str());
            }
            else if(std::string("fFeEgG").find(format[i])!=std::string::npos){auto v=real(arg(argument),arg(argument+1));argument+=2;std::snprintf(buffer,sizeof(buffer),spec.c_str(),v);}
            else std::snprintf(buffer,sizeof(buffer),spec.c_str(),arg(argument++));
            formatted+=buffer;
        }
        auto s=wide(formatted);out.eax=DxLib::DrawString(int(arg(0)),int(arg(1)),s.c_str(),arg(2));break;
    }
    case 0x43e0d0: out.eax=DxLib::SetCreateSoundDataType(int(arg(0)));break;
    case 0x4434d0: {
        auto path=asset_path(request.texts.at(0));out.eax=DxLib::LoadSoundMem(path.c_str(),int(arg(0)),int(arg(1)));
        if(out.eax==U(-1))throw std::runtime_error("Cannot load sound: "+path.u8string());break;
    }
    case 0x43e010: out.eax=DxLib::DeleteSoundMem(int(arg(0)));break;
    case 0x43fd40: out.eax=DxLib::ChangeVolumeSoundMem(options_.muted?0:int(arg(0)),int(arg(1)));break;
    case 0x440240: out.eax=DxLib::StopSoundMem(int(arg(0)));break;
    case 0x441b20: out.eax=DxLib::CheckSoundMem(int(arg(0)));break;
    case 0x4425f0: if(options_.muted)DxLib::ChangeVolumeSoundMem(0,int(arg(0)));out.eax=DxLib::PlaySoundMem(int(arg(0)),int(arg(1)),int(arg(2)));break;
    case 0x406770: out.eax=DxLib::SeekMovieToGraph(int(arg(0)),int(arg(1)));break;
    case 0x406690: out.eax=DxLib::PlayMovieToGraph(int(arg(0)),int(arg(1)),int(arg(2)));break;
    case 0x406700: out.eax=DxLib::PauseMovieToGraph(int(arg(0)));break;
    case 0x4067e0: out.eax=DxLib::GetMovieStateToGraph(int(arg(0)));break;
    case 0x40a9a0: { MATRIX matrix{};std::memcpy(&matrix,a.data(),64);out.eax=DxLib::SetTransformToViewport(&matrix);break; }
    case 0x455740:
        if(!local)throw std::runtime_error("Formatting requires argument memory");
        out.eax=format_menu_replay_integer(*local,{arg(0),arg(1),arg(2),arg(3)});break;
    case 0x697128:out.eax=next_allocation_;next_allocation_+=0x110000;break;
    case 0x69712c:out.eax=0;break;
    case 0x697044: {
        std::string name;
        try{name=memory_string(arg(0));}catch(const GameStartEntryAccessFault&){out.eax=U(-1);break;}
        auto path=data_path(name);std::filesystem::create_directories(path.parent_path());
        auto mode=std::ios::binary|std::ios::in;
        if(arg(1)&0x40000000u)mode|=std::ios::out;
        if(arg(4)==2)mode|=std::ios::trunc;
        File file;file.stream.open(path,mode);
        if(!file.stream){out.eax=U(-1);break;}
        out.eax=++next_handle_;files_.emplace(out.eax,std::move(file));break;
    }
    case 0x697048: {
        auto it=files_.find(arg(0));if(it==files_.end()){out.eax=U(-1);break;}
        auto& f=it->second.stream;f.clear();auto mode=arg(3)==2?std::ios::end:arg(3)==1?std::ios::cur:std::ios::beg;
        f.seekg(std::int32_t(arg(1)),mode);out.eax=U(f.tellg());break;
    }
    case 0x6970e0: {
        auto it=files_.find(arg(0));U count=0;
        if(it!=files_.end()) { Bytes bytes(arg(2));auto& f=it->second.stream;f.read(reinterpret_cast<char*>(bytes.data()),bytes.size());count=U(f.gcount());bytes.resize(count);out.writes.push_back({arg(1),std::move(bytes)});out.eax=1; }
        out.writes.push_back({arg(3),word(count)});break;
    }
    case 0x697040: {
        auto it=files_.find(arg(0));U count=0;
        if(it!=files_.end()) { auto bytes=memory->read(arg(1),arg(2));it->second.stream.write(reinterpret_cast<const char*>(bytes.data()),bytes.size());if(it->second.stream){count=arg(2);out.eax=1;} }
        out.writes.push_back({arg(3),word(count)});break;
    }
    case 0x6971fc:out.eax=files_.erase(arg(0))?1:0;break;
    case 0x438f70:break;
    default:
        if(api>=0x600000 && api<0x690000 && render_component){render_component(api);break;}
        { std::ostringstream error;error<<"Unimplemented desktop API 0x"<<std::hex<<api;throw std::runtime_error(error.str()); }
    }
    if(options_.trace&&drawing(api)&&out.eax==U(-1)){
        const bool empty=(api==0x41c910||api==0x41c3c0)&&real(a[2],a[3])==0.0;
        std::ofstream log(options_.userdata/"platform.log",std::ios::app);log<<(empty?"draw_empty frame=":"draw_failed frame=")<<frames<<" api="<<std::hex<<api;for(auto value:a)log<<' '<<value;log<<'\n';
    }
    return out;
}

ApplicationStartupPlatformResult WindowsHost::invoke(const ApplicationStartupRequest& request) {
    ApplicationStartupPlatformResult out;const auto& a=request.words;
    switch(request.api) {
    case 0x4f4572: {
        auto path=data_path(request.texts.at(0));auto mode=request.texts.at(1).find('w')!=std::string::npos?std::ios::out|std::ios::trunc:std::ios::in;
        std::fstream f(path,mode|std::ios::binary);if(f){out.eax=++next_handle_;config_files_.emplace(out.eax,std::move(f));}break;
    }
    case 0x4f4366: {
        auto it=config_files_.find(a.at(2));if(it==config_files_.end())break;
        Bytes bytes;char c;while(bytes.size()+1<a.at(1)&&it->second.get(c)){bytes.push_back(std::uint8_t(c));if(c=='\n')break;}
        if(!bytes.empty()){bytes.push_back(0);out.writes.push_back({a.at(0),std::move(bytes)});out.eax=a.at(0);}break;
    }
    case 0x4f4257: { auto it=config_files_.find(a.at(1));if(it!=config_files_.end())it->second<<request.texts.at(0);break; }
    case 0x4f4600:config_files_.erase(a.at(0));break;
    case 0x443d80:initialize_window();break;
    case 0x40db50:if(initialized_)out.eax=DxLib::SetDrawMode(int(a.at(0)));break;
    case 0x41f450:if(initialized_)out.eax=DxLib::SetDrawScreen(a.at(0)==U(DX_SCREEN_BACK)?canvas_:int(a.at(0)));break;
    case 0x40dd90:draw_area_={{int(a.at(0)),int(a.at(1)),int(a.at(2)),int(a.at(3))}};if(initialized_)out.eax=DxLib::SetDrawArea(draw_area_[0],draw_area_[1],draw_area_[2],draw_area_[3]);break;
    case 0x42e940:output_width_=int(a.at(0));output_height_=int(a.at(1));color_depth_=int(a.at(2));break;
    case 0x4463a0:windowed_=a.at(0)!=0;break;
    case 0x446510:window_scale_=real(a.at(0),a.at(1));break;
    case 0x4494c0:software_rendering_=a.at(0)==0;break;
    case 0x444500:out.eax=DxLib::SetMainWindowText(wide(request.texts.at(0)).c_str());break;
    case 0x444530:out.eax=DxLib::SetAlwaysRunFlag(int(a.at(0)));break;
    case 0x444540:out.eax=DxLib::SetWindowPosition(int(a.at(0)),int(a.at(1)));break;
    default:throw std::runtime_error("Unknown application startup API");
    }
    return out;
}
bool WindowsHost::open(std::string_view name,bool writing) {
    close();auto path=data_path(name);if(writing)std::filesystem::create_directories(path.parent_path());
    save_file_.open(path,std::ios::binary|(writing?std::ios::out|std::ios::trunc:std::ios::in));return bool(save_file_);
}
std::size_t WindowsHost::read(std::uint8_t* data,std::size_t size) {
    save_file_.read(reinterpret_cast<char*>(data),size);return std::size_t(save_file_.gcount());
}
void WindowsHost::write(const std::uint8_t* data,std::size_t size) {
    save_file_.write(reinterpret_cast<const char*>(data),size);if(!save_file_)throw std::runtime_error("Cannot write game save data");
}
void WindowsHost::close(){if(save_file_.is_open())save_file_.close();save_file_.clear();}
}
