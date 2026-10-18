#include "text.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/fonts.hpp"
#ifdef TH_SDL3
#include "platform/Fonts.hpp"
#endif
#include <cstring>
#include <cwchar>
namespace th20::source::text {
namespace pe=program_entry;namespace n=th20::recovered;
namespace {
std::int32_t add(std::int32_t a,std::int32_t b) noexcept {auto bits=static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(b);std::memcpy(&a,&bits,4);return a;}
std::int32_t signed_bits(std::uint32_t bits) noexcept {std::int32_t result;std::memcpy(&result,&bits,4);return result;}
}
void Renderer::register_job(const sprite::Vec3& position,Job& job) {
    job.frames=fields_1a1d4[7]?signed_bits(fields_1a1d4[7]):1;
    job.position.x=n::mul32(position.x,pe::window_state.scale);job.position.y=n::mul32(position.y,pe::window_state.scale);
    job.color=color;job.field_650=field_1a1c8;job.field_654=shadow_color;job.layer=signed_bits(fields_1a1d4[6]);
    job.scale_x=scale_x;job.scale_y=scale_y;job.align_x=fields_1a1d4[8];job.align_y=fields_1a1d4[9];job.rotation=rotation;
    job.shadow=fields_1a1d4[2];job.font=fields_1a1d4[7];job.fields_628[2]=fields_1a1d4[5];
    animations[0].base.flags[3]=(animations[0].base.flags[3]&~0xffu)|6;
    scheduler::initialize_link(job.link,reinterpret_cast<scheduler::Node*>(&job));scheduler::append(jobs,job.link);
}
bool rectangles_overlap(std::int32_t x,std::int32_t y,std::int32_t width,std::int32_t height,
    std::int32_t other_x,std::int32_t other_y,std::int32_t other_width,std::int32_t other_height) noexcept {
    return !(add(other_x,other_width)<x || add(x,width)<other_x || add(other_y,other_height)<y || add(y,height)<other_y);
}
Point Renderer::find_atlas_position(std::int32_t width,std::int32_t height) {
    std::lock_guard lock(runtime::shared_locks().slot(18));Point position{0,0};
    for(;;) {
        scheduler::Iterator it(jobs.sentinel.next);
        while(it.current) {
            auto& job=*reinterpret_cast<Job*>(it.current->value);auto* rectangle=job.rectangle;
            if(rectangles_overlap(position.x,position.y,width,height,signed_bits(rectangle[0]),signed_bits(rectangle[1]),add(signed_bits(rectangle[2]),-1),add(signed_bits(rectangle[3]),-1)))break;
            it.advance();
        }
        if(!it.current)return position;
        auto& job=*reinterpret_cast<Job*>(it.current->value);auto* rectangle=job.rectangle;
        const auto right=add(signed_bits(rectangle[0]),signed_bits(rectangle[2]));
        if(add(right,width)<signed_bits(texture_width))position.x=right;
        else {
            position.x=0;position.y=add(add(signed_bits(rectangle[1]),1),signed_bits(rectangle[3]));
            if(signed_bits(texture_height)<add(position.y,height))return {-1,-1};
        }
    }
}
void Renderer::enqueue_task(std::function<void()> function) {
    std::lock_guard lock(runtime::shared_locks().slot(18));pending_tasks.push_back(std::move(function));
}
Point measure_text(const char* cp932,std::int32_t font) {
#ifdef TH_SDL3
    const auto& slot=platform_window::font_slots[font];int cx=0,cy=0;
    web::fonts::measure(cp932,slot.height,slot.family,cx,cy);
    return {add(cx,4),add(cy,4)};
#else
    WCHAR text[258];std::memset(text,0,0x202);MultiByteToWideChar(932,0,cp932,-1,text,256);
    auto dc=GetDC(pe::window_state.window);auto previous=SelectObject(dc,platform_window::fonts[font]);SIZE size;
    GetTextExtentPoint32W(dc,text,static_cast<int>(std::wcslen(text)),&size);size.cx=add(size.cx,4);size.cy=add(size.cy,4);SelectObject(dc,previous);
    // The specimen4156e0 does not call ReleaseDC; preserve its actual lifetime.
    return {size.cx,size.cy};
#endif
}
}
