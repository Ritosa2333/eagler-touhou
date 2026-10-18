#pragma once
#include <d3d9.h>
#include "../program_entry/device.hpp"
namespace th20::source::platform_window::directx {
struct Vector3 {float x,y,z;};
// These are normal DirectX SDK library calls used by the original program.
// They are not calls into the game's executable or retained game machine code.
// TH_SDL3 replaces them with the equivalent float math (platform/GraphicsMath).
void look_at(D3DMATRIX&,const Vector3& eye,const Vector3& target,const Vector3& up);
void perspective(D3DMATRIX&,float fov,float aspect,float near_plane,float far_plane);
#ifndef TH_SDL3
HRESULT copy_surface(IDirect3DSurface9* destination,const RECT&,IDirect3DSurface9* source,const RECT&);
#else
HRESULT copy_surface(DeviceTexture* destination,const RECT&,DeviceTexture* source,const RECT&);
#endif
}
