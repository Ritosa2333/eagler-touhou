#include "shot_data.hpp"
#include "../archive/resource_manager.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <cstring>
namespace th20::source::player_entity {
void* load_shot_data(const char* name){
    auto bytes=resources::read(name);if(!bytes)return nullptr;
    auto* result=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes->size()));if(!result)return nullptr;
    std::memcpy(result,bytes->data(),bytes->size());
    try{relocate_shot_data({result,bytes->size()});}catch(...){runtime::release_bytes(result);return nullptr;}
    return result;
}
}
