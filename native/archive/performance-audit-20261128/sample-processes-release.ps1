$ErrorActionPreference='Stop'
$taskRoot='D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf'
$rows=[Collections.Generic.List[object]]::new()
$clock=[Diagnostics.Stopwatch]::StartNew()
for($sample=0;$sample -lt 60;$sample++) {
  $procs=Get-CimInstance Win32_Process | Where-Object { $_.Name -in @('touhouonline.exe','msedgewebview2.exe') }
  $roots=@($procs | Where-Object { $_.Name -eq 'touhouonline.exe' -and $_.ExecutablePath -like '*\exhaustive-release-app\*' })
  $ids=[Collections.Generic.HashSet[int]]::new()
  foreach($p in $roots){[void]$ids.Add([int]$p.ProcessId)}
  for($depth=0;$depth -lt 5;$depth++) {foreach($p in $procs){if($ids.Contains([int]$p.ParentProcessId)){[void]$ids.Add([int]$p.ProcessId)}}}
  $phase='building'
  if(Test-Path "$taskRoot/exhaustive-live.json") {try{$phase=(Get-Content "$taskRoot/exhaustive-live.json" -Raw | ConvertFrom-Json).phase}catch{}}
  $measurements=@(foreach($p in $procs) {
    if(!$ids.Contains([int]$p.ProcessId)){continue}
    $live=Get-Process -Id $p.ProcessId -ErrorAction SilentlyContinue
    if($null -ne $live){[pscustomobject]@{id=$live.Id;parent=$p.ParentProcessId;name=$live.ProcessName;cpuSeconds=$live.TotalProcessorTime.TotalSeconds;workingSet=$live.WorkingSet64;privateBytes=$live.PrivateMemorySize64;handles=$live.HandleCount;threads=$live.Threads.Count}}
  })
  $rows.Add([pscustomobject]@{seconds=$clock.Elapsed.TotalSeconds;phase=$phase;processes=$measurements})
  [pscustomobject]@{scope='Own profile app and WebView child process tree | resident sum can count shared pages twice';samples=$rows} | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath "$taskRoot/process-tree-release.json" -Encoding utf8
  Start-Sleep -Seconds 2
}
