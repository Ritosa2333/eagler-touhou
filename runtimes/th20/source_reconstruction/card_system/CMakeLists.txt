add_library(th20_card_system STATIC lifecycle.cpp update.cpp draw.cpp timing.cpp encoding.cpp start.cpp finish.cpp)
target_compile_features(th20_card_system PUBLIC cxx_std_20)
target_compile_options(th20_card_system PRIVATE /W4 /utf-8 /fp:strict /arch:SSE2)
target_link_libraries(th20_card_system PUBLIC th20_sprite_renderer th20_game_session th20_runtime_state th20_damage_regions th20_hud_system th20_effect_system th20_stage_completion)
