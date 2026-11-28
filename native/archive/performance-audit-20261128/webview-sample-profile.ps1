param([ValidateSet('baseline','adapter')][string]$Variant)
$ErrorActionPreference='Stop'
$root='C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/webview-adapter'
$folder=if($Variant -eq 'baseline'){'profile-baseline'}else{'preview'}
$expected=[IO.Path]::GetFullPath("$root/$folder/touhouonline-profile.exe")
$owned=@(Get-CimInstance Win32_Process -Filter "Name='touhouonline-profile.exe'" | Where-Object ExecutablePath -EQ $expected)
if($owned.Count -ne 1){throw 'Expected one matching public-profile test process'}
$profilePid=[int]$owned[0].ProcessId
$samples=@(for($sampleIndex=0;$sampleIndex -lt 8;$sampleIndex++){
 $process=Get-Process -Id $profilePid -ErrorAction Stop
 [pscustomobject]@{atUtc=[DateTime]::UtcNow.ToString('o');cpuSeconds=$process.CPU;workingSet=$process.WorkingSet64;privateBytes=$process.PrivateMemorySize64;threads=$process.Threads.Count;handles=$process.HandleCount}
 Start-Sleep -Seconds 1
})
$all=@(Get-CimInstance Win32_Process -Filter "Name='msedgewebview2.exe'")
$tree=@();$parents=@($profilePid)
while($parents.Count){$children=@($all | Where-Object { $parents -contains $_.ParentProcessId });$tree+=$children;$parents=@($children | ForEach-Object ProcessId)}
$web=@(foreach($item in $tree){try{$process=Get-Process -Id $item.ProcessId -ErrorAction Stop;[pscustomobject]@{pid=$item.ProcessId;parentPid=$item.ParentProcessId;workingSet=$process.WorkingSet64;privateBytes=$process.PrivateMemorySize64}}catch{}})
$gpu=@(Get-CimInstance Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory | Where-Object Name -Like "pid_${profilePid}_*" | Select-Object Name,DedicatedUsage,SharedUsage,TotalCommitted)
$report=[pscustomobject]@{variant=$Variant;pid=$profilePid;image=$expected;samples=$samples;webProcesses=$web;gpu=$gpu}
$report | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath "$root/profile-$Variant-observed.json" -Encoding utf8
$report | Select-Object variant,pid,gpu,@{n='webProcessCount';e={$_.webProcesses.Count}},@{n='privateMiB';e={[math]::Round($_.samples[-1].privateBytes/1MB,1)}} | ConvertTo-Json -Depth 4
