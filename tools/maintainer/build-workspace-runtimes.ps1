[CmdletBinding()]
param(
    [string] $EmsdkDirectory,
    [string] $CMake,
    [string] $Ninja,
    [switch] $EmbedLocalAssets,
    [hashtable] $GameAssetDirectories,
    [string] $Th06AssetDirectory,
    [string] $Th07AssetDirectory,
    [int] $Parallel = 6
)

$ErrorActionPreference = 'Stop'
$project = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
Import-Module (Join-Path $PSScriptRoot 'lib\workspace-layout.psm1') -Force
$workspaceLayout = Get-EaglerWorkspaceLayout -ProjectRoot $project
$workspace = $workspaceLayout.Root
$runtimeBuildConfigPath = Join-Path $project 'config\runtime-builds.json'
$runtimeBuildConfig = Get-Content -LiteralPath $runtimeBuildConfigPath -Raw | ConvertFrom-Json
if ($runtimeBuildConfig.schema -ne 'eagler-touhou/runtime-builds/1' -or -not $runtimeBuildConfig.games) {
    throw "Invalid Runtime build config: $runtimeBuildConfigPath"
}
$cmakeGames = @(
    $runtimeBuildConfig.games.PSObject.Properties |
        Where-Object { $_.Value.builder -eq 'cmake' } |
        ForEach-Object { [string]$_.Name }
)
if ($cmakeGames.Count -eq 0) { throw 'Runtime build config declares no CMake products' }
foreach ($game in $cmakeGames) {
    $entry = $runtimeBuildConfig.games.$game
    if (-not $entry.variants.development -or -not $entry.variants.external) {
        throw "$game CMake workspace build requires development and external Runtime variants"
    }
}
if (-not $EmsdkDirectory) { $EmsdkDirectory = Get-EaglerWorkspacePath $workspaceLayout 'toolchains' 'emsdk' }
$emsdk = (Resolve-Path -LiteralPath $EmsdkDirectory).Path
$emcmake = Join-Path $emsdk 'upstream\emscripten\emcmake.exe'
if (-not (Test-Path -LiteralPath $emcmake -PathType Leaf)) { throw "emcmake not found: $emcmake" }
$env:EM_CONFIG = Join-Path $emsdk '.emscripten'

function Find-BuildTool([string] $Value, [string] $Name, [string] $VisualStudioPattern) {
    if ($Value) { return (Get-Command $Value -ErrorAction Stop).Source }
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $vswhere -PathType Leaf) {
        $match = & $vswhere -latest -products * -find $VisualStudioPattern | Select-Object -First 1
        if ($match) { return $match }
    }
    throw "$Name not found; pass -$Name with its executable path"
}
$CMake = Find-BuildTool $CMake 'CMake' 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$Ninja = Find-BuildTool $Ninja 'Ninja' 'Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe'

function Resolve-RuntimeBuildPlan([string] $Game, [string] $Variant, [string] $AssetRoot) {
    $resolver = Join-Path $project 'scripts\resolve-runtime-build.mjs'
    $resolverArgs = @($resolver, "--game=$Game", "--variant=$Variant", '--runtime-extension=')
    if ($AssetRoot) { $resolverArgs += "--asset-root=$AssetRoot" }
    $json = & node @resolverArgs
    if ($LASTEXITCODE -ne 0) { throw "$Game/$Variant Runtime build profile resolution failed: $LASTEXITCODE" }
    return $json | ConvertFrom-Json
}

if (-not $GameAssetDirectories) { $GameAssetDirectories = @{} }
$legacyAssetDirectories = @{
    th06 = $Th06AssetDirectory
    th07 = $Th07AssetDirectory
}
foreach ($game in $legacyAssetDirectories.Keys) {
    if (-not $GameAssetDirectories.ContainsKey($game) -and $legacyAssetDirectories[$game]) {
        $GameAssetDirectories[$game] = $legacyAssetDirectories[$game]
    }
}
if (-not $EmbedLocalAssets -and $GameAssetDirectories.Count -gt 0) {
    throw 'Asset directories require -EmbedLocalAssets'
}
$assetDirectories = @{}
if ($EmbedLocalAssets) {
    foreach ($game in $cmakeGames) {
        $profile = $runtimeBuildConfig.games.$game
        $directory = if ($GameAssetDirectories.ContainsKey($game) -and $GameAssetDirectories[$game]) {
            [string]$GameAssetDirectories[$game]
        } else {
            Get-EaglerWorkspacePath $workspaceLayout ([string]$profile.workspaceRepository) 'assets'
        }
        $assetDirectories[$game] = (Resolve-Path -LiteralPath $directory).Path
    }
}

foreach ($game in $cmakeGames) {
    $variant = if ($EmbedLocalAssets) { 'development' } else { 'external' }
    $profileAssetRoot = if ($EmbedLocalAssets) { $assetDirectories[$game] } else { $null }
    $plan = Resolve-RuntimeBuildPlan $game $variant $profileAssetRoot
    $source = Get-EaglerWorkspacePath $workspaceLayout $plan.workspaceRepository
    if (-not (Test-Path -LiteralPath (Join-Path $source 'CMakeLists.txt') -PathType Leaf)) {
        throw "Runtime source not found: $source"
    }
    # Source-only and playable development builds must never share a CMake cache.
    # Reconfiguring one directory between TH_EXTERNAL_ASSETS=ON/OFF leaves the
    # development host pointing at a runtime with no game archives.
    $buildName = if ($EmbedLocalAssets) { 'build-web-eagler-default' } else { 'build-web-eagler-external' }
    $build = Join-Path $source $buildName
    $configureArguments = @(
        '-S', $source, '-B', $build, '-G', 'Ninja', "-DCMAKE_MAKE_PROGRAM=$Ninja",
        @($plan.cmakeArguments)
    )
    & $emcmake $CMake @configureArguments
    if ($LASTEXITCODE -ne 0) { throw "$game Web configure failed: $LASTEXITCODE" }
    & $CMake --build $build --parallel $Parallel
    if ($LASTEXITCODE -ne 0) { throw "$game Web build failed: $LASTEXITCODE" }
}

$kind = if ($EmbedLocalAssets) { 'playable local development' } else { 'source-only external-assets' }
$labels = @($cmakeGames | ForEach-Object { $_.ToUpperInvariant() })
Write-Host "$([string]::Join(', ', $labels)) $kind Web runtimes are ready."
