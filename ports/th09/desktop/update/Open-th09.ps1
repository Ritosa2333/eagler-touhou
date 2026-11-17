[CmdletBinding(PositionalBinding = $false)]
param(
    [Uri]$FeedUri = 'https://touhouonline.com/native/th09/windows-x64/manifest.json',
    [switch]$UpdateOnly
)

$ErrorActionPreference = 'Stop'
if ($FeedUri.Scheme -ne 'https' -and !($FeedUri.Scheme -eq 'http' -and $FeedUri.IsLoopback)) {
    throw 'The update feed must use HTTPS.'
}
$installation = $PSScriptRoot
$state = Join-Path $installation '.th09-update'
$versions = Join-Path $state 'versions'
$pointer = Join-Path $state 'current.txt'
$required = @(
    'eagler-desktop.exe', 'catalog.template.json', 'runtime/th09.exe',
    'runtime/ChillRoundGothic_Medium.ttf', 'runtime/fonts/cp932.bin', 'runtime/fonts/blend.bin'
)

function Get-ActiveInstall {
    if ([IO.File]::Exists($pointer)) {
        $share = [IO.FileShare]::Read -bor [IO.FileShare]::Delete
        $stream = [IO.File]::Open($pointer, 'Open', 'Read', $share)
        try {
            $reader = [IO.StreamReader]::new($stream)
            try { $id = $reader.ReadToEnd().Trim() }
            finally { $reader.Dispose() }
        } finally { $stream.Dispose() }
        if ($id -match '^[a-f0-9]{64}-[a-f0-9]{8}$') {
            $candidate = Join-Path $versions $id
            $missing = $required.Where({ ![IO.File]::Exists((Join-Path $candidate $_)) }, 'First')
            if (!$missing) { return $candidate }
        }
    }
    return $installation
}

function Get-UpdateHash($data) {
    $hash = [Security.Cryptography.SHA256]::Create()
    try {
        return [BitConverter]::ToString($hash.ComputeHash($data)).Replace('-', '').ToLowerInvariant()
    } finally { $hash.Dispose() }
}

function Test-RuntimeFile($path, $entry) {
    if (![IO.File]::Exists($path) -or ([IO.FileInfo]::new($path)).Length -ne $entry.size) {
        return $false
    }
    $stream = [IO.File]::OpenRead($path)
    try { return (Get-UpdateHash $stream) -eq $entry.sha256 }
    finally { $stream.Dispose() }
}

function Install-RuntimeFile($entry, $target, $scratch) {
    $object = [Uri]::new($FeedUri, ('objects/' + $entry.sha256 + '.gz'))
    Invoke-WebRequest -Uri $object -UseBasicParsing -MaximumRedirection 0 -TimeoutSec 120 -OutFile $scratch
    if (([IO.FileInfo]::new($scratch)).Length -ne $entry.downloadSize) {
        throw "Incomplete download for $($entry.path)"
    }

    $archive = [IO.File]::OpenRead($scratch)
    try {
        $gzip = [IO.Compression.GZipStream]::new($archive, [IO.Compression.CompressionMode]::Decompress)
        try {
            $output = [IO.File]::Open($target, [IO.FileMode]::CreateNew)
            try {
                $buffer = [byte[]]::new(131072)
                while (($count = $gzip.Read($buffer, 0, $buffer.Length)) -gt 0) {
                    if (($output.Length + $count) -gt $entry.size) {
                        throw "Uncompressed size exceeded for $($entry.path)"
                    }
                    $output.Write($buffer, 0, $count)
                }
            } finally { $output.Dispose() }
        } finally { $gzip.Dispose() }
    } finally { $archive.Dispose() }

    if (!(Test-RuntimeFile $target $entry)) {
        throw "Invalid checksum for $($entry.path)"
    }
}

function Update-Th09 {
    [IO.Directory]::CreateDirectory($versions) | Out-Null
    try { $lock = [IO.File]::Open((Join-Path $state 'update.lock'), 'OpenOrCreate', 'ReadWrite', 'None') }
    catch [IO.IOException] {
        if (($_.Exception.HResult -band 0xffff) -eq 32) { return }
        throw
    }
    try {
        $active = Get-ActiveInstall
        $response = Invoke-WebRequest -Uri $FeedUri -UseBasicParsing -MaximumRedirection 0 -TimeoutSec 12
        $manifest = $response.Content | ConvertFrom-Json
        if ($manifest.schema -ne 'eagler-th09-windows-update/2' -or
            $manifest.files.Count -lt $required.Count -or $manifest.files.Count -gt 128) {
            throw 'Invalid th09 update manifest.'
        }
        $paths = @{}
        foreach ($entry in $manifest.files) {
            if ($entry.path -cnotmatch '^(eagler-desktop\.exe|catalog\.template\.json|runtime/[A-Za-z0-9][A-Za-z0-9._-]*\.(exe|dll|ttf|bmp|txt)|runtime/fonts/(blend|cp932)\.bin|licenses/[A-Za-z0-9][A-Za-z0-9._-]*\.txt)$' -or
                ([IO.Path]::GetFileName($entry.path) -match '^(CON|PRN|AUX|NUL|COM[0-9]|LPT[0-9])\.') -or
                $paths.ContainsKey($entry.path) -or $entry.sha256 -cnotmatch '^[a-f0-9]{64}$' -or
                $entry.size -notmatch '^[1-9][0-9]{0,8}$' -or $entry.downloadSize -notmatch '^[1-9][0-9]{0,8}$') {
                throw 'Invalid file entry in th09 update manifest.'
            }
            $paths[$entry.path] = $entry
        }
        foreach ($name in $required) {
            if (!$paths.ContainsKey($name)) { throw "Update is missing $name" }
        }

        $revision = Get-UpdateHash ([Text.Encoding]::UTF8.GetBytes($response.Content))
        if ($active -and ([IO.Path]::GetFileName($active)).StartsWith($revision + '-')) {
            $valid = $true
            foreach ($entry in $manifest.files) {
                if (!(Test-RuntimeFile (Join-Path $active $entry.path) $entry)) {
                    $valid = $false
                    break
                }
            }
            if ($valid) {
                Write-Host 'th09 is up to date.'
                return $active
            }
        }

        $id = $revision + '-' + [Guid]::NewGuid().ToString('N').Substring(0, 8)
        $stage = [IO.Directory]::CreateDirectory((Join-Path $versions ('.staging-' + $id))).FullName
        $download = Join-Path $stage 'download.gz'
        $pending = Join-Path $state ($id + '.tmp')
        try {
            [long]$received = 0
            foreach ($entry in $manifest.files) {
                $target = Join-Path $stage $entry.path
                [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target)) | Out-Null
                if ($active -and (Test-RuntimeFile (Join-Path $active $entry.path) $entry)) {
                    $original = Join-Path $active $entry.path
                    try {
                        New-Item -ItemType HardLink -Path $target -Target $original | Out-Null
                    } catch { [IO.File]::Copy($original, $target) }
                } else {
                    Write-Host "Downloading $($entry.path)"
                    Install-RuntimeFile $entry $target $download
                    [IO.File]::Delete($download)
                    $received += [long]$entry.downloadSize
                }
            }
            $installed = Join-Path $versions $id
            [IO.Directory]::Move($stage, $installed)
            [IO.File]::WriteAllText($pending, $id, [Text.Encoding]::ASCII)
            if ([IO.File]::Exists($pointer)) {
                [IO.File]::Replace($pending, $pointer, [NullString]::Value)
            } else { [IO.File]::Move($pending, $pointer) }
            Write-Host "th09 updated. Downloaded $received bytes."
            return $installed
        } finally {
            if ([IO.Directory]::Exists($stage) -and
                [IO.Path]::GetDirectoryName($stage) -eq [IO.Path]::GetFullPath($versions) -and
                !((Get-Item -LiteralPath $stage -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) {
                Remove-Item -LiteralPath $stage -Recurse -Force
            }
            [IO.File]::Delete($pending)
        }
    } finally { $lock.Dispose() }
}

if ($UpdateOnly) {
    try { Update-Th09 | Out-Null }
    catch {
        $message = '[{0:u}] {1}' -f [datetime]::UtcNow, $_.Exception.Message
        Set-Content -LiteralPath (Join-Path $state 'last-error.txt') -Value $message -Encoding UTF8
        throw
    }
    [IO.File]::Delete((Join-Path $state 'last-error.txt'))
    exit 0
}

$active = Get-ActiveInstall
$runtime = Join-Path $active 'runtime'
$executable = Join-Path $runtime 'th09.exe'
$launcher = Join-Path $active 'eagler-desktop.exe'
if (![IO.File]::Exists($executable) -or ![IO.File]::Exists($launcher)) {
    throw 'Keep Open-th09.cmd, eagler-desktop.exe and the runtime folder together.'
}

$font = Join-Path $runtime 'fonts/msgothic.ttc'
if (![IO.File]::Exists($font)) {
    $fontSource = Join-Path $installation 'runtime/fonts/msgothic.ttc'
    if (![IO.File]::Exists($fontSource)) { $fontSource = Join-Path $env:SystemRoot 'Fonts/msgothic.ttc' }
    if (![IO.File]::Exists($fontSource)) { $fontSource = Join-Path $runtime 'ChillRoundGothic_Medium.ttf' }
    try { [IO.File]::Copy($fontSource, $font) }
    catch [IO.IOException] { if (![IO.File]::Exists($font)) { throw } }
}

$env:EAGLER_DESKTOP_DATA = Join-Path $installation 'data'
$env:WEBVIEW2_USER_DATA_FOLDER = Join-Path $env:EAGLER_DESKTOP_DATA 'webview'
[IO.Directory]::CreateDirectory($env:EAGLER_DESKTOP_DATA) | Out-Null

$env:EAGLER_DESKTOP_CATALOG = Join-Path $active 'catalog.local.json'
$catalog = Get-Content -LiteralPath (Join-Path $active 'catalog.template.json') -Encoding UTF8 -Raw | ConvertFrom-Json
$catalog.games.th09 | Add-Member -NotePropertyName executable -NotePropertyValue $executable -Force
$catalog.games.th09.environment | Add-Member -NotePropertyName EAGLER_OVERLAY_FONT -NotePropertyValue (Join-Path $runtime 'ChillRoundGothic_Medium.ttf') -Force
$localCatalog = Join-Path $active ('catalog-' + [Guid]::NewGuid().ToString('N') + '.tmp')
try {
    [IO.File]::WriteAllText($localCatalog, ($catalog | ConvertTo-Json -Depth 10))
    if ([IO.File]::Exists($env:EAGLER_DESKTOP_CATALOG)) {
        [IO.File]::Replace($localCatalog, $env:EAGLER_DESKTOP_CATALOG, [NullString]::Value)
    } else { [IO.File]::Move($localCatalog, $env:EAGLER_DESKTOP_CATALOG) }
} catch [IO.IOException] {
    if (![IO.File]::Exists($env:EAGLER_DESKTOP_CATALOG)) { throw }
} finally { [IO.File]::Delete($localCatalog) }

Start-Process -FilePath $launcher -WorkingDirectory $installation -WindowStyle Normal

$worker = "& '" + $PSCommandPath.Replace("'", "''") + "' -UpdateOnly -FeedUri '" + $FeedUri.AbsoluteUri.Replace("'", "''") + "'"
$encoded = [Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($worker))
$powershell = Join-Path $env:SystemRoot 'System32/WindowsPowerShell/v1.0/powershell.exe'
Start-Process -FilePath $powershell -WindowStyle Hidden -ArgumentList @(
    '-NoProfile', '-NonInteractive', '-ExecutionPolicy', 'Bypass', '-EncodedCommand', $encoded
)
