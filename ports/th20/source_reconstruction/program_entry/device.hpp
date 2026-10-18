// Device type alias used across the reconstructed modules. The Windows build
// drives the real IDirect3DDevice9; the TH_SDL3 (wasm) build drives the
// semantic th20::web::GraphicsDevice (th20_web/cpp/platform/Graphics.hpp).
// Both names refer to the device object the game frame graph is wired to.
#pragma once

#ifdef TH_SDL3
#include "platform/Graphics.hpp"
namespace th20::source {
using Device = web::GraphicsDevice;
using DeviceTexture = web::Texture;
using DeviceTextureLock = web::TextureLock;
using DevicePrimitive = touhou::graphics::Topology;
}
#else
#include <d3d9.h>
namespace th20::source {
using Device = IDirect3DDevice9;
using DeviceTexture = IDirect3DTexture9;
using DevicePrimitive = D3DPRIMITIVETYPE;
}
#endif
