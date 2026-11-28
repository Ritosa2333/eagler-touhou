param([string]$Run='before',[string]$Target='perf_test.dart',[switch]$TraceSkia,[switch]$ReuseBinary)
$ErrorActionPreference='Stop'
$taskRoot='D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf'
$stageRoot='D:/workspace/eagler/artifacts/.native-room-client-win'
$m=Get-CimInstance Win32_PerfFormattedData_PerfOS_Memory
if(($m.CommitLimit-$m.CommittedBytes) -lt 4GB){throw 'Insufficient native profile headroom'}
New-Item -ItemType Directory -Force -Path "$taskRoot/test-temp" | Out-Null
$env:PUB_CACHE='D:/workspace/eagler/toolchains/flutter-pub-cache'
$env:TEMP="$taskRoot/test-temp"
$env:TMP=$env:TEMP
$env:PATH='D:/workspace/eagler/toolchains/nuget;'+$env:PATH
$env:CL='/I"D:/workspace/eagler/toolchains/flutter-android-gradle/preview-local/account-atl/Microsoft.VC.14.50.18.0.ATL.Headers.base/Contents/VC/Tools/MSVC/14.50.35717/atlmfc/include"'
$env:_LINK_='/LIBPATH:"D:/workspace/eagler/toolchains/flutter-android-gradle/preview-local/account-atl/Microsoft.VC.14.50.18.0.ATL.X64.base/Contents/VC/Tools/MSVC/14.50.35717/atlmfc/lib/x64"'
$env:TOUHOU_PERF_REPORT="$taskRoot/$Run.json"
& 'C:/Python314/python.exe' 'D:/workspace/eagler/artifacts/native-launcher-85941f0d56/account-web/sync-stage.py' *> "$taskRoot/stage-sync-$Run.log"
if($LASTEXITCODE -ne 0){throw 'Stage sync failed'}
Set-Location -LiteralPath $stageRoot
Copy-Item -LiteralPath "$taskRoot/perf_driver.dart" -Destination "$stageRoot/integration_test/perf_driver.dart" -Force
$motionArguments=@('drive','--profile','--no-pub','-d','windows','--driver',"$stageRoot/integration_test/perf_driver.dart",'--target',"$taskRoot/$Target")
if($TraceSkia){$motionArguments+=@('--trace-skia','--trace-skia-allowlist=skia.shaders','--endless-trace-buffer')}
if($ReuseBinary){$motionArguments+=@('--use-application-binary',"$stageRoot/build/windows/x64/runner/Profile/touhouonline.exe")}
& 'D:/workspace/eagler/toolchains/flutter-stable/bin/flutter.bat' @motionArguments *> "$taskRoot/$Run.log"
$result=$LASTEXITCODE
Get-Content "$taskRoot/$Run.log" -Tail 20
exit $result
