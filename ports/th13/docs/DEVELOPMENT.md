# th13 开发指南

## 构建布局

网页和 WASI harness 共享 `web/cpp/game/` 中的 C++ 实现。SDL3 / Emscripten 设备层在 `web/cpp/sdl/`；原版函数锚点见 `web/cpp/rebuild/AddressMap.hpp`。

正式构建使用 eagler-touhou monorepo 布局：本仓根目录放在 `ports/th13/`，共享依赖位于 monorepo 根目录的 `common/` 与 `tools/build/`。独立仓库不复制共享依赖。安装 Emscripten 后设置 `EMSDK`；WASI harness 使用 `WASI_SDK_PATH`（以构建脚本的环境变量为准）。

## 容器运行

在 monorepo 根目录运行：

```sh
podman build --target dev -t th13-dev \
  --build-arg DEV_UID=$(id -u) --build-arg DEV_GID=$(id -g) \
  ports/th13/container
podman run --rm --userns=keep-id -e HOME=/home/dev \
  -v "$PWD:/work:Z" -w /work/ports/th13 th13-dev \
  bash -lc 'node portable/build.mjs'
```

若使用已有外层开发工作区，保留其 `make exec CMD='...'` 与 `make shot CMD='...'` 入口。不在宿主安装构建或浏览器依赖。

## 构建命令

在 `ports/th13/` 运行：

```sh
node portable/build.mjs
node portable/build-harness.mjs
```

网页产物为 `web/artifacts/sdl3/th13-sdl.mjs` 和 `.wasm`。构建报告记录输入与产物哈希。WASI harness 位于 `web/artifacts/wasi/`，只用于局部行为、事件及绘制诊断，不能独立认证浏览器最终输出。

Launcher 集成运行 `node portable/build-eagler.mjs`（构建后调用 `portable/package-eagler.mjs`）生成 `build-eagler/` 与 `runtime-files.json`；运行时按上游协议接收资源、配置、键盘、启动和存档请求。具体命令与验证结果见 [ADAPTATION.md](../ADAPTATION.md)。

## 测试

在端口目录运行源码契约测试：

```sh
node --test tests/rng.test.mjs tests/harness-paths.test.mjs tests/submission.test.mjs tools/run-child.test.mjs
node --test tests/ui.test.mjs
```

C++ 套件默认以端口根目录为工作目录。如果测试需要合法原版资源，可设置 `TH13_TEST_WORKSPACE` 指向拥有 `artifacts/reference/raw/` 的本地分析工作区。资源缺失或断言失败应保留失败结果，不修改测试期望、golden、容差或过滤字段来放行。

```sh
TH13_TEST_WORKSPACE=/path/to/private-workspace node --test tests/ui.test.mjs
```

源码测试不等于原作行为一致；截图不等于数值认证。参考资格、捕获完整性、严格行为/绘制一致、独立原版 GPU 与浏览器最终输出、场景覆盖分别报告。缺少独立原版 GPU 真值时，最终画面认证保持 BLOCKED。

## 资源与分析边界

用户自行提供合法的 v1.00c 数据与字体。解包后的资源、音乐、录像、截图、反编译 dump 和对照轨迹只存放于 ignored 目录，不提交到任一源码仓库。资源格式见 [ASSETS.md](ASSETS.md)，许可见 [NOTICE.md](../NOTICE.md)。

现有生产音频读取 `/assets/thbgm.fmt` 与单个 `/assets/thbgm.ogg`，按原版虚拟 PCM 偏移和循环点播放。不要将其替换为未经验证的逐曲文件布局。
