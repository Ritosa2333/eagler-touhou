param([switch]$Package)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$cmakeExecutable = (Get-Command cmake -ErrorAction SilentlyContinue).Source
if (-not $cmakeExecutable) {
    $vsLocator = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $vsLocator) {
        $vsDirectory = & $vsLocator -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        $cmakeExecutable = Join-Path $vsDirectory 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
    }
    if (-not $cmakeExecutable -or -not (Test-Path -LiteralPath $cmakeExecutable)) { throw 'Install CMake 3.25 or later, or the CMake component in Visual Studio.' }
}
Push-Location -LiteralPath $projectRoot
try {
    & $cmakeExecutable --preset windows-msvc
    if ($LASTEXITCODE) { throw 'CMake configuration failed' }
    & $cmakeExecutable --build --preset windows-release
    if ($LASTEXITCODE) { throw 'Compilation failed' }
    if ($Package) {
        & $cmakeExecutable --install build/windows --config Release --prefix dist/thkkd-windows-x64
        if ($LASTEXITCODE) { throw 'Packaging failed' }
        $pythonExecutable = (Get-Command python -ErrorAction SilentlyContinue).Source
        if (-not $pythonExecutable) { throw 'Install Python 3.11 or later to check release privacy.' }
        & $pythonExecutable -X utf8 -B tools/release.py --prepare-only
        if ($LASTEXITCODE) { throw 'Release preparation or privacy check failed' }
    }
} finally { Pop-Location }
