param(
    [string]$SourceExecutable = 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe'
)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path $PSScriptRoot -Parent
$taskCache = Join-Path $taskRoot '.cache'
$taskThirdParty = Join-Path $taskRoot 'third_party'
$taskGhidraZip = Join-Path $taskCache 'ghidra_11.4.2_PUBLIC_20250826.zip'
$taskJdkZip = Join-Path $taskCache 'jdk-21_windows-x64_bin.zip'
$taskGhidraExpected = '795a02076af16257bd6f3f4736c4fc152ce9ff1f95df35cd47e2adc086e037a6'
$taskJdkExpected = '9f5d3f285558bef31744514232af6647377d1b7ba42358580f9f99bbb9ff270d'
if ((Get-FileHash -LiteralPath $taskGhidraZip -Algorithm SHA256).Hash.ToLowerInvariant() -ne $taskGhidraExpected) { throw 'Ghidra official SHA256 mismatch' }
if ((Get-FileHash -LiteralPath $taskJdkZip -Algorithm SHA256).Hash.ToLowerInvariant() -ne $taskJdkExpected) { throw 'Oracle JDK official SHA256 mismatch' }
$taskGhidraDir = Join-Path $taskThirdParty 'ghidra_11.4.2_PUBLIC'
if (-not (Test-Path -LiteralPath (Join-Path $taskGhidraDir 'support\analyzeHeadless.bat'))) {
    python -m zipfile -e $taskGhidraZip $taskThirdParty
    if ($LASTEXITCODE -ne 0) { throw 'Ghidra extraction failed' }
}
$taskJdkRoot = Join-Path $taskThirdParty 'ghidra_jdk'
if (-not (Test-Path -LiteralPath $taskJdkRoot)) {
    python -m zipfile -e $taskJdkZip $taskJdkRoot
    if ($LASTEXITCODE -ne 0) { throw 'JDK extraction failed' }
}
$taskJdk = Get-ChildItem -LiteralPath $taskJdkRoot -Directory | Select-Object -First 1
$env:JAVA_HOME = $taskJdk.FullName
$env:PATH = "$($taskJdk.FullName)\bin;$env:PATH"
$taskAnalysis = Join-Path $taskRoot 'analysis\ghidra'
$taskProject = Join-Path $taskRoot 'analysis\ghidra_project'
New-Item -ItemType Directory -Path $taskAnalysis,$taskProject -Force | Out-Null
$taskHeadless = Join-Path $taskGhidraDir 'support\analyzeHeadless.bat'
& $taskHeadless $taskProject 'Touhou20' -import $SourceExecutable -overwrite -analysisTimeoutPerFile 1200 -max-cpu 4 -scriptPath (Join-Path $PSScriptRoot 'ghidra') -postScript 'ExportEvidence.java' $taskAnalysis -log (Join-Path $taskAnalysis 'analysis.log') -scriptlog (Join-Path $taskAnalysis 'script.log')
if ($LASTEXITCODE -ne 0) { throw "Ghidra failed with exit code $LASTEXITCODE" }
$taskEclFailure = Import-Csv -LiteralPath (Join-Path $taskAnalysis 'decompile_failures.csv') | Where-Object entry_va -eq '0x0048c010'
if ($taskEclFailure) {
    & $taskHeadless $taskProject 'Touhou20' -process 'th20.exe' -noanalysis -max-cpu 4 -scriptPath (Join-Path $PSScriptRoot 'ghidra') -postScript 'RetryEcl.java' $taskAnalysis -log (Join-Path $taskAnalysis 'ecl_retry.log')
    if ($LASTEXITCODE -ne 0) { throw "Ghidra ECL retry failed with exit code $LASTEXITCODE" }
    python (Join-Path $PSScriptRoot 'binary_merge_retry.py')
    if ($LASTEXITCODE -ne 0) { throw 'Failed to merge ECL retry statistics' }
}
python (Join-Path $PSScriptRoot 'binary_ghidra_index.py')
if ($LASTEXITCODE -ne 0) { throw 'Failed to validate exported pseudocode and create indices' }
Get-Content -LiteralPath (Join-Path $taskAnalysis 'summary.properties')
