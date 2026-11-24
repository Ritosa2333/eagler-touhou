param(
    [Parameter(Mandatory = $true)][string]$LibraryDirectory,
    [Parameter(Mandatory = $true)][string]$Archive,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)
$ErrorActionPreference = 'Stop'
if ([IntPtr]::Size -ne 4) { throw 'This helper requires x86 PowerShell for the official x86 thtk 12 DLL.' }
$library = [System.IO.Path]::GetFullPath($LibraryDirectory)
$output = [System.IO.Path]::GetFullPath($OutputDirectory)
if (-not (Test-Path -LiteralPath $output -PathType Container)) { throw 'Audit output directory must already exist.' }
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class Th128ArchiveAudit {
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern bool SetDllDirectory(string path);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
    public static extern IntPtr thtk_io_open_file_w(string path, string mode, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int thtk_io_close(IntPtr stream);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr thdat_open(uint version, IntPtr stream, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int thdat_entry_count(IntPtr archive, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int thdat_entry_get_size(IntPtr archive, int index, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr thdat_entry_get_name(IntPtr archive, int index, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int thdat_entry_read_data(IntPtr archive, int index, IntPtr output, ref IntPtr error);
    [DllImport("thtk.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void thdat_free(IntPtr archive);
}
'@
if (-not [Th128ArchiveAudit]::SetDllDirectory($library)) { throw 'Could not select thtk library directory.' }
$nativeError = [IntPtr]::Zero
$stream = [Th128ArchiveAudit]::thtk_io_open_file_w($Archive, 'rb', [ref]$nativeError)
if ($stream -eq [IntPtr]::Zero) { throw 'Cannot open archive read-only.' }
$archiveHandle = [IntPtr]::Zero
try {
    $archiveHandle = [Th128ArchiveAudit]::thdat_open(128, $stream, [ref]$nativeError)
    if ($archiveHandle -eq [IntPtr]::Zero) { throw 'Cannot parse TH128 archive.' }
    $count = [Th128ArchiveAudit]::thdat_entry_count($archiveHandle, [ref]$nativeError)
    if ($count -le 0 -or $count -gt 4096) { throw 'Invalid native archive entry count.' }
    $records = @()
    [long]$expandedTotal = 0
    for ($index = 0; $index -lt $count; $index++) {
        $pointer = [Th128ArchiveAudit]::thdat_entry_get_name($archiveHandle, $index, [ref]$nativeError)
        if ($pointer -eq [IntPtr]::Zero) { throw 'Cannot read native archive name.' }
        $declaredSize = [Th128ArchiveAudit]::thdat_entry_get_size($archiveHandle, $index, [ref]$nativeError)
        if ($declaredSize -le 0 -or $declaredSize -gt 134217728) { throw 'Native archive entry exceeds byte budget.' }
        $expandedTotal += $declaredSize
        if ($expandedTotal -gt 1073741824) { throw 'Physical native entries exceed aggregate decoded-byte budget.' }
        $records += [pscustomobject]@{ name = [Runtime.InteropServices.Marshal]::PtrToStringAnsi($pointer); index = $index; expectedBytes = $declaredSize }
    }
    $audits = @()
    foreach ($group in ($records | Group-Object name | Where-Object Count -gt 1)) {
        $entries = @()
        foreach ($entry in $group.Group) {
            $target = [IO.Path]::Combine($output, ('entry-{0}.bin' -f $entry.index))
            $destination = [Th128ArchiveAudit]::thtk_io_open_file_w($target, 'wb', [ref]$nativeError)
            if ($destination -eq [IntPtr]::Zero) { throw 'Cannot create numbered audit file.' }
            try {
                $size = [Th128ArchiveAudit]::thdat_entry_read_data($archiveHandle, $entry.index, $destination, [ref]$nativeError)
                if ($size -le 0) { throw 'Cannot decode duplicate entry.' }
            } finally { [void][Th128ArchiveAudit]::thtk_io_close($destination) }
            if (([IO.FileInfo]$target).Length -ne $entry.expectedBytes) { throw 'Duplicate output length differs from its declared size.' }
            $hasher = [Security.Cryptography.SHA256]::Create()
            $inputStream = [IO.File]::OpenRead($target)
            try { $checksum = [BitConverter]::ToString($hasher.ComputeHash($inputStream)).Replace('-', '').ToLowerInvariant() }
            finally { $inputStream.Dispose(); $hasher.Dispose() }
            $entries += [pscustomobject]@{ index = $entry.index; bytes = ([IO.FileInfo]$target).Length; sha256 = $checksum }
        }
        $audits += [pscustomobject]@{ name = $group.Name; identical = (@($entries | Select-Object -ExpandProperty sha256 -Unique).Count -eq 1); entries = $entries }
    }
    $json = ConvertTo-Json -InputObject @($audits) -Depth 8
    [IO.File]::WriteAllText([IO.Path]::Combine($output, 'audit.json'), $json, [Text.UTF8Encoding]::new($false))
} finally {
    if ($archiveHandle -ne [IntPtr]::Zero) { [Th128ArchiveAudit]::thdat_free($archiveHandle) }
    [void][Th128ArchiveAudit]::thtk_io_close($stream)
}
