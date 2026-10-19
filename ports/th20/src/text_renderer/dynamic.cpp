#include "text.hpp"
#include "raster.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/binding.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <emmintrin.h>
namespace th20::source::text {
namespace s=sprite;namespace n=th20::recovered;
namespace {float divide(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}}
void initialize_animation_sprite(s::AnimationFile& file,s::Animation& animation,std::int32_t index) {
    s::reset_animation_state(animation);s::clear_animation_suffix(animation);animation.base.fields_10_28[3]=file.id;s::assign_animation_sprite(file,animation,index);
}
void set_text_rectangle(s::Controller& controller,s::Animation& animation,std::int32_t x,std::int32_t y,std::int32_t width,std::int32_t height) {
    auto& data=s::current_sprite(controller,animation);auto& base=animation.base;const auto right=n::signed_bits(std::uint32_t(x)+std::uint32_t(width)),bottom=n::signed_bits(std::uint32_t(y)+std::uint32_t(height));
    base.vectors_378[0].x=base.vectors_378[2].x=divide(n::int_float(x),data.texture_extent_20);
    base.vectors_378[1].x=base.vectors_378[3].x=divide(n::int_float(right),data.texture_extent_20);
    base.vectors_378[0].y=base.vectors_378[1].y=divide(n::int_float(y),data.texture_extent_1c);
    //Original470b80 uses+20, unlike the top edge's+1c.
    base.vectors_378[2].y=base.vectors_378[3].y=divide(n::int_float(bottom),data.texture_extent_20);
    base.vector_398={divide(n::int_float(width),data.texture_extent_20),divide(n::int_float(height),data.texture_extent_1c)};
    base.vector_70={n::int_float(width),n::int_float(height)};
    constexpr s::Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};base.matrix_3b8=base.matrix_3f8=identity;
    base.matrix_3b8.elements[0]=divide(base.vector_70.x,256.f);base.matrix_3b8.elements[5]=divide(base.vector_70.y,256.f);
    base.matrix_3f8.elements[0]=n::mul32(divide(base.vector_70.x,data.texture_extent_20),data.scale_50);
    base.matrix_3f8.elements[5]=n::mul32(divide(base.vector_70.y,data.texture_extent_1c),data.scale_54);
    animation.matrix_57c=base.matrix_3b8;base.fields_3a0[2]=x;base.fields_3a0[3]=y;base.fields_3a0[4]=right;base.fields_3a0[5]=bottom;
}
SIZE write_animation_text(s::Controller& controller,s::Animation& animation,std::uint32_t foreground,std::uint32_t background,
    std::int32_t font,std::int32_t x,std::int32_t spacing,std::uint8_t* ready,std::function<void()> completion,const char* format,...) {
    std::lock_guard lock(runtime::shared_locks().slot(9));char formatted[1280];va_list arguments;va_start(arguments,format);vsprintf_s(formatted,sizeof(formatted),format,arguments);va_end(arguments);
    auto& data=s::current_sprite(controller,animation);RECT rectangle;std::memcpy(&rectangle,animation.base.fields_3a0+2,sizeof(rectangle));
    animation.base.flags[0]|=0x10000u;const bool outline=(animation.base.flags[1]&0x400u)==0;
    auto* texture=s::texture(controller,data.texture_id);
    return queue_text(rectangle,n::signed_bits(std::uint32_t(x)<<1),foreground,outline?background:0,formatted,*texture,font,n::signed_bits(std::uint32_t(spacing)<<1),outline,ready,std::move(completion));
}
void Renderer::write_text(const s::Vec3& position,const char* format,...) {
    std::lock_guard lock(runtime::shared_locks().slot(18));char formatted[256];va_list arguments;va_start(arguments,format);vsprintf_s(formatted,sizeof(formatted),format,arguments);va_end(arguments);write_text_literal(position,formatted);
}
void Renderer::write_text_literal(const s::Vec3& position,const char* string) {
    std::lock_guard lock(runtime::shared_locks().slot(18));std::pmr::string value(string);Job* found=nullptr;auto& controller=*program_entry::sprite_controller;
    for(scheduler::Iterator it(jobs.sentinel.next);it.current;it.advance()){
        auto& job=*reinterpret_cast<Job*>(it.current->value);if(job.text!=value||job.fields_628[1]!=fields_1a1d4[4])continue;found=&job;
        if(!job.frames){job.position.x=n::mul32(position.x,program_entry::window_state.scale);job.position.y=n::mul32(position.y,program_entry::window_state.scale);job.color=color;job.field_654=shadow_color;job.field_650=field_1a1c8;job.frames=1;}
        else {auto* clone=create_job();clone->fields_628[1]=fields_1a1d4[4];clone->fields_628[3]=job.fields_628[3];clone->fields_628[4]=job.fields_628[4];std::memcpy(clone->rectangle,job.rectangle,sizeof(job.rectangle));
            initialize_animation_sprite(*program_entry::graphics_state.surface_animation,clone->animation,0x34);set_text_rectangle(controller,clone->animation,clone->rectangle[0],clone->rectangle[1],clone->rectangle[2],clone->rectangle[3]);clone->text=value;clone->ready=1;clone->external_ready=&job.ready;register_job(position,*clone);}
        break;
    }
    if(found)return;auto* job=create_job();register_job(position,*job);initialize_animation_sprite(*program_entry::graphics_state.surface_animation,job->animation,0x1b);job->fields_628[1]=fields_1a1d4[4];Point extent;
    if(!job->fields_628[2])extent=measure_text(value.c_str(),fields_1a1d4[4]);
    else {extent=measure_text("\x89\xbc",fields_1a1d4[4]);extent.x=n::signed_bits(std::uint32_t(extent.x)+std::uint32_t(value.size())*job->fields_628[2]);}
    job->fields_628[3]=job->rectangle[2]=extent.x;job->fields_628[4]=job->rectangle[3]=extent.y;
    const auto location=find_atlas_position(job->rectangle[2],job->rectangle[3]);if(location.x<0){destroy_job(job);return;}
    job->rectangle[0]=location.x;job->rectangle[1]=location.y;set_text_rectangle(controller,job->animation,location.x,location.y,job->rectangle[2],job->rectangle[3]);job->ready=0;job->canceled.store(false);job->text=value;
    enqueue_task([job,text=value]{
        std::lock_guard lock(runtime::shared_locks().slot(18));if(job->canceled.load()){destroy_job(job);return;}
        //46a4b0 ->40e5e0 is the specimen's literal empty completion function.
        write_animation_text(*program_entry::sprite_controller,job->animation,job->field_650,job->field_654,job->fields_628[1],0,job->fields_628[2],&job->ready,[]{},text.c_str());
    });
}
}
