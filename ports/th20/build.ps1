param(
    [switch]$Validate,
    [string]$GameDirectory = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders'
)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    & cmake -S . -B build -G 'Visual Studio 16 2019' -A x64
    if ($LASTEXITCODE) { throw 'CMake configuration failed' }
    & cmake --build build --config Release
    if ($LASTEXITCODE) { throw 'C++ build failed' }
    & ctest --test-dir build -C Release --output-on-failure
    if ($LASTEXITCODE) { throw 'C++ parser tests failed' }
    if ($Validate) {
        & python tools/source_manifest.py --source $GameDirectory --verify
        if ($LASTEXITCODE) { throw 'Original files differ from analysis baseline' }
        & ./build/Release/th20_inspect.exe ecl-scan assets/raw reports/ecl_binary_scan.json
        if ($LASTEXITCODE) { throw 'ECL binary scan failed' }
        & python tools/verify_artifacts.py
        if ($LASTEXITCODE) { throw 'Recovered artifact hashes differ' }
        & python tools/validate_native.py --exe (Join-Path $GameDirectory 'th20.exe') --dll build/native_recovered/Release/th20_native_recovered.dll
        if ($LASTEXITCODE) { throw 'Original-machine-code differential tests failed' }
        & cmake -S tests/native_cpu -B build_cpu -G 'Visual Studio 16 2019' -A Win32
        if ($LASTEXITCODE) { throw 'CPU comparison tool configuration failed' }
        & cmake --build build_cpu --config Release
        if ($LASTEXITCODE) { throw 'CPU comparison tool build failed' }
        & ./tests/validate_native_cpu.ps1 -SourceExe (Join-Path $GameDirectory 'th20.exe')
    }
    Write-Host 'Analysis tools built. For the playable incremental game package, run ./build_playable.ps1.'
} finally {
    Pop-Location
}
