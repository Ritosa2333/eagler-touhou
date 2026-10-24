param(
    [Parameter(Mandatory=$true)][ValidateRange(1,2147483647)][int]$OwnerProcessId,
    [Parameter(Mandatory=$true)][ValidateNotNullOrEmpty()][string]$PageTitle,
    [ValidateSet('normal','minimized')][string]$WindowState = 'normal'
)
$ErrorActionPreference = 'Stop'
Add-Type -TypeDefinition @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class EaglerOwnedWindow {
    public delegate bool EnumProc(IntPtr hwnd, IntPtr state);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc callback, IntPtr state);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hwnd, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr hwnd, StringBuilder text, int size);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr hwnd, StringBuilder text, int size);
    [DllImport("user32.dll")] public static extern bool ShowWindowAsync(IntPtr hwnd, int command);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hwnd);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr hwnd);
}
'@
$taskWindows = [System.Collections.Generic.List[System.IntPtr]]::new()
$taskCallback = [EaglerOwnedWindow+EnumProc]{param($windowHandle,$state)
    [uint32]$windowProcessId = 0
    [void][EaglerOwnedWindow]::GetWindowThreadProcessId($windowHandle,[ref]$windowProcessId)
    if ($windowProcessId -ne $OwnerProcessId) { return $true }
    $windowClass = [System.Text.StringBuilder]::new(256)
    $windowTitle = [System.Text.StringBuilder]::new(4096)
    [void][EaglerOwnedWindow]::GetClassName($windowHandle,$windowClass,$windowClass.Capacity)
    [void][EaglerOwnedWindow]::GetWindowText($windowHandle,$windowTitle,$windowTitle.Capacity)
    if ($windowClass.ToString() -eq 'Chrome_WidgetWin_1' -and $windowTitle.ToString().StartsWith($PageTitle,[System.StringComparison]::Ordinal)) {
        $taskWindows.Add($windowHandle)
    }
    return $true
}
[void][EaglerOwnedWindow]::EnumWindows($taskCallback,[System.IntPtr]::Zero)
if ($taskWindows.Count -ne 1) {
    @{status='incomplete';matchingWindows=$taskWindows.Count;processId=$OwnerProcessId;reason='Expected one owned Chrome window with the observed page title'} | ConvertTo-Json -Compress
    exit 0
}
$ownedHandle = $taskWindows[0]
function Get-OwnedActivationState {
    $foregroundHandle = [EaglerOwnedWindow]::GetForegroundWindow()
    [uint32]$foregroundProcessId = 0
    [void][EaglerOwnedWindow]::GetWindowThreadProcessId($foregroundHandle,[ref]$foregroundProcessId)
    return @{at=[DateTime]::UtcNow.ToString('o');foregroundHandle=$foregroundHandle.ToInt64().ToString();
        foregroundProcessId=$foregroundProcessId;ownedForeground=($foregroundHandle -eq $ownedHandle);
        minimized=[EaglerOwnedWindow]::IsIconic($ownedHandle)}
}
$before = Get-OwnedActivationState
$showWindowAttempted = $false
$showWindowCallReturned = $null
$stateCommand = $null
if ($WindowState -eq 'minimized') { $stateCommand = 6 }
elseif ($before.minimized) { $stateCommand = 9 }
if ($null -ne $stateCommand) {
    $showWindowAttempted = $true
    $showWindowCallReturned = [EaglerOwnedWindow]::ShowWindowAsync($ownedHandle,$stateCommand)
}
$afterShow = Get-OwnedActivationState
$foregroundAttempted = $WindowState -eq 'normal'
$foregroundCallReturned = $null
if ($foregroundAttempted) { $foregroundCallReturned = [EaglerOwnedWindow]::SetForegroundWindow($ownedHandle) }
$afterForeground = Get-OwnedActivationState
@{status='requested';processId=$OwnerProcessId;windowHandle=$ownedHandle.ToInt64().ToString();
    requestedState=$WindowState;minimizedObserved=$afterForeground.minimized;
    showWindowAttempted=$showWindowAttempted;showWindowCommand=$stateCommand;showWindowCallReturned=$showWindowCallReturned;
    foregroundAttempted=$foregroundAttempted;foregroundCallReturned=$foregroundCallReturned;
    foregroundObserved=$afterForeground.ownedForeground;before=$before;afterShow=$afterShow;afterForeground=$afterForeground} | ConvertTo-Json -Compress -Depth 4
