add_library(th20_ending_scene STATIC lifecycle.cpp records.cpp resources.cpp update.cpp text.cpp script.cpp)
target_compile_features(th20_ending_scene PUBLIC cxx_std_20)
target_compile_options(th20_ending_scene PRIVATE /W4 /utf-8 /fp:strict /arch:SSE2)
target_link_libraries(th20_ending_scene PUBLIC th20_sprite_renderer th20_text_renderer th20_progress_state th20_hud_system th20_trophy_system th20_screen_effect)
