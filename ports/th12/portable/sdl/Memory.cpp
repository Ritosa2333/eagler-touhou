// Small-block pool behind the global operator new / delete. The core and the
// front-end allocate thousands of small objects a frame in busy patterns
// (hash map and list nodes, shared_ptr control blocks, short vectors), and
// newlib's malloc on the Switch takes a lock and searches its bins for every
// one. Blocks of up to 256 bytes come from per-thread free lists by size
// instead; larger ones go to malloc. Pooled memory is reused, never returned.
// Every other new/delete form in libstdc++ forwards to these two, and the
// aligned forms stay paired with each other.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

namespace {
constexpr std::size_t Header=16,Step=16,Classes=16,ChunkBytes=64*1024; // classes of 16..256 bytes
constexpr std::uint32_t Large=0xffffffffu;
struct Block {Block* next;};
struct Pool {Block* lists[Classes]{};char* chunk=nullptr;std::size_t left=0;};
thread_local Pool pool;
// The header in front of each block holds its size class (or Large).
inline std::uint32_t& tag(void* payload){return *reinterpret_cast<std::uint32_t*>(static_cast<char*>(payload)-Header);}
void* allocate(std::size_t n){
    if(n>Classes*Step){
        char* p=static_cast<char*>(std::malloc(n+Header));if(!p)return nullptr;
        *reinterpret_cast<std::uint32_t*>(p)=Large;return p+Header;
    }
    const std::size_t c=n?(n-1)/Step:0;Pool& p=pool;
    if(Block* b=p.lists[c]){p.lists[c]=b->next;return b;}
    const std::size_t bytes=Header+(c+1)*Step;
    if(p.left<bytes){p.chunk=static_cast<char*>(std::malloc(ChunkBytes));if(!p.chunk){p.left=0;return nullptr;}p.left=ChunkBytes;}
    char* at=p.chunk;p.chunk+=bytes;p.left-=bytes;
    *reinterpret_cast<std::uint32_t*>(at)=std::uint32_t(c);return at+Header;
}
void release(void* payload){
    if(!payload)return;const std::uint32_t c=tag(payload);
    if(c==Large){std::free(static_cast<char*>(payload)-Header);return;}
    Block* b=static_cast<Block*>(payload);Pool& p=pool;b->next=p.lists[c];p.lists[c]=b;
}
}

void* operator new(std::size_t n){if(void* p=allocate(n))return p;throw std::bad_alloc();}
void operator delete(void* p)noexcept{release(p);}
void operator delete(void* p,std::size_t)noexcept{release(p);}
