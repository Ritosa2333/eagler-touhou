param(
    [ValidateRange(1024,65535)][int]$Port = 8134,
    [switch]$Build
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$workspace = (Resolve-Path (Join-Path $root '..')).Path
$node = (Get-Command node -ErrorAction Stop).Source
$url = "http://127.0.0.1:$Port/"
$output = Join-Path $root 'artifacts/presentation-lab'
New-Item -ItemType Directory -Force -Path $output | Out-Null

Push-Location $root
try {
    if ($Build) {
        if (!$env:EMSDK) { $env:EMSDK = Join-Path $workspace 'toolchains/emsdk' }
        if (!$env:EAGLER_FONT_ROOT) { $env:EAGLER_FONT_ROOT = Join-Path $workspace 'th10-eagler/build-eagler/fonts' }
        & $node portable/build.mjs --presentation-lab
        if ($LASTEXITCODE) { throw "Lab compilation failed: $LASTEXITCODE" }
        & $node portable/package-eagler.mjs --presentation-lab
        if ($LASTEXITCODE) { throw "Lab packaging failed: $LASTEXITCODE" }
    }
    $buildIdentity = Get-Content 'th10_web/artifacts/presentation-lab/build.json' -Raw | ConvertFrom-Json
    $existing = $null
    try { $existing = Invoke-RestMethod ($url + 'build.json') -TimeoutSec 2 } catch {}
    if ($existing) {
        if ($existing.game -ne 'th10' -or $existing.wasm -ne $buildIdentity.sha256) { throw "Port $Port belongs to another build/service. Choose another port." }
        Write-Output "Already running: $url"
        return
    }
    $oldPort = $env:PORT
    try {
        $env:PORT = "$Port"
        $server = Start-Process -FilePath $node -ArgumentList ('"' + (Join-Path $PSScriptRoot 'serve.mjs') + '"') -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $output 'server-out.log') -RedirectStandardError (Join-Path $output 'server-error.log')
    } finally { $env:PORT = $oldPort }
    for ($i=0; $i -lt 80; $i++) {
        Start-Sleep -Milliseconds 250
        if ($server.HasExited) { throw (Get-Content (Join-Path $output 'server-error.log') -Raw) }
        try {
            $ready = Invoke-RestMethod ($url + 'build.json') -TimeoutSec 1
            if ($ready.game -eq 'th10' -and $ready.wasm -eq $buildIdentity.sha256) {
                @{pid=$server.Id;url=$url;wasm=$ready.wasm} | ConvertTo-Json | Set-Content (Join-Path $output 'server-process.json') -Encoding utf8
                Write-Output "TH10 Presentation Lab: $url"
                Write-Output "PID: $($server.Id) / WASM: $($ready.wasm)"
                return
            }
        } catch {}
    }
    throw 'Local lab did not become ready; inspect artifacts/presentation-lab/server-error.log.'
} finally { Pop-Location }
