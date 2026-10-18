// Generated from purple THPrac (MIT), TH07 source sha256 b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0.
#pragma once
#include <eagler/thprac/PracticeInput.hpp>
namespace PracticeRuntime {
struct PurpleInput : eagler::thprac::PracticeInput {
 std::uint16_t filter(std::uint16_t buttons, bool enabled, bool replay) {
  if (!enabled || replay) return buttons;
  std::uint8_t state[256]{}; state[90]=(buttons&1)?0x80:0;state[88]=(buttons&2)?0x80:0;
  state[160]=state[161]=(buttons&4)?0x80:0; state[27]=(buttons&8)?0x80:0; state[82]=(buttons&0x4000)?0x80:0; apply(state);
  return (buttons & ~std::uint16_t(0x400f)) | (state[90]?1:0) | (state[88]?2:0) | ((state[160]||state[161])?4:0) | (state[27]?8:0) | (state[82]?0x4000:0);
 }
};
}
