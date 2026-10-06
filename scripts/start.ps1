$ErrorActionPreference = 'Stop'
$titleDirectory = Split-Path -Parent $PSScriptRoot
Set-Location -LiteralPath $titleDirectory
$nodeCommand = Get-Command node -ErrorAction SilentlyContinue
if (-not $nodeCommand) {
    throw 'Node.js 22 or newer is required. Install Node.js, then run this launcher again.'
}
Write-Host 'TH12 development preview: http://127.0.0.1:8112/'
try {
    $existingPreview = Invoke-WebRequest -Uri 'http://127.0.0.1:8112/' -UseBasicParsing -TimeoutSec 2
    if ($existingPreview.StatusCode -eq 200 -and $existingPreview.Content.Contains('UNDEFINED FANTASTIC OBJECT')) {
        Write-Host 'The TH12 preview is already running. Open the address above.'
        Read-Host 'Press Enter to close this launcher window'
        exit 0
    }
} catch { }
Write-Host 'Keep this window open while playing. Press Ctrl+C to stop.'
& $nodeCommand.Source (Join-Path $PSScriptRoot 'serve.mjs')
if ($LASTEXITCODE -ne 0) { throw "The local server exited with code $LASTEXITCODE" }
