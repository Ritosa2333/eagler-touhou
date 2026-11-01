# Paraglide 与三语界面

站点界面使用 Paraglide JS 编译后的消息函数，提供 `zh-CN`、`en`、`ja`。原有 `translate(locale, key, params)`、`t(key, params)` 接口作为兼容层调用 Paraglide，业务组件无需引入第二套国际化上下文。

## 消息来源与生成

- `messages/zh-CN.json`、`messages/en.json`、`messages/ja.json` 是三语文案的唯一来源。
- `messages/keys.json` 将原有带点的业务键映射到 Paraglide 标识符，并声明参数名。新增消息应同时补齐三语和这份映射。
- `project.inlang/settings.json` 从锁文件管理的本地 `@inlang/plugin-message-format` 加载插件，生成期间无需从 CDN 下载插件。
- `src/paraglide/` 由编译器生成并自动忽略，不能手工维护。`bun run i18n:compile` 可独立生成；Vite 插件也会自动编译。类型检查、正式构建和完整 UI 测试入口均先生成消息。

所有组件均显式传递 locale，消息函数不依赖跨请求共享的当前语言。项目仅做静态预渲染，没有运行时 SSR；编译器使用 `baseLocale` 策略并禁用不需要的 AsyncLocalStorage。若未来新增运行时 SSR，应重新评估服务端 locale 隔离。

`UI_MESSAGES` 保留为惰性兼容检查接口；日常翻译不再读取双语元组并进行正则替换。参数不足时仍保留 `{parameter}`，未知键仍返回键名，兼容原有诊断入口。

## 路由与偏好

中文入口为 `/`，英文入口为 `/en.html`，日语入口为 `/ja.html`。日语入口参与静态预渲染、子路径发布、离线缓存图及发布产物校验。

联机大厅共用 `/lobby.html`；语言切换时使用 `lang` 查询参数，同时保留原有参数的编码、hash 和历史状态。未指定语言的大厅恢复浏览器内保存的界面语言。独立说明页与服务状态界面读取相同的语言偏好。

FAQ、首次使用须知、联机玩法与关于页的正文同样使用 Paraglide。富文本译文保留链接和结构，并通过原有内容清洗器渲染。首次使用和联机玩法保留内容加载、错误重试与 seen 状态；切换语言后重新展示对应正文。

帮助链接携带 `lang` 参数，因此首次打开日语入口且尚未保存偏好时，帮助页也使用日语。独立页同步正文、浏览器标题、描述和返回入口。

兼容性帮助提供 `compatibility.html`、`compatibility.en.html`、`compatibility.ja.html`。英文和日文在内容构建时调用 Paraglide 生成静态 HTML，并保持 ES5 检测脚本；页面无需加载现代 React 或国际化运行时，禁用 JavaScript 后仍可阅读和选择语言。

界面语言和游戏资源语言是两个设置。游戏包、语音、用户昵称、导入文件、原作资源及独立的数据迁移操作页继续由原有所有者提供。校验和协议错误保留原始标识，在界面展示边界翻译已登记的错误；未知错误和技术堆栈保留原文。

## 顶栏与侧栏

顶栏在桌面和移动端独立保留品牌与语言选择器。至少 1024px 时，左侧栏支持 208px 图文模式及 76px 图标模式；折叠状态存储于 `eagler-touhou-sidebar-collapsed-v1`。图标模式保留可访问名称和鼠标提示。

窄屏继续使用图文底栏；桌面折叠状态不会隐藏手机文字。帮助与设置面板避开上下导航区域。站点导航处于原有设置遮罩与工具窗下方，原生 dialog、游戏和房间工作区继续拥有自己的焦点与层级。

## 验证

在专用远程机器的 `launcher/` 执行：

```sh
bun run i18n:check
bun run typecheck:ui-rewrite
bun run typecheck:ui-rewrite:tools
bun run build:ui-rewrite
bun run test:paraglide:browser -- <候选站地址>
```

[前端工具链](frontend-tooling.md)说明 Biome、Vitest 和文案审计入口。

`i18n:check` 验证三语键完整性、参数一致性及原有 1,273 条中英文文案的哈希。浏览器检查覆盖顶栏、折叠持久化、真实语言选择器、日语独立入口、刷新、单机／联机往返、日语输入设置和手机上下栏避让。
