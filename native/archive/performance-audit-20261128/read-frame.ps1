param([string]$Phase)
$ErrorActionPreference='Stop'
Add-Type -TypeDefinition @"
using System;using System.Runtime.InteropServices;
public static class NativeFrameEvidence {
 [StructLayout(LayoutKind.Sequential)] public struct Rect {public int left,top,right,bottom;}
 [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h,out Rect r);
 [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h,out Rect r);
 [DllImport("user32.dll")] public static extern uint GetDpiForWindow(IntPtr h);
 [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h,out uint p);
 [DllImport("user32.dll")] public static extern bool IsZoomed(IntPtr h);
}
"@
$frameHandle=[IntPtr]31656818
$ownerPid=[uint32]0
[NativeFrameEvidence]::GetWindowThreadProcessId($frameHandle,[ref]$ownerPid) | Out-Null
if($ownerPid -ne 39464){throw 'Unexpected preview window owner'}
$outer=New-Object NativeFrameEvidence+Rect
$client=New-Object NativeFrameEvidence+Rect
if(-not [NativeFrameEvidence]::GetWindowRect($frameHandle,[ref]$outer)){throw 'No outer rect'}
if(-not [NativeFrameEvidence]::GetClientRect($frameHandle,[ref]$client)){throw 'No client rect'}
$preview=Get-Process -Id $ownerPid -ErrorAction Stop
$record=[pscustomobject]@{phase=$Phase;atUtc=[DateTime]::UtcNow.ToString('o');pid=$ownerPid;dpi=[NativeFrameEvidence]::GetDpiForWindow($frameHandle);outer=$outer;client=$client;maximized=[NativeFrameEvidence]::IsZoomed($frameHandle);workingSet=$preview.WorkingSet64;privateBytes=$preview.PrivateMemorySize64}
$record | ConvertTo-Json -Depth 3 -Compress | Add-Content -LiteralPath 'C:/Users/Ritosa/AppData/Local/Temp/native-process-proof-20261128/window-frames.jsonl' -Encoding utf8
$record | ConvertTo-Json -Depth 3
