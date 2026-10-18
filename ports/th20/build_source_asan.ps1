param([switch]$Run)
$ErrorActionPreference='Stop'
$cmake='D:\cmake\bin\cmake.exe'
$build=Join-Path $PSScriptRoot 'build_source_asan'
$source=Join-Path $PSScriptRoot 'source_reconstruction'
& $cmake -S $source -B $build -G 'Visual Studio 16 2019' -A Win32 '-DCMAKE_CXX_FLAGS=/DWIN32 /D_WINDOWS /W3 /GR /EHsc' '-DTH20_SOURCE_ASAN=ON' '-DCMAKE_EXE_LINKER_FLAGS=' '-DCMAKE_TRY_COMPILE_CONFIGURATION=RelWithDebInfo'
if($LASTEXITCODE){throw 'Memory diagnostic configuration failed'}
& $cmake --build $build --config RelWithDebInfo --target th20_source --parallel 6
if($LASTEXITCODE){throw 'Memory diagnostic build failed'}
$output=Join-Path $build 'source_game\RelWithDebInfo'
foreach($name in @('th20.dat','thbgm.dat')){
    $inputAsset=Join-Path $PSScriptRoot "dist\source_game\$name"
    $outputAsset=Join-Path $output $name
    if(-not(Test-Path -LiteralPath $outputAsset)){New-Item -ItemType HardLink -Path $outputAsset -Target $inputAsset | Out-Null}
}
$compilerRuntime='D:\VS2019\IDE\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x86\clang_rt.asan_dynamic-i386.dll'
Copy-Item -LiteralPath $compilerRuntime -Destination $output
if($Run){
    $runRoot=Join-Path $source ('diagnostics\asan_run_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))
    $configRoot=Join-Path $runRoot 'userdata\ShanghaiAlice\th20'
    New-Item -ItemType Directory -Path $configRoot -Force | Out-Null
    $config=[IO.File]::ReadAllBytes((Join-Path $PSScriptRoot 'dist\source_game\userdata\ShanghaiAlice\th20\th20.cfg'))
    [BitConverter]::GetBytes([int]4).CopyTo($config,0x78)
    $config[0x82]=1
    $config[0x84]=$config[0x84] -band 0x7f
    [IO.File]::WriteAllBytes((Join-Path $configRoot 'th20.cfg'),$config)
    $env:ASAN_OPTIONS='halt_on_error=1:alloc_dealloc_mismatch=1:windows_hook_legacy_allocators=false:log_path="'+((Join-Path $runRoot 'asan').Replace('\','/'))+'"'
    & (Join-Path $PSScriptRoot 'build_sources\diagnostics\RelWithDebInfo\th20_source_debugger.exe') (Join-Path $output 'th20_source.exe') (Join-Path $runRoot 'userdata') $runRoot
    exit $LASTEXITCODE
}
Write-Host "Memory diagnostic executable: $output"
