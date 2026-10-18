#include "menu_support.hpp"
#include <new>
#include <stdexcept>
namespace th20::source::pause {
namespace {
void grow(menu::DequeStorage& d){ //4c59c0, MSVC four-element-block deque map
    std::uint32_t count=d.block_count?d.block_count:1;
    do{if(count>0x1fffffffu)throw std::length_error("deque<T> too long");count*=2;}while(count<8||count<=d.block_count);
    auto** map=static_cast<std::int32_t**>(::operator new(count*sizeof(*d.blocks)));for(std::uint32_t i=0;i<count;++i)map[i]=nullptr;
    const auto offset=d.first/4;
    for(std::uint32_t i=0;i<d.block_count;++i){const auto target=i<offset?i+d.block_count:i;map[target]=d.blocks[i];}
    ::operator delete(d.blocks);d.blocks=map;d.block_count=count;
}
void push(menu::DequeStorage& d,int value){ //4c5160
    if((d.first+d.size)%4==0&&d.block_count<=(d.size+4)/4)grow(d);
    d.first&=d.block_count*4-1;const auto offset=d.first+d.size;const auto block=(offset/4)&(d.block_count-1);
    if(!d.blocks[block])d.blocks[block]=static_cast<std::int32_t*>(::operator new(4*sizeof(std::int32_t)));
    d.blocks[block][offset%4]=value;++d.size;
}
int pop(menu::DequeStorage& d){const auto offset=d.first+d.size-1;const int value=d.blocks[(offset/4)&(d.block_count-1)][offset%4];if(--d.size==0)d.first=0;return value;}
}
void save_cursor(menu::Cursor& cursor){push(cursor.history,cursor.current);push(cursor.secondary_history,cursor.count);cursor.excluded.clear();}
void restore_cursor(menu::Cursor& cursor){if(cursor.history.size){cursor.current=pop(cursor.history);cursor.count=pop(cursor.secondary_history);}cursor.excluded.clear();}
void exclude(menu::Cursor& cursor,int value){if(cursor.is_excluded(value))return;cursor.excluded.push_back(value);while(cursor.is_excluded(cursor.current)){cursor.current=recovered::signed_bits(static_cast<unsigned>(cursor.current)+1);if(cursor.count<=cursor.current)cursor.current=0;}}
}
