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
#include "GfxInterface.hpp"
#include "utils.hpp"

#ifdef TH_ENABLE_THPRAC
struct ImDrawData;
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

class GlesGraphics : public GfxInterface
{
  public:
    static GfxInterface *Init();

    ~GlesGraphics() override
    {
        Exit();
    }

    void Exit();

    void BeginFrame() override;
    void EndFrame() override;
    void SetFogRange(f32 nearPlane, f32 farPlane) override;
    void SetFogColor(ZunColor color) override;
    void SetColorOp(TextureOpComponent component, ColorOp op) override;
    void SetTextureFactor(ZunColor factor) override;
    void SetTransformMatrix(TransformMatrix type, const ZunMatrix &matrix) override;
    void SetTextureFilter() override;

    void GetViewport(ZunViewport &viewport);
    void SetViewport(const ZunViewport &viewport);

    void Enable(Capabilities cap) override;
    void Disable(Capabilities cap) override;

    void SetBlendMode(BlendMode srcMode, BlendMode dstMode);
    void SetDepthMask(bool enable) override;
    void SetDepthFunc(DepthFunc func) override;

    void SetTextureArg(TextureArg arg) override;

    void SetClearDepth(f32 depth) override;
    void SetClearColor(ZunColor color);
    void SetClearColor(f32 r, f32 g, f32 b, f32 a) override;
    void SetAlphaTestRef(u8 ref) override;
    void Clear(u32 clearBits) override;

    GfxTextureHandle CreateTexture() override;
    void BindTexture(GfxTextureHandle handle) override;
    void DeleteTexture(GfxTextureHandle handle) override;
    void SetTextureImage(u32 width, u32 height, PixelFormat fmt, PixelDataType type,
                         const void *data) override;
    void SetTextureSubImage(i32 xoffset, i32 yoffset, i32 width, i32 height,
                            const void *data) override;

    void ReadPixels(i32 x, i32 y, i32 width, i32 height, void *pixels);
    void DrawPrimitive(PrimitiveType type, i32 startVertex, i32 primitiveCount);
    void DrawPrimitiveUP(PrimitiveType type, i32 primitiveCount, const void *vertexData,
                         i32 vertexStride);

    void ToggleVertexAttribute(u8 attr, bool enable) override;
    void SetAttributePointer(VertexAttributeArrays attr, std::size_t stride, void *ptr) override;
    void GetViewport(u32 *viewport) override;
    void GetDepthRange(f32 *depthRange) override;
    void SetViewport(i32 x, i32 y, i32 width, i32 height) override;
    void SetDepthRange(f32 nearPlane, f32 farPlane) override;
    bool HasError() override;
    void SetBlendMode(BlendMode mode) override;
    void ReadPixels(i32 x, i32 y, i32 width, i32 height, const void *pixels) override;
    void Draw(PrimitiveType type, i32 start, i32 count) override;

    void SwapBuffers() override;

#ifdef TH_ENABLE_THPRAC
    void RenderImGui(const ImDrawData *drawData);
#endif

  private:
    SDL_GLContext ctx = nullptr;
    u32 shaderProgram = 0;
    u32 vaos[3][3] = {};
    u32 vbos[3] = {};
    u32 curVbo = 0xFFFFFFFF;
    u32 blitProgram = 0;
    u32 blitVao = 0;
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
    GLuint boundTexture = 0;

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

    GLint u_Model = -1, u_View = -1, u_Proj = -1, u_TextureMatrix = -1;
    GLint u_ScreenSpace = -1, u_Viewport = -1;
    GLint u_UseTexture = -1, u_Texture = -1;
    GLint u_ColorOpRgb = -1, u_ColorOpAlpha = -1, u_TexArg = -1, u_TextureFactor = -1;
    GLint u_AlphaTest = -1, u_AlphaRef = -1;
    GLint u_FogEnabled = -1, u_FogColor = -1, u_FogNear = -1, u_FogFar = -1;
    GLint u_BlitTexture = -1;

    CachedState stateCache;
    void *compatAttribPointers[3] = {};
    size_t compatAttribStrides[3] = {};
    u8 compatEnabledAttributes = 0;

    u64 prevTicks = 0;

    void Flush() {}

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
            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "shader compile error: %s", log);
            utils::DebugPrint("shader compile error: %s\n", log);
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
};
