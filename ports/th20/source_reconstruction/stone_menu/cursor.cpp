#include "cursor.hpp"
#include "../../native_recovered/native_core.hpp"
#include <algorithm>
#include <new>
namespace th20::source::menu {
DequeStorage::DequeStorage():proxy(new Proxy{this,nullptr}),blocks(nullptr),block_count(0),first(0),size(0){}
DequeStorage::~DequeStorage(){
    // StoneMenu does not enqueue cursor histories, but the owning destructor
    // releases every allocated original four-int block if a history is present.
    // No externally live iterator is permitted after its container's lifetime.
    size=0;first=0;
    if(blocks){for(std::uint32_t i=block_count;i>0;--i)::operator delete(blocks[i-1]);::operator delete(blocks);blocks=nullptr;block_count=0;}
    delete proxy;proxy=nullptr;
}
Cursor::Cursor():current(0),previous(0),count(999),minimum(0),wrapping(1){}
bool Cursor::is_excluded(std::int32_t value) const{return std::find(excluded.begin(),excluded.end(),value)!=excluded.end();}
std::int32_t Cursor::select(std::int32_t value){
    if(count==0)current=value;
    else {
        current=value<count?std::max(value,0):recovered::signed_bits(static_cast<unsigned>(count)-1);
        while(is_excluded(current)){current=recovered::signed_bits(static_cast<unsigned>(current)+1);if(count<=current)current=0;}
    }
    return current;
}
std::int32_t Cursor::move(std::int32_t amount){
    if(count<1)return current;
    do {
        current=recovered::signed_bits(static_cast<unsigned>(current)+static_cast<unsigned>(amount));
        while(count<=current){if(!wrapping)current=recovered::signed_bits(static_cast<unsigned>(count)-1);else current=recovered::signed_bits(static_cast<unsigned>(current)-(static_cast<unsigned>(count)-static_cast<unsigned>(minimum)));}
        while(current<minimum){if(!wrapping)current=minimum;else current=recovered::signed_bits((static_cast<unsigned>(count)-static_cast<unsigned>(minimum))+static_cast<unsigned>(current));}
    }while(is_excluded(current));
    return current;
}
}
