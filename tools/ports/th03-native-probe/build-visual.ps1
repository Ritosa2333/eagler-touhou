param(
    [Parameter(Mandatory = $true)][string]$Source,
    [Parameter(Mandatory = $true)][string]$Emsdk,
    [Parameter(Mandatory = $true)][string]$Cmake,
    [Parameter(Mandatory = $true)][string]$Ninja,
    [Parameter(Mandatory = $true)][string]$Build,
    [Parameter(Mandatory = $true)][string]$Content,
    [Parameter(Mandatory = $true)][string]$Chrome
)
$ErrorActionPreference = 'Stop'
$revision = '0139181441e9eb895a311da66aa08463ba98ac54'
$sourcePath = (Resolve-Path -LiteralPath $Source).Path
$sdkPath = (Resolve-Path -LiteralPath $Emsdk).Path
$buildPath = [IO.Path]::GetFullPath($Build)
$actual = (& git -C $sourcePath rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or $actual -ne $revision) { throw "Expected th98port revision $revision, got $actual" }
$workspace = Join-Path $buildPath 'source'
New-Item -ItemType Directory -Force -Path $workspace | Out-Null
& git -C $sourcePath archive --format=tar $revision -o (Join-Path $buildPath 'source.tar')
if ($LASTEXITCODE -ne 0) { throw 'Source archive failed' }
& tar -xf (Join-Path $buildPath 'source.tar') -C $workspace
if ($LASTEXITCODE -ne 0) { throw 'Source extraction failed' }
$cmakeFile = Join-Path $workspace 'CMakeLists.txt'
$original = [IO.File]::ReadAllText($cmakeFile)
$modified = [regex]::Replace($original, 'if\(NOT WIN32\)(\s+find_package\(CURL REQUIRED\))', 'if(NOT WIN32 AND NOT EMSCRIPTEN)$1')
if ($original -eq $modified) { throw 'Expected CURL configuration not found' }
[IO.File]::WriteAllText($cmakeFile, $modified)
& python (Join-Path $PSScriptRoot 'apply-assists.py') $workspace
if ($LASTEXITCODE -ne 0) { throw 'TH03 assist patch failed' }
$env:EM_CONFIG = Join-Path $sdkPath '.emscripten'
$obj = Join-Path $buildPath 'objects'
& $Cmake -S $workspace -B $obj -G Ninja "-DCMAKE_MAKE_PROGRAM=$Ninja" "-DCMAKE_TOOLCHAIN_FILE=$sdkPath/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DTH03_WARNINGS_AS_ERRORS=OFF
if ($LASTEXITCODE -ne 0) { throw 'Configure failed' }
& $Cmake --build $obj --target th03_app_main --parallel 6
if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
$libraries = Get-ChildItem -LiteralPath $obj -Filter '*.a' | ForEach-Object FullName
$output = Join-Path $buildPath 'visual.js'
& (Join-Path $sdkPath 'upstream/emscripten/em++.exe') '-std=c++23' '-O1' "-I$workspace/include" (Join-Path $PSScriptRoot 'visual.cpp') '-Wl,--start-group' @libraries '-Wl,--end-group' '-sMODULARIZE=1' '-sEXPORT_NAME=createTh03Visual' '-sENVIRONMENT=web,node' '-sFORCE_FILESYSTEM=1' '-sEXPORTED_RUNTIME_METHODS=["FS","HEAPU8"]' '-sEXPORTED_FUNCTIONS=["_th03_visual_create","_th03_visual_step","_th03_visual_pixels","_th03_visual_destroy"]' '-sSTACK_SIZE=4194304' '-sALLOW_MEMORY_GROWTH=1' '-o' $output
if ($LASTEXITCODE -ne 0) { throw 'Visual link failed' }
& (Join-Path $sdkPath 'upstream/emscripten/em++.exe') '-std=c++23' '-O2' "-I$workspace/include" (Join-Path $PSScriptRoot 'classic.cpp') '-Wl,--start-group' @libraries '-Wl,--end-group' '-sMODULARIZE=1' '-sEXPORT_NAME=createTh03Classic' '-sENVIRONMENT=web,node' '-sFORCE_FILESYSTEM=1' '-sEXPORTED_RUNTIME_METHODS=["FS","HEAPU8","HEAP32"]' '-sEXPORTED_FUNCTIONS=["_th03_classic_create","_th03_classic_external_music","_th03_classic_step","_th03_classic_step_pair","_th03_classic_pixels","_th03_classic_audio","_th03_classic_audio_frames","_th03_classic_destroy","_th03_classic_assist","_th03_classic_assist_pair","_th03_classic_touch","_th03_classic_point_x","_th03_classic_point_y","_th03_classic_point_x2","_th03_classic_point_y2","_th03_classic_scene","_th03_classic_start_versus","_th03_classic_boot_state","_th03_classic_paused","_th03_classic_surrender_allowed","_th03_classic_surrender","_th03_semantic_capture","_th03_semantic_restore","_th03_semantic_state_hash","_th03_semantic_round","_th03_selection_index","_th03_classic_match_serial","_th03_classic_match_winner","_th03_semantic_replay_samples","_th03_classic_capture","_th03_classic_restore","_th03_classic_defer_audio","_th03_classic_speculative","_th03_classic_boundary","_th03_classic_audio_commands","_th03_classic_audio_command_count","_th03_classic_audio_commit","_th03_classic_replay_serial","_th03_classic_replay_data","_th03_classic_replay_size","_th03_classic_charge_enable","_th03_classic_charge_set","_th03_classic_charge_get"]' '-sSTACK_SIZE=4194304' '-sALLOW_MEMORY_GROWTH=1' '-o' (Join-Path $buildPath 'classic.js')
if ($LASTEXITCODE -ne 0) { throw 'Classic link failed' }
& node (Join-Path $PSScriptRoot 'visual-test.cjs') $output $Content
if ($LASTEXITCODE -ne 0) { throw 'Node visual test failed' }
& node (Join-Path $PSScriptRoot 'visual-browser-test.cjs') $buildPath $Content $Chrome
if ($LASTEXITCODE -ne 0) { throw 'Browser visual test failed' }
