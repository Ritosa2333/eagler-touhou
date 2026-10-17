param([ValidateSet(6,7)][int]$Game, [Parameter(Mandatory=$true)][string]$OutputDirectory)
$vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
$installation = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $installation) { throw 'Visual C++ x86 build tools are required' }
$source = Split-Path -Parent $MyInvocation.MyCommand.Path
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$command = '"{0}\VC\Auxiliary\Build\vcvars32.bat" >nul && cl /nologo /LD /O2 /DTH_GAME={1} /Fo:"{2}\d3d8.obj" /Fe:"{2}\d3d8.dll" "{3}\d3d8.cpp" /link /DEF:"{3}\d3d8.def"' -f $installation,$Game,$OutputDirectory,$source
cmd /d /c $command
if ($LASTEXITCODE -ne 0) { throw "Present observer build failed: $LASTEXITCODE" }
$injector = '"{0}\VC\Auxiliary\Build\vcvars32.bat" >nul && cl /nologo /O2 /Fo:"{1}\injector.obj" /Fe:"{1}\present-injector.exe" "{2}\injector.cpp"' -f $installation,$OutputDirectory,$source
cmd /d /c $injector
if ($LASTEXITCODE -ne 0) { throw "Present injector build failed: $LASTEXITCODE" }
