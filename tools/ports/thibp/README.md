# 东方鬼葬剑 1.10b Web 单机版

启动器通过 eagler-touhou/1 导入本机游戏包。运行时使用 ph3-modern 的 C++、SDL3 和 WebGL2，保留标题、剧情、结局、两种内置练习和游戏内 Replay。开启「显示设置 → 高刷新率」后，画面使用相邻逻辑帧的插值，游戏按原版 60 Hz 推进。右下角显示实际的画面呈现率。

## 本地审核

准备 Git、Node.js 24、Python 3.12 或更新版本、仓库要求的 Bun 和 Emscripten 6.0.9。从仓库根目录运行以下 PowerShell 命令。`$engine` 指向固定版本的源码 ZIP，`$package` 指向单独提供的 `thibp-1.10b-zh-local.zip`。[ph3-modern 源码 ZIP](https://codeload.github.com/Ritosa2333/ph3-modern/zip/12ac7141b5cdcd9cabd4479ef76e6453f19fcdf3)

```powershell
$env:EMSDK = 'PATH_TO_EMSDK'
$engine = 'PATH_TO_PH3_MODERN_ZIP'
$package = 'PATH_TO_THIBP_OFFLINE_PACKAGE'

python tools/ports/thibp/build_runtime.py $engine launcher/runtime/thibp
python tools/ports/thibp/prepare_launcher.py site-manifest $package .cache/thibp-site.json
$env:EAGLER_DEVELOPMENT_SITE_MANIFEST = (Resolve-Path -LiteralPath .cache/thibp-site.json).Path
$env:EAGLER_TOUHOU_HOST = '127.0.0.1'

Set-Location launcher
bun install --frozen-lockfile --ignore-scripts
node scripts/build-launcher.mjs
node scripts/serve.mjs 8814
```

打开试玩页，选「导入游戏包」，选择 `$package`，然后启动。要验证高刷，在开局前进入游戏设置的「显示设置」开启高刷新率。运行时产物和站点清单请保存在本地构建目录。[本地试玩页](http://127.0.0.1:8814/?game=thibp)

### 准备离线包

`prepare_content.py` 接受已解包的 1.10b 发行目录。汉化版可通过 `--japanese` 指定日文目录，用于检测并转码 GBK 脚本。`prepare_launcher.py package` 将生成的 DATA 与本机可用的 `msgothic.ttc` 组装为离线包。原始游戏脚本、图像、音频和字体均由本机资源包提供。

```powershell
New-Item -ItemType Directory -Path .cache -Force | Out-Null
python tools/ports/thibp/prepare_content.py PATH_TO_UNPACKED_ZH .cache/thibp.data --japanese PATH_TO_UNPACKED_JA
python tools/ports/thibp/prepare_launcher.py package .cache/thibp.data PATH_TO_FONT .cache/thibp-package
```

## 验证范围

浏览器回归脚本支持 Playwright、Chromium，以及现有的离线包。可通过 `THIBP_TEST_REFRESH_HZ=120` 或 `144` 测试高刷，并用 `THIBP_TEST_LOCK_DISPLAY=1` 测试锁 60。`--save-replay-and-quit` 验证命名保存、退出、重启和回放。[验收记录](../../../archive/thibp-web-review-20261125/README.md) · [源码来源](../../../docs/migration/thibp-ph3-modern.md)

```text
node tools/ports/thibp/check-browser.mjs URL ZIP NEW_REPORT_DIRECTORY BROWSER PLAYWRIGHT --save-replay-and-quit
```
