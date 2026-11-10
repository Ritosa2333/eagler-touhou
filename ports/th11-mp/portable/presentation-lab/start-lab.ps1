param(
    [ValidateRange(1024,65535)][int]$Port = 8136,
    [switch]$Build
)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$node = (Get-Command node -ErrorAction Stop).Source
$url = "http://127.0.0.1:$Port/"
Push-Location $root
try {
    if ($Build) {
        & $node portable/build.mjs --presentation-lab
        if ($LASTEXITCODE) { throw "Lab compilation failed: $LASTEXITCODE" }
    }
    $oldPort = $env:PORT
    try { $env:PORT = "$Port"; node portable/presentation-lab/serve.mjs } finally { $env:PORT = $oldPort }
} finally { Pop-Location }
