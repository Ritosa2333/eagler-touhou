param([ValidateSet('connected','reset')][string]$Run='connected')
$ErrorActionPreference='Stop'
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class NativeIdleCpu {
 [DllImport("kernel32.dll",SetLastError=true)] static extern IntPtr OpenProcess(uint a,bool i,uint p);
 [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetProcessTimes(IntPtr p,out long c,out long e,out long k,out long u);
 [DllImport("kernel32.dll",SetLastError=true)] static extern uint WaitForSingleObject(IntPtr p,uint ms);
 [DllImport("kernel32.dll",SetLastError=true)] static extern bool GetExitCodeProcess(IntPtr p,out uint code);
 [DllImport("kernel32.dll",SetLastError=true,CharSet=CharSet.Unicode)] static extern bool QueryFullProcessImageName(IntPtr p,uint f,StringBuilder b,ref int n);
 [DllImport("user32.dll")] static extern IntPtr GetForegroundWindow();
 [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);
 public static object Read(uint id) {
  var h=OpenProcess(0x101000,false,id);
  if(h==IntPtr.Zero)return new{id,open=false,error=Marshal.GetLastWin32Error()};
  try {
   var name=new StringBuilder(32768);int size=name.Capacity;
   if(!QueryFullProcessImageName(h,0,name,ref size)||!name.ToString().Equals(@"D:\workspace\eagler\artifacts\native-launcher-85941f0d56\hover-detail-perf\preview\touhouonline.exe",StringComparison.OrdinalIgnoreCase))throw new Exception("Unexpected process");
   long c,e,k,u;GetProcessTimes(h,out c,out e,out k,out u);uint code;GetExitCodeProcess(h,out code);uint foreground;GetWindowThreadProcessId(GetForegroundWindow(),out foreground);
   return new{id,open=true,created=c,exited=e,kernel100ns=k,user100ns=u,exitCode=code,waitResult=WaitForSingleObject(h,0),foregroundPid=foreground};
  } finally{CloseHandle(h);}
 }
}
'@
$idleRecords=@()
$sampleClock=[Diagnostics.Stopwatch]::StartNew()
for($i=0;$i -lt 11;$i++) {
 $idleRecords += [pscustomobject]@{seconds=$sampleClock.Elapsed.TotalSeconds;visible=[NativeIdleCpu]::Read(43284);closed=[NativeIdleCpu]::Read(39724)}
 if($i -lt 10){Start-Sleep -Seconds 1}
}
$results=@(foreach($name in @('visible','closed')) {
 $first=$idleRecords[0].$name;$last=$idleRecords[-1].$name
 if($first.open -and $last.open -and $first.created -eq $last.created) {
  [pscustomobject]@{name=$name;id=$first.id;oneCorePercent=100*($last.kernel100ns+$last.user100ns-$first.kernel100ns-$first.user100ns)/10000000/($idleRecords[-1].seconds-$idleRecords[0].seconds);foregroundSamples=@($idleRecords | Where-Object {$_.visible.foregroundPid -eq 43284}).Count}
 }
})
[pscustomobject]@{run=$Run;input='Ordinary Release | no pointer input | native process times and foreground PID, no UIA tree query during sample';summary=$results;samples=$idleRecords} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath "C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/idle-$Run.json" -Encoding utf8
$results | ConvertTo-Json
