param(
    [string]$SourceExe = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe',
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot)
)
$ErrorActionPreference = 'Stop'
$expected = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
$actual = (Get-FileHash -LiteralPath $SourceExe -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actual -cne $expected) { throw 'EXE differs from the analyzed revision; hardware oracle was not started.' }
$program = Join-Path $ProjectRoot 'build_cpu\Release\th20_native_cpu_compare.exe'
$report = Join-Path $ProjectRoot 'reports\native_cpu_validation.json'
& $program $SourceExe $report
if ($LASTEXITCODE -ne 0) { throw "Native hardware comparison returned $LASTEXITCODE; inspect $report" }
