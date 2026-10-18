#include "text.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
#include <new>
namespace th20::source::text {
OwnedAnimation::OwnedAnimation(){sprite::construct_animation(*this);}
OwnedAnimation::~OwnedAnimation(){sprite::destroy_animation_contents(*this);}
Job::Job():position{},frames(0),layer(0),fields_628{},rectangle{},color(0),field_650(0),field_654(0),
    scale_x(0),scale_y(0),align_x(1),align_y(1),field_668(0),rotation(0),shadow(0),font(0),ready(0),external_ready(nullptr),canceled(false){}
Job::~Job() {scheduler::unlink(link);}
void destroy_job(Job* job) {
    if(!job)return;job->~Job();std::lock_guard lock(runtime::shared_locks().slot(1));::operator delete(job);
}
Job* create_job() {
    auto* memory=::operator new(sizeof(Job),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Job));
    try{return ::new(memory)Job;}catch(...){::operator delete(memory);throw;}
}
}
