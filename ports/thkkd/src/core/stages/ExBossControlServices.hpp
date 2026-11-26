#pragma once
#include <cstdint>
namespace kaikeidou {
struct ExBossControlServices {
    virtual ~ExBossControlServices()=default;
    virtual std::uint32_t draw_circle(std::uint32_t x,std::uint32_t y,std::uint32_t radius,std::uint32_t color,std::uint32_t filled)=0;
};
}
