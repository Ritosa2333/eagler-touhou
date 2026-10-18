# 永夜抄原生平台迁移

游戏规则仍由本项目的 TH08 GameApplication 和各个游戏系统负责。共用 eagler-touhou Launcher、运行时协议、SDL3/WebGL2 渲染器、触摸手势和精确数值快速路径；不引入联机、练习扩展、高刷插值等功能。

C++ 持有主循环、输入、资源解压、图片解码、SDL_ttf 文字、PCM 音效和 FLAC 音乐。浏览器只负责安装资源、IDBFS 持久化、界面协议与 MIDI 合成设备。MIDI 事件顺序由原有 C++ MidiPlayer 决定，这与参考 TH07 MidiWeb 的职责划分相同。

DAT 通过 SDL_IOStream 按段读取，不再把整个资源包复制进 Wasm。解压缓存上限 32 MiB，CPU 图片缓存上限 8 MiB；启动时只解码标题常用图片，其他图片按需解码。Music Room 常用字形分批提前生成。进入一面的同一浏览器检查中，Wasm 堆由最初迁移时约 256 MiB 降为约 92 MiB；此数字不包括 JavaScript 资源、浏览器缓存和 GPU 内存。

保留原版字体、阴影与像素格式转换；SDL_ttf 的字形边缘不保证与 Windows GDI 逐像素相同。音乐由原 PCM 无损压缩，并逐首完整解码核对。原存档 `th08-native-1.00d` 迁入 `/savesth08`，不覆盖已有新存档。

构建：`node portable/build.mjs --th08`；资源准备：`python portable/prepare-native-music.py --th08`；打包：`node portable/package-architecture.mjs --th08`。依赖版本与风神录共用。验证结果放在对应候选包的 validation 目录；编译成功不等于已验证整款游戏所有行为。

验证包括：完整 character0-final 录像与 2.2.1 平台的 100,153 tick / 3,339 次状态抽样对照；317 个原版 DAT 资源在解密前后共 634 次字节比较；21 首音乐完整 PCM 比较；浏览器原生触摸、旧录像迁移、离线重启游戏与持久删除。复用旧版录像轨迹仅证明这条路径的平台变更一致，并不证明所有角色、难度和分支均与原版完全一致。

发布包内 `source` 提供对应源代码和构建脚本。进入 `source` 后，使用包内 Node 执行 `portable/restore-release-assets.mjs`，从旁边经哈希校验的 `site` 恢复 DAT、字库和音乐；无需重新处理原始 PCM。执行 `python tools/download-emscripten.py` 安装固定 SDK，使用 `npm install --prefix tools/architecture/typescript typescript@5.9.3` 安装前端编译器，再执行上述构建和打包命令。历史原版测试夹具不包含在发行源码目录中。
