#pragma once

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define GLES_SILENCE_DEPRECATION
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl3.h>
#endif
#else
#include <GLES3/gl3.h>
#endif

#include <SDL3/SDL_video.h>

#include "AnmManager.hpp"
#include "ZunGraphics.hpp"

#ifdef TH_ENABLE_THPRAC
struct ImDrawData;
#endif

#ifndef __EMSCRIPTEN__
struct GlesNativePerfCounters
{
    u64 drawCalls;
    u64 drawVertices;
    u64 bufferSubDataCalls;
    u64 bufferSubDataBytes;
    u64 bufferDataCalls;
    u64 bufferDataBytes;
    u64 texSubImageCalls;
    u64 texSubImageBytes;
    u64 bindTextureCalls;
    u64 uniformCalls;
    u64 swapCalls;
};

GlesNativePerfCounters GlesTakeNativePerfCounters();
#endif

struct CachedState
{
    void Invalidate()
    {
        dirtyMatrix = true;
        dirtyFog = true;
        dirtyViewport = true;
        dirtyColorOp = true;
        dirtyTexArg = true;
        dirtyTexFactor = true;
        dirtyAlphaTest = true;

        currentStride = -1;
        currentVao = 0xFFFFFFFF;
    }
    bool dirtyMatrix = true;
    bool dirtyFog = true;
    bool dirtyViewport = true;
    bool dirtyColorOp = true;
    bool dirtyTexArg = true;
    bool dirtyTexFactor = true;
    bool dirtyAlphaTest = true;

    i32 currentStride = -1;
    GLuint currentVao = 0xFFFFFFFF;
};

class GlesGraphics : public ZunGraphics
{
  public:
    static ZunGraphics *Init();

    ~GlesGraphics() override
    {
        Exit();
    }

    void Exit() override;

    RendererType GetType() override
    {
        return RENDERER_OPENGLES;
    }

    void BeginFrame() override;
    void EndFrame() override;
    void SetFogRange(f32 nearPlane, f32 farPlane) override;
    void SetFogColor(ZunColor color) override;
    void SetColorOp(TextureOpComponent component, ColorOp op) override;
    void SetTextureFactor(ZunColor factor) override;
    void SetTransformMatrix(TransformMatrix type, const ZunMatrix &matrix) override;
    void SetTextureFilter() override;

    void GetViewport(ZunViewport &viewport) override;
    void SetViewport(const ZunViewport &viewport) override;

    void Enable(Capabilities cap) override;
    void Disable(Capabilities cap) override;

    void SetBlendMode(BlendMode srcMode, BlendMode dstMode) override;
    void SetDepthMask(bool enable) override;
    void SetDepthFunc(DepthFunc func) override;

    void SetTextureArg(TextureArg arg) override;

    void SetClearDepth(f32 depth) override;
    void SetClearColor(ZunColor color) override;
    void SetAlphaTestRef(u8 ref) override;
    void Clear(u32 clearBits) override;

    GfxTextureHandle CreateTexture() override;
    void BindTexture(GfxTextureHandle handle) override;
    void DeleteTexture(GfxTextureHandle handle) override;
    void SetTextureImage(u32 width, u32 height, PixelFormat fmt, PixelDataType type,
                         const void *data) override;
    void SetTextureSubImage(i32 xoffset, i32 yoffset, i32 width, i32 height,
                            const void *data) override;

    void ReadPixels(i32 x, i32 y, i32 width, i32 height, void *pixels) override;
    void DrawPrimitive(PrimitiveType type, i32 startVertex, i32 primitiveCount) override;
    void DrawPrimitiveUP(PrimitiveType type, i32 primitiveCount, const void *vertexData,
                         i32 vertexStride) override;

    void SwapBuffers() override;

#ifdef TH_ENABLE_THPRAC
    void RenderImGui(const ImDrawData *drawData);
#endif

  private:
    SDL_GLContext ctx;
    u32 shaderProgram;
    u32 vaos[3][3];
    u32 vbos[3];
    u32 curVbo = 0xFFFFFFFF;
    u32 blitProgram;
    u32 blitVao;
    u8 alphaRef = 0;
    TextureArg texArg = TEX_ARG_DIFFUSE;
    ZunColor textureFactor = {0xFFFFFFFF};
    ColorOp colorOpRgb = COLOR_OP_MODULATE;
    ColorOp colorOpAlpha = COLOR_OP_MODULATE;

    static constexpr size_t VBO_CAPACITY = 1024 * 1024;
    size_t vboOffset = 0;

    GLuint defaultFbo = 0;
    GLuint fbo = 0;
    GLuint fboColor = 0;
    GLuint fboDepth = 0;

    GLuint unitQuadVao = 0;
    GLuint unitQuadVbo = 0;

#ifdef TH_ENABLE_THPRAC
    GLuint imguiProgram = 0;
    GLuint imguiVao = 0;
    GLuint imguiVbo = 0;
    GLuint imguiEbo = 0;
    GLuint imguiFontTexture = 0;
    GLint imguiProjMtx = -1;
    GLint imguiTexture = -1;
#endif

    ZunMatrix transforms[4];
    ZunViewport viewport;
    bool fogEnabled = false;
    f32 fogNear = 0.0f;
    f32 fogFar = 1.0f;
    ZunColor fogColor = {0};
    ZunColor clearColor = {0};
    bool blendEnabled = false;
    bool depthTestEnabled = false;
    bool alphaTestEnabled = false;
    bool depthMaskEnabled = true;

    GLint u_Model, u_View, u_Proj, u_TextureMatrix;
    GLint u_ScreenSpace, u_Viewport;
    GLint u_UseTexture, u_Texture;
    GLint u_ColorOpRgb, u_ColorOpAlpha, u_TexArg, u_TextureFactor;
    GLint u_AlphaTest, u_AlphaRef;
    GLint u_FogEnabled, u_FogColor, u_FogNear, u_FogFar;
    GLint u_BlitTexture = -1;

    CachedState stateCache;

    u64 prevTicks = 0;

    void Flush()
    {
        if (g_AnmManager && g_AnmManager->spritesToDraw != 0)
        {
            g_AnmManager->Flush();
        }
    }

    static GLuint CompileShader(GLenum type, const char *source)
    {
        u32 shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (!isCompiled)
        {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            Supervisor::DebugPrint("shader compile error: %s\n", log);
            return 0;
        }
        return shader;
    }
};
