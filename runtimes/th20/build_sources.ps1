param(
    [switch]$Validate,
    [string]$GameDirectory = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders'
)
$ErrorActionPreference = 'Stop'
$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
$cmakeExecutable = if ($cmakeCommand) { $cmakeCommand.Source } else { 'D:\cmake\bin\cmake.exe' }
$ctestExecutable = Join-Path (Split-Path $cmakeExecutable) 'ctest.exe'
$oracleOption = if ($Validate) { 'ON' } else { 'OFF' }
Push-Location $PSScriptRoot
try {
    & $cmakeExecutable -S source_reconstruction -B build_sources -G 'Visual Studio 16 2019' -A Win32 "-DTH20_BUILD_ORACLE=$oracleOption" "-DTH20_BUILD_ECL_ORACLE=$oracleOption" "-DTH20_BUILD_WINDOW_ORACLE=$oracleOption" "-DTH20_BUILD_RUNTIME_ORACLE=$oracleOption" "-DTH20_BUILD_INPUT_ORACLE=$oracleOption" "-DTH20_BUILD_AUDIO_ORACLE=$oracleOption" "-DTH20_BUILD_GAMEPLAY_ORACLE=$oracleOption"
    if ($LASTEXITCODE) { throw 'Source module configuration failed' }
    & $cmakeExecutable --build build_sources --config Release
    if ($LASTEXITCODE) { throw 'Source module compilation failed' }
    & $ctestExecutable --test-dir build_sources -C Release --output-on-failure
    if ($LASTEXITCODE) { throw 'Independent source tests failed' }
    if ($Validate) {
        & ./build_sources/archive/Release/th20_source_archive_verify.exe (Join-Path $GameDirectory 'th20.dat') ./assets/raw ./source_reconstruction/archive/archive_validation.json
        if ($LASTEXITCODE) { throw 'C++ archive bytes differ from independent extraction' }
        & ./build_sources/Release/th20_resource_manager_tests.exe (Join-Path $GameDirectory 'th20.dat') ./assets/raw ./source_reconstruction/archive/manager_validation.json
        if ($LASTEXITCODE) { throw 'C++ archive manager lifecycle or members differ' }
        & ./build_sources/runtime_state/Release/th20_state_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/runtime_state/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ shared RNG state differs from original isolated routines' }
        & ./build_sources/runtime_state/Release/th20_motion_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/runtime_state/motion_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ shared motion differs from original isolated routines' }
        & ./build_sources/core_scheduler/Release/th20_scheduler_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/core_scheduler/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ scheduler differs from original isolated routines' }
        & ./build_sources/platform_services/Release/th20_platform_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/platform_services/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ configuration/clock differs from original isolated routines' }
        & ./build_sources/platform_window/Release/th20_window_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/platform_window/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ window layout differs from original isolated routines' }
        & ./build_sources/runtime_core/Release/th20_runtime_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/runtime_core/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ runtime differs from original isolated routines' }
        & ./build_sources/ecl_vm/Release/th20_ecl_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/ecl_vm/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ ECL VM differs from original isolated routines' }
        & ./build_sources/input/Release/th20_input_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/input/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ input state differs from original isolated routines' }
        & ./build_sources/sprite_renderer/Release/th20_sprite_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/sprite_renderer/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ sprite/animation state differs from original isolated routines' }
        & ./build_sources/sprite_renderer/Release/th20_controller_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/sprite_renderer/controller_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ full animation controller construction or teardown differs from original' }
        & ./build_sources/sprite_renderer/Release/th20_file_preload_tests.exe (Join-Path $GameDirectory 'th20.dat') ./scripts/recovered/anm ./source_reconstruction/sprite_renderer/file_preload_validation.json
        if ($LASTEXITCODE) { throw 'C++ ANM resource preload differs from independent DSL' }
        & ./build_sources/sprite_renderer/Release/th20_texture_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./assets/raw ./source_reconstruction/sprite_renderer/texture_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ texture loading differs from original on the real D3D9 device' }
        & ./build_sources/game_session/Release/th20_session_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/game_session/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ game session differs from original isolated routines' }
        & ./build_sources/gameplay/Release/th20_gameplay_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/gameplay/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ GameController differs from original isolated routines' }
        & ./build_sources/gameplay/Release/th20_player_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/gameplay/player_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ player state differs from original isolated routines' }
        & ./build_sources/gameplay/Release/th20_enemy_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/gameplay/enemy_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ enemy controller differs from original isolated routines' }
        & ./build_sources/gameplay/Release/th20_enemy_resource_tests.exe (Join-Path $GameDirectory 'th20.dat') ./source_reconstruction/gameplay/enemy_resource_validation.json
        if ($LASTEXITCODE) { throw 'C++ enemy script loading differs from independent ECL parsing' }
        & ./build_sources/stage_background/Release/th20_background_state_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/stage_background/state_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ stage background state differs from original isolated routines' }
        & ./build_sources/stage_background/Release/th20_background_vm_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/stage_background/vm_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ stage background VM differs from original isolated routines' }
        & ./build_sources/screen_effect/Release/th20_screen_effect_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/screen_effect/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ screen effects differ from original isolated routines' }
        & ./build_sources/bomb_system/Release/th20_bomb_state_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/bomb_system/state_cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ bomb state differs from original isolated routines' }
        & ./build_sources/gameplay/Release/th20_enemy_frame_tests.exe ./source_reconstruction/gameplay/enemy_frame_validation.json
        if ($LASTEXITCODE) { throw 'C++ enemy frame sequencing tests failed' }
        & ./build_sources/audio_runtime/Release/th20_audio_cpu_compare.exe (Join-Path $GameDirectory 'th20.exe') ./source_reconstruction/audio_runtime/cpu_validation.json
        if ($LASTEXITCODE) { throw 'C++ audio differs from original isolated routines' }
    }
    Write-Host 'Independent source modules compiled. The complete game is NOT implemented or linked; see source_reconstruction/README.md.'
} finally {
    Pop-Location
}
