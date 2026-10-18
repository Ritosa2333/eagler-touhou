#include "texture_load.hpp"
#include "texture_edges.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <system_error>
#ifdef TH_SDL3
#include "platform/TextureLoader.hpp"
#endif
namespace th20::source::sprite {
namespace {
struct ImageInfo {std::uint32_t width,height,depth,mip_levels,format,resource_type,file_format;};
#ifndef TH_SDL3
struct TextureLibrary {
    HMODULE module;
    using Create=HRESULT(WINAPI*)(IDirect3DDevice9*,UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DTexture9**);
    using Load=HRESULT(WINAPI*)(IDirect3DSurface9*,const PALETTEENTRY*,const RECT*,const void*,UINT,const RECT*,DWORD,D3DCOLOR,ImageInfo*);
    Create create;Load load;
    TextureLibrary():module(LoadLibraryW(L"d3dx9_43.dll")) {
        if(!module)throw std::system_error(GetLastError(),std::system_category(),"DirectX texture library");
        create=reinterpret_cast<Create>(GetProcAddress(module,"D3DXCreateTexture"));
        load=reinterpret_cast<Load>(GetProcAddress(module,"D3DXLoadSurfaceFromFileInMemory"));
        if(!create||!load){FreeLibrary(module);throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX texture exports");}
    }
    ~TextureLibrary(){FreeLibrary(module);}
};
TextureLibrary& library(){static TextureLibrary api;return api;}
#endif
std::int32_t scale_dimension(std::int32_t dimension,float scale) {
    return th20::recovered::truncate32(th20::recovered::mul32(th20::recovered::mul32(th20::recovered::int_float(dimension),scale),.5f));
}
template<class T>T read(const std::uint8_t* memory) {T value;std::memcpy(&value,memory,sizeof(T));return value;}
bool downsample(const TextureRecord& record,float scale) {
#ifdef TH_SDL3
    // Logical game coordinates do not describe the output pixel density.
    // Keep the original atlas texels at every window size; sprite descriptors
    // already derive logical extents independently from the texture dimensions.
    return false;
#else
    return reinterpret_cast<const std::uint8_t*>(record.header)[0x22]!=0&&scale<2.f;
#endif
}
}
int create_embedded_texture(TextureRecord& record,const std::uint8_t* thtx,UINT format,
                            std::int32_t width,std::int32_t height,TextureContext& context) {
    if(format>8)throw std::out_of_range("Texture format exceeds original table");
    record.flags&=~3u;
    RECT destination{0,0,read<std::int16_t>(thtx+8),read<std::int16_t>(thtx+10)};
    const bool is_downsampled=downsample(record,context.screen_scale);
    if(is_downsampled) {
        destination.right=scale_dimension(destination.right,context.screen_scale);
        destination.bottom=scale_dimension(destination.bottom,context.screen_scale);
        width=scale_dimension(width,context.screen_scale);height=scale_dimension(height,context.screen_scale);
    }
#ifdef TH_SDL3
    record.texture=context.device.create_texture(width,height,touhou::graphics::PixelFormat::Bgra8,false);
    if(!record.texture)return -1;
    // Same filter selection as the original D3DX call: point copy, or the
    // triangle+mirror downsample for tagged textures on small screens.
    web::load_surface_from_memory(context.device,record.texture,reinterpret_cast<const std::int32_t*>(&destination.left),thtx+16,
        read<std::uint32_t>(thtx+12),nullptr,is_downsampled?0x30004u:1u);
    context.device.discard_cpu_copy(record.texture);
#else
    IDirect3DSurface9* surface=nullptr;
    const auto status=library().create(&context.device,width,height,1,0,format?D3DFMT_A8R8G8B8:D3DFMT_UNKNOWN,D3DPOOL_MANAGED,&record.texture);
    if(status!=S_OK){if(surface)surface->Release();return -1;}
    record.texture->GetSurfaceLevel(0,&surface);
    ImageInfo information;
    library().load(surface,nullptr,&destination,thtx+16,read<std::uint32_t>(thtx+12),nullptr,is_downsampled?0x30004u:1u,0,&information);
    if(surface)surface->Release();
#endif
    record.bytes_per_pixel=4;
    return th20::recovered::signed_bits(std::uint32_t(width)*std::uint32_t(height)*4u);
}
int create_external_texture(TextureRecord& record,UINT format,std::int32_t width,std::int32_t height,
                            std::int32_t x,std::int32_t y,std::int32_t original_width,
                            std::int32_t original_height,TextureContext& context) {
    if(format>8)throw std::out_of_range("Texture format exceeds original table");
    record.flags&=~3u;
    RECT source{x,y,th20::recovered::signed_bits(std::uint32_t(x)+std::uint32_t(std::min(width,original_width))),
                    th20::recovered::signed_bits(std::uint32_t(y)+std::uint32_t(std::min(height,original_height)))};
    const bool is_downsampled=downsample(record,context.screen_scale);
    if(is_downsampled){width=scale_dimension(width,context.screen_scale);height=scale_dimension(height,context.screen_scale);}
    RECT destination{0,0,width,height};
#ifdef TH_SDL3
    record.texture=context.device.create_texture(width,height,touhou::graphics::PixelFormat::Bgra8,false);
    if(!record.texture)return -1;
    const auto loaded=web::load_surface_from_memory(context.device,record.texture,reinterpret_cast<const std::int32_t*>(&destination.left),
        reinterpret_cast<const std::uint8_t*>(record.unknown_04),record.unknown_08,reinterpret_cast<const std::int32_t*>(&source.left),
        is_downsampled?0x30004u:1u);
    if(!loaded)return -1;
#else
    library().create(&context.device,width,height,1,0,format?D3DFMT_A8R8G8B8:D3DFMT_UNKNOWN,D3DPOOL_MANAGED,&record.texture);
    IDirect3DSurface9* surface=nullptr;record.texture->GetSurfaceLevel(0,&surface);
    const auto status=library().load(surface,nullptr,&destination,reinterpret_cast<const void*>(record.unknown_04),record.unknown_08,&source,is_downsampled?0x30004u:1u,0,nullptr);
    if(surface)surface->Release();if(status!=S_OK)return -1;
#endif
    repair_transparent_texels(*record.texture);record.bytes_per_pixel=4;
#ifdef TH_SDL3
    context.device.discard_cpu_copy(record.texture);
    // WebGL keeps the decoded texture in the graphics host. The compressed
    // source has no further use after the upload and can be reclaimed now.
    if(record.unknown_04){runtime::release_bytes(reinterpret_cast<void*>(record.unknown_04));record.unknown_04=0;record.unknown_08=0;}
#endif
    return th20::recovered::signed_bits(std::uint32_t(width)*std::uint32_t(height)*4u);
}
}
