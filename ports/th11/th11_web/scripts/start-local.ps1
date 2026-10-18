param(
    [ValidateRange(1, 65535)][int]$Port = 8113,
    [switch]$Development
)
$ErrorActionPreference = 'Stop'
try {
    $project = Split-Path -Parent $PSScriptRoot
    $workspace = Split-Path -Parent $project
    $output = if ($Development) { 'artifacts\architecture-preview' } else { 'artifacts\sdl-release' }
    $release = Join-Path $project $output
    $manifestPath = Join-Path $release 'site\manifest.json'
    if (!(Test-Path -LiteralPath $manifestPath)) { throw 'Build and package TH11 first. See docs/BUILD.md.' }
    $manifest = Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($manifest.game -ne 'th11' -or $manifest.execution.kind -ne 'cpp-sdl3') { throw 'This is not a TH11 SDL3 release.' }
    if (!$Development -and !$manifest.completeGame) { throw 'Final acceptance is incomplete. Use -Development only for the isolated preview.' }
    $node = Join-Path $workspace 'th10_web\tools\node.exe'
    if (!(Test-Path -LiteralPath $node)) { $node = (Get-Command node -ErrorAction Stop).Source }
    Write-Host "TH11: http://127.0.0.1:$Port/"
    Write-Host 'Keep this window open while playing. Press Ctrl+C to stop.'
    & $node (Join-Path $release 'scripts\serve.mjs') --port $Port
    exit $LASTEXITCODE
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
