param(
    [string]$CMake = 'D:\cmake\bin\cmake.exe'
)
$ErrorActionPreference = 'Stop'
$assetRoot = Split-Path -Parent $PSScriptRoot
$assetSource = Join-Path $assetRoot 'third_party\thtk'
$assetFlex = Join-Path $PSScriptRoot 'asset_win_flex_bison'
if (-not (Test-Path -LiteralPath (Join-Path $assetSource 'CMakeLists.txt'))) {
    git clone https://github.com/thpatch/thtk.git $assetSource
    if ($LASTEXITCODE -ne 0) { throw 'Cannot clone thtk' }
}
git -C $assetSource checkout 892114a0fcaa0bbdaaecf3cb4ad56f758683fb40
if ($LASTEXITCODE -ne 0) { throw 'Cannot select pinned thtk source revision' }
git -C $assetSource submodule update --init --depth 1
if ($LASTEXITCODE -ne 0) { throw 'Cannot initialize thtk dependencies' }
$assetPatch = Join-Path $PSScriptRoot 'asset_thtk_th20.patch'
git -C $assetSource apply --reverse --check $assetPatch 2>$null
if ($LASTEXITCODE -ne 0) {
    git -C $assetSource apply --check $assetPatch
    if ($LASTEXITCODE -ne 0) { throw 'TH20 patch does not match this source tree; inspect local changes' }
    git -C $assetSource apply $assetPatch
    if ($LASTEXITCODE -ne 0) { throw 'Cannot apply TH20 compatibility patch' }
}
if (-not (Test-Path -LiteralPath (Join-Path $assetFlex 'win_bison.exe'))) {
    $assetZip = Join-Path $PSScriptRoot 'asset_win_flex_bison-2.5.25.zip'
    Invoke-WebRequest -Uri 'https://github.com/lexxmark/winflexbison/releases/download/v2.5.25/win_flex_bison-2.5.25.zip' -OutFile $assetZip
    Expand-Archive -LiteralPath $assetZip -DestinationPath $assetFlex -Force
}
& $CMake -S $assetSource -B (Join-Path $assetSource 'build') -G 'Visual Studio 16 2019' -A x64 "-DBISON_EXECUTABLE=$assetFlex\win_bison.exe" "-DFLEX_EXECUTABLE=$assetFlex\win_flex.exe" -DBUILD_SHARED_LIBS=OFF -DWITH_OPENMP=OFF -DWITH_LIBPNG_SOURCE=ON
if ($LASTEXITCODE -ne 0) { throw 'Cannot configure thtk' }
& $CMake --build (Join-Path $assetSource 'build') --config Release --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'Cannot build thtk' }
$assetIndexSource = Join-Path $PSScriptRoot 'asset_index'
$assetIndexBuild = Join-Path $assetIndexSource 'build'
& $CMake -S $assetIndexSource -B $assetIndexBuild -G 'Visual Studio 16 2019' -A x64
if ($LASTEXITCODE -ne 0) { throw 'Cannot configure archive index adapter' }
& $CMake --build $assetIndexBuild --config Release --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'Cannot build archive index adapter' }
