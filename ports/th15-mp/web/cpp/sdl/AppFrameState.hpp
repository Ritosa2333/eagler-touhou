#pragma once
#include <memory>
#include <string>
namespace th15::sdl {
struct ApplicationState;
class AppFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
    static auto fields(ApplicationState&);
public:
    mutable std::string error;
    AppFrameState();~AppFrameState();
    AppFrameState(AppFrameState&&)noexcept;
    AppFrameState& operator=(AppFrameState&&)noexcept;
    static bool ready(const ApplicationState&);
    bool capture(ApplicationState&);
    bool restore(ApplicationState&)const;
    void discard()noexcept;
};
}
