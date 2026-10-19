#include "laser.hpp"
namespace th20::source::laser {
void Controller::erase_all(std::int32_t first,std::int32_t second){ //4c9490
    scheduler::Iterator iterator(active.sentinel.next);
    for(;iterator.current;iterator.advance()){
        auto& value=*reinterpret_cast<Laser*>(iterator.current->value);
        if(value.state!=1)value.erase(first,second);
    }
}
}
