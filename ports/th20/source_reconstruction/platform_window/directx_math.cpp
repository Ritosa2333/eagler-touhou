#include "directx_math.hpp"
#include <system_error>
#ifdef TH_SDL3
#include "platform/GraphicsMath.hpp"
#endif
namespace th20::source::platform_window::directx {
#ifndef TH_SDL3
namespace {
struct MathLibrary {
    HMODULE module;
    using LookAt=D3DMATRIX*(WINAPI*)(D3DMATRIX*,const Vector3*,const Vector3*,const Vector3*);
    using Perspective=D3DMATRIX*(WINAPI*)(D3DMATRIX*,float,float,float,float);
    using LoadSurface=HRESULT(WINAPI*)(IDirect3DSurface9*,const PALETTEENTRY*,const RECT*,IDirect3DSurface9*,const PALETTEENTRY*,const RECT*,DWORD,D3DCOLOR);
    LoadSurface load_surface;
    LookAt look_at;Perspective perspective;
    MathLibrary():module(LoadLibraryW(L"d3dx9_43.dll")) {
        if(!module) throw std::system_error(static_cast<int>(GetLastError()),std::system_category(),"DirectX d3dx9_43.dll");
        look_at=reinterpret_cast<LookAt>(GetProcAddress(module,"D3DXMatrixLookAtLH"));
        perspective=reinterpret_cast<Perspective>(GetProcAddress(module,"D3DXMatrixPerspectiveFovLH"));
        load_surface=reinterpret_cast<LoadSurface>(GetProcAddress(module,"D3DXLoadSurfaceFromSurface"));
        if(!look_at || !perspective || !load_surface) {
            FreeLibrary(module);throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX matrix exports");
        }
    }
    ~MathLibrary() {FreeLibrary(module);}
};
MathLibrary& library() {static MathLibrary api;return api;}
}
#endif
void look_at(D3DMATRIX& matrix,const Vector3& eye,const Vector3& target,const Vector3& up) {
#ifdef TH_SDL3
    web::math::look_at(&matrix._11,
        *reinterpret_cast<const web::math::Vec3*>(&eye),
        *reinterpret_cast<const web::math::Vec3*>(&target),
        *reinterpret_cast<const web::math::Vec3*>(&up));
#else
    library().look_at(&matrix,&eye,&target,&up);
#endif
}
void perspective(D3DMATRIX& matrix,float fov,float aspect,float near_plane,float far_plane) {
#ifdef TH_SDL3
    web::math::perspective_fov_lh(&matrix._11,fov,aspect,near_plane,far_plane);
#else
    library().perspective(&matrix,fov,aspect,near_plane,far_plane);
#endif
}
#ifdef TH_SDL3
HRESULT copy_surface(DeviceTexture* destination,const RECT& destination_rect,DeviceTexture* source,const RECT& source_rect) {
    // RECT is four contiguous 32-bit fields, matching the i32 rectangle shape.
    return web::shared_device().copy_surface(destination,reinterpret_cast<const std::int32_t*>(&destination_rect.left),
        source,reinterpret_cast<const std::int32_t*>(&source_rect.left))?0:(HRESULT)0x8876086cL;
}
#else
HRESULT copy_surface(IDirect3DSurface9* destination,const RECT& destination_rect,IDirect3DSurface9* source,const RECT& source_rect) {
    return library().load_surface(destination,nullptr,&destination_rect,source,nullptr,&source_rect,2,0);
}
#endif
}
