param([switch]$SkipCompile,[string]$Output='ports/thkkd/build/site')
$ErrorActionPreference='Stop'
$repository=Resolve-Path (Join-Path $PSScriptRoot '../../..')
Push-Location $repository
try {
  if($SkipCompile){node ports/thkkd/scripts/build-eagler.mjs --package-only}
  else {node ports/thkkd/scripts/build-eagler.mjs}
  if($LASTEXITCODE){throw 'WASM 构建失败'}
  python -X utf8 -B ports/thkkd/scripts/prepare-site-inputs.py
  if($LASTEXITCODE){throw '站点素材准备失败'}
  node ports/thkkd/scripts/prepare-import-site.mjs
  if($LASTEXITCODE){throw '导入站点配置失败'}
  bun run --cwd launcher build
  if($LASTEXITCODE){throw '启动器构建失败'}
  node launcher/scripts/package-server.mjs --games=thkkd --test-build=1 --thkkd-build=ports/thkkd/runtime --no-download=1 --site-manifest=ports/thkkd/build/site-inputs/import-site.json --artwork-dir=ports/thkkd/build/site-inputs/artwork --profile=web-validation-thkkd-original --site-url=http://127.0.0.1:8174/ "--output=$Output"
  if($LASTEXITCODE){throw '站点打包失败'}
} finally {Pop-Location}
