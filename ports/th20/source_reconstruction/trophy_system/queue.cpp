#include "trophy.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::trophy {
namespace {
template<class T>T* allocate(Queue& q,std::size_t count){return static_cast<T*>(q.allocator->allocate(sizeof(T)*count,alignof(T)));}
void grow(Queue& q){
    const auto old=q.map_size;std::uint32_t size=old?old:1;
    while(size-old<1||size<8){if(size>0x0fffffffu)throw std::length_error("deque too long");size*=2;}
    auto** next=allocate<std::int32_t*>(q,size);std::memset(next,0,size*4);
    const auto first=q.offset/4;
    for(std::uint32_t i=0;i<old;++i)next[i<first?(i+old)%size:i]=q.map[i];
    if(q.map)q.allocator->deallocate(q.map,old*4,4);q.map=next;q.map_size=size;
}
}
Queue::Queue():allocator(std::pmr::get_default_resource()),proxy(nullptr),map(nullptr),map_size(0),offset(0),count(0){proxy=allocate<Proxy>(*this,1);proxy->container=&proxy;proxy->iterator=nullptr;}
Queue::~Queue(){for(unsigned i=0;i<map_size;++i)if(map[i])allocator->deallocate(map[i],16,4);if(map)allocator->deallocate(map,map_size*4,4);map=nullptr;map_size=offset=count=0;allocator->deallocate(proxy,sizeof(Proxy),4);proxy=nullptr;}
void Queue::push(std::int32_t value){
    if((offset+count)%4==0&&map_size<=(count+4)/4)grow(*this);
    offset&=map_size*4-1;const auto position=offset+count,block=(position/4)&(map_size-1);
    if(!map[block])map[block]=allocate<std::int32_t>(*this,4);
    map[block][position%4]=value;++count;
}
std::int32_t Queue::pop_front(){
    if(!count)return 0;const auto result=map[(offset/4)&(map_size-1)][offset%4];
    if(--count==0)offset=0;else ++offset;return result;
}
}
