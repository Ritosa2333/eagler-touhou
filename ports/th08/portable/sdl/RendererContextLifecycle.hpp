#pragma once

namespace touhou::sdl::detail {

template<class Flags, class IsInitialized, class Initialize>
bool initialize_missing_subsystems(Flags required, IsInitialized&& is_initialized,
                                   Initialize&& initialize) {
    const Flags missing = required & ~is_initialized(required);
    return !missing || initialize(missing);
}

// GL object names are valid only while their owning context exists and is
// current. Keeping this boundary generic makes partial initialization and
// context re-selection testable without a browser GL implementation.
template<class Window, class Context, class MakeCurrent, class ReleaseObjects>
bool release_with_current_context(Window* window, Context context,
                                  MakeCurrent&& make_current,
                                  ReleaseObjects&& release_objects) {
    if (!window || !context || !make_current(window, context)) return false;
    release_objects();
    return true;
}

} // namespace touhou::sdl::detail
