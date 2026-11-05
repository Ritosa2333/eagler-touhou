#pragma once
#include "../text_renderer/text.hpp"
namespace th20::source::hud::draw_detail {
inline void color_alpha(text::Renderer& r,std::uint8_t alpha) noexcept {r.color=(r.color&0xffffffu)|(std::uint32_t(alpha)<<24);} //488940
inline void shadow_alpha(text::Renderer& r,std::uint8_t alpha) noexcept {r.shadow_color=(r.shadow_color&0xffffffu)|(std::uint32_t(alpha)<<24);} //4b8600
inline void layer(text::Renderer& r,std::uint32_t value) noexcept {r.fields_1a1d4[6]=value;} //488a00
inline void font(text::Renderer& r,std::uint32_t value) noexcept {r.fields_1a1d4[3]=value;} //488b00
inline void alignment(text::Renderer& r,std::uint32_t x,std::uint32_t y) noexcept {r.fields_1a1d4[8]=x;r.fields_1a1d4[9]=y;} //4b8620
inline void scale(text::Renderer& r,float x,float y) noexcept {r.scale_x=x;r.scale_y=y;} //4b8d90
}
