#pragma once
#include "AnmExecutor.hpp"
#include "AnmRenderer.hpp"
#include "Presentation.hpp"
#include "PresentationVisual.hpp"

namespace th08 {
struct AsciiString {
    char text[64]{};
    Vec3 position;
    u32 color=0;
    float scale_x=0,scale_y=0;
    i32 selected=0,gui=0;
};
struct AsciiPopup {
    u8 text[12]{};
    Vec3 position;
    u32 color=0;
    Timer timer;
    float scale_x=0,scale_y=0;
    u8 in_use=0,characters=0,reserved[2]{};
    u32 unused=0;
};
struct PauseMenuState { u32 state=0; i32 frames=0; AnmVm sprites[10],background; };
struct RetryMenuState { u32 state=0; i32 frames=0; AnmVm sprites[6],background; };
struct AsciiState {
    AnmVm large_text,small_score_text,popup_text,gauge,human_icon,youkai_icon,cursor,percentage,border;
    AnmVm boss_markers[4];
    i32 boss_states[4]{};
    AsciiString strings[256];
    i32 string_count=0;
    u32 color=0xffffffff;
    float scale_x=1,scale_y=1;
    i32 gui=0,selected=0,gauge_interrupt=0,space_width=13;
    u32 frame=0;
    AnmLoaded* ascii=nullptr;
    AnmLoaded* capture=nullptr;
    i32 next_score=0,next_player=0,next_time=0,unused=0;
    PauseMenuState pause;
    RetryMenuState retry;
    AnmVm demo;
    AsciiPopup score_popups[723],time_popups[128];
    float blindness_radius=0;
    u32 blindness_color=0;
    AnmVm blindness;
};
static_assert(sizeof(AsciiString)==0x60 && sizeof(AsciiPopup)==0x38);
static_assert(sizeof(AsciiState)==0x171b0);
static_assert(offsetof(AsciiState,strings)==0x2264 && offsetof(AsciiState,score_popups)==0xb4dc);

// Values owned by the game and supplied to the display layer each frame.
struct AsciiContext {
    Vec3 player;
    Vec2 arcade_origin{32,16},arcade_size{384,448};
    i32 gauge=0,point_value=0;
    i16 human_limit=-10000,youkai_limit=10000;
    i16 human_effects=-8000,youkai_effects=8000,human_tint=-2000,youkai_tint=2000;
    bool paused=false,retry=false,freeze_popups=false,demo=false,software_texturing=false,fog_disabled=false;
    AnmLoaded* effects=nullptr;
};
struct OverlayRect {float left,top,right,bottom;};
struct AsciiOverlay {
    virtual ~AsciiOverlay()=default;
    virtual void begin(bool disable_fog)=0;
    virtual void rectangle(const OverlayRect& rect,u32 color)=0;
};

class AsciiManager {
public:
    AsciiState state;
    AsciiManager(AnmExecutor& executor,AnmRenderer& renderer,AsciiOverlay& overlay)
        :executor(executor),renderer(renderer),overlay(overlay){}
    void reset();
    void initialize_vms(const AsciiContext& context);
    void snapshot_presentation(const AsciiContext& context);
    void set_gauge_interrupt(i32 interrupt);
    void tick_popups(const AsciiContext& context,const FrameTiming& timing);
    void tick_vms(bool demo);
    bool add_string(const Vec3& position,const char* text,bool software_texturing);
    i32 add_format(const Vec3& position,bool software_texturing,const char* format,...);
    void create_score(const Vec3& position,i32 number,u32 color,const AsciiContext& context,bool player=false);
    void create_time(const Vec3& position,i32 number,i32 multiplier,u32 color,const AsciiContext& context,bool familiar=false);
    void draw_strings(const AsciiContext& context);
    void draw_overlays(const AsciiContext& context);
    void draw_percentage(const Vec3& position,i32 percentage,u32 color);
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_presentation_sample(uintptr_t object)const;
#endif
private:
    struct PopupPresentation {Vec3 position{};i32 timer=-2;u8 in_use=0,characters=0;};
    struct PresentationState {
        Vec3 boss_markers[4]{},player{};
        presentation::VisualSample boss_marker_vm[4],gauge_vm,human_icon_vm,youkai_icon_vm,cursor_vm,percentage_vm,border_vm;
        i32 gauge=0;float blindness_radius=0;u32 blindness_color=0;bool valid=false;
    } presentation_state;
    PopupPresentation score_popup_previous[723]{};
#if defined(TH_PRESENTATION_AUDIT)
    u32 score_popup_generation[723]{},time_popup_generation[128]{};
#endif
    presentation::SnapshotMarker presentation_marker,popup_marker;
    AnmExecutor& executor;
    AnmRenderer& renderer;
    AsciiOverlay& overlay;
    void start(AnmVm& vm,AnmLoaded& file,i32 script);
    void set_sprite(AnmVm& vm,i32 sprite,bool initialize=false);
    void direct_sprite(AnmVm& vm,i32 sprite);
};
}
