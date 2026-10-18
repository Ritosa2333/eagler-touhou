#include "../th10_web/cpp/game/GameInput.hpp"
#include "../th10_web/cpp/game/Presentation.hpp"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <new>
#include <type_traits>
#include <vector>

using namespace th10;

namespace {
constexpr u16 screenshot_key=0x0800;
static_assert(std::is_same_v<decltype(PresentationEnvironment::pressed_keys),const u16*>);
static_assert(offsetof(GameInput,raw_pressed)==6 && sizeof(GameInput)==0x58);
static_assert(sizeof(PresentationEnvironment)==6*sizeof(void*));

// Use the real input object. raw_pressed must be two-byte aligned, but not
// four-byte aligned, just as it is in Application's player input profile.
alignas(4) GameInput input{};
ApplicationState application{};
AnmManager animations{};
i32 reset_frames=0,parameters=0,device=0,surface_tokens[3]{};

enum Call { Present,Release,Reset,Configure,CaptureTexture,CaptureScreen,Directory,Exists,Save };

bool equal(CaptureRectangle a,CaptureRectangle b){return a.left==b.left&&a.top==b.top&&a.width==b.width&&a.height==b.height;}

struct Environment final:PresentationEnvironment {
    AnmManager* manager=&::animations;
    CaptureRequests* requests;
    AnmCaptureBuffers* buffers;
    std::vector<Call> calls;
    std::vector<void*> released;
    i32 present_result=0;
    u32 occupied_names=0,lookups=0,saved=0;
    char saved_name[sizeof("snapshot/th000.bmp")]{};
    Environment(){
        application=&::application;animations=&manager;pressed_keys=&input.raw_pressed;
        reset_frames=&::reset_frames;presentation_parameters=&parameters;
        application->device=&device;
        requests=new(manager->header) CaptureRequests{};
        requests->texture.target_file=-1;requests->texture.reserved_000=-1;
        buffers=new(manager->resource_state+4) AnmCaptureBuffers{};
        const auto base=reinterpret_cast<std::uintptr_t>(static_cast<PresentationEnvironment*>(this));
        assert(reinterpret_cast<std::uintptr_t>(&this->application)-base==sizeof(void*));
        assert(reinterpret_cast<std::uintptr_t>(&pressed_keys)-base==3*sizeof(void*));
        assert(reinterpret_cast<std::uintptr_t>(&presentation_parameters)-base==5*sizeof(void*));
    }
    void clear_calls(){calls.clear();released.clear();lookups=0;}
    i32 present(void* p)override{assert(p==&device);calls.push_back(Present);return present_result;}
    void reset_device(void* p,void* params)override{assert(p==&device&&params==&parameters);calls.push_back(Reset);}
    void release_surface(void* p)override{calls.push_back(Release);released.push_back(p);}
#ifdef TH_NATIVE_PLATFORM
    void configure_graphics()override{calls.push_back(Configure);}
#else
#error This resource-free fixture uses the native presentation host contract.
#endif
    void capture_texture(AnmManager& a,i32 file,u32 flags,CaptureRectangle source,CaptureRectangle destination)override{
        assert(&a==manager&&file==7&&flags==0x91);
        assert(equal(source,{1,2,3,4})&&equal(destination,{5,6,7,8}));calls.push_back(CaptureTexture);
    }
    void capture_screen(AnmManager& a,i32 slot,CaptureRectangle source,CaptureRectangle destination)override{
        assert(&a==manager&&slot==4);
        assert(equal(source,{9,10,11,12})&&equal(destination,{13,14,15,16}));calls.push_back(CaptureScreen);
    }
    void create_directory(const char* path)override{assert(std::strcmp(path,"snapshot")==0);calls.push_back(Directory);}
    bool file_exists(const char* path)override{
        char expected[sizeof("snapshot/th000.bmp")];
        assert(lookups<1000);std::snprintf(expected,sizeof(expected),"snapshot/th%03u.bmp",lookups);
        assert(std::strcmp(path,expected)==0);calls.push_back(Exists);
        return lookups++<occupied_names;
    }
    void save_screenshot(ApplicationState& a,const char* path)override{
        assert(&a==application);++saved;std::strcpy(saved_name,path);calls.push_back(Save);
    }
    void queue_captures(){
        requests->texture={4,7,{1,2,3,4},{5,6,7,8},0x91};
        requests->screen_source={9,10,11,12};requests->screen_destination={13,14,15,16};
    }
};

u32 submissions=0;
void submit(Environment& env){
    // Presentation consumes the edge without changing any raw/held/released
    // input words. This also protects the sentinels adjacent to raw_pressed.
    const auto before=input;Presentation{env}.submit();
    assert(std::memcmp(&before,&input,sizeof(input))==0);++submissions;
}
void only_present(Environment& env){
    env.clear_calls();const auto saved=env.saved;submit(env);
    assert(env.calls==std::vector<Call>{Present});assert(env.saved==saved);
}
void edge_and_neighbors(Environment& env){
    assert(reinterpret_cast<std::uintptr_t>(&input.raw_pressed)%alignof(u16)==0);
    assert(reinterpret_cast<std::uintptr_t>(&input.raw_pressed)%alignof(u32)==2);
    // The adjacent raw_repeat/raw_released words must never act as the edge.
    for(u16 sentinel:{u16(0),screenshot_key,u16(0xffff)}){
        input.raw_repeat=sentinel;input.raw_released=sentinel;input.raw_held_frames[0]=sentinel;
        input.raw_pressed=0;only_present(env);
        input.raw_pressed=u16(~screenshot_key);only_present(env);
        input.raw_pressed=screenshot_key;env.clear_calls();const auto saved=env.saved;submit(env);
        assert((env.calls==std::vector<Call>{Present,Directory,Exists,Save}));assert(env.saved==saved+1);
        assert(std::strcmp(env.saved_name,"snapshot/th000.bmp")==0);
    }
    for(u32 bit=1;bit<=0x8000;bit<<=1)if(bit!=screenshot_key){input.raw_pressed=u16(bit);only_present(env);}
}
void held_and_released(Environment& env){
    input={};const auto saved=env.saved;
    input.update_raw(screenshot_key);env.clear_calls();submit(env);assert(env.saved==saved+1);
    for(unsigned frame=0;frame<60;++frame){input.update_raw(screenshot_key);assert(input.raw_pressed==0);only_present(env);}
    input.update_raw(0);assert(input.raw_released==screenshot_key);only_present(env);
    input.update_raw(screenshot_key);env.clear_calls();submit(env);assert(env.saved==saved+2);
    input.update_raw(0);only_present(env);
}
void screenshot_names(Environment& env){
    input.raw_pressed=screenshot_key;env.occupied_names=2;env.clear_calls();auto saved=env.saved;submit(env);
    assert((env.calls==std::vector<Call>{Present,Directory,Exists,Exists,Exists,Save}));
    assert(env.saved==saved+1&&std::strcmp(env.saved_name,"snapshot/th002.bmp")==0);
    env.occupied_names=1000;env.clear_calls();saved=env.saved;submit(env);
    assert(env.saved==saved&&env.lookups==1000&&env.calls.size()==1002);
    assert(env.calls[0]==Present&&env.calls[1]==Directory);
    for(std::size_t i=2;i<env.calls.size();++i)assert(env.calls[i]==Exists);
    env.occupied_names=0;
}
void captures_and_reset(Environment& env){
    input.raw_pressed=0;env.queue_captures();env.clear_calls();submit(env);
    assert((env.calls==std::vector<Call>{Present,CaptureTexture,CaptureScreen}));
    assert(env.requests->texture.target_file==-1&&env.requests->texture.reserved_000==-1);only_present(env);

    env.buffers->textures[0]=&surface_tokens[0];env.buffers->textures[7]=&surface_tokens[1];env.buffers->textures[31]=&surface_tokens[2];
    env.buffers->surfaces[3]=&surface_tokens[1];env.buffers->pixels[5]=&surface_tokens[2];
    env.manager->current_texture=&surface_tokens[0];std::memset(env.manager->cached_draw_state,0,sizeof(env.manager->cached_draw_state));
    env.queue_captures();env.present_result=-1;input.raw_pressed=screenshot_key;env.clear_calls();const auto saved=env.saved;submit(env);
    assert((env.calls==std::vector<Call>{Present,Release,Release,Release,Reset,Configure,CaptureTexture,CaptureScreen,Directory,Exists,Save}));
    assert((env.released==std::vector<void*>{&surface_tokens[0],&surface_tokens[1],&surface_tokens[2]}));
    for(auto* texture:env.buffers->textures)assert(texture==nullptr);
    assert(env.buffers->surfaces[3]==&surface_tokens[1]&&env.buffers->pixels[5]==&surface_tokens[2]);
    assert(reset_frames==2&&env.saved==saved+1&&env.manager->current_texture==nullptr);
    assert(env.manager->cached_draw_state[0]==3&&env.manager->cached_draw_state[1]==255&&env.manager->cached_draw_state[2]==255&&env.manager->cached_draw_state[4]==255);
    assert(env.requests->texture.target_file==-1&&env.requests->texture.reserved_000==-1);
    env.present_result=0;input.update_raw(0);only_present(env);
}
} // namespace

int main(){
    Environment env;edge_and_neighbors(env);held_and_released(env);screenshot_names(env);captures_and_reset(env);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    constexpr const char* variant="multiplayer";
#else
    constexpr const char* variant="ordinary";
#endif
    std::printf("{\"suite\":\"presentation-input\",\"variant\":\"%s\",\"submissions\":%u,\"saved_screenshots\":%u,\"pressed_alignment_mod4\":2,\"input_unchanged\":true,\"passed\":true}\n",variant,submissions,env.saved);
}
