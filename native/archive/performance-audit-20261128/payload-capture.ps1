$ErrorActionPreference='Stop'
$captureRoot='D:/Temp/native-performance-20261128/payload-cost/captures'
foreach ($folder in @('core','game-settings','game-settings-layout','lobby')) {
  New-Item -ItemType Directory -Path "$captureRoot/$folder" -Force | Out-Null
}
$env:PUB_CACHE='D:/workspace/eagler/toolchains/flutter-pub-cache'
$env:TEMP='D:/Temp/native-performance-20261128/payload-cost/flutter-temp'
$env:TMP=$env:TEMP
$env:TOUHOU_CAPTURE_FONTS='D:/workspace/eagler/toolchains/flutter-stable/bin/cache/artifacts/material_fonts'
$env:TOUHOU_CAPTURE_DIR="$captureRoot/core"
$env:TOUHOU_GAME_SETTINGS_CAPTURE="$captureRoot/game-settings"
$env:TOUHOU_SETTINGS_LAYOUT_CAPTURE="$captureRoot/game-settings-layout"
$env:TOUHOU_LOBBY_CAPTURE="$captureRoot/lobby"
Set-Location -LiteralPath 'D:/workspace/eagler/artifacts/.native-room-client-win'
$files=@('tools/capture_native_baseline_test.dart','test/launcher_language_test.dart','test/lobby_page_test.dart','test/launcher_navigation_test.dart')
for ($index=0; $index -lt $files.Count; $index++) {
  $log="D:/Temp/native-performance-20261128/payload-cost/capture-$index.log"
  & 'D:/workspace/eagler/toolchains/flutter-stable/bin/flutter.bat' test --no-pub --concurrency=1 $files[$index] *> $log
  if ($LASTEXITCODE -ne 0) { Get-Content -LiteralPath $log -Tail 20; exit $LASTEXITCODE }
  Get-Content -LiteralPath $log -Tail 1
}
