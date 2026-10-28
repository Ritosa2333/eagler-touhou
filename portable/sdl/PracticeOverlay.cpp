#include "PracticeOverlay.hpp"
#include <algorithm>
#include <SDL3/SDL.h>
#include "Renderer.hpp"
#include "RuntimeOptions.hpp"
#include "wasm/Api.hpp"
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
#include "imgui.h"
extern "C" {
int th12_practice_ui_init(const unsigned char*,int,int);
void th12_practice_ui_shutdown();void th12_practice_ui_key(int,int);
void th12_practice_ui_cancel();void th12_practice_ui_clear_keys();void th12_practice_ui_mobile(int,int);
int th12_practice_ui_pointer(int,int,float,float,int);int th12_practice_ui_wheel(float,float,float,float);
void th12_practice_ui_context(int,int,int,int,int);void th12_practice_ui_params(const int32_t*,int);
const int32_t* th12_practice_ui_parameters();void th12_practice_ui_tick(int);
int th12_practice_ui_capture();int th12_practice_ui_action();
}
#endif
namespace th12host {
void PracticeOverlay::initialize(Renderer& gpu){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized||!runtime_options.thprac)return;
    host::read_file(host::paths().data_file("unifont.otf"),font,32*1024*1024);
    if(font.empty())host::read_file(host::paths().data_file("msgothic.ttc"),font,32*1024*1024);
#ifdef _WIN32
    if(font.empty())if(const char* windows=SDL_getenv("WINDIR"))host::read_file(std::string(windows)+"/Fonts/msgothic.ttc",font,32*1024*1024);
#endif
    initialized=th12_practice_ui_init(font.empty()?nullptr:font.data(),int(font.size()),runtime_options.language)==1;
    if(!initialized)return;
    unsigned char* pixels;int width,height;ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    texture=gpu.create_texture(unsigned(width),unsigned(height),pixels,true);
    ImGui::GetIO().Fonts->SetTexID(reinterpret_cast<ImTextureID>(uintptr_t(texture)));
    th12_practice_enabled(1);
#else
    (void)gpu;
#endif
}
void PracticeOverlay::shutdown(Renderer& gpu){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized){gpu.delete_texture(texture);th12_practice_ui_shutdown();initialized=false;font.clear();}
#else
    (void)gpu;
#endif
}
void PracticeOverlay::key(int code,bool down){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized)th12_practice_ui_key(code,down);
#else
    (void)code;(void)down;
#endif
}
bool PracticeOverlay::pointer(int type,int id,float x,float y){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    return initialized&&th12_practice_ui_pointer(type,id,x,y,1)!=0;
#else
    (void)type;(void)id;(void)x;(void)y;return false;
#endif
}
void PracticeOverlay::clear(bool pointers){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized){if(pointers)th12_practice_ui_cancel();else th12_practice_ui_clear_keys();}
#else
    (void)pointers;
#endif
}
void PracticeOverlay::mobile(bool enabled,bool visible){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized)th12_practice_ui_mobile(enabled,visible);
#else
    (void)enabled;(void)visible;
#endif
}
void PracticeOverlay::wheel(float x,float y,float horizontal,float vertical){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(initialized)th12_practice_ui_wheel(x,y,horizontal,vertical);
#else
    (void)x;(void)y;(void)horizontal;(void)vertical;
#endif
}
int PracticeOverlay::tick(int scene,bool ready,bool replay,int difficulty,unsigned held,std::array<int32_t,20>& params){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(!initialized)return 0;
    th12_practice_ui_context(scene,ready,replay,difficulty,runtime_options.language);
    if(scene==1)th12_practice_ui_params(params.data(),int(params.size()));
    th12_practice_ui_tick(int(held));
    if(scene==1)std::copy_n(th12_practice_ui_parameters(),params.size(),params.begin());
    return th12_practice_ui_action();
#else
    (void)scene;(void)ready;(void)replay;(void)difficulty;(void)held;(void)params;return 0;
#endif
}
bool PracticeOverlay::captures()const{
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    return initialized&&th12_practice_ui_capture();
#else
    return false;
#endif
}
void PracticeOverlay::render(Renderer& gpu){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    if(!initialized)return;const ImDrawData* data=ImGui::GetDrawData();if(!data)return;
    gpu.flush();gpu.primitive(Renderer::Triangles);gpu.material(0);gpu.fog_color(0,0,0);
    for(int l=0;l<data->CmdListsCount;++l){const ImDrawList& list=*data->CmdLists[l];
        for(const ImDrawCmd& command:list.CmdBuffer){
            if(command.UserCallback){if(command.UserCallback!=ImDrawCallback_ResetRenderState)command.UserCallback(&list,&command);continue;}
            gpu.flush();gpu.bind({unsigned(uintptr_t(command.TextureId)),1,1,false,false,true},false);
            gpu.scissor(int(command.ClipRect.x),int(command.ClipRect.y),int(command.ClipRect.z-command.ClipRect.x),int(command.ClipRect.w-command.ClipRect.y));
            for(unsigned i=0;i<command.ElemCount;++i){const auto& v=list.VtxBuffer[int(command.VtxOffset+list.IdxBuffer[int(command.IdxOffset+i)])];const auto c=v.col;
                gpu.push({v.pos.x-data->DisplayPos.x,v.pos.y-data->DisplayPos.y,v.uv.x,v.uv.y,
                    float((c>>IM_COL32_R_SHIFT)&255)/255,float((c>>IM_COL32_G_SHIFT)&255)/255,
                    float((c>>IM_COL32_B_SHIFT)&255)/255,float((c>>IM_COL32_A_SHIFT)&255)/255,1,0});}
            gpu.flush();
        }
    }
    gpu.clip(0);
#else
    (void)gpu;
#endif
}
}
