# Run interactively by the user when the agent's approval service is unavailable.
# Only containers that currently publish the requested host port are stopped.
param([ValidateRange(1,65535)][int]$Port=3007)
$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $project
$manifestPath=Join-Path $project 'artifacts\sdl-release\site\manifest.json'
$stateDirectory=Join-Path $project 'artifacts\public-test'
try {
    $expected=Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    if ($expected.game -ne 'th11' -or !$expected.completeGame) { throw 'The accepted TH11 release is missing.' }
    $ready=$false
    try {
        $current=Invoke-RestMethod -Uri "http://127.0.0.1:$Port/manifest.json" -TimeoutSec 2
        $ready=$current.game -eq 'th11' -and $current.version -eq $expected.version
    } catch {}
    if (!$ready) {
        $rows=& docker ps --format '{{json .}}'
        if ($LASTEXITCODE -ne 0) { throw 'Cannot read Docker. Start Docker Desktop and run this launcher from your Windows account.' }
        $owners=@($rows | ForEach-Object { $_ | ConvertFrom-Json } | Where-Object { $_.Ports -match (':' + $Port + '->') })
        if ($owners.Count) {
            New-Item -ItemType Directory -Path $stateDirectory -Force | Out-Null
            $owners | Select-Object ID,Names,Image,Ports | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $stateDirectory 'previous-public-containers.json') -Encoding UTF8
            foreach ($owner in $owners) {
                Write-Host "Temporarily stopping $($owner.Names), which publishes port $Port."
                & docker stop --time 15 $owner.ID
                if ($LASTEXITCODE -ne 0) { throw "Could not stop container $($owner.Names)." }
            }
        }
    }
    # Reuse the existing public host manager and the existing token-file tunnel.
    # It validates process ownership and the game manifest before reporting ready.
    $hostScript=Join-Path $workspace 'th10_web\scripts\public-host.ps1'
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $hostScript -Action start -Game th11 -Port $Port
    if ($LASTEXITCODE -ne 0) { throw 'Public host startup failed. See th10_web\artifacts\public-host logs.' }
    $verified=$false
    for ($attempt=0; $attempt -lt 12; $attempt++) {
        try {
            $remote=Invoke-RestMethod -Uri 'https://api.steinsgateon.com/manifest.json' -TimeoutSec 5
            if ($remote.game -eq 'th11' -and $remote.version -eq $expected.version -and $remote.execution.sha256 -eq $expected.execution.sha256) { $verified=$true; break }
        } catch {}
        Start-Sleep -Seconds 2
    }
    if ($verified) { Write-Host 'Public TH11 release verified: https://api.steinsgateon.com/' -ForegroundColor Green }
    else { Write-Host "Local TH11 is ready on $Port; public verification is pending. The Cloudflare route must point to http://127.0.0.1:$Port." -ForegroundColor Yellow }
    Write-Host 'Previous containers were stopped only; their files and restart policies were not changed.'
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
