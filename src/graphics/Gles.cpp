#include "Gles.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <new>
#include <vector>

#define COLOR_R(c) (((c) >> 16) & 0xff)
#define COLOR_G(c) (((c) >> 8) & 0xff)
#define COLOR_B(c) ((c) & 0xff)
#define COLOR_A(c) (((c) >> 24) & 0xff)

#include "AnmManager.hpp"
#include "GameWindow.hpp"
#include "Supervisor.hpp"

#ifdef TH_ENABLE_THPRAC
#include "ThpracImGui.hpp"
#endif

#ifdef USING_GL
#define GLSL_VERSION "#version 330 core\n"
#define GLSL_PRECISION
#else
#define GLSL_VERSION "#version 300 es\n"
#define GLSL_PRECISION "precision mediump float;\n"
#endif

// clang-format off
const char *vertexShaderSource =
    GLSL_VERSION
    "uniform mat4 u_Model;\n"
    "uniform mat4 u_View;\n"
    "uniform mat4 u_Proj;\n"
    "uniform mat4 u_TextureMatrix;\n"
    "uniform bool u_ScreenSpace;\n"
    "uniform vec4 u_Viewport;\n"
    "\n"
    "layout(location = 0) in vec3 a_Position;\n"
    "layout(location = 1) in vec4 a_Color;\n"
    "layout(location = 2) in vec2 a_TexCoord;\n"
    "\n"
    "out vec4 v_Color;\n"
    "out vec2 v_TexCoord;\n"
    "out float v_FogFragCoord;\n"
    "\n"
    "void main() {\n"
    // TH06's ColorData is already laid out as RGBA. TH07 stores a packed
    // D3D-style BGRA ZunColor in the vertex and therefore needs .bgra here;
    // applying that swizzle to TH06 swaps red and blue a second time.
    "    v_Color = a_Color;\n"
    "    if (u_ScreenSpace) {\n"
    "        float x = (a_Position.x - u_Viewport.x) / u_Viewport.z * 2.0 - 1.0;\n"
    "        float y = 1.0 - (a_Position.y - u_Viewport.y) / u_Viewport.w * 2.0;\n"
    "        gl_Position = vec4(x, y, a_Position.z, 1.0);\n"
    "        v_TexCoord = a_TexCoord;\n"
    "        v_FogFragCoord = a_Position.z;\n"
    "    } else {\n"
    "        vec4 worldPos = u_Model * vec4(a_Position, 1.0);\n"
    "        vec4 viewPos = u_View * worldPos;\n"
    "        gl_Position = u_Proj * viewPos;\n"
    "        v_TexCoord = (u_TextureMatrix * vec4(a_TexCoord, 1.0, 0.0)).xy;\n"
    "        v_FogFragCoord = length(viewPos.xyz);\n"
    "    }\n"
    "}\n";

const char *fragmentShaderSource =
    GLSL_VERSION
    GLSL_PRECISION
    "\n"
    "in vec4 v_Color;\n"
    "in vec2 v_TexCoord;\n"
    "in float v_FogFragCoord;\n"
    "\n"
    "uniform sampler2D u_Texture;\n"
    "uniform bool u_UseTexture;\n"
    "uniform int u_ColorOpRgb;\n"
    "uniform int u_ColorOpAlpha;\n"
    "uniform int u_TexArg;\n"
    "uniform vec4 u_TextureFactor;\n"
    "uniform bool u_AlphaTest;\n"
    "uniform float u_AlphaRef;\n"
    "uniform bool u_FogEnabled;\n"
    "uniform vec4 u_FogColor;\n"
    "uniform float u_FogNear;\n"
    "uniform float u_FogFar;\n"
    "\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main() {\n"
    "    vec4 texColor = vec4(1.0);\n"
    "    if (u_UseTexture) {\n"
    "        texColor = texture(u_Texture, v_TexCoord);\n"
    "    }\n"
    "    \n"
    "    vec4 argColor = v_Color;\n"
    "    if (u_TexArg == 1) { // TEXTURE\n"
    "        argColor = vec4(1.0);\n"
    "    } else if (u_TexArg == 2) { // TFACTOR\n"
    "        argColor = u_TextureFactor;\n"
    "    }\n"
    "    \n"
    "    vec4 finalColor = v_Color;\n"
    "    \n"
    "    if (u_UseTexture) {\n"
    "        if (u_ColorOpRgb == 0) finalColor.rgb = texColor.rgb * argColor.rgb;\n"
    "        else if (u_ColorOpRgb == 1) finalColor.rgb = min(texColor.rgb + argColor.rgb, "
    "vec3(1.0));\n"
    "        else if (u_ColorOpRgb == 2) finalColor.rgb = texColor.rgb;\n"
    "        else if (u_ColorOpRgb == 3) finalColor.rgb = argColor.rgb;\n"
    "        \n"
    "        if (u_ColorOpAlpha == 0) finalColor.a = texColor.a * argColor.a;\n"
    "        else if (u_ColorOpAlpha == 1) finalColor.a = min(texColor.a + argColor.a, 1.0);\n"
    "        else if (u_ColorOpAlpha == 2) finalColor.a = texColor.a;\n"
    "        else if (u_ColorOpAlpha == 3) finalColor.a = argColor.a;\n"
    "    } else {\n"
    "        finalColor = argColor;\n"
    "    }\n"
    "    \n"
    "    if (u_AlphaTest && finalColor.a < u_AlphaRef) {\n"
    "        discard;\n"
    "    }\n"
    "    \n"
    "    if (u_FogEnabled) {\n"
    "        float f = (u_FogFar - v_FogFragCoord) / (u_FogFar - u_FogNear);\n"
    "        f = clamp(f, 0.0, 1.0);\n"
    "        finalColor.rgb = mix(u_FogColor.rgb, finalColor.rgb, f);\n"
    "    }\n"
    "    \n"
    "    FragColor = finalColor;\n"
    "}\n";

const char *blitVSSource =
    GLSL_VERSION
    "out vec2 v_TexCoord;\n"
    "void main() {\n"
    "    float x = float((gl_VertexID & 1) << 2) - 1.0;\n"
    "    float y = float((gl_VertexID & 2) << 1) - 1.0;\n"
    "    v_TexCoord = vec2((x + 1.0) * 0.5, (y + 1.0) * 0.5);\n"
    "    gl_Position = vec4(x, y, 0.0, 1.0);\n"
    "}\n";

const char *blitFSSource =
    GLSL_VERSION
    GLSL_PRECISION
    "in vec2 v_TexCoord;\n"
    "uniform sampler2D u_Texture;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = texture(u_Texture, v_TexCoord);\n"
    "}\n";

#ifdef TH_ENABLE_THPRAC
const char *imguiVertexShaderSource =
    GLSL_VERSION
    GLSL_PRECISION
    "uniform mat4 u_ProjMtx;\n"
    "layout(location = 0) in vec2 Position;\n"
    "layout(location = 1) in vec2 UV;\n"
    "layout(location = 2) in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main() {\n"
    "    Frag_UV = UV;\n"
    // The pinned thprac imconfig.h stores ImGui colors as BGRA bytes so its
    // old D3D backends can consume them directly. GLES reads vertex bytes in
    // memory order, therefore this swizzle is local to the ImGui backend and
    // must not be applied to TH06 game vertices.
    "    Frag_Color = Color.bgra;\n"
    "    gl_Position = u_ProjMtx * vec4(Position.xy, 0.0, 1.0);\n"
    "}\n";

const char *imguiFragmentShaderSource =
    GLSL_VERSION
    GLSL_PRECISION
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "uniform sampler2D Texture;\n"
    "out vec4 Out_Color;\n"
    "void main() {\n"
    "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
    "}\n";
#endif
// clang-format on

GfxInterface *GlesGraphics::Init()
{
    GlesGraphics *gfx = new (std::nothrow) GlesGraphics();
    if (gfx == nullptr)
    {
        return nullptr;
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(g_GameWindow.window);
    if (!ctx)
    {
        delete gfx;
        utils::DebugPrint("gles renderer create failed: %s\n", SDL_GetError());
        return nullptr;
    }
    gfx->ctx = ctx;

    if (!SDL_GL_MakeCurrent(g_GameWindow.window, ctx))
    {
        utils::DebugPrint("gles renderer context bind failed: %s\n", SDL_GetError());
        delete gfx;
        return nullptr;
    }

    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    SDL_Log("GLES renderer: %s; GLSL: %s", version != nullptr ? reinterpret_cast<const char *>(version) : "unknown",
            glslVersion != nullptr ? reinterpret_cast<const char *>(glslVersion) : "unknown");

    glGenFramebuffers(1, &gfx->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gfx->fbo);

    glGenTextures(1, &gfx->fboColor);
    glBindTexture(GL_TEXTURE_2D, gfx->fboColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 640, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gfx->fboColor, 0);

    glGenRenderbuffers(1, &gfx->fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gfx->fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 640, 480);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              gfx->fboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: GLES framebuffer is incomplete");
        delete gfx;
        return nullptr;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, gfx->fbo);

    RenderVertexInfo unitQuadData[4] = {{{-128.0f, -128.0f, 0.0f}, {0.0f, 0.0f}},
                                        {{128.0f, -128.0f, 0.0f}, {1.0f, 0.0f}},
                                        {{-128.0f, 128.0f, 0.0f}, {0.0f, 1.0f}},
                                        {{128.0f, 128.0f, 0.0f}, {1.0f, 1.0f}}};

    glGenVertexArrays(1, &gfx->unitQuadVao);
    glGenBuffers(1, &gfx->unitQuadVbo);
    glBindVertexArray(gfx->unitQuadVao);
    glBindBuffer(GL_ARRAY_BUFFER, gfx->unitQuadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitQuadData), unitQuadData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderVertexInfo),
                          (void *)offsetof(RenderVertexInfo, position));
    glDisableVertexAttribArray(1);
    glVertexAttrib4f(1, 1.0f, 1.0f, 1.0f, 1.0f);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertexInfo),
                          (void *)offsetof(RenderVertexInfo, textureUV));
    glBindVertexArray(0);

#ifdef __EMSCRIPTEN__
    // Browser presentation is driven by requestAnimationFrame. Keep the
    // previously validated Emscripten swap behavior; native strict-VSync work
    // must not change this path.
    g_PresentationVsyncEnabled = SDL_GL_SetSwapInterval(-1) || SDL_GL_SetSwapInterval(1);
    if (!g_PresentationVsyncEnabled)
    {
        utils::DebugPrint("SDL_GL_SetSwapInterval failed: %s\n", SDL_GetError());
    }
#else
    g_PresentationVsyncEnabled = SDL_GL_SetSwapInterval(1);
    if (!g_PresentationVsyncEnabled)
    {
        // The native GameWindow also caps presentation to the current display
        // refresh rate, but strict interval 1 is preferred because adaptive
        // VSync (-1) may tear on missed refresh deadlines.
        utils::DebugPrint("SDL_GL_SetSwapInterval(1) failed: %s\n", SDL_GetError());
    }
#endif

    u32 vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    u32 fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (vertexShader == 0 || fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        delete gfx;
        return nullptr;
    }

    gfx->shaderProgram = glCreateProgram();
    glAttachShader(gfx->shaderProgram, vertexShader);
    glAttachShader(gfx->shaderProgram, fragmentShader);
    glLinkProgram(gfx->shaderProgram);
    GLint linked = GL_FALSE;
    glGetProgramiv(gfx->shaderProgram, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE)
    {
        char log[512] = {};
        glGetProgramInfoLog(gfx->shaderProgram, sizeof(log), nullptr, log);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: main shader link failed: %s", log);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        delete gfx;
        return nullptr;
    }
    glUseProgram(gfx->shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertexShader = CompileShader(GL_VERTEX_SHADER, blitVSSource);
    fragmentShader = CompileShader(GL_FRAGMENT_SHADER, blitFSSource);
    if (vertexShader == 0 || fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        delete gfx;
        return nullptr;
    }

    gfx->blitProgram = glCreateProgram();
    glAttachShader(gfx->blitProgram, vertexShader);
    glAttachShader(gfx->blitProgram, fragmentShader);
    glLinkProgram(gfx->blitProgram);
    linked = GL_FALSE;
    glGetProgramiv(gfx->blitProgram, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE)
    {
        char log[512] = {};
        glGetProgramInfoLog(gfx->blitProgram, sizeof(log), nullptr, log);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: blit shader link failed: %s", log);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        delete gfx;
        return nullptr;
    }
    glUseProgram(gfx->blitProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    gfx->u_Model = glGetUniformLocation(gfx->shaderProgram, "u_Model");
    gfx->u_View = glGetUniformLocation(gfx->shaderProgram, "u_View");
    gfx->u_Proj = glGetUniformLocation(gfx->shaderProgram, "u_Proj");
    gfx->u_TextureMatrix = glGetUniformLocation(gfx->shaderProgram, "u_TextureMatrix");
    gfx->u_ScreenSpace = glGetUniformLocation(gfx->shaderProgram, "u_ScreenSpace");
    gfx->u_Viewport = glGetUniformLocation(gfx->shaderProgram, "u_Viewport");
    gfx->u_UseTexture = glGetUniformLocation(gfx->shaderProgram, "u_UseTexture");
    gfx->u_Texture = glGetUniformLocation(gfx->shaderProgram, "u_Texture");
    gfx->u_ColorOpRgb = glGetUniformLocation(gfx->shaderProgram, "u_ColorOpRgb");
    gfx->u_ColorOpAlpha = glGetUniformLocation(gfx->shaderProgram, "u_ColorOpAlpha");
    gfx->u_TexArg = glGetUniformLocation(gfx->shaderProgram, "u_TexArg");
    gfx->u_TextureFactor = glGetUniformLocation(gfx->shaderProgram, "u_TextureFactor");
    gfx->u_AlphaTest = glGetUniformLocation(gfx->shaderProgram, "u_AlphaTest");
    gfx->u_AlphaRef = glGetUniformLocation(gfx->shaderProgram, "u_AlphaRef");
    gfx->u_FogEnabled = glGetUniformLocation(gfx->shaderProgram, "u_FogEnabled");
    gfx->u_FogColor = glGetUniformLocation(gfx->shaderProgram, "u_FogColor");
    gfx->u_FogNear = glGetUniformLocation(gfx->shaderProgram, "u_FogNear");
    gfx->u_FogFar = glGetUniformLocation(gfx->shaderProgram, "u_FogFar");
    gfx->u_BlitTexture = glGetUniformLocation(gfx->blitProgram, "u_Texture");

    glUseProgram(gfx->shaderProgram);
    glUniform1i(gfx->u_Texture, 0);

    // SDL surfaces are tightly packed by their pitch, and TH06 also uploads
    // RGB24 data. The OpenGL default alignment of four bytes corrupts rows
    // whose byte width is not divisible by four.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glUseProgram(gfx->blitProgram);
    glUniform1i(gfx->u_BlitTexture, 0);

    glGenVertexArrays(9, &gfx->vaos[0][0]);
    glGenBuffers(3, gfx->vbos);

    for (i32 i = 0; i < 3; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, gfx->vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, VBO_CAPACITY, nullptr, GL_DYNAMIC_DRAW);
    }

    for (i32 i = 0; i < 3; i++)
    {
        glBindVertexArray(gfx->vaos[0][i]);
        glBindBuffer(GL_ARRAY_BUFFER, gfx->vbos[i]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTex1DiffuseXyzrhw),
                              (void *)offsetof(VertexTex1DiffuseXyzrhw, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexTex1DiffuseXyzrhw),
                              (void *)offsetof(VertexTex1DiffuseXyzrhw, diffuse));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTex1DiffuseXyzrhw),
                              (void *)offsetof(VertexTex1DiffuseXyzrhw, textureUV));

        glBindVertexArray(gfx->vaos[1][i]);
        glBindBuffer(GL_ARRAY_BUFFER, gfx->vbos[i]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTex1DiffuseXyz),
                              (void *)offsetof(VertexTex1DiffuseXyz, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexTex1DiffuseXyz),
                              (void *)offsetof(VertexTex1DiffuseXyz, diffuse));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTex1DiffuseXyz),
                              (void *)offsetof(VertexTex1DiffuseXyz, textureUV));

        glBindVertexArray(gfx->vaos[2][i]);
        glBindBuffer(GL_ARRAY_BUFFER, gfx->vbos[i]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexDiffuseXyzrhw),
                              (void *)offsetof(VertexDiffuseXyzrhw, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexDiffuseXyzrhw),
                              (void *)offsetof(VertexDiffuseXyzrhw, diffuse));
        glDisableVertexAttribArray(2);
    }
    glBindVertexArray(0);

    glGenVertexArrays(1, &gfx->blitVao);

    for (i32 i = 0; i < 4; i++)
    {
        gfx->transforms[i].Identity();
    }

    utils::DebugPrint("using gles rendering.\n");

    return gfx;
}

void GlesGraphics::Exit()
{
    if (this->ctx == nullptr)
    {
        return;
    }
    if (!SDL_GL_MakeCurrent(g_GameWindow.window, this->ctx))
    {
        SDL_GL_DestroyContext(this->ctx);
        this->ctx = nullptr;
        return;
    }
    glDeleteVertexArrays(9, &this->vaos[0][0]);
    glDeleteBuffers(3, this->vbos);
    glDeleteVertexArrays(1, &this->unitQuadVao);
    glDeleteBuffers(1, &this->unitQuadVbo);
    glDeleteVertexArrays(1, &this->blitVao);
#ifdef TH_ENABLE_THPRAC
    glDeleteVertexArrays(1, &this->imguiVao);
    glDeleteBuffers(1, &this->imguiVbo);
    glDeleteBuffers(1, &this->imguiEbo);
    glDeleteProgram(this->imguiProgram);
    glDeleteTextures(1, &this->imguiFontTexture);
#endif
    glDeleteProgram(this->shaderProgram);
    glDeleteProgram(this->blitProgram);
    glDeleteRenderbuffers(1, &this->fboDepth);
    glDeleteTextures(1, &this->fboColor);
    glDeleteFramebuffers(1, &this->fbo);
    SDL_GL_DestroyContext(this->ctx);
    this->ctx = nullptr;
}

void GlesGraphics::BeginFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    curVbo = (curVbo + 1) % 3;

    glBindBuffer(GL_ARRAY_BUFFER, vbos[curVbo]);
    glBufferData(GL_ARRAY_BUFFER, VBO_CAPACITY, nullptr, GL_STREAM_DRAW);
    vboOffset = 0;

    stateCache.Invalidate();
}

void GlesGraphics::EndFrame()
{
    Flush();
}

#ifdef TH_ENABLE_THPRAC
void GlesGraphics::RenderImGui(const ImDrawData *drawData)
{
    if (drawData == nullptr || drawData->CmdListsCount <= 0 || drawData->TotalVtxCount <= 0 ||
        drawData->TotalIdxCount <= 0)
    {
        return;
    }

    // Save the state owned by the game renderer. ImGui is drawn into the
    // 640x480 game FBO before the normal presentation blit, so changing the
    // window's default framebuffer or its pillarbox viewport here would make
    // the menu scale differently from the game.
    GLint lastFramebuffer = 0;
    GLint lastActiveTexture = GL_TEXTURE0;
    GLint lastProgram = 0;
    GLint lastTexture = 0;
    GLint lastTexture0 = 0;
    GLint lastArrayBuffer = 0;
    GLint lastElementArrayBuffer = 0;
    GLint lastVertexArray = 0;
    GLint lastUnpackAlignment = 4;
    GLint lastViewport[4] = {};
    GLint lastScissorBox[4] = {};
    GLint lastBlendSrcRgb = GL_SRC_ALPHA;
    GLint lastBlendDstRgb = GL_ONE_MINUS_SRC_ALPHA;
    GLint lastBlendSrcAlpha = GL_SRC_ALPHA;
    GLint lastBlendDstAlpha = GL_ONE_MINUS_SRC_ALPHA;
    GLint lastBlendEquationRgb = GL_FUNC_ADD;
    GLint lastBlendEquationAlpha = GL_FUNC_ADD;
    GLboolean lastBlend = glIsEnabled(GL_BLEND);
    GLboolean lastDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lastCullFace = glIsEnabled(GL_CULL_FACE);
    GLboolean lastScissorTest = glIsEnabled(GL_SCISSOR_TEST);
    GLboolean lastDepthMask = GL_TRUE;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &lastFramebuffer);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &lastActiveTexture);
    glGetIntegerv(GL_CURRENT_PROGRAM, &lastProgram);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture0);
    glActiveTexture(static_cast<GLenum>(lastActiveTexture));
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &lastArrayBuffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &lastElementArrayBuffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVertexArray);
    glGetIntegerv(GL_VIEWPORT, lastViewport);
    glGetIntegerv(GL_SCISSOR_BOX, lastScissorBox);
    glGetIntegerv(GL_BLEND_SRC_RGB, &lastBlendSrcRgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &lastBlendDstRgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &lastBlendSrcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &lastBlendDstAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &lastBlendEquationRgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &lastBlendEquationAlpha);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &lastUnpackAlignment);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &lastDepthMask);

    if (this->imguiProgram == 0)
    {
        const GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, imguiVertexShaderSource);
        const GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, imguiFragmentShaderSource);
        if (vertexShader == 0 || fragmentShader == 0)
        {
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return;
        }

        this->imguiProgram = glCreateProgram();
        glAttachShader(this->imguiProgram, vertexShader);
        glAttachShader(this->imguiProgram, fragmentShader);
        glLinkProgram(this->imguiProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint linked = GL_FALSE;
        glGetProgramiv(this->imguiProgram, GL_LINK_STATUS, &linked);
        if (linked != GL_TRUE)
        {
            char log[512] = {};
            glGetProgramInfoLog(this->imguiProgram, sizeof(log), nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: ImGui shader link failed: %s", log);
            glDeleteProgram(this->imguiProgram);
            this->imguiProgram = 0;
            return;
        }

        this->imguiProjMtx = glGetUniformLocation(this->imguiProgram, "u_ProjMtx");
        this->imguiTexture = glGetUniformLocation(this->imguiProgram, "Texture");
        glGenVertexArrays(1, &this->imguiVao);
        glGenBuffers(1, &this->imguiVbo);
        glGenBuffers(1, &this->imguiEbo);

        glBindVertexArray(this->imguiVao);
        glBindBuffer(GL_ARRAY_BUFFER, this->imguiVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->imguiEbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                              reinterpret_cast<void *>(offsetof(ImDrawVert, pos)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                              reinterpret_cast<void *>(offsetof(ImDrawVert, uv)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
                              reinterpret_cast<void *>(offsetof(ImDrawVert, col)));
        glBindVertexArray(0);
    }

    if (this->imguiFontTexture == 0)
    {
        unsigned char *pixels = nullptr;
        int width = 0;
        int height = 0;
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
        if (pixels == nullptr || width <= 0 || height <= 0)
        {
            return;
        }

        glGenTextures(1, &this->imguiFontTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->imguiFontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     pixels);
        io.Fonts->TexID = reinterpret_cast<ImTextureID>(
            static_cast<uintptr_t>(this->imguiFontTexture));
        io.Fonts->ClearTexData();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glViewport(0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_SCISSOR_TEST);
    glUseProgram(this->imguiProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->imguiVao);
    glBindBuffer(GL_ARRAY_BUFFER, this->imguiVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->imguiEbo);
    glUniform1i(this->imguiTexture, 0);

    const ImVec2 clipOffset = drawData->DisplayPos;
    const ImVec2 clipScale = drawData->FramebufferScale;
    const float displayHeight = drawData->DisplaySize.y * clipScale.y;
    const float left = drawData->DisplayPos.x;
    const float right = drawData->DisplayPos.x + drawData->DisplaySize.x;
    const float top = drawData->DisplayPos.y;
    const float bottom = drawData->DisplayPos.y + drawData->DisplaySize.y;
    const float projection[4][4] = {
        {2.0f / (right - left), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {(right + left) / (left - right), (top + bottom) / (bottom - top), 0.0f, 1.0f},
    };
    glUniformMatrix4fv(this->imguiProjMtx, 1, GL_FALSE, &projection[0][0]);

    const GLsizeiptr vertexBytes = static_cast<GLsizeiptr>(drawData->TotalVtxCount * sizeof(ImDrawVert));
    const GLsizeiptr indexBytes = static_cast<GLsizeiptr>(drawData->TotalIdxCount * sizeof(ImDrawIdx));
    glBufferData(GL_ARRAY_BUFFER, vertexBytes, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, nullptr, GL_STREAM_DRAW);

    GLsizeiptr vertexOffset = 0;
    GLsizeiptr indexOffset = 0;
    for (int listIndex = 0; listIndex < drawData->CmdListsCount; ++listIndex)
    {
        const ImDrawList *commandList = drawData->CmdLists[listIndex];
        const GLsizeiptr listVertexBytes =
            static_cast<GLsizeiptr>(commandList->VtxBuffer.Size * sizeof(ImDrawVert));
        const GLsizeiptr listIndexBytes =
            static_cast<GLsizeiptr>(commandList->IdxBuffer.Size * sizeof(ImDrawIdx));
        glBufferSubData(GL_ARRAY_BUFFER, vertexOffset, listVertexBytes, commandList->VtxBuffer.Data);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexOffset, listIndexBytes,
                        commandList->IdxBuffer.Data);

        for (int commandIndex = 0; commandIndex < commandList->CmdBuffer.Size; ++commandIndex)
        {
            const ImDrawCmd *command = &commandList->CmdBuffer[commandIndex];
            if (command->UserCallback != nullptr)
            {
                if (command->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    glUseProgram(this->imguiProgram);
                    glBindVertexArray(this->imguiVao);
                    glUniformMatrix4fv(this->imguiProjMtx, 1, GL_FALSE, &projection[0][0]);
                }
                else
                {
                    command->UserCallback(commandList, command);
                }
                continue;
            }

            ImVec4 clipRect;
            clipRect.x = (command->ClipRect.x - clipOffset.x) * clipScale.x;
            clipRect.y = (command->ClipRect.y - clipOffset.y) * clipScale.y;
            clipRect.z = (command->ClipRect.z - clipOffset.x) * clipScale.x;
            clipRect.w = (command->ClipRect.w - clipOffset.y) * clipScale.y;
            if (clipRect.x >= clipRect.z || clipRect.y >= clipRect.w || clipRect.z <= 0.0f ||
                clipRect.w <= 0.0f || clipRect.x >= drawData->DisplaySize.x * clipScale.x ||
                clipRect.y >= displayHeight)
            {
                continue;
            }

            const GLint scissorX = static_cast<GLint>(std::floor(std::max(clipRect.x, 0.0f)));
            const GLint scissorY = static_cast<GLint>(
                std::floor(std::max(displayHeight - clipRect.w, 0.0f)));
            const GLsizei scissorWidth = static_cast<GLsizei>(
                std::ceil(std::min(clipRect.z, drawData->DisplaySize.x * clipScale.x)) - scissorX);
            const GLsizei scissorHeight = static_cast<GLsizei>(
                std::ceil(displayHeight - std::max(clipRect.y, 0.0f)) - scissorY);
            if (scissorWidth <= 0 || scissorHeight <= 0)
            {
                continue;
            }
            glScissor(scissorX, scissorY, scissorWidth, scissorHeight);

            const GLuint texture = command->TextureId != nullptr
                                        ? static_cast<GLuint>(reinterpret_cast<uintptr_t>(command->TextureId))
                                        : this->imguiFontTexture;
            glBindTexture(GL_TEXTURE_2D, texture);

            // The draw lists are concatenated into one streaming VBO/EBO.
            // ImDrawIdx values are local to each ImDrawList (and may also use
            // ImDrawCmd::VtxOffset), so the vertex attribute base has to move
            // with the list/command. Without this, every list after the first
            // indexes vertices from the beginning of the combined VBO and
            // renders unrelated geometry.
            const GLsizeiptr commandVertexOffset =
                vertexOffset + static_cast<GLsizeiptr>(command->VtxOffset * sizeof(ImDrawVert));
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                                  reinterpret_cast<void *>(commandVertexOffset + offsetof(ImDrawVert, pos)));
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                                  reinterpret_cast<void *>(commandVertexOffset + offsetof(ImDrawVert, uv)));
            glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
                                  reinterpret_cast<void *>(commandVertexOffset + offsetof(ImDrawVert, col)));
            const void *indexPointer = reinterpret_cast<const void *>(
                indexOffset + static_cast<GLsizeiptr>(command->IdxOffset * sizeof(ImDrawIdx)));
            const GLenum indexType = sizeof(ImDrawIdx) == sizeof(std::uint16_t) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(command->ElemCount), indexType,
                           indexPointer);
        }

        vertexOffset += listVertexBytes;
        indexOffset += listIndexBytes;
    }

    glBindVertexArray(static_cast<GLuint>(lastVertexArray));
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(lastArrayBuffer));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(lastElementArrayBuffer));
    glUseProgram(static_cast<GLuint>(lastProgram));
    glPixelStorei(GL_UNPACK_ALIGNMENT, lastUnpackAlignment);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(lastTexture0));
    glActiveTexture(static_cast<GLenum>(lastActiveTexture));
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(lastTexture));
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(lastFramebuffer));
    glViewport(lastViewport[0], lastViewport[1], lastViewport[2], lastViewport[3]);
    glScissor(lastScissorBox[0], lastScissorBox[1], lastScissorBox[2], lastScissorBox[3]);
    glBlendEquationSeparate(static_cast<GLenum>(lastBlendEquationRgb),
                            static_cast<GLenum>(lastBlendEquationAlpha));
    glBlendFuncSeparate(static_cast<GLenum>(lastBlendSrcRgb), static_cast<GLenum>(lastBlendDstRgb),
                        static_cast<GLenum>(lastBlendSrcAlpha), static_cast<GLenum>(lastBlendDstAlpha));
    if (lastBlend)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (lastDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    if (lastCullFace)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    if (lastScissorTest)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
    glDepthMask(lastDepthMask);
    this->stateCache.Invalidate();
}
#endif

void GlesGraphics::SetFogRange(f32 nearPlane, f32 farPlane)
{
    if (fogNear != nearPlane || fogFar != farPlane)
    {
        fogNear = nearPlane;
        fogFar = farPlane;
        stateCache.dirtyFog = true;
    }
}

void GlesGraphics::SetFogColor(ZunColor color)
{
    if (fogColor != color)
    {
        fogColor = color;
        stateCache.dirtyFog = true;
    }
}

void GlesGraphics::SetColorOp(TextureOpComponent component, ColorOp op)
{
    if (component == COMPONENT_RGB)
    {
        if (colorOpRgb != op)
        {
            colorOpRgb = op;
            stateCache.dirtyColorOp = true;
        }
    }
    else
    {
        if (colorOpAlpha != op)
        {
            colorOpAlpha = op;
            stateCache.dirtyColorOp = true;
        }
    }
}

void GlesGraphics::SetTextureFactor(ZunColor factor)
{
    if (textureFactor != factor)
    {
        textureFactor = factor;
        stateCache.dirtyTexFactor = true;
    }
}

void GlesGraphics::SetTextureArg(TextureArg arg)
{
    if (texArg != arg)
    {
        texArg = arg;
        stateCache.dirtyTexArg = true;
    }
}

void GlesGraphics::SetTransformMatrix(TransformMatrix type, const ZunMatrix &matrix)
{
    transforms[type] = matrix;
    stateCache.dirtyMatrix = true;
}

void GlesGraphics::SetTextureFilter()
{
}

void GlesGraphics::GetViewport(ZunViewport &viewport)
{
    viewport = this->viewport;
}

void GlesGraphics::SetViewport(const ZunViewport &viewport)
{
    this->viewport = viewport;
    // TH06's ZunViewport::Set already converts its top-left D3D coordinates
    // to OpenGL's bottom-left coordinates. TH07 passes unconverted viewport
    // values, so copying its second Y flip here put different TH06 draw paths
    // in different coordinate systems.
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    stateCache.dirtyViewport = true;
}

void GlesGraphics::Enable(Capabilities cap)
{
    switch (cap)
    {
    case CAPS_BLEND:
        if (!blendEnabled)
        {
            glEnable(GL_BLEND);
            blendEnabled = true;
        }
        break;
    case CAPS_DEPTH_TEST:
        if (!depthTestEnabled)
        {
            glEnable(GL_DEPTH_TEST);
            depthTestEnabled = true;
        }
        break;
    case CAPS_ALPHA_TEST:
        if (!alphaTestEnabled)
        {
            alphaTestEnabled = true;
            stateCache.dirtyAlphaTest = true;
        }
        break;
    case CAPS_FOG:
        if (!fogEnabled)
        {
            fogEnabled = true;
            stateCache.dirtyFog = true;
        }
        break;
    }
}

void GlesGraphics::Disable(Capabilities cap)
{
    switch (cap)
    {
    case CAPS_BLEND:
        if (blendEnabled)
        {
            glDisable(GL_BLEND);
            blendEnabled = false;
        }
        break;
    case CAPS_DEPTH_TEST:
        if (depthTestEnabled)
        {
            glDisable(GL_DEPTH_TEST);
            depthTestEnabled = false;
        }
        break;
    case CAPS_ALPHA_TEST:
        if (alphaTestEnabled)
        {
            alphaTestEnabled = false;
            stateCache.dirtyAlphaTest = true;
        }
        break;
    case CAPS_FOG:
        if (fogEnabled)
        {
            fogEnabled = false;
            stateCache.dirtyFog = true;
        }
        break;
    }
}

void GlesGraphics::SetBlendMode(BlendMode srcMode, BlendMode dstMode)
{
    GLenum glSrcMode = GL_SRC_ALPHA;
    switch (srcMode)
    {
    case BLEND_ALPHA:
        glSrcMode = GL_SRC_ALPHA;
        break;
    case BLEND_ONE:
        glSrcMode = GL_ONE;
        break;
    case BLEND_INV_SRC_ALPHA:
        glSrcMode = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case BLEND_NONE:
        glSrcMode = GL_ONE;
        break;
    }

    GLenum glDstMode = GL_ONE_MINUS_SRC_ALPHA;
    switch (dstMode)
    {
    case BLEND_ALPHA:
        glDstMode = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case BLEND_ONE:
        glDstMode = GL_ONE;
        break;
    case BLEND_INV_SRC_ALPHA:
        glDstMode = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case BLEND_NONE:
        glDstMode = GL_ZERO;
        break;
    }
    glBlendFunc(glSrcMode, glDstMode);
}

void GlesGraphics::SetDepthMask(bool enable)
{
    depthMaskEnabled = enable;
    glDepthMask(enable);
}

void GlesGraphics::SetDepthFunc(DepthFunc func)
{
    switch (func)
    {
    case DEPTH_FUNC_LEQUAL:
        glDepthFunc(GL_LEQUAL);
        break;
    case DEPTH_FUNC_ALWAYS:
        glDepthFunc(GL_ALWAYS);
        break;
    }
}

void GlesGraphics::SetClearDepth(f32 depth)
{
#ifdef USING_GL
    glClearDepth(depth);
#else
    glClearDepthf(depth);
#endif
}

void GlesGraphics::SetClearColor(ZunColor color)
{
    clearColor = color;
    glClearColor(COLOR_R(color) / 255.0f, COLOR_G(color) / 255.0f, COLOR_B(color) / 255.0f,
                 COLOR_A(color) / 255.0f);
}

void GlesGraphics::SetAlphaTestRef(u8 ref)
{
    if (alphaRef != ref)
    {
        alphaRef = ref;
        stateCache.dirtyAlphaTest = true;
    }
}

void GlesGraphics::Clear(u32 clearBits)
{
    GLbitfield bits = 0;
    if (clearBits & CLEAR_COLOR_BUFFER)
    {
        bits |= GL_COLOR_BUFFER_BIT;
    }
    if (clearBits & CLEAR_DEPTH_BUFFER)
    {
        bits |= GL_DEPTH_BUFFER_BIT;
        if (!depthMaskEnabled)
        {
            glDepthMask(GL_TRUE);
        }
    }
    glClear(bits);
    if ((clearBits & CLEAR_DEPTH_BUFFER) && !depthMaskEnabled)
    {
        glDepthMask(GL_FALSE);
    }
}

GfxTextureHandle GlesGraphics::CreateTexture()
{
    GLuint tex;
    glGenTextures(1, &tex);
    return GfxTextureHandle(tex);
}

void GlesGraphics::BindTexture(GfxTextureHandle handle)
{
    glBindTexture(GL_TEXTURE_2D, handle.id);
    boundTexture = handle.id;
}

void GlesGraphics::DeleteTexture(GfxTextureHandle handle)
{
    GLuint tex = handle.id;
    glDeleteTextures(1, &tex);
    if (boundTexture == tex)
    {
        boundTexture = 0;
    }
}

void GlesGraphics::SetTextureImage(u32 width, u32 height, PixelFormat fmt, PixelDataType type,
                                   const void *data)
{
    GLenum internalformat;
    GLenum format;
    GLenum datatype;

    switch (type)
    {
    case PIXEL_UNSIGNED_BYTE:
        datatype = GL_UNSIGNED_BYTE;
        break;
    case PIXEL_UNSIGNED_SHORT_5_5_5_1:
        datatype = GL_UNSIGNED_SHORT_5_5_5_1;
        break;
    case PIXEL_UNSIGNED_SHORT_5_6_5:
        datatype = GL_UNSIGNED_SHORT_5_6_5;
        break;
    case PIXEL_UNSIGNED_SHORT_4_4_4_4:
        datatype = GL_UNSIGNED_SHORT_4_4_4_4;
        break;
    }

    if (fmt == PIXEL_RGB)
    {
        format = GL_RGB;
        internalformat = type == PIXEL_UNSIGNED_SHORT_5_6_5 ? GL_RGB565 : GL_RGB8;
    }
    else
    {
        format = GL_RGBA;
        if (type == PIXEL_UNSIGNED_SHORT_5_5_5_1)
            internalformat = GL_RGB5_A1;
        else if (type == PIXEL_UNSIGNED_SHORT_4_4_4_4)
            internalformat = GL_RGBA4;
        else
            internalformat = GL_RGBA8;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, datatype, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GlesGraphics::SetTextureSubImage(i32 xoffset, i32 yoffset, i32 width, i32 height,
                                      const void *data)
{
    // TH06 uses this path only for temporary RGB24 surfaces created by
    // ApplySurfaceToColorBuffer. Uploading them as RGBA was both the wrong
    // byte layout and illegal for the GL_RGB8 backing texture in WebGL 2.
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                    data);
}

void GlesGraphics::ReadPixels(i32 x, i32 y, i32 width, i32 height, void *pixels)
{
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    u32 rowSize = width * 4;
    u8 *p = (u8 *)pixels;
    u8 *tempRow = new u8[rowSize];
    for (i32 i = 0; i < height / 2; ++i)
    {
        u8 *top = p + i * rowSize;
        u8 *bottom = p + (height - 1 - i) * rowSize;
        memcpy(tempRow, top, rowSize);
        memcpy(top, bottom, rowSize);
        memcpy(bottom, tempRow, rowSize);
    }
    delete[] tempRow;
}

void GlesGraphics::DrawPrimitive(PrimitiveType type, i32 startVertex, i32 primitiveCount)
{
    i32 vertexCount = 0;
    GLenum glMode = GL_TRIANGLES;

    if (type == PRIM_TRIANGLES)
    {
        vertexCount = primitiveCount * 3;
        glMode = GL_TRIANGLES;
    }
    else if (type == PRIM_TRIANGLE_STRIP)
    {
        vertexCount = primitiveCount + 2;
        glMode = GL_TRIANGLE_STRIP;
    }
    else if (type == PRIM_TRIANGLE_FAN)
    {
        vertexCount = primitiveCount + 2;
        glMode = GL_TRIANGLE_FAN;
    }

    if (stateCache.currentVao != unitQuadVao)
    {
        glBindVertexArray(unitQuadVao);
        stateCache.currentVao = unitQuadVao;
    }

    if (stateCache.currentStride != sizeof(RenderVertexInfo))
    {
        glUniform1i(u_ScreenSpace, false);
        glUniform1i(u_UseTexture, true);
        stateCache.currentStride = sizeof(RenderVertexInfo);
    }

    if (stateCache.dirtyViewport)
    {
        glUniform4f(u_Viewport, (f32)viewport.x, (f32)viewport.y, (f32)viewport.width,
                    (f32)viewport.height);
        stateCache.dirtyViewport = false;
    }

    if (stateCache.dirtyMatrix)
    {
        glUniformMatrix4fv(u_Model, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_MODEL]);
        glUniformMatrix4fv(u_View, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_VIEW]);
        glUniformMatrix4fv(u_Proj, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_PROJECTION]);
        glUniformMatrix4fv(u_TextureMatrix, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_TEXTURE]);
        stateCache.dirtyMatrix = false;
    }

    if (stateCache.dirtyColorOp)
    {
        glUniform1i(u_ColorOpRgb, colorOpRgb);
        glUniform1i(u_ColorOpAlpha, colorOpAlpha);
        stateCache.dirtyColorOp = false;
    }

    if (stateCache.dirtyTexArg)
    {
        glUniform1i(u_TexArg, texArg);
        stateCache.dirtyTexArg = false;
    }

    if (stateCache.dirtyTexFactor)
    {
        glUniform4f(u_TextureFactor, COLOR_R(textureFactor) / 255.0f, COLOR_G(textureFactor) / 255.0f,
                    COLOR_B(textureFactor) / 255.0f, COLOR_A(textureFactor) / 255.0f);
        stateCache.dirtyTexFactor = false;
    }

    if (stateCache.dirtyAlphaTest)
    {
        glUniform1i(u_AlphaTest, alphaTestEnabled);
        glUniform1f(u_AlphaRef, alphaRef / 255.0f);
        stateCache.dirtyAlphaTest = false;
    }

    if (stateCache.dirtyFog)
    {
        glUniform1i(u_FogEnabled, fogEnabled);
        glUniform4f(u_FogColor, COLOR_R(fogColor) / 255.0f, COLOR_G(fogColor) / 255.0f,
                    COLOR_B(fogColor) / 255.0f, COLOR_A(fogColor) / 255.0f);
        glUniform1f(u_FogNear, fogNear);
        glUniform1f(u_FogFar, fogFar);
        stateCache.dirtyFog = false;
    }

    glDrawArrays(glMode, startVertex, vertexCount);
}

void GlesGraphics::DrawPrimitiveUP(PrimitiveType type, i32 primitiveCount, const void *vertexData,
                                   i32 vertexStride)
{
    if (primitiveCount <= 0 || vertexData == nullptr || vertexStride <= 0)
    {
        return;
    }
    i32 vertexCount = 0;
    GLenum glMode = GL_TRIANGLES;

    if (type == PRIM_TRIANGLES)
    {
        vertexCount = primitiveCount * 3;
        glMode = GL_TRIANGLES;
    }
    else if (type == PRIM_TRIANGLE_STRIP)
    {
        vertexCount = primitiveCount + 2;
        glMode = GL_TRIANGLE_STRIP;
    }
    else if (type == PRIM_TRIANGLE_FAN)
    {
        vertexCount = primitiveCount + 2;
        glMode = GL_TRIANGLE_FAN;
    }

    GLsizeiptr bytesNeeded = vertexCount * vertexStride;
    if (vertexCount <= 0 || bytesNeeded <= 0 || static_cast<size_t>(bytesNeeded) > VBO_CAPACITY)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: invalid immediate draw size (%lld bytes)",
                     static_cast<long long>(bytesNeeded));
        return;
    }
    vboOffset = ((vboOffset + vertexStride - 1) / vertexStride) * vertexStride;
    if (vboOffset + bytesNeeded > VBO_CAPACITY)
    {
        glBufferData(GL_ARRAY_BUFFER, VBO_CAPACITY, nullptr, GL_STREAM_DRAW);
        vboOffset = 0;
    }
    glBufferSubData(GL_ARRAY_BUFFER, vboOffset, bytesNeeded, vertexData);

    GLint firstVertex = (GLint)(vboOffset / vertexStride);

    bool isScreenSpace = false;
    bool hasTex = false;
    GLuint targetVao = 0;
    switch (vertexStride)
    {
    case sizeof(VertexTex1DiffuseXyzrhw):
        // Unlike TH07, TH06 already supplies inverseViewportMatrix() as the
        // projection for transformed XYZRHW vertices. Keep them on the normal
        // matrix path so sprites, bullets and enemies share one contract.
        isScreenSpace = false;
        hasTex = true;
        targetVao = vaos[0][curVbo];
        break;
    case sizeof(VertexTex1DiffuseXyz):
        isScreenSpace = false;
        hasTex = true;
        targetVao = vaos[1][curVbo];
        break;
    case sizeof(VertexDiffuseXyzrhw):
        isScreenSpace = false;
        hasTex = false;
        targetVao = vaos[2][curVbo];
        break;
    }

    if (targetVao == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "th06: unsupported immediate vertex stride %d", vertexStride);
        return;
    }

    vboOffset += bytesNeeded;

    if (stateCache.currentVao != targetVao)
    {
        glBindVertexArray(targetVao);
        stateCache.currentVao = targetVao;
    }

    if (stateCache.currentStride != vertexStride)
    {
        glUniform1i(u_ScreenSpace, isScreenSpace);
        glUniform1i(u_UseTexture, hasTex);
        stateCache.currentStride = vertexStride;
    }

    if (stateCache.dirtyViewport)
    {
        glUniform4f(u_Viewport, (f32)viewport.x, (f32)viewport.y, (f32)viewport.width,
                    (f32)viewport.height);
        stateCache.dirtyViewport = false;
    }

    if (stateCache.dirtyMatrix)
    {
        glUniformMatrix4fv(u_Model, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_MODEL]);
        glUniformMatrix4fv(u_View, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_VIEW]);
        glUniformMatrix4fv(u_Proj, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_PROJECTION]);
        glUniformMatrix4fv(u_TextureMatrix, 1, GL_FALSE, (GLfloat *)&transforms[MATRIX_TEXTURE]);
        stateCache.dirtyMatrix = false;
    }

    if (stateCache.dirtyColorOp)
    {
        glUniform1i(u_ColorOpRgb, colorOpRgb);
        glUniform1i(u_ColorOpAlpha, colorOpAlpha);
        stateCache.dirtyColorOp = false;
    }

    if (stateCache.dirtyTexArg)
    {
        glUniform1i(u_TexArg, texArg);
        stateCache.dirtyTexArg = false;
    }

    if (stateCache.dirtyTexFactor)
    {
        glUniform4f(u_TextureFactor, COLOR_R(textureFactor) / 255.0f, COLOR_G(textureFactor) / 255.0f,
                    COLOR_B(textureFactor) / 255.0f, COLOR_A(textureFactor) / 255.0f);
        stateCache.dirtyTexFactor = false;
    }

    if (stateCache.dirtyAlphaTest)
    {
        glUniform1i(u_AlphaTest, alphaTestEnabled);
        glUniform1f(u_AlphaRef, alphaRef / 255.0f);
        stateCache.dirtyAlphaTest = false;
    }

    if (stateCache.dirtyFog)
    {
        glUniform1i(u_FogEnabled, fogEnabled);
        glUniform4f(u_FogColor, COLOR_R(fogColor) / 255.0f, COLOR_G(fogColor) / 255.0f,
                    COLOR_B(fogColor) / 255.0f, COLOR_A(fogColor) / 255.0f);
        glUniform1f(u_FogNear, fogNear);
        glUniform1f(u_FogFar, fogFar);
        stateCache.dirtyFog = false;
    }

    glDrawArrays(glMode, firstVertex, vertexCount);
}

void GlesGraphics::SwapBuffers()
{
    i32 drawableWidth, drawableHeight;
    SDL_GetWindowSizeInPixels(g_GameWindow.window, &drawableWidth, &drawableHeight);
    if (drawableWidth <= 0 || drawableHeight <= 0)
    {
        return;
    }

#if defined(__APPLE__) && TARGET_OS_IPHONE
    SDL_PropertiesID props = SDL_GetWindowProperties(g_GameWindow.window);
    this->defaultFbo = (GLuint)SDL_GetNumberProperty(
        props, SDL_PROP_WINDOW_UIKIT_OPENGL_FRAMEBUFFER_NUMBER, this->defaultFbo);
#endif

    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->fbo);
#ifndef USING_GL
    const GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
    glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
#endif

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->defaultFbo);

    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, drawableWidth, drawableHeight);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // the original game didnt pillarbox but that looks really ugly so im pillarboxing anyways
    f32 targetAspect = 640.0f / 480.0f;
    f32 windowAspect = (f32)drawableWidth / (f32)drawableHeight;

    i32 dstWidth;
    i32 dstHeight;
    i32 dstX;
    i32 dstY;

    if (windowAspect > targetAspect)
    {
        dstHeight = drawableHeight;
        dstWidth = (i32)(dstHeight * targetAspect);
        dstX = (drawableWidth - dstWidth) / 2;
        dstY = 0;
    }
    else
    {
        dstWidth = drawableWidth;
        dstHeight = (i32)(dstWidth / targetAspect);
        dstX = 0;
        dstY = (drawableHeight - dstHeight) / 2;
    }

    glViewport(dstX, dstY, dstWidth, dstHeight);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);

    glUseProgram(this->blitProgram);

    glActiveTexture(GL_TEXTURE0);
    const GLuint gameplayTexture = boundTexture;
    glBindTexture(GL_TEXTURE_2D, this->fboColor);

    glBindVertexArray(this->blitVao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

#if defined(__APPLE__) && TARGET_OS_IPHONE
    glBindRenderbuffer(
        GL_RENDERBUFFER,
        SDL_GetNumberProperty(props, SDL_PROP_WINDOW_UIKIT_OPENGL_RENDERBUFFER_NUMBER, 0));
#endif
    SDL_GL_SwapWindow(g_GameWindow.window);

    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

    if (blendEnabled)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    if (depthTestEnabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glDepthMask(depthMaskEnabled ? GL_TRUE : GL_FALSE);

    glClearColor(COLOR_R(clearColor) / 255.0f, COLOR_G(clearColor) / 255.0f,
                 COLOR_B(clearColor) / 255.0f, COLOR_A(clearColor) / 255.0f);

    glUseProgram(this->shaderProgram);
    glBindTexture(GL_TEXTURE_2D, gameplayTexture);
    stateCache.Invalidate();
}

void GlesGraphics::ToggleVertexAttribute(u8 attr, bool enable)
{
    if (enable)
        compatEnabledAttributes |= attr;
    else
        compatEnabledAttributes &= ~attr;
}

void GlesGraphics::SetAttributePointer(VertexAttributeArrays attr, std::size_t stride, void *ptr)
{
    compatAttribPointers[attr] = ptr;
    compatAttribStrides[attr] = stride;
}

void GlesGraphics::GetViewport(u32 *out)
{
    out[0] = viewport.x;
    out[1] = viewport.y;
    out[2] = viewport.width;
    out[3] = viewport.height;
}

void GlesGraphics::GetDepthRange(f32 *range)
{
    range[0] = viewport.minZ;
    range[1] = viewport.maxZ;
}

void GlesGraphics::SetViewport(i32 x, i32 y, i32 width, i32 height)
{
    ZunViewport value = {x, y, width, height, viewport.minZ, viewport.maxZ};
    SetViewport(value);
}

void GlesGraphics::SetDepthRange(f32 nearPlane, f32 farPlane)
{
    viewport.minZ = nearPlane;
    viewport.maxZ = farPlane;
    glDepthRangef(nearPlane, farPlane);
}

bool GlesGraphics::HasError()
{
    return glGetError() != GL_NO_ERROR;
}

void GlesGraphics::SetBlendMode(BlendMode mode)
{
    SetBlendMode(BLEND_ALPHA, mode == BLEND_ONE ? BLEND_ONE : BLEND_ALPHA);
}

void GlesGraphics::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
    SetClearColor(((u32)(a * 255.0f) << 24) | ((u32)(r * 255.0f) << 16) |
                  ((u32)(g * 255.0f) << 8) | (u32)(b * 255.0f));
}

void GlesGraphics::ReadPixels(i32 x, i32 y, i32 width, i32 height, const void *pixels)
{
    ReadPixels(x, y, width, height, const_cast<void *>(pixels));
}

void GlesGraphics::Draw(PrimitiveType type, i32 start, i32 count)
{
    if (!compatAttribPointers[VERTEX_ARRAY_POSITION] || count <= 0)
        return;

    i32 primitiveCount = type == PRIM_TRIANGLES ? count / 3 : count - 2;
    const size_t stride = compatAttribStrides[VERTEX_ARRAY_POSITION];
    const u8 *base = static_cast<const u8 *>(compatAttribPointers[VERTEX_ARRAY_POSITION]) + start * stride;

    if (stride == sizeof(VertexTex1Xyzrhw))
    {
        std::vector<VertexTex1DiffuseXyzrhw> converted(count);
        const VertexTex1Xyzrhw *source = reinterpret_cast<const VertexTex1Xyzrhw *>(base);
        for (i32 i = 0; i < count; ++i)
        {
            converted[i].position = source[i].position;
            converted[i].diffuse = {0xffffffff};
            converted[i].textureUV = source[i].textureUV;
        }
        DrawPrimitiveUP(type, primitiveCount, converted.data(), sizeof(converted[0]));
    }
    else
    {
        DrawPrimitiveUP(type, primitiveCount, base, stride);
    }
}
