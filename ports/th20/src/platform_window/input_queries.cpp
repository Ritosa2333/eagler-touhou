#include "platform_window.hpp"
namespace th20::source::platform_window {
std::uint32_t pressed(const InputPrefix& i,std::uint32_t mask) noexcept {return i.pressed&mask;}
int repeated_or_pressed(const InputPrefix& i,std::uint32_t mask) noexcept {
    return pressed(i,mask)!=0 || (i.repeat8&mask)!=0;
}
}
