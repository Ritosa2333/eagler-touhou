# 网页性能与兼容验证

游戏逻辑固定为 60 Hz。性能测量分别报告每秒实际呈现次数（FPS）、每秒核心逻辑步数和录像输入消费频率；CPU 4 倍限速由浏览器 DevTools 协议施加，并以 iframe 内固定整数工作量校验倍率。比较窗口使用同一机体、同一输入和相同起止游标。

## Web 地址映射后台

恢复的核心通过原程序地址访问各个状态对象。Web 构建以 `WebMemory.hpp` 接入同一接口的 `IndexedGameMemory.hpp`，原 `src/core` 和生成的 `src/render` 保持原字节。所有 WASM 翻译单元使用相同的强制包含入口；构建缓存同时记录这些头文件，避免旧对象混用不同类型布局。

后台把地址区间编成索引，并按完整的地址、长度、注册顺序复用最多 32 份布局。缓存保存绑定序号，实际状态指针属于各自实例。标量读取使用栈缓冲，写入在单一区间内直接找到相关绑定。跨区间读写保留原注册优先级、复制边界、错误地址和部分写入行为。

别名关系使用连续存储。未变化的连续段通过 `memcmp` 核对，变化段按原字节顺序处理；冲突、重复封存、中途失败及随机数游标同步沿用原语义。每次写入继续接纳全部别名状态，保持回调与直接字段写入的兼容行为。

固定 Emscripten 6.0.9、`-O2 -fno-fast-math -ffp-contract=off` 和原 24 位有效数计算。测试入口同时编译原映射与 Web 映射，对同一物理缓冲区逐操作恢复前态，比较返回值、全部写入、异常及随机数游标。

## 呈现缓冲

WebGL 按原命令顺序绘制，以贴图、混合模式、透明标志和裁剪区域作为批次边界。整帧顶点写入一块按需扩容、持续复用的 `Float32Array`，一次上传后按原批次的起点与长度绘制。相邻批次只更新发生变化的透明、混合与裁剪状态。

精灵坐标直接计算四个角点，复用默认颜色和深度参数；历史命令映射直接填充复用的 Map，减少逐逻辑帧的临时数组。顶点数值、三角形顺序、混合与插值计算保持一致。额外呈现继续只读命令快照。九机体 27 个场景的逐像素对照、调用次数与耗时见[绘制开销验证](../../../archive/thkkd-render-overhead-20261126/README.md)。

## 复现入口

以下命令在网页仓库根目录执行。构建与资源准备见[开发说明](DEVELOPMENT.md)，本地成品使用相同正式启动器和 Runtime。

```powershell
python -X utf8 ports/thkkd/scripts/test-indexed-memory.py
node ports/thkkd/scripts/build-eagler.mjs
node ports/thkkd/scripts/build-eagler.mjs --probe
python -X utf8 ports/thkkd/scripts/verify-wasm.py --archive ../recovery-archive --all
node ports/thkkd/tests/replay-regression-browser.mjs --url=http://127.0.0.1:8174/ --replays=path/to/authorized-replays --output=ports/thkkd/build/replay-regression
node ports/thkkd/tests/cpu-performance-browser.mjs --url=http://127.0.0.1:8174/ --replay=path/to/authorized-ex.rpy --output=ports/thkkd/build/cpu-performance
node ports/thkkd/tests/renderer-differential.mjs
```

无绘图回归逐字节核对原有不可变状态流，覆盖全部 17 份录像、42 组 Omniverse 和 EX 返回。浏览器录像检查从正式文件协议导入，使用原生播放及定位流程，记录每 1024 个输入的状态检查点和结束状态；`--baseline=旧版结果.json` 比较计时、随机数及游戏对象摘要。`--cases=corpus-01,corpus-02` 可分组执行，完整验收核对全部 17 个编号与输入身份。

分组结果用 `compare-browser-replays.py --before 旧版记录1.json 旧版记录2.json --after 新版记录1.json 新版记录2.json --output=汇总.json` 合并。工具按录像编号收集已完成记录，核对两组各自单一的 Runtime 身份、全部 17 份输入、每个检查点及结束状态，并保留检查点的 SHA-256。

`profile-browser.mjs` 在实际游戏中采集 DevTools 调用栈。构建增加 `--symbols` 生成 `build/web/thkkd.mjs.symbols`，同时核对 WASM 哈希与正式版本相同；`summarize-profile.py --profiles=采样目录 --symbols=符号表 --demangler=toolchains/emsdk/upstream/bin/llvm-cxxfilt.exe` 映射为函数名。调用栈采样服务于瓶颈定位，正式 FPS 测量关闭采样并单独运行。

顶点差分检查从 Git 基线读取原呈现器，比较 1800 条命令在七个插值参数下每次实际绘制的 GPU 状态与 Float32 顶点流，覆盖缓冲扩容、混合、裁剪、逐顶点颜色及透视，并核对 4800 条精灵命令；源码包可用 `--reference=旧版renderer.mjs` 指定参照文件。

`node ports/thkkd/tests/render-overhead-browser.mjs --data=本机测试DATA路径` 使用独立临时浏览器和合成原版格式录像，在九种机体的射击、Bomb 和后续场景中比较新旧呈现器。三个插值位置逐像素核对，绘制前后检查逻辑状态和命令只读性；计时交替运行同一帧的新旧实现。`gl.finish` 用于每次计时之前排空 GPU 队列，生产呈现流程保持异步提交。另行测量每帧 1200 个精灵的命令准备开销。测试 DATA 使用本机 `scripts/prepare-assets.py` 的输出，原始资源保留在本机。
