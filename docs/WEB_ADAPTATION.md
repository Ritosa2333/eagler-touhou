# Eagler Touhou 接入

还原基线为 `492eddf`，用户已同意作为本轮还原验收通过版本；`experiments` 保留该基线，
`main` 承担 Web 适配。基线验收不等于覆盖全部可能的录像、菜单组合及设备。

协议依据为 `YomotsuHisami/eagler-touhou` 的 `608729a1`，该版本已迁入 monorepo：
产品规则在 `common/contracts/product-catalog.mjs` 及对应 `.d.mts`，启动器在 `launcher/`。
本地平台登记修改仍需随 Runtime 一起审阅、集成，未提交到对方仓库。
对应的可审阅改动同时保存在 `integration/eagler-touhou-th12.patch`，以免只推送本仓库时
遗漏另一个工作区中的产品登记、类型声明、内容布局和构建配置。

## 本轮实现

- 完整游戏入口接入 `eagler-touhou/1`，与单关预览的旧桥接独立。
- 校验父窗口、来源、导航 epoch；命令串行执行，错误回复保留请求身份。
- 存储和受管理 DATA 就绪后发送 `ready`；`launch` 才启动原版加载画面与游戏。
- 真实呈现后发送 `first-frame`，另发版本、帧率、音频状态；退出前等待存档落盘。
- 正式入口直接读取 Package Store 传入的 DATA，贴图、脚本、背景和 Demo 均不回退到开发服务器。
- 原版素材及转换数据放在独立 `th12.data`；18 首 OGG 作为音乐组件单独安装。
  片尾曲保留原版 `th10_17.ogg` 文件名。
- 文件接口使用存档白名单和相对路径描述；原版 `.rpy` 可导入；未知成绩、配置编码明确拒绝导入。
- 失焦释放输入，隐藏页停止推进并保存；WebGL 丢失后停止游戏并报告错误。
- 接受启动器的键盘映射；手柄映射在每个逻辑 tick 读取，启用时替代游戏内原始手柄轮询。

## 生成包

先按 BUILD.md 从自己持有的原版生成本地资源，随后：

```sh
npm run build
npm run package:eagler
npm run prepare:eagler-content
```

`build-eagler/` 为代码包，包含 `th12.html`、相对模块依赖、Wasm、声明和
`runtime-files.json`。它不包含 EXE、DAT、贴图、音频和用户录像。

`artifacts/eagler-content/` 为本地素材包，包含 `th12.data`、`music/` 与来源清单。
它包含原权利人的内容，不纳入源代码提交。`--assets=DIR` 可指定已准备的素材目录，
`--output=DIR` 可指定包输出目录。

Runtime URL 使用启动器提供的 `managedData=1`、`gameGeneration` 和正整数
`runtimeEpoch`。正式入口不自动启动，也不使用实验预览的录像语料清单。

本地开发服务器的 `/adapter-preview.html` 是人工检查入口：它挂载上面两个生成目录，
通过相同协议启动完整游戏，并可查看当前浏览器的存档列表。此页面不是对方启动器，
不进入 Runtime Release，不代表 Hosted／External／Import 平台链已验收。

平台登记的源码目录为 `ports/th12`。当前独立源码仍在 Tweet-META/th12；
正式平台构建前需把这份 main 的源码按 monorepo 的目录归属集成，不能把旧预览目录冒充源码。

## 尚未完成的必需项

当前是接入初版，`manifest.json` 明确标记 `completeAdapter:false`。
触控全套输入、流式 OGG、常显判定点、原版成绩／配置编解码、平台图卡与字体、
完整的 Hosted／External／Import 发布及离线链尚待完成。
未接通的配置会明确拒绝，不能将占位成功响应视为已支持。

高刷新率插值、THPrac、THCRAP、多人均未声明启用；后续若实现，按平台隔离规则另开实验工作区。
用户要求不自行测试，本轮没有执行浏览器试玩、游戏回归或平台验收，打包成功不代表适配验收通过。
