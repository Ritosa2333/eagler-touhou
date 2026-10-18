#pragma once
#include "Types.hpp"
namespace th08 {
enum class JobResult:i32 {Remove=0,Continue=1,Again=2,Break=3,Exit=4,Error=5,Restart=6};
using JobCallback=JobResult(*)(void*);
using LifetimeCallback=i32(*)(void*);
struct ChainElement {
    i16 priority=0;u16 flags=0;
    JobCallback callback=nullptr;
    LifetimeCallback added=nullptr,deleted=nullptr;
    ChainElement *previous=nullptr,*next=nullptr,*reference=this;
    void* argument=nullptr;
    ~ChainElement();
    void set_callback(JobCallback value) noexcept {callback=value;added=nullptr;deleted=nullptr;}
};
// The game worker executes callbacks serially. Original Windows critical sections
// protected this same ordered list; no OS or CPU state is part of the game class.
class Chain {
public:
    ChainElement calculation,drawing;
    i32 add(ChainElement* element,i32 priority,bool draw=false);
    i32 run(bool draw=false);
    void cut(ChainElement* element);
    void release();
    static ChainElement* create(JobCallback callback);
private:
    bool contains(ChainElement* element) const noexcept;
};
static_assert(sizeof(void*)!=4||sizeof(ChainElement)==32);
}
