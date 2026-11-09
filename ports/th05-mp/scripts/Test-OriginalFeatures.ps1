param([Parameter(Mandatory=$true)][string]$GameDirectory)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
# The regular suite prepares the dependency libraries and isolated retail sandbox.
& (Join-Path $PSScriptRoot 'Test-Practice.ps1') -GameDirectory $GameDirectory
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$task=Join-Path $projectRoot 'artifacts\practice'
$response=Get-Content -LiteralPath (Join-Path $task 'test-link.rsp') -Raw
$response=$response.Replace('PracticeTests','OriginalFeaturesTests')
[IO.File]::WriteAllText((Join-Path $task 'original-link.rsp'),$response,[Text.Encoding]::ASCII)
@('@echo off',('call "'+(Join-Path $vs 'VC\Auxiliary\Build\vcvars32.bat')+'" >nul'),'cl /nologo /EHsc /MT /O2 /DWIN32 /DUNICODE /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /Foartifacts\practice\OriginalFeaturesTests.obj /c tests\OriginalFeaturesTests.cpp','if errorlevel 1 exit /b 1','link @artifacts\practice\original-link.rsp') | Set-Content -LiteralPath (Join-Path $task 'original-build.cmd') -Encoding ascii
Push-Location $projectRoot
try {& cmd /c (Join-Path $task 'original-build.cmd');if($LASTEXITCODE){throw 'Original feature test build failed.'}} finally {Pop-Location}
Push-Location (Join-Path $task 'sandbox')
try {& .\OriginalFeaturesTests.exe;if($LASTEXITCODE){throw 'Original feature checks failed.'}} finally {Pop-Location}
