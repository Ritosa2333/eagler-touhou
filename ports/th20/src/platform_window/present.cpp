#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
#include "../platform_services/services.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include <emmintrin.h>

namespace th20::source::platform_window {
namespace pe=program_entry;
namespace {
double add(double a,double b) {return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double sub(double a,double b) {return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double mul(double a,double b) {return _mm_cvtsd_f64(_mm_mul_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double div(double a,double b) {return _mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(a),_mm_set_sd(b)));}
int trunc(double a) {return _mm_cvttsd_si32(_mm_set_sd(a));}
int increment(int a) {return static_cast<int>(static_cast<unsigned>(a)+1);}
double frame_period() {return div(data::value_0056c468,data::value_0056c470);}
double clock(WindowStatePrefix& w) {return platform::read_clock(w);}
auto& delay_counter(WindowStatePrefix& w) {return w.repeat[w.input_latch];} // 41ca50
#ifndef TH_SDL3
void recover_after_present_failure(bool require_successful_reset) {
    auto& g=pe::graphics_state;
    release_render_surfaces();
    pe::unrecovered::fn_0041dbe0(pe::sprite_controller);
    const HRESULT result=g.device->Reset(&g.presentation);
    if(require_successful_reset && result!=D3D_OK) return;
    pe::unrecovered::fn_0041d9f0(pe::sprite_controller);
    acquire_render_surfaces(g);initialize_render_state();g.reset_countdown=2;
}
#endif
void present_frame() { // Present cannot fail on the semantic device.
#ifdef TH_SDL3
    pe::graphics_state.device->present();
#else
    auto& g=pe::graphics_state;
    if(FAILED(g.device->Present(nullptr,nullptr,nullptr,nullptr))) recover_after_present_failure(true);
#endif
}
void present_frame_lenient() {
#ifdef TH_SDL3
    pe::graphics_state.device->present();
#else
    auto& g=pe::graphics_state;
    if(FAILED(g.device->Present(nullptr,nullptr,nullptr,nullptr))) recover_after_present_failure(false);
#endif
}
}
void after_present() {
    if(unrecovered::scheduler_object_005c4a00) unrecovered::update_frame_statistics(unrecovered::scheduler_object_005c4a00);
    unrecovered::update_post_frame_game_state();
}
void finish_unlimited_frame(WindowStatePrefix& w) {
    auto& g=pe::graphics_state;
    w.current_time=clock(w);
    w.sleep_budget=trunc(sub(mul(sub(add(frame_period(),w.previous_draw_time),w.current_time),data::value_0056cd88),data::value_0056cd78));
    auto& delay=delay_counter(w);
    if(delay.second<w.sleep_budget) {
        delay.elapsed=increment(delay.elapsed);
        if(delay.elapsed>=15) {if(delay.second<delay.first) delay.second=increment(delay.second);delay.elapsed=0;}
    } else {delay.second=w.sleep_budget<0?0:w.sleep_budget;delay.elapsed=0;}
    if(w.sleep_budget<0) w.sleep_budget=0;
    const double now=clock(w);
    if(now<w.current_draw_time) w.current_draw_time=now;
    if(sub(now,w.current_draw_time)>=data::value_0056cd60) {
        if(delay_counter(w).second>0) --delay_counter(w).second;
    }
#ifndef TH_SDL3
    else if(!g.presentation.Windowed) {
        D3DRASTER_STATUS raster{};
        while(sub(clock(w),w.current_draw_time)<data::value_0056cd58) Sleep(1);
        raster.InVBlank=FALSE;
        do {if(g.device->GetRasterStatus(0,&raster)!=D3D_OK) break;} while(!raster.InVBlank);
    }
#endif
    w.current_draw_time=clock(w);
    before_present();
    present_frame();
    after_present();clock(w); // original has an extra, intentionally discarded clock read
    if(delay_counter(w).second>0) {
#ifdef TH_SDL3
        web::time::sleep(static_cast<std::uint32_t>(delay_counter(w).second));
#else
        timeBeginPeriod(1);Sleep(static_cast<DWORD>(delay_counter(w).second));timeEndPeriod(1);
#endif
    }
    w.previous_draw_time=clock(w);
}
void finish_timed_frame(WindowStatePrefix&) {
    before_present();
    present_frame_lenient();
    after_present();
}
void finish_present_paced_frame(WindowStatePrefix& w) {
    auto& g=pe::graphics_state;
    w.current_time=clock(w);
    const auto elapsed=sub(w.current_time,w.previous_draw_time);
    if(g.configuration.presentation_mode==1 && elapsed<frame_period() && elapsed>data::value_0056cd50) {
        const int remaining=trunc(sub(mul(sub(add(frame_period(),w.previous_draw_time),w.current_time),data::value_0056cd88),data::value_0056cd80));
#ifdef TH_SDL3
        if(remaining>0) web::time::sleep(static_cast<std::uint32_t>(remaining));
#else
        if(remaining>0) Sleep(static_cast<DWORD>(remaining));
#endif
    }
    w.current_draw_time=clock(w);before_present();
    present_frame_lenient();
    w.previous_draw_time=clock(w);after_present();
}
}
namespace th20::source::program_entry::unrecovered {
void fn_004dda60(GraphicsStatePrefix& g) {platform_window::disable_fog(g);}
void fn_004193e0(WindowStatePrefix& w) {platform_window::finish_unlimited_frame(w);}
void fn_004199a0(WindowStatePrefix& w) {platform_window::finish_timed_frame(w);}
void fn_00419a50(WindowStatePrefix& w) {platform_window::finish_present_paced_frame(w);}
}
