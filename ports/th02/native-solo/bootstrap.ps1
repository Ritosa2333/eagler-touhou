# Fetches the pinned libth03 (th98port) revision and applies the port's patch.
#
# external/th98port is deliberately NOT vendored in this repository: the
# dependency has no published license, so we only reference it and ship the
# small set of changes this port needs.
[CmdletBinding()]
param(
    [string]$Destination,
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
$root = if ($PSScriptRoot) { $PSScriptRoot } else { Split-Path -Parent $MyInvocation.MyCommand.Path }
if (-not $Destination) { $Destination = Join-Path $root 'external/th98port' }

$commit = '5d7e1ba0b139c4b048478f62bdd0f027082c7d08'
$url = 'https://github.com/christianazinn/th98port.git'
$patch = Join-Path $root 'patches/th98port-soew.patch'

$midiPatch = Join-Path $root 'patches/tinysoundfont-soew.patch'
$fmCheckpointPatch = Join-Path $root 'patches/fm-checkpoint-soew.patch'
foreach ($file in @($patch, $midiPatch, $fmCheckpointPatch)) {
    if (-not (Test-Path -LiteralPath $file)) { throw "Missing patch file: $file" }
}

if (Test-Path -LiteralPath $Destination) {
    if (-not $Force) {
        throw "Destination already exists: $Destination (use -Force to recreate)"
    }
    Remove-Item -LiteralPath $Destination -Recurse -Force
}

git clone --no-checkout $url $Destination
if ($LASTEXITCODE -ne 0) { throw "git clone failed" }
git -C $Destination checkout --detach $commit
if ($LASTEXITCODE -ne 0) { throw "git checkout $commit failed" }
git -C $Destination apply --whitespace=nowarn $patch $midiPatch
if ($LASTEXITCODE -ne 0) { throw "git apply failed" }
git -C $Destination apply --whitespace=nowarn $fmCheckpointPatch
if ($LASTEXITCODE -ne 0) { throw "FM checkpoint patch failed" }

Write-Host "libth03 ready at $Destination ($commit + SoEW dependency patches)"
