#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../runtime_core/worker.hpp"
#include <algorithm>
#ifndef TH_SDL3
namespace Gdiplus {using std::min;using std::max;} // Windows SDK header expects unqualified min/max.
#include <gdiplus.h>
#else
#include "platform/PngWriter.hpp"
#include "platform/Files.hpp"
#include "platform/Cp932.hpp"
#endif
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <new>
#include <string>
#include <vector>

namespace th20::source::platform_window {
namespace pe=program_entry;
namespace {
std::wstring wide_path(const char* path) {
#ifdef TH_SDL3
    std::wstring result(std::strlen(path)+1,L'\0');
    result.resize(web::cp932::to_utf16(path,result.data(),result.size()));
    return result;
#else
    const int count=MultiByteToWideChar(932,0,path,-1,nullptr,0);
    std::wstring result(count,'\0');
    MultiByteToWideChar(932,0,path,-1,result.data(),count);
    if(!result.empty()) result.pop_back();return result;
#endif
}
std::string narrow_path(const std::filesystem::path& path) {
#ifdef TH_SDL3
    // libc++ paths already store the byte form the snapshot writer expects.
    return path.string();
#else
    const auto& wide=path.native();
    const int count=WideCharToMultiByte(932,0,wide.c_str(),-1,nullptr,0,nullptr,nullptr);
    std::string result(count,'\0');
    WideCharToMultiByte(932,0,wide.c_str(),-1,result.data(),count,nullptr,nullptr);
    if(!result.empty()) result.pop_back();return result;
#endif
}
void launch_snapshot_worker(GraphicsStatePrefix& graphics) { // 40b1d0 specialization
    auto& worker=*std::launder(reinterpret_cast<runtime::Worker*>(graphics.worker_storage[1]));
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));
    {
        std::lock_guard<std::recursive_mutex> nested(runtime::shared_locks().slot(6));
        if(worker.thread.joinable()) worker.thread.detach(); // 40bc60, does not set close flag
    }
    worker.close_requested.store(false,std::memory_order_seq_cst);
    worker.thread=TH20_WORKER_THREAD("snapshot",[](int){save_snapshot_worker();},0);
}
}
#ifndef TH_SDL3
bool find_image_encoder(const wchar_t* mime_type,CLSID& identifier) {
    UINT count=0,size=0;Gdiplus::GetImageEncodersSize(&count,&size);
    if(!size) return false;
    auto* codecs=static_cast<Gdiplus::ImageCodecInfo*>(runtime::allocate_bytes(size));
    if(!codecs) return false;
    Gdiplus::GetImageEncoders(count,size,codecs);
    for(UINT index=0;index<count;++index) {
        if(std::wcscmp(codecs[index].MimeType,mime_type)==0) {
            identifier=codecs[index].Clsid;runtime::release_bytes(codecs);return true;
        }
    }
    runtime::release_bytes(codecs);return false;
}
void save_bitmap_png(HBITMAP bitmap,const char* cp932_path) {
    Gdiplus::GdiplusStartupInput startup(nullptr,FALSE,FALSE);
    ULONG_PTR token;Gdiplus::GdiplusStartup(&token,&startup,nullptr);
    auto* image=Gdiplus::Bitmap::FromHBITMAP(bitmap,nullptr);
    wchar_t path[262];std::memset(path,0,0x20a);
    MultiByteToWideChar(932,0,cp932_path,-1,path,260);
    CLSID encoder;
    if(find_image_encoder(L"image/png",encoder)) {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
        image->Save(path,&encoder,nullptr);
    }
    delete image;Gdiplus::GdiplusShutdown(token);
}
#endif
void save_snapshot_worker() {
    auto& graphics=pe::graphics_state;auto& window=pe::window_state;
    auto* source=graphics.snapshot_pixels;
    const int x_offset=graphics.viewports[2].offset_x*4;
    const int y_offset=graphics.viewports[2].offset_y*graphics.snapshot_pitch;
#ifdef TH_SDL3
    // The GDI+ path wrote RGB through a 24bpp DIB; the PNG writer takes RGBA
    // directly from the captured backbuffer bytes (alpha is not stored).
    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(window.scaled_width)*window.scaled_height*4);
    for(int row=window.scaled_height-1;row>=0;--row) {
        const auto* pixel=source+graphics.snapshot_pitch*row+y_offset+x_offset;
        auto* out=rgba.data()+static_cast<std::size_t>(row)*window.scaled_width*4;
        for(int column=0;column<window.scaled_width;++column) {
            out[0]=pixel[2];out[1]=pixel[1];out[2]=pixel[0];out[3]=255;
            pixel+=4;out+=4;
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
        std::vector<std::uint8_t> png;
        if(web::encode_png(png,rgba.data(),
            static_cast<std::uint32_t>(window.scaled_width),static_cast<std::uint32_t>(window.scaled_height)))
            web::files::write_all(graphics.snapshot_path,png.data(),static_cast<std::uint32_t>(png.size()));
    }
#else
    auto* info=static_cast<BITMAPINFO*>(runtime::allocate_bytes(sizeof(BITMAPINFO))); // 4d8740
    std::memset(info,0,sizeof(BITMAPINFO));
    info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth=window.scaled_width;info->bmiHeader.biHeight=window.scaled_height;
    info->bmiHeader.biPlanes=1;info->bmiHeader.biBitCount=24;info->bmiHeader.biCompression=BI_RGB;
    // The original constructs an unused BITMAPFILEHEADER here; it is never read.
    HDC screen=GetDC(nullptr);void* bits;
    HBITMAP bitmap=CreateDIBSection(screen,info,DIB_RGB_COLORS,&bits,nullptr,0);
    auto* destination=static_cast<std::uint8_t*>(bits);
    for(int row=window.scaled_height-1;row>=0;--row) {
        const auto* pixel=source+graphics.snapshot_pitch*row+y_offset+x_offset;
        for(int column=0;column<window.scaled_width;++column) {
            destination[2]=pixel[2];destination[1]=pixel[1];destination[0]=pixel[0];
            pixel+=4;destination+=3;
        }
    }
    save_bitmap_png(bitmap,graphics.snapshot_path);DeleteObject(bitmap);
    if(info) runtime::release_bytes(info);
    ReleaseDC(nullptr,screen);
#endif
    if(graphics.snapshot_pixels) {runtime::release_bytes(graphics.snapshot_pixels);graphics.snapshot_pixels=nullptr;}
}
void capture_snapshot(GraphicsStatePrefix& graphics,const char* cp932_path) {
    join_graphics_worker(graphics,1);
#ifdef TH_SDL3
    auto* surface=graphics.device->back_buffer();
    if(!surface) return;
#else
    IDirect3DSurface9* surface=nullptr;
    graphics.device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&surface);
    if(!surface) return;
#endif
    strcpy_s(graphics.snapshot_path,sizeof(graphics.snapshot_path),cp932_path);
    const auto format=graphics.presentation.BackBufferFormat;
    if(format==D3DFMT_A8R8G8B8 || format==D3DFMT_X8R8G8B8) {
#ifdef TH_SDL3
        const auto locked=graphics.device->lock(surface);
        if(!locked.pixels) return;
        auto& global=pe::graphics_state;
        const auto bytes=static_cast<std::uint32_t>(locked.pitch)*static_cast<std::uint32_t>(pe::window_state.viewport_height);
        global.snapshot_pixels=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes));
        std::memcpy(global.snapshot_pixels,locked.pixels,bytes);global.snapshot_pitch=locked.pitch;
        graphics.device->unlock(surface);
        launch_snapshot_worker(graphics);
#else
        D3DLOCKED_RECT locked;surface->LockRect(&locked,nullptr,0);
        auto& global=pe::graphics_state;
        const auto bytes=static_cast<std::uint32_t>(locked.Pitch)*static_cast<std::uint32_t>(pe::window_state.viewport_height);
        global.snapshot_pixels=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes));
        std::memcpy(global.snapshot_pixels,locked.pBits,bytes);global.snapshot_pitch=locked.Pitch;
        surface->UnlockRect();launch_snapshot_worker(graphics);
#endif
    } else if(format==D3DFMT_R5G6B5) {
        runtime::log_printf(pe::log_buffer,data::snapshot_16bit_unsupported);
    } else {
        runtime::log_printf(pe::log_buffer,"error : snapShotScreen\n");
        return; // 4de1d9 really skips Release for this unsupported-format branch.
    }
#ifndef TH_SDL3
    surface->Release();
#endif
}
void before_present() {
    process_surface_copies(*pe::sprite_controller);
    if(!input || !pressed(*input,0x40000) || (pe::window_state.flags&(1u<<6))) return;
    const auto directory=std::filesystem::path(wide_path(pe::window_state.user_data_directory))/L"snapshot";
    std::filesystem::create_directory(directory);
    for(int index=0;index<10000;++index) {
        wchar_t filename[128];swprintf_s(filename,L"th20_%.3d.png",index);
        const auto path=directory/filename;bool exists;
        {
            std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(2));
            exists=std::filesystem::exists(path);
        }
        if(!exists) {const auto narrow=narrow_path(path);capture_snapshot(pe::graphics_state,narrow.c_str());break;}
    }
}
}
