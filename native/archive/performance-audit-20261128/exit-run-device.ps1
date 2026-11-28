$ErrorActionPreference='Stop'
$testRoot='C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128'
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class DeviceLifetimeWait {
 [DllImport("kernel32.dll",SetLastError=true)] static extern IntPtr OpenProcess(uint a,bool i,uint p);
 [DllImport("kernel32.dll",SetLastError=true)] static extern uint WaitForSingleObject(IntPtr p,uint ms);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetExitCodeProcess(IntPtr p,out uint c);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool TerminateProcess(IntPtr p,uint code);
 [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
 [DllImport("kernel32.dll",SetLastError=true,CharSet=CharSet.Unicode)] static extern bool QueryFullProcessImageName(IntPtr p,uint f,StringBuilder b,ref int n);
 public static object Observe(uint id) {
  var p=OpenProcess(0x101001,false,id);
  if(p==IntPtr.Zero)return new{id,open=false,error=Marshal.GetLastWin32Error()};
  try {
   var name=new StringBuilder(32768);int n=name.Capacity;
   if(!QueryFullProcessImageName(p,0,name,ref n)||!name.ToString().Equals(@"C:\Users\Ritosa\AppData\Local\Temp\native-process-proof-20261128\device_host.exe",StringComparison.OrdinalIgnoreCase))throw new Exception("Unexpected test process");
   var wait=WaitForSingleObject(p,3500);uint code;GetExitCodeProcess(p,out code);
   bool stopped=false;if(wait==258){stopped=TerminateProcess(p,91);if(stopped)WaitForSingleObject(p,2000);}
   return new{id,open=true,waitResult=wait,exitCode=code,stoppedHungTest=stopped};
  } finally {CloseHandle(p);}
 }
}
'@
$deviceRecords=@(foreach($mode in @('scoped','static','scoped','static','scoped','static')) {
 $iteration=($deviceRecords | Measure-Object).Count
 $sampleName="$mode-$([guid]::NewGuid().ToString('N').Substring(0,6))"
 $output=Join-Path $testRoot "$sampleName.log"
 $child=Start-Process -FilePath "$testRoot/device_host.exe" -ArgumentList $mode -WorkingDirectory $testRoot -WindowStyle Hidden -PassThru -RedirectStandardOutput $output
 $clock=[Diagnostics.Stopwatch]::StartNew()
 $result=[DeviceLifetimeWait]::Observe([uint32]$child.Id)
 [pscustomobject]@{mode=$mode;milliseconds=$clock.Elapsed.TotalMilliseconds;process=$result;output=(Get-Content -LiteralPath $output -Raw)}
})
$deviceRecords | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath "$testRoot/device-results.json" -Encoding utf8
$deviceRecords | ConvertTo-Json -Depth 5
