# 准备自己的 TH12 游戏资源

代码仓库不包含原版贴图、音乐、音效、脚本、演示录像或游戏安装文件。以下入口读取你本机的 TH12 1.00b，生成本地资源与可供运行时导入的 DATA。准备过程只解析文件和转换音频，不启动原版 EXE、Unicorn、浏览器或游戏模拟。

## 安装工具

- Node.js 22 或更新版本。
- Python 3.10 或更新版本，以及公开 Python 包 `Pillow`、`pefile`、`imageio-ffmpeg`。
- [thpatch/thtk](https://github.com/thpatch/thtk) 的 `thdat`。可以使用自己安装或编译的版本；Windows 二进制发行包中的随附 DLL 需保留在工具目录中。
- 自有的、未经修改的日文 TH12 1.00b 安装目录，其中有 `th12.exe`、`th12.dat` 和 `thbgm.dat`。

建议为准备脚本创建独立 Python 环境：

```powershell
python -m venv .venv
.\.venv\Scripts\python.exe -m pip install Pillow pefile imageio-ffmpeg
```

Linux 或 macOS 对应的 Python 路径为 `.venv/bin/python`。准备脚本不依赖工作区相邻目录中的 `tools/python`、原版行为采样器或私有 Python 模块。

音效与部分展示元数据读取 EXE 中固定地址的表，因此仅接受下面的 EXE SHA-256：

```text
99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417
```

其他版本、汉化补丁或修改过的 EXE 会被拒绝；脚本不会据此猜测其他版本的地址。

## 一次准备

在本仓库目录执行，替换示例中的安装目录与工具路径：

```powershell
node scripts/prepare-retail.mjs --original="D:\Games\TH12" --thdat="D:\Tools\thtk\thdat.exe" --python=".venv\Scripts\python.exe"
```

Linux 或 macOS 示例：

```sh
node scripts/prepare-retail.mjs --original="$HOME/Games/th12" --thdat=/opt/thtk/bin/thdat --python=.venv/bin/python
```

`--help` 仅显示选项，不提取或转换资源。各个路径选项也支持 `--original "D:\Games\TH12"` 这样的分开写法。

入口依次完成：

1. 检查三个安装文件、EXE 版本和 Python 依赖。
2. 调用 `thdat -x12 <原版安装目录>/th12.dat` 静态解包。该语法及“解包到当前目录”的行为见 [thtk 的 thdat 手册](https://github.com/thpatch/thtk/blob/master/thdat/thdat.1)。`thbgm.dat` 由音乐转换脚本直接读取，不交给 thdat 解包。
3. 准备 ANM 图集、ECL/SHT/STD/MSG 文件、展示表、音效、OGG 音乐、菜单、结局和演示录像元数据，以及独立的 Host 图卡。
4. 调用 `scripts/prepare-managed-content.mjs` 构建 `th12.data`、音乐目录及来源清单。

音效索引由 `prepare-sounds.py` 静态读取 PE 的 `0x4ae5a0` 音效表与 `0x4aea50` 文件名指针表，输出 60 条音效的编号、文件名、音量和循环设置。无需运行 `scripts/reverse/sample-sound-queue.py`。演示录像步骤只读取原版 `.rpy` 的元数据，不播放录像。

## 输出与路径配置

| 用途 | 命令行选项 | 环境变量 | 默认位置 |
| --- | --- | --- | --- |
| 原版安装文件 | `--original` | `TH12_ORIGINAL` | `local/original/` |
| thtk 解包工具 | `--thdat` | `TH12_THDAT`，其次 `THDAT` | PATH 中的 `thdat` |
| Python 解释器 | `--python` | `TH12_PYTHON` | PATH 中的 `python` |
| 解包资源 | `--retail` | `TH12_RETAIL` | `local/retail/` |
| 转换后的独立资源 | `--assets` | `TH12_ASSETS` | `site/assets/` |
| DATA 和音乐包 | `--output` | `TH12_CONTENT` | `artifacts/eagler-content/` |

命令行优先于环境变量。相对路径以调用入口时的当前目录为基准；不指定路径时，默认目录以仓库根目录为基准。环境变量 `TH12_NODE` 可供单独运行菜单脚本时指定 Node.js；总入口自动传入当前 Node.js 解释器。`imageio-ffmpeg` 使用其提供的 FFmpeg，也支持该包的 `IMAGEIO_FFMPEG_EXE` 环境变量。

成功后可使用 `artifacts/eagler-content/th12.data` 以及同目录的 `music/`。`provenance.json` 记录生成内容的哈希；`source-identity.json` 记录原版三个输入文件的大小与 SHA-256。`local/content-identity.json` 是独立准备脚本共享的本地来源清单。

`artifacts/eagler-content/artwork/th12-card.webp` 单独交给 Host 装配，不进入 Runtime 或 DATA。图卡使用原版 `title.anm` 的已解码 PNG：script102/103 的 sprite68/69 作为底图，script100/101 的 sprite66/67 作为灵梦标题主图，最后叠加 script108 的 sprite70 Logo。坐标、中心或左上锚点、入场插值终点及绘制层级直接读取对应脚本初始段，按原版 640×480 画布合成；不包含菜单、版本、版权文字或转场条带。图卡按平台现有卡片风格以 WebP quality55/method6 编码；同目录的 `th12-card.source.json` 记录原资源哈希、裁切、落点及图层来源。无需运行标题动画或游戏取截图。

总入口在新建的 `local/prepare-retail-*` 临时目录中提取、转换并打包，成功后才将结果复制到目标目录。新 DATA 只包含此次准备的资源，不读取旧目标目录中的遗留文件。复制会更新同名生成文件，保留目标目录中的其他文件；原版安装目录始终只读。三个输出目录必须互不包含，并与原版安装目录分开。

成功时会移除本次临时目录；失败时保留临时文件并打印位置。修正缺失输入、依赖或工具路径后，可重新执行总入口。此流程会处理全部资源与音乐，耗时及占用空间取决于本机环境。

## 单独执行某一步

各 Python 脚本都读取相同的 `TH12_ORIGINAL`、`TH12_RETAIL`、`TH12_ASSETS` 环境变量。例如已经解包后，重新生成资源：

```powershell
$env:TH12_ORIGINAL = 'D:\Games\TH12'
$env:TH12_RETAIL = (Resolve-Path 'local\retail').Path
$env:TH12_ASSETS = Join-Path (Get-Location) 'site\assets'
.\.venv\Scripts\python.exe scripts/prepare-assets.py
.\.venv\Scripts\python.exe scripts/prepare-presentation.py
.\.venv\Scripts\python.exe scripts/prepare-sounds.py
.\.venv\Scripts\python.exe scripts/prepare-music.py
.\.venv\Scripts\python.exe scripts/prepare-menu-assets.py
.\.venv\Scripts\python.exe scripts/prepare-host-artwork.py
node scripts/prepare-managed-content.mjs --assets="$env:TH12_ASSETS" --output="artifacts/eagler-content"
```

`prepare-assets.py` 先写入图集及来源清单，`prepare-presentation.py` 再补充固定展示表；菜单脚本需要已有的图集。`prepare-menu-assets.py --metadata-only` 可只刷新既有菜单中的符卡名、音乐室提示和加载元数据。

单独执行 `prepare-host-artwork.py` 时，`TH12_ASSETS` 指向已准备图集，`TH12_ARTWORK` 可更改图卡目录；默认写入 `artifacts/eagler-content/artwork/`。总入口始终将图卡放入所选 `--output` 的 `artwork/` 子目录。

原版输入与生成内容留在本地，默认输出目录已被 Git 忽略。资源准备成功仅表示生成内容完成，不代表游戏行为、整局还原或浏览器运行已验收。
