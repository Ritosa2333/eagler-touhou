#pragma once

#include <GLES3/gl3.h>
#include <RmlUi/Core/RenderInterface.h>
#include <nlohmann/json.hpp>
#include <windows.h>
#include <wincrypt.h>
#include "src/webp/decode.h"
#include "../portable/sdl/third_party/stb_image.h"

#include <algorithm>
#include <array>
#include <memory>
#include <string_view>
#include <utility>
#include <cstddef>
#include <cstdio>

namespace th09::desktop {

class RmlGles final : public Rml::RenderInterface {
public:
    ~RmlGles() override {
        glDeleteTextures(1, &white_);
        glDeleteProgram(program_);
    }

    bool Start() {
        if (program_) return true;
        constexpr const char* vertex = R"(
#version 300 es
layout(location=0) in vec2 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 uv;
uniform vec2 screen;
uniform vec2 offset;
out vec4 tint;
out vec2 coordinate;
void main() {
    vec2 p = (position + offset) / screen;
    gl_Position = vec4(2.0 * p.x - 1.0, 1.0 - 2.0 * p.y, 0.0, 1.0);
    tint = color;
    coordinate = uv;
}
)";
        constexpr const char* fragment = R"(
#version 300 es
precision mediump float;
in vec4 tint;
in vec2 coordinate;
uniform sampler2D page;
out vec4 rgba;
void main() { rgba = tint * texture(page, coordinate); }
)";
        // GLSL requires the version directive before any other tokens.
        const auto vs = shader(GL_VERTEX_SHADER, vertex + 1);
        const auto fs = shader(GL_FRAGMENT_SHADER, fragment + 1);
        if (vs && fs) {
            program_ = glCreateProgram();
            glAttachShader(program_, vs);
            glAttachShader(program_, fs);
            glLinkProgram(program_);
            GLint linked = 0;
            glGetProgramiv(program_, GL_LINK_STATUS, &linked);
            if (!linked) {
                glDeleteProgram(program_);
                program_ = 0;
            }
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        if (!program_) return false;
        screen_ = glGetUniformLocation(program_, "screen");
        offset_ = glGetUniformLocation(program_, "offset");
        const Rml::byte white[] = {255, 255, 255, 255};
        white_ = static_cast<GLuint>(GenerateTexture({white, sizeof(white)}, {1, 1}));
        return white_ != 0;
    }

    struct Frame {
        explicit Frame(RmlGles& renderer) : renderer(renderer) {
            glGetIntegerv(GL_CURRENT_PROGRAM, &program);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &array);
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buffer);
            glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
            glGetIntegerv(GL_VIEWPORT, viewport.data());
            glGetIntegerv(GL_SCISSOR_BOX, scissorBox.data());
            glGetBooleanv(GL_COLOR_WRITEMASK, mask.data());
            glGetIntegerv(GL_BLEND_SRC_RGB, &blend[0]);
            glGetIntegerv(GL_BLEND_DST_RGB, &blend[1]);
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend[2]);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &blend[3]);
            glGetIntegerv(GL_BLEND_EQUATION_RGB, &equation[0]);
            glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &equation[1]);
            for (unsigned i = 0; i < capabilities.size(); ++i) enabled[i] = glIsEnabled(capabilities[i]);
            glActiveTexture(GL_TEXTURE0);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
            glGetIntegerv(GL_SAMPLER_BINDING, &sampler);
            glBindSampler(0, 0);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_SCISSOR_TEST);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }

        ~Frame() {
            glUseProgram(program);
            glBindVertexArray(array);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindSampler(0, sampler);
            glActiveTexture(activeTexture);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);
            glColorMask(mask[0], mask[1], mask[2], mask[3]);
            glBlendFuncSeparate(blend[0], blend[1], blend[2], blend[3]);
            glBlendEquationSeparate(equation[0], equation[1]);
            for (unsigned i = 0; i < capabilities.size(); ++i) {
                if (enabled[i]) glEnable(capabilities[i]);
                else glDisable(capabilities[i]);
            }
        }

        void Viewport(int x, int bottom, int w, int h, Rml::Vector2i dimensions) {
            renderer.x_ = x;
            renderer.bottom_ = bottom;
            renderer.w_ = w;
            renderer.h_ = h;
            renderer.dimensions_ = dimensions;
            glViewport(x, bottom, w, h);
            glUseProgram(renderer.program_);
            glUniform2f(renderer.screen_, static_cast<float>(dimensions.x), static_cast<float>(dimensions.y));
        }

        RmlGles& renderer;
        GLint program = 0, array = 0, buffer = 0, activeTexture = 0, texture = 0, sampler = 0;
        std::array<GLint, 4> viewport{}, scissorBox{}, blend{};
        std::array<GLint, 2> equation{};
        std::array<GLboolean, 4> mask{};
        static constexpr std::array<GLenum, 5> capabilities = {
            GL_BLEND, GL_CULL_FACE, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_SCISSOR_TEST};
        std::array<GLboolean, capabilities.size()> enabled{};
    };

    Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override {
        auto* geometry = new Geometry;
        geometry->count = static_cast<GLsizei>(indices.size());
        glGenVertexArrays(1, &geometry->array);
        glGenBuffers(2, geometry->buffers.data());
        glBindVertexArray(geometry->array);
        glBindBuffer(GL_ARRAY_BUFFER, geometry->buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Rml::Vertex), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->buffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex),
                              reinterpret_cast<void*>(offsetof(Rml::Vertex, position)));
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rml::Vertex),
                              reinterpret_cast<void*>(offsetof(Rml::Vertex, colour)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Rml::Vertex),
                              reinterpret_cast<void*>(offsetof(Rml::Vertex, tex_coord)));
        for (unsigned index = 0; index < 3; ++index) glEnableVertexAttribArray(index);
        return reinterpret_cast<Rml::CompiledGeometryHandle>(geometry);
    }

    void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) override {
        const auto* geometry = reinterpret_cast<Geometry*>(handle);
        glUniform2f(offset_, translation.x, translation.y);
        glBindTexture(GL_TEXTURE_2D, texture ? static_cast<GLuint>(texture) : white_);
        glBindVertexArray(geometry->array);
        glDrawElements(GL_TRIANGLES, geometry->count, GL_UNSIGNED_INT, nullptr);
    }

    void ReleaseGeometry(Rml::CompiledGeometryHandle handle) override {
        auto* geometry = reinterpret_cast<Geometry*>(handle);
        glDeleteVertexArrays(1, &geometry->array);
        glDeleteBuffers(2, geometry->buffers.data());
        delete geometry;
    }

    void SetAvatars(nlohmann::json assets) { avatars_ = std::move(assets); }

    Rml::TextureHandle LoadTexture(Rml::Vector2i& size, const Rml::String& path) override {
        const auto name = path.substr(path.find_last_of("/|") + 1);
        constexpr std::string_view prefix = "avatar-";
        if (!avatars_.is_object() || name.rfind(prefix,0) != 0) return 0;
        const auto entry = avatars_.find(name.substr(prefix.size()));
        if (entry == avatars_.end() || !entry->is_object()) return 0;
        const auto data = entry->value("data",std::string{});
        if (data.empty() || data.size() > 10924) return 0;
        std::array<unsigned char,8192> bytes;
        DWORD count = static_cast<DWORD>(bytes.size());
        if (!CryptStringToBinaryA(data.data(),static_cast<DWORD>(data.size()),CRYPT_STRING_BASE64,
            bytes.data(), &count,nullptr,nullptr)) return 0;
        std::array<unsigned char,64 * 64 * 4> rgba;
        const auto format = entry->value("type",std::string{});
        int w = 0, h = 0;
        const auto dimensionsValid = [&] { return w > 0 && w == h && w <= 64; };
        if (format == "webp") {
            if (!WebPGetInfo(bytes.data(),count,&w,&h) || !dimensionsValid() ||
                !WebPDecodeRGBAInto(bytes.data(),count,rgba.data(),rgba.size(),w * 4)) return 0;
        } else if (format == "jpeg") {
            int channels = 0;
            if (!stbi_info_from_memory(bytes.data(),count,&w,&h,&channels) || !dimensionsValid()) return 0;
            std::unique_ptr<stbi_uc,decltype(&stbi_image_free)> pixels(
                stbi_load_from_memory(bytes.data(),count,&w,&h,&channels,4),stbi_image_free);
            if (!pixels || !dimensionsValid()) return 0;
            std::copy_n(pixels.get(),w * h * 4,rgba.data());
        } else return 0;
        for (int i = 0; i < w * h * 4; i += 4) {
            for (int color = 0; color < 3; ++color) rgba[i+color] = rgba[i+color] * rgba[i+3] / 255;
        }
        size = {w,h};
        return GenerateTexture({rgba.data(),static_cast<size_t>(w * h * 4)},size);
    }

    Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i size) override {
        GLuint texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source.data());
        return texture;
    }

    void ReleaseTexture(Rml::TextureHandle handle) override {
        const auto texture = static_cast<GLuint>(handle);
        glDeleteTextures(1, &texture);
    }

    void EnableScissorRegion(bool enable) override {
        if (enable) glEnable(GL_SCISSOR_TEST);
        else glDisable(GL_SCISSOR_TEST);
    }

    void SetScissorRegion(Rml::Rectanglei rect) override {
        const int left = rect.Left() * w_ / dimensions_.x;
        const int right = rect.Right() * w_ / dimensions_.x;
        const int down = rect.Bottom() * h_ / dimensions_.y;
        const int up = rect.Top() * h_ / dimensions_.y;
        glScissor(x_ + left, bottom_ + h_ - down, right - left, down - up);
    }

private:
    static GLuint shader(GLenum type, const char* source) {
        const auto object = glCreateShader(type);
        glShaderSource(object, 1, &source, nullptr);
        glCompileShader(object);
        GLint compiled = 0;
        glGetShaderiv(object, GL_COMPILE_STATUS, &compiled);
        if (compiled) return object;
        char log[512]{};
        glGetShaderInfoLog(object, sizeof(log), nullptr, log);
        std::fprintf(stderr, "RmlUi shader: %s\n", log);
        glDeleteShader(object);
        return 0;
    }

    struct Geometry {
        GLuint array = 0;
        std::array<GLuint, 2> buffers{};
        GLsizei count = 0;
    };
    nlohmann::json avatars_;
    GLuint white_ = 0, program_ = 0;
    GLint screen_ = -1, offset_ = -1;
    int x_ = 0, bottom_ = 0, w_ = 0, h_ = 0;
    Rml::Vector2i dimensions_{1, 1};
};

} // namespace th09::desktop
