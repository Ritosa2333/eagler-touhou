#pragma once

#include "TitleState.hpp"

#include <cstdio>
#include <string>

namespace th13 {

inline constexpr i32 kTitleScreenManual = 16;

struct TitleManualPlatform {
    virtual ~TitleManualPlatform() = default;
    virtual bool open_manual() { return false; }
    virtual bool update_manual(u32 pressed, u32 repeated) {
        (void)pressed;
        (void)repeated;
        return true;
    }
    virtual bool manual_finished() const { return false; }
    virtual void close_manual() {}
    virtual std::string help_page_name(i32 page) const {
        char buffer[32];
        std::snprintf(buffer, sizeof buffer, "help_%02d.png", int(page) + 1);
        return buffer;
    }
};

class TitleManualMenu {
    TitleState& state;
    TitleManualPlatform& host;
    bool fail(const std::string& reason);

public:
    std::string error;
    TitleManualMenu(TitleState& state_, TitleManualPlatform& host_) : state(state_), host(host_) {}
    bool update(u32 pressed, u32 repeated);
};

}
