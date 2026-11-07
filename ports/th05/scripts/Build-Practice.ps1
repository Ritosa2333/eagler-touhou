param([Parameter(Mandatory=$true)][string]$GameDirectory)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
$gamePath=(Resolve-Path -LiteralPath $GameDirectory).Path
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$installation=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format json | ConvertFrom-Json | Select-Object -First 1
if(!$installation){throw 'Install Visual Studio C++ Desktop tools and Windows SDK.'}
$version=([version]$installation.installationVersion).Major
$toolset=switch($version){18{'v145'}17{'v143'}16{'v142'}15{'v141'}default{'v140'}}
$toolsRoot=Join-Path $installation.installationPath 'VC\Tools\MSVC'
$compilerVersion=Get-ChildItem -LiteralPath $toolsRoot -Directory | Sort-Object {[version]$_.Name} -Descending | Select-Object -First 1
$tools=Join-Path $compilerVersion.FullName 'bin\Hostx64\x86'
$msbuild=Join-Path $installation.installationPath 'MSBuild\Current\Bin\MSBuild.exe'
Push-Location $projectRoot
try {
 & python (Join-Path $PSScriptRoot 'prepare-imports.py') --game $gamePath --tools $tools
 if($LASTEXITCODE){throw 'Dependency preparation failed.'}
 & $msbuild rt5sandbox.vcxproj /p:Configuration=Release /p:Platform=Win32 "/p:PlatformToolset=$toolset" /p:WindowsTargetPlatformVersion=10.0 /m /verbosity:minimal
 if($LASTEXITCODE){throw 'Practice build failed.'}
 Write-Host "Built: $(Join-Path $projectRoot 'Release\uth05win-thprac.exe')"
} finally {Pop-Location}
