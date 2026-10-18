param(
    [ValidateSet('Release','RelWithDebInfo')][string]$Configuration='RelWithDebInfo',
    [string]$GameDirectory='E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders'
)
$ErrorActionPreference='Stop'
$cmakeExecutable='D:\cmake\bin\cmake.exe'
if (-not (Test-Path -LiteralPath $cmakeExecutable)) { $cmakeExecutable=(Get-Command cmake -ErrorAction Stop).Source }
$sourceRoot=Join-Path $PSScriptRoot 'source_reconstruction'
$buildRoot=Join-Path $PSScriptRoot 'build_sources'
$packageRoot=Join-Path $PSScriptRoot 'dist\source_game'
function Get-SourceGuard {
    $guard=@{}
    $roots=@($sourceRoot,(Join-Path $PSScriptRoot 'native_recovered'),(Join-Path $PSScriptRoot 'src'),(Join-Path $PSScriptRoot 'include'))
    $files=& rg --files @roots -g '*.cpp' -g '*.hpp' -g '*.h' -g '*.rc' -g 'CMakeLists.txt' -g '!**/build*/**' -g '!**/oracle/**' -g '!**/audit/**' -g '!**/*compare*' -g '!**/*test*'
    if ($LASTEXITCODE) { throw 'Cannot enumerate source inputs for the build snapshot' }
    foreach ($file in $files) { $guard[$file]=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash }
    return $guard
}
$stableBuild=$false
for ($attempt=0; $attempt -lt 4; ++$attempt) {
    & $cmakeExecutable -S $sourceRoot -B $buildRoot -G 'Visual Studio 16 2019' -A Win32
    if ($LASTEXITCODE) { throw 'Source game CMake configuration failed' }
    $beforeBuild=Get-SourceGuard
    & $cmakeExecutable --build $buildRoot --config $Configuration --target th20_source th20_source_debugger --parallel 4
    if ($LASTEXITCODE) { throw 'The independent source game has not linked; see compilation and linker errors above' }
    $afterBuild=Get-SourceGuard
    $stableBuild=$beforeBuild.Count -eq $afterBuild.Count
    foreach ($file in $beforeBuild.Keys) { if ($beforeBuild[$file] -ne $afterBuild[$file]) { $stableBuild=$false; break } }
    if ($stableBuild) { break }
    Write-Host 'Source inputs changed during compilation; rebuilding before packaging.'
}
if (-not $stableBuild) { throw 'Source inputs are still changing; candidate packaging was deferred' }
New-Item -ItemType Directory -Path $packageRoot -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $packageRoot 'userdata') -Force | Out-Null
$builtExe=Join-Path $buildRoot "source_game\$Configuration\th20_source.exe"
Copy-Item -LiteralPath $builtExe -Destination (Join-Path $packageRoot 'th20_source.exe')
$builtPdb=Join-Path $buildRoot "source_game\$Configuration\th20_source.pdb"
if (Test-Path -LiteralPath $builtPdb) { Copy-Item -LiteralPath $builtPdb -Destination (Join-Path $packageRoot 'th20_source.pdb') }
$originalManifest=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'reports\source_manifest.json') -Raw | ConvertFrom-Json
$assets=@()
foreach ($name in @('th20.dat','thbgm.dat')) {
    $inputFile=Join-Path $GameDirectory $name
    $expected=($originalManifest.files | Where-Object name -EQ $name).sha256
    $digest=(Get-FileHash -LiteralPath $inputFile -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($digest -ne $expected) { throw "Original asset hash mismatch: $name" }
    $destination=Join-Path $packageRoot $name
    if (-not (Test-Path -LiteralPath $destination) -or (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant() -ne $digest) {
        Copy-Item -LiteralPath $inputFile -Destination $destination
    }
    $assets+=@{name=$name;sha256=$digest}
}
# Enumerate actual MSBuild compile inputs in the executable's dependency tree.
$visitedProjects=[System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
$compiledFiles=[System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
function Read-SourceProject([string]$projectFile) {
    $projectFile=[System.IO.Path]::GetFullPath($projectFile)
    if (-not $visitedProjects.Add($projectFile)) { return }
    [xml]$projectXml=Get-Content -LiteralPath $projectFile -Raw
    $projectDirectory=Split-Path -Parent $projectFile
    foreach ($node in $projectXml.SelectNodes("//*[local-name()='ClCompile' or local-name()='ResourceCompile'][@Include]")) {
        $path=$node.GetAttribute('Include')
        if (-not [System.IO.Path]::IsPathRooted($path)) { $path=Join-Path $projectDirectory $path }
        if (Test-Path -LiteralPath $path -PathType Leaf) { $null=$compiledFiles.Add([System.IO.Path]::GetFullPath($path)) }
    }
    foreach ($node in $projectXml.SelectNodes("//*[local-name()='ProjectReference'][@Include]")) {
        $path=$node.GetAttribute('Include')
        if (-not [System.IO.Path]::IsPathRooted($path)) { $path=Join-Path $projectDirectory $path }
        Read-SourceProject $path
    }
}
Read-SourceProject (Join-Path $buildRoot 'link_probe\th20_source.vcxproj')
$sources=@{}
foreach ($file in $compiledFiles) { $sources[[System.IO.Path]::GetRelativePath($PSScriptRoot,$file)]=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant() }
$manifest=@{
    status='independently_compiled_source_candidate';full_game_equivalence_verified=$false;
    configuration=$Configuration;executable_sha256=(Get-FileHash -LiteralPath $builtExe -Algorithm SHA256).Hash.ToLowerInvariant();
    original_executable_is_build_input=$false;source_inputs_stable_during_build=$true;assets=$assets;compiled_sources=$sources;
    note='This source build still needs end-to-end gameplay and replay validation. The separate historical playable package is not used.'
}
$manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $packageRoot 'build_manifest.json') -Encoding UTF8
$launch="@echo off`r`ncd /d `"%~dp0`"`r`nset `"APPDATA=%~dp0userdata`"`r`nstart `"`" /D `"%~dp0`" `"%~dp0th20_source.exe`"`r`n"
[System.IO.File]::WriteAllText((Join-Path $packageRoot 'Play.cmd'),$launch,[System.Text.Encoding]::ASCII)
Write-Host "Independent source candidate: $packageRoot"
