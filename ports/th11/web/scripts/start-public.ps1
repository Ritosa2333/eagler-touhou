param([ValidateRange(1,65535)][int]$Port=8113)
$ErrorActionPreference='Stop'
try {
    $origin="http://127.0.0.1:$Port"
    $manifest=Invoke-RestMethod -Uri "$origin/manifest.json" -TimeoutSec 10
    if ($manifest.game -ne 'th11' -or !$manifest.completeGame) { throw 'Start the accepted TH11 local release first.' }
    $cloudflared='E:\chatgpt2api\cloudflared.exe'
    if (!(Test-Path -LiteralPath $cloudflared)) { $cloudflared=(Get-Command cloudflared -ErrorAction Stop).Source }
    Write-Host "Opening a temporary public URL for $origin. Keep this window open."
    Write-Host 'The trycloudflare.com URL printed below is the mobile test address.'
    & $cloudflared tunnel --url $origin --protocol http2 --no-autoupdate
    exit $LASTEXITCODE
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
