param([int]$TargetProcess=15288,[string]$Phase='library')
$ErrorActionPreference='Stop'
$expected='D:/workspace/eagler/artifacts/.native-room-client-win/build/windows/x64/runner/Release/touhouonline.exe'
Add-Type @'
using System;using System.Runtime.InteropServices;
public static class PerfForeground {
 [DllImport("user32.dll")] static extern IntPtr GetForegroundWindow();
 [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);
 public static uint Read(){uint p;GetWindowThreadProcessId(GetForegroundWindow(),out p);return p;}
}
'@
$process=Get-Process -Id $TargetProcess
if([IO.Path]::GetFullPath($process.Path) -ine [IO.Path]::GetFullPath($expected)){throw 'Wrong performance input'}
$created=$process.StartTime.ToFileTimeUtc();$clock=[Diagnostics.Stopwatch]::StartNew()
$samples=@(for($i=0;$i -lt 11;$i++){
 $p=Get-Process -Id $TargetProcess
 if($p.StartTime.ToFileTimeUtc() -ne $created){throw 'Process changed'}
 [pscustomobject]@{seconds=$clock.Elapsed.TotalSeconds;cpu=$p.CPU;foregroundPid=[PerfForeground]::Read();workingSet=$p.WorkingSet64;privateBytes=$p.PrivateMemorySize64;threads=$p.Threads.Count;handles=$p.HandleCount}
 if($i -lt 10){Start-Sleep -Seconds 1}
})
$gpu=@(Get-CimInstance Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory | Where-Object Name -Like "pid_${TargetProcess}_*" | Select-Object Name,DedicatedUsage,SharedUsage,TotalCommitted)
$summary=[pscustomobject]@{phase=$Phase;pid=$TargetProcess;created=$created;input=$expected;corePercent=100*($samples[-1].cpu-$samples[0].cpu)/($samples[-1].seconds-$samples[0].seconds);foregroundSamples=@($samples|Where-Object foregroundPid -EQ $TargetProcess).Count;samples=$samples;gpu=$gpu}
$summary | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath "C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/webview-adapter/production-$Phase.json" -Encoding utf8
$summary | Select-Object phase,pid,corePercent,foregroundSamples,@{n='privateMiB';e={[math]::Round($_.samples[-1].privateBytes/1MB,1)}},@{n='workingSetMiB';e={[math]::Round($_.samples[-1].workingSet/1MB,1)}},gpu | ConvertTo-Json -Depth 4
