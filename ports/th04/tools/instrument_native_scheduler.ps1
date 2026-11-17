$ErrorActionPreference = 'Stop'
$ProjectRoot = (Resolve-Path "$PSScriptRoot/..").Path
$env:BINARYEN_CORES = '2'
$WasmOpt = "$ProjectRoot/build/binaryen-native-132/binaryen-version_132/bin/wasm-opt.exe"
& $WasmOpt "$ProjectRoot/build/native-scheduler/out-native/wdosbox-x-jspi.wasm" --all-features -O1 --asyncify -g '--pass-arg=asyncify-imports@env.emscripten_sleep,env.__asyncjs__syncSleep,env.__asyncjs__em_unload' '--pass-arg=asyncify-removelist@TH_CPU_Synchronous()*,TH_Read*,TH_Write*,TH_Fetch*,TH_Push*,TH_Pop*,TH_EA_16_*,TH_DoString*' -o "$ProjectRoot/build/native-scheduler/full-asyncify.wasm"
if ($LASTEXITCODE -ne 0) { throw 'Full source-candidate Asyncify instrumentation failed' }
node "$PSScriptRoot/prepare_native_scheduler_runtime.cjs"
if ($LASTEXITCODE -ne 0) { throw 'Native scheduler wrapper preparation failed' }
node "$PSScriptRoot/prepare_native_frame_presenter.cjs"
if ($LASTEXITCODE -ne 0) { throw 'Native framebuffer presenter preparation failed' }
