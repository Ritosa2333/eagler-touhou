param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('replay', 'music-room')]
    [string]$Mode,

    [Parameter(Mandatory = $true)]
    [string]$Fixture,

    [int]$Port = 8136
)

$ErrorActionPreference = 'Stop'
$Repo = Split-Path -Parent $PSScriptRoot
$Build = Join-Path $Repo 'build-web-ordinary-audit'
$Cache = Join-Path $Build 'CMakeCache.txt'
$CmakeLine = Select-String -LiteralPath $Cache -Pattern '^CMAKE_COMMAND:INTERNAL=(.+)$'
if (-not $CmakeLine) { throw "CMAKE_COMMAND is missing from $Cache" }
$Cmake = $CmakeLine.Matches[0].Groups[1].Value
$FixturePath = (Resolve-Path -LiteralPath $Fixture).Path
$StageDir = Join-Path $Build 'web-assets-BASE\replay'
$StagedFixture = Join-Path $StageDir 'th6_01.rpy'
$Generated = @('th06.data', 'th06.html', 'th06.js', 'th06.wasm') | ForEach-Object { Join-Path $Build $_ }

function Rebuild-Ordinary {
    foreach ($Path in $Generated) {
        if (Test-Path -LiteralPath $Path) { Remove-Item -LiteralPath $Path -Force }
    }
    & $Cmake --build $Build -j 8
    if ($LASTEXITCODE -ne 0) { throw "ordinary Web build failed with exit $LASTEXITCODE" }
}

try {
    if ($Mode -eq 'replay') {
        New-Item -ItemType Directory -Force -Path $StageDir | Out-Null
        Copy-Item -LiteralPath $FixturePath -Destination $StagedFixture -Force
        Rebuild-Ordinary
    }
    python (Join-Path $PSScriptRoot 'ordinary-browser-smoke.py') $Mode --fixture $FixturePath --port $Port
    if ($LASTEXITCODE -ne 0) { throw "ordinary browser smoke failed with exit $LASTEXITCODE" }
}
finally {
    if ($Mode -eq 'replay' -and (Test-Path -LiteralPath $StagedFixture)) {
        Remove-Item -LiteralPath $StagedFixture -Force
        if ((Test-Path -LiteralPath $StageDir) -and -not (Get-ChildItem -LiteralPath $StageDir -Force)) {
            Remove-Item -LiteralPath $StageDir -Force
        }
        Rebuild-Ordinary
    }
}
