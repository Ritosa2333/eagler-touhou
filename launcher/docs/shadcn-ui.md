# 东方主题组件

启动器使用 shadcn/ui 的源码组件方式，组件放在 `app/components/ui/`，CLI 配置为 `components.json`。`Button`、`Input`、`Textarea`、`Badge`、`Switch` 与 `Progress` 通过共享语义 token 接入现有界面。`Switch`、`Progress` 和 `asChild` 组合使用 Radix，样式变体使用 CVA，类名合并使用 `cn()`。

## 主题映射

`public/styles.css` 继续定义原站的颜色、字体和动效，`app/shadcn.css` 将其映射到 shadcn 的语义接口。

| shadcn token | 东方站来源 | 用途 |
| --- | --- | --- |
| `--background` / `--foreground` | `--ink` / `--paper` | 墨色背景与纸色文字 |
| `--card` / `--popover` | `--panel` | 面板与浮层 |
| `--primary` / `--destructive` | `--red` | 朱红操作与危险操作 |
| `--ring` / `--accent` | `--red-bright` | 焦点与强调 |
| `--border` / `--input` | `--line` | 边界与输入框 |
| `--muted-foreground` | 原有 `--muted` | 次要文字 |
| `--color-muted` | `--muted-surface` | 次要背景面 |
| `--font-sans` / `--font-heading` | `--ui-font` / `--art-font` | Yatra、寒蝉圆体及原站字体回退 |
| `--ease-touhou` | `--ease` | 原站缓动 |

原有 `--muted` 表示文字颜色；shadcn 中同名语义表示背景面，因此背景使用独立的 `--muted-surface`，原有变量及其消费者保持原义。`touhou98` 字标、封面、背景插画、纸纹和作品配色继续由原站样式定义。

Tailwind 只载入 theme 与 utilities，不载入全局 preflight，保留原站的标题、表单、原生弹窗和游戏画布布局。已有的无 layer 样式继续控制专用界面；新组件使用语义工具类。`hidden`、系统减弱动效和站内 `.less-motion` 都有明确兼容规则。

## 组件使用

新界面优先使用普通 shadcn 变体：

```tsx
<Button onClick={save}>保存</Button>
<Button variant="outline" asChild><a href="lobby.html">联机大厅</a></Button>
<Input aria-label="房间码" inputMode="numeric"/>
<Switch checked={enabled} onCheckedChange={setEnabled} aria-label="启用功能"/>
<Progress value={percent} aria-label="下载进度"/>
```

复用已有启动器样式的按钮使用 `variant="legacy" size="legacy"`，保留其几何、图标、封面和动效；已有 `option-switch` 使用 `Switch variant="legacy"`，防止旧的 transform 与新的 translate 叠加。业务方继续传入原有 ID、ref、事件和原生 `type`；组件不默认把提交按钮改成普通按钮。

弹窗、全屏游戏、回放时间轴、触控编辑器和主选择器保留专门的生命周期与指针处理。其内部普通操作按钮复用 Button；原生 `dialog.showModal()`、路由返回、全屏层级和原生 select 的值／change 入口继续由原有业务组件管理。

添加组件时在远程构建机的 `launcher/` 执行 `bunx --bun shadcn@latest add <组件名>`，检查 diff 并复用上述 token。保留现有 `components.json` 和主题映射；不要通过重新初始化预设覆盖东方主题。

## 图标与动效

通用操作图标使用 `lucide-react` 的具名导入，覆盖播放、导入、返回、刷新、键盘、手柄和站点导航。描边读取 `--icon-stroke-width`，颜色继承所在控件；图标设为 `aria-hidden`，操作名称继续由按钮文字或 `aria-label` 提供。东方字标、阴阳标识、作品封面和品牌素材保留原有表现。

导航菜单、房间操作菜单与通用工具窗的入场使用 `motion/mini`。`--motion-duration` 继承原站页面时长，`--motion-ease` 继承 `--ease`，菜单与面板位移分别由 `--motion-distance-menu`、`--motion-distance-panel` 控制。原生说明弹窗继续沿用现有的动画完成与焦点生命周期。

Motion 控制入场期间的透明度和位移，保留浮层居中所需的原有 transform。完成、关闭或减弱动效开启时，清理临时内联样式和观察器；已交给 Motion 的节点不重复播放旧 CSS 入场。系统 `prefers-reduced-motion` 和站内 `.less-motion` 都能关闭这层动画，关闭时的导航、焦点与原有业务状态仍由各组件管理。

## 响应式站点导航

`LauncherMasthead` 统一单机、联机及独立说明页的导航。宽度至少 1024px 时使用 208px 左侧栏，较窄视口使用底部四入口导航；原站 `styles.css` 保持不变，覆盖层集中在 `app/site-navigation.css`。尺寸由 `--site-rail-width`、`--site-bar-height` 控制，浮层和选中态继续读取纸色、墨色及朱红 token。CSS 和组件中的 1024px 媒体断点须同步修改。

手机底栏及正文补偿 `safe-area-inset-bottom`，通知和提示浮层避开底栏。帮助及设置面板只显示当前分组，支持方向键进入、Escape 关闭并返回打开按钮；面板未展开时使用 `inert`。菜单可见性立即更新，高度及 Motion 入场负责动画，避免 `visibility` 过渡阻止初始焦点。游戏及房间工作区状态隐藏导航并取消导航占位。

远程构建候选后可执行 `bun run test:responsive-navigation:browser -- <候选站地址>`。浏览器回归覆盖 1024/1023px 边界、平板、320px 手机、横屏、路由切换、说明页、键盘焦点及减弱动效；游戏和房间仅做 CSS 状态探针，不会启动游戏或加入房间。

## 顶栏、折叠与三语

顶栏独立保留品牌与语言切换；桌面侧栏宽度由 `--site-rail-expanded`、`--site-rail-collapsed` 控制，手机继续使用底栏。导航层级低于原有设置遮罩，避免遮住返回与关闭按钮。站点界面已由 Paraglide 编译消息接管并支持日语，维护方式见 `paraglide-i18n.md`。
