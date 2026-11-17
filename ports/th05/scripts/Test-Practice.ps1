param([Parameter(Mandatory=$true)][string]$GameDirectory)
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
$gamePath=(Resolve-Path -LiteralPath $GameDirectory).Path
& (Join-Path $PSScriptRoot 'Build-Practice.ps1') -GameDirectory $gamePath
$vswhere=Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$task=Join-Path $projectRoot 'artifacts\practice'
$sandbox=Join-Path $task 'sandbox'
New-Item -ItemType Directory -Path $sandbox -Force | Out-Null
foreach($name in @('kaiki1.DAT','kaiki2.DAT','ZUN.COM','anex86.bmp','glew32.dll','PMDWin.dll','replay.bmp')) {
 Copy-Item -LiteralPath (Join-Path $gamePath $name) -Destination $sandbox -Force
}
foreach($musicAsset in @('music.dat','music-font.bmp')) { if(Test-Path -LiteralPath (Join-Path $gamePath $musicAsset)) {Copy-Item -LiteralPath (Join-Path $gamePath $musicAsset) -Destination $sandbox -Force} }
Copy-Item -LiteralPath (Join-Path $projectRoot 'practice.bmp') -Destination $sandbox -Force
Copy-Item -LiteralPath (Join-Path $gamePath 'SE') -Destination $sandbox -Recurse -Force
New-Item -ItemType Directory -Path (Join-Path $sandbox 'replay') -Force | Out-Null
$objects=Get-ChildItem -LiteralPath (Join-Path $projectRoot 'Release') -Filter '*.obj' | Where-Object {$_.Name -ne 'main.obj'} | ForEach-Object {'"'+$_.FullName+'"'}
@('/nologo','/MACHINE:X86','/SUBSYSTEM:CONSOLE','/OUT:artifacts\practice\sandbox\PracticeTests.exe','artifacts\practice\PracticeTests.obj','lib\glew32.lib','opengl32.lib','glu32.lib','kernel32.lib','user32.lib','gdi32.lib','shell32.lib','ole32.lib','oleaut32.lib','uuid.lib','comdlg32.lib','advapi32.lib')+$objects | Set-Content -LiteralPath (Join-Path $task 'test-link.rsp') -Encoding ascii
@('@echo off',('call "'+(Join-Path $vs 'VC\Auxiliary\Build\vcvars32.bat')+'" >nul'),'cl /nologo /EHsc /MT /O2 /DWIN32 /DUNICODE /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /Ithird_party\imgui /Foartifacts\practice\PracticeTests.obj /c tests\PracticeTests.cpp','if errorlevel 1 exit /b 1','link @artifacts\practice\test-link.rsp') | Set-Content -LiteralPath (Join-Path $task 'test-build.cmd') -Encoding ascii
Push-Location $projectRoot
try {& cmd /c (Join-Path $task 'test-build.cmd');if($LASTEXITCODE){throw 'Test build failed.'}} finally {Pop-Location}
Push-Location $sandbox
try {& .\PracticeTests.exe;if($LASTEXITCODE){throw 'Practice checks failed.'}} finally {Pop-Location}
