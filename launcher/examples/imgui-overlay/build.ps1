param(
    [string]$Output = 'D:/workspace/eagler/artifacts/th09-windows/imgui-overlay',
    [string]$Sdl = "$env:TEMP/th07-overlay/prefix",
    [string]$Json = 'D:/workspace/eagler/THL/.local/netlab/libdatachannel/deps/json/include',
    [string]$Compiler = 'C:/msys64/ucrt64/bin/g++.exe'
)
$ErrorActionPreference = 'Stop'
$memory = Get-CimInstance Win32_PerfFormattedData_PerfOS_Memory
if ($memory.CommitLimit - $memory.CommittedBytes -lt 2GB) { throw 'Windows commit headroom is below 2 GiB.' }
$repo = (Resolve-Path "$PSScriptRoot/../../..").Path
$imgui = "$repo/common/third_party/imgui"
New-Item -ItemType Directory -Force $Output | Out-Null
$include = "$Output/third-party"
New-Item -ItemType Directory -Force $include | Out-Null
$stb = "$include/imstb_truetype.h"
if (!(Test-Path -LiteralPath $stb)) {
    Invoke-WebRequest 'https://raw.githubusercontent.com/ocornut/imgui/v1.82/imstb_truetype.h' -OutFile $stb
}
if ((Get-FileHash $stb).Hash -ne 'BB24084056CD1FE17078B755CBD600D23449173D6A927AD7F067D3CE6FBDD1F0') { throw 'Unexpected stb_truetype header hash' }
$flags = @("-I$include", "-I$PSScriptRoot", '-std=c++20', '-O2', '-DNDEBUG', '-DIMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS',
    '-DIMGUI_DISABLE_INCLUDE_IMCONFIG_H', '-DIMGUI_USE_BGRA_PACKED_COLOR', '-DIMGUI_INCLUDE_IMGUI_USER_H',
    '-DIMGUI_INCLUDE_IMGUI_USER_INL', '-DIMGUI_USER_CONFIG="overlay-imconfig.h"', '-DImDrawIdx=unsigned int',
    "-I$imgui", "-I$repo/common/include", "-I$Sdl/include", "-I$Json")
$imguiSources = @('imgui', 'imgui_draw', 'imgui_tables', 'imgui_widgets') | ForEach-Object { "$imgui/$_.cpp" }
& $Compiler @flags "$PSScriptRoot/main.cpp" @imguiSources "-L$Sdl/lib" '-lSDL3' '-static-libgcc' '-static-libstdc++' '-o' "$Output/th09-imgui-overlay.exe"
if ($LASTEXITCODE) { throw "ImGui build failed ($LASTEXITCODE)" }
Copy-Item "$Sdl/bin/SDL3.dll","$(Split-Path $Compiler)/libwinpthread-1.dll" $Output -Force
& ffmpeg -v error -y -i "$repo/launcher/app/components/launcher/book-covers/th09.webp" -vf scale=480:-1 "$Output/th09-cover.bmp"
if ($LASTEXITCODE) { throw "Cover conversion failed ($LASTEXITCODE)" }
& bun "$PSScriptRoot/bridge.mts" --fixtures $Output
if ($LASTEXITCODE) { throw "Fixture generation failed ($LASTEXITCODE)" }
