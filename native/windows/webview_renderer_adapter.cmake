# Keep WebView textures and plugin cleanup compatible with the pinned engine.
# Patch generated build inputs, leaving the Pub cache and SDK intact.
function(launcher_align_webview_adapter)
  set(_plugin flutter_inappwebview_windows_plugin)
  if(NOT TARGET ${_plugin})
    message(FATAL_ERROR "The launcher requires the Windows WebView plugin")
  endif()
  get_target_property(_upstream ${_plugin} SOURCE_DIR)
  file(STRINGS "${_upstream}/../pubspec.yaml" _version REGEX "^version:")
  if(NOT _version STREQUAL "version: 0.7.0-beta.3")
    message(FATAL_ERROR "Review the WebView adapter patch before changing its pinned version")
  endif()
  set(_inputs
    "custom_platform_view/graphics_context.h|4386aae0bd6c39e1e596162a1cf8af8cf3513e12abf3d740f5ee541f70e7f66b|413f931ebcda54889b0e4e68b25320e2d76430e4aef43a01d926df94e7ff092b"
    "custom_platform_view/graphics_context.cc|e1eba76fbfc804e1bf81fde0354f6ebd0be42ffbb423ef1ff3126be426765e3e|836ddcf95d748714e2f4c5fa324d446231b7bc6863f70174c15697f228b1df9a"
    "custom_platform_view/util/d3dutil.h|9ac80f75b468454a0f3436452243148c864559e41ee1dd9cd7e2cf4ea7fb2b56|cb160eaa9c0416edc2a4a3f659df22d1e9ab391e201af6142474b1b8dae3d0b0"
    "in_app_webview/in_app_webview_manager.cpp|371661fcca87a233db55608f2f9f090fdfee0dff1dab2414f9067606eb19ba5a|0124ad03a2c1ac8a4a7b874403145249037e8212ec09fcee0e53c4e23632575c"
    "in_app_webview/in_app_webview_manager.h|7fd060d7febc708da8b89cd84d47fae93a4c71591592282707661eacbbd2d444|029b426b7e6a91cf778ea408005b0f30f92f68968c8e508f5b5a247dd6498c36"
    "flutter_inappwebview_windows_plugin.cpp|e8b0c1ebe4758e900fac91fbd2faaf6ce41a94c8fdf73c20e479874a74f91407|04fe4649f3f16d8607cd45124c2da4f1d35b1e2c3d853e833261a23023883034"
    "custom_platform_view/texture_bridge.cc|03dec091996f07905b61fffb51e71dc499f9b3b080d11f3450f0cb5346171333|09cf6ea298be36f5adcaffdd1bb5859401da2f8afb5639efaeec1190e5a23071"
  )
  set(_work "${CMAKE_CURRENT_BINARY_DIR}/webview_adapter_input")
  set(_overlay "${CMAKE_CURRENT_BINARY_DIR}/webview_renderer_adapter")
  set(_paths)
  foreach(_record IN LISTS _inputs)
    string(REPLACE "|" ";" _parts "${_record}")
    list(GET _parts 0 _path)
    list(GET _parts 1 _expected)
    file(SHA256 "${_upstream}/${_path}" _actual)
    if(NOT _actual STREQUAL _expected)
      message(FATAL_ERROR "Unexpected upstream WebView source: ${_path}")
    endif()
    file(READ "${_upstream}/${_path}" _content)
    string(REPLACE "\r\n" "\n" _content "${_content}")
    get_filename_component(_parent "${_work}/${_path}" DIRECTORY)
    file(MAKE_DIRECTORY "${_parent}")
    file(WRITE "${_work}/${_path}" "${_content}")
    list(APPEND _paths "${_path}")
  endforeach()
  file(GLOB_RECURSE _upstream_files RELATIVE "${_upstream}" "${_upstream}/*")
  foreach(_path IN LISTS _upstream_files)
    if(NOT _path IN_LIST _paths)
      # Compare contents so an interrupted copy is repaired on the next build.
      configure_file("${_upstream}/${_path}" "${_overlay}/${_path}" COPYONLY)
    endif()
  endforeach()
  find_package(Git REQUIRED QUIET)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "GIT_CEILING_DIRECTORIES=${_work}"
      "${GIT_EXECUTABLE}" apply --whitespace=nowarn
      "${CMAKE_CURRENT_LIST_DIR}/webview-renderer-adapter.patch"
    WORKING_DIRECTORY "${_work}"
    RESULT_VARIABLE _result ERROR_VARIABLE _error)
  if(NOT _result EQUAL 0)
    message(FATAL_ERROR "Cannot apply WebView renderer adapter fix: ${_error}")
  endif()
  foreach(_record IN LISTS _inputs)
    string(REPLACE "|" ";" _parts "${_record}")
    list(GET _parts 0 _path)
    list(GET _parts 2 _expected)
    file(SHA256 "${_work}/${_path}" _actual)
    if(NOT _actual STREQUAL _expected)
      message(FATAL_ERROR "Unexpected patched WebView source: ${_path}")
    endif()
    # COPYONLY avoids recompiling these files when their contents are unchanged.
    configure_file("${_work}/${_path}" "${_overlay}/${_path}" COPYONLY)
  endforeach()
  # These sources inline manager accessors from the changed ownership header.
  set(_header_consumers cookie_manager.cpp platform_util.cpp in_app_webview/in_app_webview.cpp)
  get_target_property(_sources ${_plugin} SOURCES)
  set(_aligned_sources)
  foreach(_source IN LISTS _sources)
    if(NOT IS_ABSOLUTE "${_source}")
      set(_source "${_upstream}/${_source}")
    endif()
    file(RELATIVE_PATH _relative "${_upstream}" "${_source}")
    if(_relative IN_LIST _paths OR _relative IN_LIST _header_consumers)
      list(APPEND _aligned_sources "${_overlay}/${_relative}")
    else()
      list(APPEND _aligned_sources "${_source}")
    endif()
  endforeach()
  set_property(TARGET ${_plugin} PROPERTY SOURCES ${_aligned_sources})
endfunction()

launcher_align_webview_adapter()

# Plugins can remove handlers while Flutter is destroying their registrar.
# At that point the messenger can outlive its engine, as it does for late replies.
function(launcher_guard_plugin_messenger)
  get_target_property(_sources flutter_wrapper_plugin SOURCES)
  set(_guarded_sources)
  set(_found FALSE)
  foreach(_source IN LISTS _sources)
    get_filename_component(_name "${_source}" NAME)
    if(_name STREQUAL "core_implementations.cc")
      file(READ "${_source}" _content)
      string(REPLACE "\r\n" "\n" _content "${_content}")
      string(SHA256 _actual "${_content}")
      if(NOT _actual STREQUAL "854e39feb1e64cdb4766e74b7daa365c8307fd706308c73cc54ef32a460fb5b6")
        message(FATAL_ERROR "Review the messenger lifetime fix before changing Flutter SDK")
      endif()
      string(REPLACE
        "    FlutterDesktopMessengerSetCallback(messenger_, channel.c_str(), nullptr,\n                                       nullptr);"
        "    if (FlutterDesktopMessengerIsAvailable(messenger_)) {\n      FlutterDesktopMessengerSetCallback(messenger_, channel.c_str(), nullptr,\n                                         nullptr);\n    }"
        _content "${_content}")
      string(SHA256 _patched "${_content}")
      if(NOT _patched STREQUAL "b992469da4631798ab69a75534bfe10689553c7f4f4dcfb0b32f5f74bba66979")
        message(FATAL_ERROR "Unexpected messenger lifetime patch output")
      endif()
      set(_work "${CMAKE_CURRENT_BINARY_DIR}/messenger_lifetime_input.cc")
      set(_overlay "${CMAKE_CURRENT_BINARY_DIR}/messenger_lifetime/core_implementations.cc")
      file(WRITE "${_work}" "${_content}")
      configure_file("${_work}" "${_overlay}" COPYONLY)
      get_filename_component(_headers "${_source}" DIRECTORY)
      target_include_directories(flutter_wrapper_plugin PRIVATE "${_headers}")
      list(APPEND _guarded_sources "${_overlay}")
      set(_found TRUE)
    else()
      list(APPEND _guarded_sources "${_source}")
    endif()
  endforeach()
  if(NOT _found)
    message(FATAL_ERROR "Flutter plugin messenger wrapper source is unavailable")
  endif()
  set_property(TARGET flutter_wrapper_plugin PROPERTY SOURCES ${_guarded_sources})
endfunction()

launcher_guard_plugin_messenger()
