# Opt-in fixture for the production TH06 multiplayer objects.
if(NOT TH_ENABLE_NETPLAY OR NOT TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    message(FATAL_ERROR "The Miss feedback browser fixture requires multiplayer netplay")
endif()
set(TH06_MISS_FEEDBACK_FIXTURE "${CMAKE_CURRENT_LIST_DIR}/multiplayer-miss-feedback-browser.cpp")
function(attach_th06_miss_feedback_fixture)
    target_sources(th06 PRIVATE "${TH06_MISS_FEEDBACK_FIXTURE}")
endfunction()
cmake_language(DEFER CALL attach_th06_miss_feedback_fixture)
