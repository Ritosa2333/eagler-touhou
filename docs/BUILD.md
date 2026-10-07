# 本地构建与原版输入

## 网页核心

安装 Node.js 22+、Python 及 Emscripten 4.0.22，并激活 Emscripten 环境。
构建脚本读取 `EMSDK`／`EMSDK_PYTHON`；也支持工作区相邻 `tools/emsdk` 的安装。

```sh
npm run build
npm run test:portable
npm start
```

构建只读取本仓库源码，不需要原版 EXE。生成的 `site/runtime/core.mjs`／`core.wasm`
是本项目网页核心，不是原版程序；不作为原版资源上传。

## 游戏资源与原版行为对照

原版文件仅在本地使用。现有资源准备工具采用以下工作区布局，克隆时可使用
`git clone https://github.com/Tweet-META/th12.git th12-eagler`：

```text
workspace/
  th12-eagler/        本仓库
    local/retail/     从自己的 DAT 提取的文件，不提交
  th12/              自有 TH12 1.00b 安装，含 EXE／DAT，不提交
  tools/             可选本地分析与构建工具，不提交
```

使用自己的 TH12 1.00b 和 thtk 12 在 `local/retail/` 准备数据，再运行：

```sh
python scripts/prepare-assets.py
python scripts/prepare-presentation.py
python scripts/prepare-music.py
python scripts/reverse/sample-sound-queue.py
node scripts/build-corpus.mjs
```

资源处理的 Python 依赖包括 Pillow、pefile 和 imageio-ffmpeg；原版 x86 隔离执行使用
Unicorn 2.1.4；反编译工具的依赖见 `scripts/reverse/requirements.txt`。
输出只进入被忽略的 `local/`、`artifacts/`、`site/assets/` 和 `site/replays/`。

资产相关测试、原版探针及全程比较使用本地输入，不能用其他作品资源替代。
完整行为 Oracle 尚未通过，子系统测试成功不代表整局还原成功。
