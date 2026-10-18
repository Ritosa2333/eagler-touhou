add_library(th20_trophy_system STATIC codec.cpp queue.cpp resources.cpp lifecycle.cpp records.cpp update.cpp)
target_compile_features(th20_trophy_system PUBLIC cxx_std_20)
target_compile_options(th20_trophy_system PRIVATE /W4 /utf-8 /fp:strict /arch:SSE2)
target_link_libraries(th20_trophy_system PUBLIC th20_sprite_renderer th20_text_renderer th20_progress_state th20_audio_runtime)
