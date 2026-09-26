# 东方花映塚 1.50a · C++ / SDL3 网页版

本地日文 1.50a 的具名 C++ 游戏逻辑重建。游戏通过 Emscripten 编译为 Wasm，使用本工作区永夜抄、风神录共用的 SDL3 / WebGL2 渲染器与触控基础。浏览器不运行原版 EXE、x86 模拟器或原版函数地址分发。

## 游玩

公网入口：`https://api.steinsgateon.com/app/th09.html`，当前 Cloudflare 回源端口为 **3007**。全局启动器：`../th10_web/scripts/public-host.ps1 -Action start -Game th09 -Port 3007`。本地测试脚本的 8096 端口独立保留。

双击 `启动花映塚网页版.cmd`，保持服务窗口开启，访问 `http://127.0.0.1:8096/app/th09.html`。独立发布目录是 `artifacts/sdl-release`，可在安装 Node.js 22+ 的电脑上执行 `npm start`，该命令使用 3007 端口。手机访问需要 HTTPS 公网入口或有安全上下文的部署。

- 键盘：方向键移动，Z 射击／确认，长按 Z 蓄力，X 快速蓄力攻击／取消，Shift 低速，Esc 暂停，Ctrl 跳过对话。
- 手机：拖动移动；“射击 开”自动连按射击，“蓄力”按住蓄力后松开释放，B 对应 X，“低速”按住使用。菜单拖动选择，点击确认；对话可点击继续或按住快进。
- 花映塚的攻击和蓄力规则保留本作实现。
- 双人本机对战可在 Key Config 选择原版 1P / 2P 键盘模式或两个手柄。浏览器联机从标题的 Network 或网页“联机”创建／加入房间。
- 联机使用双方输入同步，有 6 帧输入缓冲；网络等待时暂停推进。房间内可选择旁观席，观战端从本局第 0 帧顺序回放已确认的双人输入，不占用玩家席或发送操作。对战结束后在原版结算菜单选择“保存成绩”，各自进入录像存槽／命名界面；保存或跳过后返回联机房间，下局可重新开始。两名玩家的操作记录在同一份原版格式录像中，双方可各自保存。浏览器联机不能与 Windows 原版网络协议互通。
- 存档、设置和录像在当前浏览器的独立 `/savesth09` IndexedDB 中；“存档 / 录像”可导入或下载备份。联机服务器只转发输入和校验值，没有存档或录像。

首次资源与运行时下载共约 159 MiB（19 首 OGG、原版数据包、字体、Wasm）。后续资源通过浏览器 Cache Storage 按 SHA-256 复用。短暂断网或 502/503/504 会最多自动重试三次；连续失败可重新点击开始。资源缓存和玩家存档分开；服务器只提供明确列出的游戏文件。

## 开发目录与构建

`cpp/game`：GameSession、GameWorld、Player、BulletManager、Enemy、TitleMenus、AnmRenderer、资源／录像／音效等具名管理器。
`cpp/sdl`：Application、GraphicsDevice、Assets、FontDevice、AudioDevice。
`../portable/sdl` 与 `../portable/input`：三款游戏共用的渲染、帧调度和触控源码。完整开发交付必须包含这两个目录，不能只复制 th09_web。
`sdl-runtime`：网页、浏览器存储、输入和房间客户端。
`scripts`：构建、资源处理、发布服务器和内存房间转发。
`tests/cpp`、`tests/browser`、`tests/server`：原版逐帧对照、浏览器与服务检查。
`scripts/native`、`reference/native`：开发期 original oracle；与实际发布隔离。

工作区已配置 `../tools/emsdk`，Node 位于 `../th10_web/tools/node.exe`。命令在 th09_web 执行：

```text
npm run web:build
npm run web:package
npm start
```

`web:package` 同时打包共用启动器和游戏运行时，不再生成旧的独立调试入口作为首页。

### 接入当前 eagler-touhou 启动器

在 `th09_web` 执行 `npm run web:build`，准备 `assets/sdl-native/cp932.bin` 和 `blend.bin` 后执行 `npm run web:eagler`，再执行 `npm run web:eagler:multiplayer`。两个输出目录 `build-eagler`、`build-eagler-multiplayer` 分别作为 `eagler-touhou/scripts/package-runtime-release.mjs` 的 `--th09-build`、`--th09-multiplayer-build` 输入。两者使用同一 WASM 游戏核心，但联机版由 launcher 房间的 `netplay*` 配置直启双人对局；游戏内“妖怪对妖怪”的 Network 入口唤起同一个 launcher 房间弹窗。原版 `th09.dat`、共享字体和 19 首 OGG 由宿主包单独提供。TH09 适配器使用现行 `eagler-touhou/1` 通信及导航 epoch；旧 `web:package` 附带的启动器副本不应覆盖独立的 `eagler-touhou` 仓库。TH09 已改用 launcher 的共用房间服务，不再需要单独部署旧 `/netplay` 中继。对局传输直接链接 `eagler-common` 的 `BrowserPeerTransport`，与 TH06/07 一样在 WebRTC 与 WebSocket 中继间选择；TH09 的帧输入仍使用本作的有序双人 lockstep，而不是 TH06/07 的 rollback。

只有原版安装目录时，可先在 `th09_web` 执行 `node scripts/prepare-retail-assets.mjs "原版游戏目录"`。脚本核对原版 1.50a 的 EXE/DAT 哈希，从 `th09.dat` 提取并校验 `thbgm.fmt`，以本机 ffmpeg/ffprobe 将 `thbgm.dat` 转为 19 首 OGG，逐首核验 PCM 帧数，并从工作区 TH10 共享资源准备字体表。生成的原版素材与音乐均被 Git 忽略。编译会优先使用 `TH09_EMSDK`，然后依次查找工作区 `tools/emsdk`、相邻 `th08/tools/emsdk`。

开发比较器：`npm run cpp:build` 使用工作区 WASI SDK；`npm test` 跑 91 项常规比较测试。原版 EXE 必须匹配 target.json。完整重新解析原版包使用 `npm run test:archive-oracle`。`python scripts/prepare-assets.py` 从原版 BGM 生成保留 PCM 帧数和循环点的 OGG；这是有损音频编码，不应称作音频字节完全一致。

独立重建需要保留原始游戏目录、portable、开发工具/对应依赖。Unicorn JS 开发依赖当前在 `../th08_web/node_modules/@alexaltea/unicorn-js`；不是浏览器运行依赖。原版 EXE 和开发工具不进入公开服务白名单。

## 验证范围

2026-09-21 回放修复及测试组十项反馈复核见 [录像修复与复核](docs/2026-09-21-录像修复与复核.md)。失败样本 `th9_udyt22.rpy` 已在实际发布版由录像管理器导入、连续回放九关共 71,637 帧并正常结束；自带三个演示在本轮比较器中完成 23,993 帧原版对照。不同测试的构建哈希与验证范围保存在报告中，不能据此推断所有角色、所有录像均已验证。

详细结果见 `docs/RECONSTRUCTION.md` 和 `artifacts/cpp/verification`。2026-09-20：91 项常规比较测试通过；原版三个演示录像 17,108 帧完整世界状态对照；28 条 Story/Extra 路径、252 次换关和 28 次结局流程验证。流程测试中的强制胜利有明确标注，并不代表原版所有通关录像都已逐帧对照。

发布版已检查手机尺寸／触控、音效与音乐输出、存档刷新、录像导入保存、联机双方状态、旧站点 Service Worker 迁移和资源白名单。电脑上的手机模拟不是 Android/iOS 真机性能证明。字体使用原版 MS Gothic 的 SDL_ttf 光栅化，音频使用 OGG；不承诺所有像素、所有浮点输入或所有手机驱动都与 Windows 版完全一致。

音乐“无”模式下宿主不传输任何 OGG，因此 `AudioDevice::music` 在静音时只登记曲目、不打开解码器：否则标题与对局会因缺少 `/music/*.ogg` 直接报错。原版 OGG 挂载后仍逐首核对 PCM 帧数，缺失或帧数不符依旧是错误。

联机手势传输的是**绝对目标点**而不是按下采样瞬间算出的速度：锁步输入本身带有 `lead` 帧延迟，若在发送端用当时的机体位置换算速度，接收端会在“未来位置”上朝向目标，机体就会绕着手指打转。现在两端都在实际移动该机体的那一帧用各自的位置换算（`MotionSample.target`，协议动作模式 2/3），因此落点与手指一致且两端确定性相同。单机路径仍是原来的本地换算，录像中记录的仍是换算后的速度。

第三方组件及原版资源归属见 THIRD-PARTY-NOTICES.txt。
