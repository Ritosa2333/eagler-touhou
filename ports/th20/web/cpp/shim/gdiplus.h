// GDI+ surface for the Emscripten/SDL3 (TH_SDL3) build, used by
// platform_window/snapshot.cpp to save PNG screenshots.
// The C++ class wrappers below mirror the public gdiplus.h binding: they are
// the same inline forwarders over the documented GDI+ flat exports that the
// real SDK header ships. The actual behavior lives in the flat exports, which
// are declared here and must be provided by the wasm host (Phase 3) — until
// then they fail at link time like every other OS import.
#pragma once

#include "windows.h"

extern "C" {

typedef int GpStatus;
#define Ok 0
#define InvalidParameter 2
#define ObjectBusy 5
#define GenericError 1

typedef struct GpImage GpImage;
typedef struct GpBitmap GpBitmap;

typedef struct GdiplusStartupInput {
    UINT32 GdiplusVersion;
    void* DebugEventCallback;
    BOOL SuppressBackgroundThread;
    BOOL SuppressExternalCodecs;
    GdiplusStartupInput(void* callback = nullptr, BOOL suppress_thread = FALSE, BOOL suppress_codecs = FALSE)
        : GdiplusVersion(1), DebugEventCallback(callback),
          SuppressBackgroundThread(suppress_thread), SuppressExternalCodecs(suppress_codecs) {}
} GdiplusStartupInput;

typedef struct GdiplusStartupOutput {} GdiplusStartupOutput;

GpStatus WINAPI GdiplusStartup(ULONG_PTR*, const GdiplusStartupInput*, GdiplusStartupOutput*);
void WINAPI GdiplusShutdown(ULONG_PTR);

typedef struct ImageCodecInfo {
    CLSID Clsid;
    GUID FormatID;
    const WCHAR* CodecName;
    const WCHAR* DllName;
    const WCHAR* FormatDescription;
    const WCHAR* FilenameExtension;
    const WCHAR* MimeType;
    DWORD Flags;
    DWORD Version;
    DWORD SigCount;
    DWORD SigSize;
    const BYTE* SigPattern;
    const BYTE* SigMask;
} ImageCodecInfo;

GpStatus WINAPI GdipGetImageEncodersSize(UINT*, UINT*);
GpStatus WINAPI GdipGetImageEncoders(UINT, UINT, ImageCodecInfo*);
GpStatus WINAPI GdipCreateBitmapFromHBITMAP(HBITMAP, HPALETTE, GpBitmap**);
GpStatus WINAPI GdipSaveImageToFile(GpImage*, const WCHAR*, const CLSID*, const void*);
GpStatus WINAPI GdipDisposeImage(GpImage*);
}

namespace Gdiplus {
using ::ImageCodecInfo;
using ::GdiplusStartupInput;
using ::GdiplusStartupOutput;
using ::GpStatus;
typedef ::GpStatus Status;
inline Status GdiplusStartup(ULONG_PTR* token, const GdiplusStartupInput* input, GdiplusStartupOutput* output) {
    return ::GdiplusStartup(token, input, output);
}
inline void GdiplusShutdown(ULONG_PTR token) { ::GdiplusShutdown(token); }
inline Status GetImageEncodersSize(UINT* count, UINT* size) { return ::GdipGetImageEncodersSize(count, size); }
inline Status GetImageEncoders(UINT count, UINT size, ImageCodecInfo* codecs) {
    return ::GdipGetImageEncoders(count, size, codecs);
}
class Bitmap {
public:
    static Bitmap* FromHBITMAP(HBITMAP bitmap, HPALETTE palette) {
        GpBitmap* native = nullptr;
        if (::GdipCreateBitmapFromHBITMAP(bitmap, palette, &native) != Ok) return nullptr;
        return new Bitmap(native);
    }
    Status Save(const WCHAR* path, const CLSID* encoder, const void* parameters) {
        return ::GdipSaveImageToFile(image_, path, encoder, parameters);
    }
    ~Bitmap() { if (image_) ::GdipDisposeImage(image_); }
    Bitmap(const Bitmap&) = delete;
    Bitmap& operator=(const Bitmap&) = delete;
private:
    explicit Bitmap(GpBitmap* image) : image_(reinterpret_cast<GpImage*>(image)) {}
    GpImage* image_;
};
}
