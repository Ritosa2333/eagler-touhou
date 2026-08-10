#pragma once

#include "ZunColor.hpp"
#include "ZunMath.hpp"
#include "inttypes.hpp"
#include <cstddef>

enum DepthFunc
{
    DEPTH_FUNC_LEQUAL,
    DEPTH_FUNC_ALWAYS
};

// Position is implied, since everything uses is anyway
enum VertexAttributeFlags
{
    VERTEX_ATTR_POSITION = (1 << 2),
    VERTEX_ATTR_TEX_COORD = (1 << 0),
    VERTEX_ATTR_DIFFUSE = (1 << 1),
};

enum VertexAttributeArrays
{
    VERTEX_ARRAY_POSITION,
    VERTEX_ARRAY_TEX_COORD,
    VERTEX_ARRAY_DIFFUSE
};

enum ColorOp
{
    COLOR_OP_MODULATE,
    COLOR_OP_ADD,
    COLOR_OP_REPLACE,
    COLOR_OP_DISABLE
};

enum TextureArg
{
    TEX_ARG_DIFFUSE,
    TEX_ARG_TEXTURE,
    TEX_ARG_TFACTOR,
};

enum TextureOpComponent
{
    COMPONENT_RGB,
    COMPONENT_ALPHA
};

enum TransformMatrix
{
    MATRIX_MODEL,
    MATRIX_VIEW,
    MATRIX_PROJECTION,
    MATRIX_TEXTURE
};
enum BlendMode
{
    BLEND_ALPHA,
    BLEND_INV_SRC_ALPHA,
    BLEND_ONE,
    BLEND_NONE,
};
enum Capabilities
{
    CAPS_BLEND,
    CAPS_DEPTH_TEST,
    CAPS_ALPHA_TEST,
    CAPS_FOG,
};

enum PrimitiveType
{
    PRIM_TRIANGLE_STRIP, // GL_TRIANGLE_STRIP
    PRIM_TRIANGLES,      // GL_TRIANGLES
    PRIM_TRIANGLE_FAN,
};

enum ClearBits
{
    CLEAR_COLOR_BUFFER = 1,
    CLEAR_DEPTH_BUFFER = 2
};

enum PixelFormat
{
    PIXEL_RGBA,
    PIXEL_RGB
};

enum PixelDataType
{
    PIXEL_UNSIGNED_BYTE,
    PIXEL_UNSIGNED_SHORT_5_5_5_1,
    PIXEL_UNSIGNED_SHORT_5_6_5,
    PIXEL_UNSIGNED_SHORT_4_4_4_4
};
struct GfxTextureHandle
{
    u32 id = 0;

    constexpr GfxTextureHandle() = default;
    constexpr GfxTextureHandle(u32 id) : id(id)
    {
    }

    constexpr operator u32() const
    {
        return id;
    }
    constexpr explicit operator bool() const
    {
        return id != 0;
    }
};

// Abstraction layer for graphics API calls
// It's supposed to be compatible with any state based graphics API
struct GfxInterface
{

    // the Init function can have any arbitrary name since you just have to add it to
    // the s_RenderBackends array in GameWindow.cpp, but it should return a pointer to
    // an instance of your implementation of GfxInterface if it succeeds and
    // call the destructor of that instance and return nullptr if it fails

    virtual ~GfxInterface() = default;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    // the destructor is called when the backend fails to initialize or when the program exits,
    // so it should clean up any resources that were allocated by the init function
    // The GL backends delete the SDL window and GL context, for example

    virtual void SetFogRange(f32 nearPlane, f32 farPlane) = 0;
    virtual void SetFogColor(ZunColor color) = 0;
    virtual void ToggleVertexAttribute(u8 attr, bool enable) = 0;
    virtual void SetAttributePointer(VertexAttributeArrays attr, std::size_t stride, void *ptr) = 0;
    virtual void SetColorOp(TextureOpComponent component, ColorOp op) = 0;
    virtual void SetTextureFactor(ZunColor factor) = 0;
    virtual void SetTextureArg(TextureArg arg) {}
    virtual void SetTransformMatrix(TransformMatrix type, const ZunMatrix &matrix) = 0;

    virtual void SetTextureFilter() = 0;

    virtual void GetViewport(u32 *viewport) = 0;
    virtual void GetDepthRange(f32 *depthRange) = 0;
    virtual void SetViewport(i32 x, i32 y, i32 width, i32 height) = 0;
    virtual void SetDepthRange(f32 nearPlane, f32 farPlane) = 0;

    virtual void Enable(Capabilities cap) = 0;

    virtual bool HasError() = 0;

    virtual void SetBlendMode(BlendMode mode) = 0;
    virtual void SetDepthMask(bool enable) = 0;
    virtual void SetDepthFunc(DepthFunc func) = 0;

    virtual void SetClearDepth(f32 depth) = 0;
    virtual void SetAlphaTestRef(u8 ref) {}
    virtual void SetClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
    virtual void Clear(u32 clearBits) = 0;

    virtual GfxTextureHandle CreateTexture() = 0;
    virtual void BindTexture(GfxTextureHandle handle) = 0;
    virtual void DeleteTexture(GfxTextureHandle handle) = 0;
    virtual void SetTextureImage(u32 width, u32 height, PixelFormat fmt, PixelDataType type, const void *data) = 0;

    // should read as unsigned byte RGB
    virtual void SetTextureSubImage(i32 xoffset, i32 yoffset, i32 width, i32 height, const void *data) = 0;

    virtual void ReadPixels(i32 x, i32 y, i32 width, i32 height, const void *pixels) = 0;

    virtual void Draw(PrimitiveType type, i32 start, i32 count) = 0;
    virtual void SwapBuffers() = 0;
};
