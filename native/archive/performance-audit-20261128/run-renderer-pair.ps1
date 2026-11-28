$ErrorActionPreference='Stop'
$rendererRoot='C:/Users/Ritosa/AppData/Local/Temp/native-renderer-comparison-20261128'
foreach ($run in @('impeller-3')) {
  $mode=if ($run.StartsWith('skia')) {'skia'} else {'impeller'}
  $folder="$rendererRoot/runs/$run"
  New-Item -ItemType Directory -Path "$folder/profile","$folder/cache" -Force | Out-Null
  $resultFile="$folder/result.json"
  if ($run -ne 'skia-5') {
    $env:TOUHOU_RENDERER=$mode
    $env:TOUHOU_RENDERER_FOLDER=$folder
    $env:TOUHOU_RENDERER_RESULT=$resultFile
    & "$rendererRoot/app/touhouonline-$mode.exe" *> "$folder/engine.log"
  }
  $deadline=[DateTime]::UtcNow.AddSeconds(240)
  $samples=@()
  $lastCase=''
  $result=$null
  while ([DateTime]::UtcNow -lt $deadline) {
    if (Test-Path -LiteralPath $resultFile) {
      try { $result=Get-Content -LiteralPath $resultFile -Raw | ConvertFrom-Json } catch { Start-Sleep -Milliseconds 100; continue }
      $p=Get-Process -Id $result.pid -ErrorAction SilentlyContinue
      if ($p) {
        if ([IO.Path]::GetFullPath($p.Path) -ine [IO.Path]::GetFullPath("$rendererRoot/app/touhouonline-$mode.exe")) { throw 'Renderer process identity changed' }
        $samples+=[pscustomobject]@{utc=[DateTime]::UtcNow.ToString('o');cpuSeconds=$p.CPU;workingSet=$p.WorkingSet64;privateBytes=$p.PrivateMemorySize64;handles=$p.HandleCount;threads=$p.Threads.Count}
      }
      $case=($result.cases.PSObject.Properties | Select-Object -Last 1).Name
      if ($case -ne $lastCase) { Write-Output "$run | $case"; $lastCase=$case }
      if ($result.status) { break }
      if (!$p) { throw "$run ended before result" }
    }
    Start-Sleep -Seconds 1
  }
  $samples | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath "$folder/process-samples.json" -Encoding utf8
  if ($result.status -ne 'passed') { throw "$run failed or timed out | $($result.error)" }
  $exitDeadline=[DateTime]::UtcNow.AddSeconds(15)
  do {
    $alive=Get-Process -Id $result.pid -ErrorAction SilentlyContinue
    if (!$alive) { break }
    Start-Sleep -Milliseconds 500
  } while ([DateTime]::UtcNow -lt $exitDeadline)
  [pscustomobject]@{run=$run;pid=$result.pid;cases=@($result.cases.PSObject.Properties).Count;processGone=(!$alive)} | ConvertTo-Json -Compress | Set-Content -LiteralPath "$folder/process-exit.json" -Encoding utf8
  if ($alive) { throw "$run did not terminate after native close" }
  Write-Output "$run | passed and process gone"
}
