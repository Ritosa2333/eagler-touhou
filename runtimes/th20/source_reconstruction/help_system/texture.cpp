#include "help.hpp"
#include "../sprite_renderer/texture_edges.hpp"
#include <system_error>
#include <stdexcept>
#include <cstring>
#ifdef TH_SDL3
#include "platform/TextureLoader.hpp"
#endif
namespace th20::source::help {
#ifndef TH_SDL3
namespace {
struct Library {
    HMODULE module;
    using Load=HRESULT(WINAPI*)(IDirect3DSurface9*,const PALETTEENTRY*,const RECT*,const void*,UINT,const RECT*,DWORD,D3DCOLOR,void*);
    Load load;
    Library():module(LoadLibraryW(L"d3dx9_43.dll")){if(!module)throw std::system_error(GetLastError(),std::system_category(),"DirectX image library");load=reinterpret_cast<Load>(GetProcAddress(module,"D3DXLoadSurfaceFromFileInMemory"));if(!load)throw std::runtime_error("D3DXLoadSurfaceFromFileInMemory unavailable");}
    ~Library(){FreeLibrary(module);}
};
Library& library(){static Library value;return value;}
}
#endif
void replace_texture_image(sprite::TextureRecord& record,const std::uint8_t* bytes,std::uint32_t size,int format,bool container){
    if(format<0||format>8)throw std::out_of_range("Original texture format table index");
    record.unknown_08=size;record.flags&=~3u;
    if(container){std::uint32_t offset;std::memcpy(&offset,bytes+0x1c,4);bytes+=0x10+offset;}
#ifdef TH_SDL3
    auto& device=web::shared_device();
    web::load_surface_from_memory(device,record.texture,nullptr,bytes,size,nullptr,1);
#else
    IDirect3DSurface9* surface=nullptr;record.texture->GetSurfaceLevel(0,&surface);
    library().load(surface,nullptr,nullptr,bytes,size,nullptr,1,0,nullptr);surface->Release();
#endif
    sprite::repair_transparent_texels(*record.texture);record.bytes_per_pixel=4;
}
void clear_texture(sprite::TextureRecord& record){
#ifdef TH_SDL3
    auto& device=web::shared_device();
    const auto locked=device.lock(record.texture);
    if(locked.pixels){std::memset(locked.pixels,0,static_cast<std::uint32_t>(locked.pitch)*device.texture_height(record.texture));device.unlock(record.texture);}
#else
    IDirect3DSurface9* surface=nullptr;record.texture->GetSurfaceLevel(0,&surface);
    if(surface){D3DSURFACE_DESC description;surface->GetDesc(&description);D3DLOCKED_RECT locked;surface->LockRect(&locked,nullptr,0);std::memset(locked.pBits,0,static_cast<std::uint32_t>(locked.Pitch)*description.Height);surface->UnlockRect();surface->Release();}
#endif
}
}
