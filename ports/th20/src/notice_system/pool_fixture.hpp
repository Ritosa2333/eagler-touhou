#pragma once
namespace notice_fixture {
inline HRESULT WINAPI get_description(trophy_fixture::Surface*,D3DSURFACE_DESC* out){*out={D3DFMT_A8R8G8B8,D3DRTYPE_SURFACE,0,D3DPOOL_MANAGED,D3DMULTISAMPLE_NONE,0,1088,96};return S_OK;}
inline HRESULT WINAPI lock_surface(trophy_fixture::Surface* surface,D3DLOCKED_RECT* out,const RECT* rectangle,DWORD flags){if(flags)throw std::runtime_error("Notice fixture unexpected lock flags");surface->rectangle=rectangle?*rectangle:RECT{0,0,1088,96};++surface->locks;out->pBits=surface->pixels.data();out->Pitch=4352;return S_OK;}
}
