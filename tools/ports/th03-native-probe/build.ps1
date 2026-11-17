param(
    [Parameter(Mandatory = $true)][string]$Source,
    [Parameter(Mandatory = $true)][string]$Emsdk,
    [Parameter(Mandatory = $true)][string]$Output
)

$ErrorActionPreference = 'Stop'
$sourcePath = (Resolve-Path -LiteralPath $Source).Path
$sdkPath = (Resolve-Path -LiteralPath $Emsdk).Path
$compiler = Join-Path $sdkPath 'upstream/emscripten/em++.exe'
if (-not (Test-Path -LiteralPath $compiler)) { throw "Emscripten compiler missing: $compiler" }
$env:EM_CONFIG = Join-Path $sdkPath '.emscripten'
$sources = @('game.cpp', 'rollback_snapshot.cpp', 'frame.cpp', 'observer.cpp',
    'random.cpp', 'round_start.cpp', 'state_hash.cpp', 'trig.cpp') |
    ForEach-Object { Join-Path $sourcePath "src/core/$_" }
foreach ($file in $sources) {
    if (-not (Test-Path -LiteralPath $file)) { throw "Source file missing: $file" }
}
$outputPath = [System.IO.Path]::GetFullPath($Output)
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $outputPath) | Out-Null
& $compiler '-std=c++23' '-O1' "-I$sourcePath/include" @sources (Join-Path $PSScriptRoot 'probe.cpp') '-sENVIRONMENT=node' '-sSTACK_SIZE=2097152' '-o' $outputPath
if ($LASTEXITCODE -ne 0) { throw "Emscripten build failed: $LASTEXITCODE" }
& node $outputPath
if ($LASTEXITCODE -ne 0) { throw "WebAssembly probe failed: $LASTEXITCODE" }
$bridgePath = Join-Path (Split-Path -Parent $outputPath) 'th03-core.js'
$exports = '-sEXPORTED_FUNCTIONS=["_th03_create","_th03_step","_th03_frame","_th03_hash","_th03_save","_th03_restore","_th03_destroy"]'
& $compiler '-std=c++23' '-O1' "-I$sourcePath/include" @sources (Join-Path $PSScriptRoot 'bridge.cpp') '-sENVIRONMENT=web,node' '-sMODULARIZE=1' '-sEXPORT_NAME=createTh03Core' '-sSTACK_SIZE=2097152' $exports '-o' $bridgePath
if ($LASTEXITCODE -ne 0) { throw "WebAssembly bridge build failed: $LASTEXITCODE" }
& node (Join-Path $PSScriptRoot 'bridge-test.cjs') $bridgePath
if ($LASTEXITCODE -ne 0) { throw "WebAssembly bridge test failed: $LASTEXITCODE" }
