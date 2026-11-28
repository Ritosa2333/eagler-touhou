$ErrorActionPreference='Stop'
$adapterResults=@(foreach($caseName in @('default','nvidia','default','nvidia','default','nvidia')) {
 $adapterWatch=[Diagnostics.Stopwatch]::StartNew()
 $adapterOutput=& ".\adapter-$caseName.exe"
 [pscustomobject]@{variant=$caseName;exitCode=$LASTEXITCODE;milliseconds=[math]::Round($adapterWatch.Elapsed.TotalMilliseconds,2);output=$adapterOutput}
})
$gpuMemory=@(Get-CimInstance -Namespace root/cimv2 -ClassName Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory | Where-Object Name -Like 'pid_39464_*' | Select-Object Name,DedicatedUsage,SharedUsage,TotalCommitted)
$gpuEngines=@(Get-CimInstance -Namespace root/cimv2 -ClassName Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine | Where-Object Name -Like 'pid_39464_*' | Select-Object Name,UtilizationPercentage)
[pscustomobject]@{probe=$adapterResults;ordinaryPid=39464;processMemory=$gpuMemory;processEngines=$gpuEngines} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath '.\adapter-results.json' -Encoding utf8
$adapterResults | ConvertTo-Json
