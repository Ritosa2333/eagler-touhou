param(
    [string]$OriginalExe = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe',
    [string]$OriginalArchive = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.dat',
    [string]$CMake = 'D:\cmake\bin\cmake.exe'
)
$ErrorActionPreference = 'Stop'
$buildDirectory = Join-Path $PSScriptRoot 'build'
$reportDirectory = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$reportPath = Join-Path $reportDirectory 'music_parser_validation.json'
& $CMake -S $PSScriptRoot -B $buildDirectory -G 'Visual Studio 16 2019' -A Win32 '-DCMAKE_SYSTEM_VERSION=10.0.19041.0'
if ($LASTEXITCODE -ne 0) { throw 'Music parser oracle configuration failed.' }
& $CMake --build $buildDirectory --config Release --target th20_music_parser_cpu_compare -j 4
if ($LASTEXITCODE -ne 0) { throw 'Music parser oracle build failed.' }
& (Join-Path $buildDirectory 'Release/th20_music_parser_cpu_compare.exe') $OriginalExe $OriginalArchive $reportPath
if ($LASTEXITCODE -ne 0) { throw "Music parser oracle failed with exit code $LASTEXITCODE." }
$report = Get-Content -LiteralPath $reportPath -Raw | ConvertFrom-Json
foreach ($entry in $report.source_hashes.PSObject.Properties) {
    $sourcePath = [IO.Path]::GetFullPath((Join-Path $reportDirectory $entry.Name))
    $actualHash = (Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actualHash -ne $entry.Value) { throw "Source changed after compilation: $sourcePath" }
}
if ($report.failed -ne 0 -or $report.full_parser_cases -ne $report.requested_parser_cases) { throw 'Music parser validation is incomplete.' }
Write-Output "Music parser: $($report.passed) checks passed; build-time source hashes match. Report: $reportPath"
