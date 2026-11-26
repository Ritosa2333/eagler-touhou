#pragma once
#include <array>
#include <filesystem>
#include <string>

namespace kaikeidou {
struct DisplaySettings {
    std::array<unsigned,5> values{{0,0,1,1,0}};
    bool windowed() const { return values[0]==0; }
    double window_scale() const { constexpr double scales[]{1.0,1.5,1.6,2.0};return scales[values[1]]; }
    int width() const { constexpr int sizes[]{320,640,800,1024,1280,1280};return sizes[values[2]]; }
    int height() const { constexpr int sizes[]{240,480,600,768,800,1024};return sizes[values[2]]; }
    int color_depth() const { return values[3]==0?16:32; }
    bool software_rendering() const { return values[4]!=0; }
    std::string encode() const;
    static DisplaySettings read(const std::filesystem::path& path);
    void save(const std::filesystem::path& path) const;
};
int show_display_settings(const std::filesystem::path& userdata);
}
