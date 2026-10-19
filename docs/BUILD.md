# 本地构建与原版输入

## 网页核心

安装 Node.js 22+、Python 及 Emscripten 4.0.22，并激活 Emscripten 环境。
构建脚本读取 `EMSDK`／`EMSDK_PYTHON`；也支持工作区相邻 `tools/emsdk` 的安装。

```sh
npm run build
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

使用自己的 TH12 1.00b、thtk 12 与公开 Python 依赖准备资源：

```sh
node scripts/prepare-retail.mjs --original="自己的安装目录" --thdat="thdat路径" --python="Python路径"
```

完整路径与输出说明见 [CONTENT_PREPARATION.md](CONTENT_PREPARATION.md)。资源准备只读文件，
不执行原版程序、游戏模拟或行为采样器。
资源处理的 Python 依赖包括 Pillow、pefile 和 imageio-ffmpeg；独立的原版 x86 行为研究使用
Unicorn 2.1.4；反编译工具的依赖见 `scripts/reverse/requirements.txt`。
输出只进入被忽略的 `local/`、`artifacts/`、`site/assets/` 和 `site/replays/`。

资产相关测试、原版探针及全程比较使用本地输入，不能用其他作品资源替代。
默认游戏入口为 `site/index.html`，完整流程及局部验证范围见 [FULL_GAME_FLOW.md](FULL_GAME_FLOW.md)。
`site/preview.html` 保留独立关卡、拖动回放及检查入口。
完整行为／展示 Oracle 尚未验收，子系统及合成短流程成功不代表整局还原成功。

诊断核心与正式发布核心现为独立产物。原版函数测试钩子和局部世界开关只编译到
`TH12_DIAGNOSTIC=1` 的诊断核心；正式打包拒绝诊断构建。完整 Replay 的逐帧候选采样、
不可变金标准、quick/daily 入口及尚缺的原版完整状态 provider，见
[Replay verifier](../tools/replay-verifier/README.md)。该实现轮没有运行验证或采集原版数据。

使用原版函数钩子的既有测试已改为引用独立诊断核心。维护者之后要运行这些
测试时，先生成诊断产物，再调用原有入口；本轮没有执行下列测试命令：

```powershell
$env:TH12_DIAGNOSTIC='1'
$env:TH12_THPRAC='1'
node scripts/build.mjs
Remove-Item Env:TH12_DIAGNOSTIC
npm run test:portable
```
