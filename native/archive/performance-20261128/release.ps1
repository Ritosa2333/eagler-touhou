param([string]$Run='release-before',[string]$Target='release_probe.dart')
$ErrorActionPreference='Stop'
$taskRoot='D:/workspace/eagler/artifacts/native-launcher-85941f0d56/hover-detail-perf'
$stageRoot='D:/workspace/eagler/artifacts/.native-room-client-win'
$m=Get-CimInstance Win32_PerfFormattedData_PerfOS_Memory
if(($m.CommitLimit-$m.CommittedBytes) -lt 4GB){throw 'Insufficient native build headroom'}
$env:PUB_CACHE='D:/workspace/eagler/toolchains/flutter-pub-cache'
$env:TEMP="$taskRoot/test-temp"
$env:TMP=$env:TEMP
$env:PATH='D:/workspace/eagler/toolchains/nuget;'+$env:PATH
$env:CL='/I"D:/workspace/eagler/toolchains/flutter-android-gradle/preview-local/account-atl/Microsoft.VC.14.50.18.0.ATL.Headers.base/Contents/VC/Tools/MSVC/14.50.35717/atlmfc/include"'
$env:_LINK_='/LIBPATH:"D:/workspace/eagler/toolchains/flutter-android-gradle/preview-local/account-atl/Microsoft.VC.14.50.18.0.ATL.X64.base/Contents/VC/Tools/MSVC/14.50.35717/atlmfc/lib/x64"'
& 'C:/Python314/python.exe' 'D:/workspace/eagler/artifacts/native-launcher-85941f0d56/account-web/sync-stage.py' *> "$taskRoot/stage-sync-$Run.log"
if($LASTEXITCODE -ne 0){throw 'Stage sync failed'}
if($Run -eq 'release-before'){
  Copy-Item -LiteralPath "$taskRoot/source-before/library.dart","$taskRoot/source-before/launcher_navigation.dart" -Destination "$stageRoot/lib" -Force
}
Set-Location -LiteralPath $stageRoot
$env:SkipUnchangedFilesOnCopyAlways='true'
& 'D:/workspace/eagler/toolchains/flutter-stable/bin/flutter.bat' build windows --release --no-pub --target "$taskRoot/$Target" --dart-define="PERF_RUN=$Run" *> "$taskRoot/$Run.log"
if($LASTEXITCODE -ne 0){Get-Content "$taskRoot/$Run.log" -Tail 25;throw 'Build failed'}
$bundle="$taskRoot/$Run-app"
New-Item -ItemType Directory -Force -Path $bundle | Out-Null
Copy-Item -Path "$stageRoot/build/windows/x64/runner/Release/*" -Destination $bundle -Recurse -Force
Get-Content "$taskRoot/$Run.log" -Tail 3
