#pragma once
#include "../game/Types.hpp"
#include <cstdlib>
namespace th10::browser {
// Ownership tracking for native components whose recovered routines return
// several separately allocated blocks, including partially failed loads.
struct MemoryPool {
    struct Block {void* bytes;Block* next;};
    Block* first=nullptr;
    u32 count=0;
    bool owns(const void* bytes) const {for(auto* block=first;block;block=block->next)if(block->bytes==bytes)return true;return false;}
    ~MemoryPool(){clear();}
    void* adopt(void* bytes){if(!bytes)return nullptr;auto* block=static_cast<Block*>(std::malloc(sizeof(Block)));if(!block){std::free(bytes);return nullptr;}*block={bytes,first};first=block;++count;return bytes;}
    u8* allocate(u32 bytes){return static_cast<u8*>(adopt(std::calloc(1,bytes?bytes:1)));}
    void release(void* bytes){if(!bytes)return;for(auto** next=&first;*next;next=&(*next)->next){auto* block=*next;if(block->bytes==bytes){*next=block->next;std::free(bytes);std::free(block);--count;return;}}__builtin_trap();}
    void clear(const void* keep1=nullptr,const void* keep2=nullptr){for(auto** next=&first;*next;){auto* block=*next;if(block->bytes==keep1||block->bytes==keep2){next=&block->next;continue;}*next=block->next;std::free(block->bytes);std::free(block);--count;}}
};
}
