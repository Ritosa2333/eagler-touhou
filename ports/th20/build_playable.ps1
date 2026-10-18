param(
    [string]$GameDirectory = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders'
)
$ErrorActionPreference = 'Stop'
$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
$cmakeExecutable = if ($cmakeCommand) { $cmakeCommand.Source } else { 'D:\cmake\bin\cmake.exe' }
$pythonCommand = Get-Command python -ErrorAction SilentlyContinue
$pythonExecutable = if ($pythonCommand) { $pythonCommand.Source } else { 'C:\Users\oracat\AppData\Local\Programs\Python\Python312\python.exe' }
Push-Location $PSScriptRoot
try {
    & $cmakeExecutable -S incremental/native_bridge -B incremental/native_bridge/build -G 'Visual Studio 16 2019' -A Win32
    if ($LASTEXITCODE) { throw 'Bridge CMake configuration failed' }
    & $cmakeExecutable --build incremental/native_bridge/build --config Release
    if ($LASTEXITCODE) { throw 'Bridge compilation failed' }
    & ./incremental/native_bridge/build/Release/th20_bridge_abi_check.exe (Join-Path $GameDirectory 'th20.exe') ./incremental/native_bridge/build/Release/th20_native_bridge.dll ./incremental/native_bridge/bridge_abi_validation.json
    if ($LASTEXITCODE) { throw 'Shipped C++ DLL differs from original function ABI/behavior' }
    & $pythonExecutable tools/build_playable.py --game-dir $GameDirectory
    if ($LASTEXITCODE) { throw 'Incremental game linking or package verification failed' }
    & $cmakeExecutable -S incremental/link_tests -B incremental/link_tests/build -G 'Visual Studio 16 2019' -A Win32
    if ($LASTEXITCODE) { throw 'Final PE linkage test configuration failed' }
    & $cmakeExecutable --build incremental/link_tests/build --config Release
    if ($LASTEXITCODE) { throw 'Final PE linkage test compilation failed' }
    & ./incremental/link_tests/build/Release/th20_playable_link_check.exe (Join-Path $GameDirectory 'th20.exe') ./dist/playable/th20_rebuilt.exe ./dist/playable/th20_native_bridge.dll ./reports/playable_link_validation.json
    if ($LASTEXITCODE) { throw 'Final executable linkage differs from original CPU behavior' }
    Write-Host 'Playable incremental build: dist/playable/Play.cmd (retains original engine machine code).'
} finally {
    Pop-Location
}
