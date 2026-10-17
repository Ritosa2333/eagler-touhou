add_library(th20_stage_clear STATIC lifecycle.cpp bonus.cpp update.cpp draw.cpp)
target_compile_features(th20_stage_clear PUBLIC cxx_std_20)
target_compile_options(th20_stage_clear PRIVATE /W4 /utf-8 /fp:strict /arch:SSE2)
target_link_libraries(th20_stage_clear PUBLIC th20_sprite_renderer th20_game_session th20_runtime_state th20_damage_regions th20_hud_system th20_effect_system)
