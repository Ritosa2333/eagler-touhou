#pragma once
// COM and PMR observation boundaries exist only in the original-CPU oracle.
namespace trophy_fixture {
struct Resource:std::pmr::memory_resource {
    std::vector<std::array<std::size_t,3>> events;
    void* do_allocate(std::size_t size,std::size_t alignment)override{events.push_back({1,size,alignment});return std::pmr::new_delete_resource()->allocate(size,alignment);}
    void do_deallocate(void* p,std::size_t size,std::size_t alignment)override{events.push_back({0,size,alignment});std::pmr::new_delete_resource()->deallocate(p,size,alignment);}
    bool do_is_equal(const std::pmr::memory_resource& other)const noexcept override{return this==&other;}
};
struct Surface {
    void** table;
    std::vector<std::uint8_t> pixels;
    RECT rectangle{};
    std::vector<std::vector<std::uint8_t>> submissions;
    std::vector<RECT> rectangles;
    unsigned locks=0,unlocks=0,releases=0;
    void reset(){std::fill(pixels.begin(),pixels.end(),0x97);submissions.clear();rectangles.clear();locks=unlocks=releases=0;rectangle={};}
};
struct Texture {void** table;Surface* surface;unsigned gets=0;};
inline HRESULT WINAPI get_surface(Texture* texture,UINT level,IDirect3DSurface9** out){if(level)throw std::runtime_error("Trophy fixture unexpected texture level");++texture->gets;*out=reinterpret_cast<IDirect3DSurface9*>(texture->surface);return S_OK;}
inline HRESULT WINAPI lock_surface(Surface* surface,D3DLOCKED_RECT* out,const RECT* rectangle,DWORD flags){if(flags)throw std::runtime_error("Trophy fixture unexpected lock flags");surface->rectangle=*rectangle;++surface->locks;out->pBits=surface->pixels.data();out->Pitch=4352;return S_OK;}
inline HRESULT WINAPI unlock_surface(Surface* surface){++surface->unlocks;surface->submissions.push_back(surface->pixels);surface->rectangles.push_back(surface->rectangle);return S_OK;}
inline ULONG WINAPI release_surface(Surface* surface){return ++surface->releases;}
}
