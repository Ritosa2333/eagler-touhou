[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string] $Candidate,
    [Parameter(Mandatory)] [string] $OutputDirectory,
    [string] $UnicodeFont
)

$ErrorActionPreference = 'Stop'
$project = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$candidatePath = (Resolve-Path -LiteralPath $Candidate).Path
$output = [IO.Path]::GetFullPath($OutputDirectory)
$outputParent = Split-Path $output -Parent
$staging = Join-Path $outputParent ('.github-release-assets.staging-' + [guid]::NewGuid().ToString('N'))

if (Test-Path -LiteralPath $output) { throw "GitHub Release asset output already exists: $output" }
if ($output -eq [IO.Path]::GetPathRoot($output) -or $output -eq [IO.Path]::GetFullPath($project)) {
    throw "Unsafe GitHub Release asset output: $output"
}

& node (Join-Path $project 'scripts\verify-release-bundle.mjs') $candidatePath
if ($LASTEXITCODE -ne 0) { throw "Formal Release verification failed: $LASTEXITCODE" }

$runtimeRelease = Join-Path $candidatePath 'runtime-release'
& node (Join-Path $project 'scripts\verify-runtime-release.mjs') $runtimeRelease
if ($LASTEXITCODE -ne 0) { throw "Runtime Release verification failed: $LASTEXITCODE" }

$releaseManifest = Get-Content -LiteralPath (Join-Path $candidatePath 'release-manifest.json') -Raw | ConvertFrom-Json
$runtimeManifest = Get-Content -LiteralPath (Join-Path $runtimeRelease 'runtime-release.json') -Raw | ConvertFrom-Json

try {
    New-Item -ItemType Directory -Path $staging | Out-Null

    $selfHostDirectory = Join-Path $staging '.self-host'
    $selfHostArchive = Join-Path $staging 'EaglerTouhou-SelfHost.zip'
    $bundleArgs = @{
        RuntimeRelease = $runtimeRelease
        OutputDirectory = $selfHostDirectory
        OutputArchive = $selfHostArchive
    }
    if ($UnicodeFont) {
        $bundleArgs.UnicodeFont = (Resolve-Path -LiteralPath $UnicodeFont).Path
    }
    & (Join-Path $PSScriptRoot 'build-self-host-bundle.ps1') @bundleArgs
    Remove-Item -LiteralPath $selfHostDirectory -Recurse -Force

    $runtimeAssetRoot = Join-Path $staging '.runtime-asset'
    New-Item -ItemType Directory -Path $runtimeAssetRoot | Out-Null
    Copy-Item -LiteralPath $runtimeRelease -Destination (Join-Path $runtimeAssetRoot 'runtime-release') -Recurse
    $runtimeArchive = Join-Path $staging 'EaglerTouhou-Runtime.zip'
    Compress-Archive -Path (Join-Path $runtimeAssetRoot 'runtime-release') -DestinationPath $runtimeArchive -CompressionLevel Optimal
    Remove-Item -LiteralPath $runtimeAssetRoot -Recurse -Force

    $assets = @($selfHostArchive, $runtimeArchive)
    $checksums = foreach ($asset in $assets) {
        $hash = (Get-FileHash -LiteralPath $asset -Algorithm SHA256).Hash.ToLowerInvariant()
        "$hash  $([IO.Path]::GetFileName($asset))"
    }
    Set-Content -LiteralPath (Join-Path $staging 'SHA256SUMS.txt') -Value $checksums -Encoding ascii

    $metadata = [ordered]@{
        schema = 'eagler-touhou/github-release-assets/1'
        sourceReleaseId = [string]$releaseManifest.releaseId
        runtimeReleaseSchema = [string]$runtimeManifest.schema
        runtimeGames = @($runtimeManifest.games.PSObject.Properties.Name)
        recommendedAsset = 'EaglerTouhou-SelfHost.zip'
        advancedAsset = 'EaglerTouhou-Runtime.zip'
        excludedFormalOutputs = @('hosted-site', 'external-site', 'import-site', 'game-package', 'offline-zip')
    }
    Set-Content -LiteralPath (Join-Path $staging 'release-assets.json') -Value ($metadata | ConvertTo-Json -Depth 5) -Encoding utf8

    Move-Item -LiteralPath $staging -Destination $output
    Write-Host "GitHub Release assets ready: $output"
    Write-Host 'Recommended download: EaglerTouhou-SelfHost.zip'
    Write-Host 'Advanced Runtime-only download: EaglerTouhou-Runtime.zip'
} finally {
    if (Test-Path -LiteralPath $staging) { Remove-Item -LiteralPath $staging -Recurse -Force }
}
