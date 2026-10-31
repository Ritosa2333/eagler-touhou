# 前端检查与测试

启动器使用 Biome 统一检查和格式化 React 前端、三语消息以及相关构建与测试配置；Vitest 统一运行 TypeScript 单元测试、UI 契约、支持夹具和原有 Node 断言测试。仓库根目录提供相同的 `lint`、`format` 和 `test:unit` 转发入口。

## 常用命令

在 `launcher/` 执行：

```sh
bun run lint
bun run lint:fix
bun run format:check
bun run format
bun run i18n:check
bun run i18n:audit
bun run test:unit
bun run test:unit:watch
bun run test:ui-rewrite
bun run test:support
bun run test:repository
bun run test:workspace
bun run test
```

`bun run test` 先生成 Paraglide 和启动器服务模块，再由 Vitest 执行全部仓库内测试。首次运行缺少 React 构建产物时会自动补齐；已有产物先校验再复用。原有 Node/React 环境检查继续执行。默认范围包括 `tests/vitest/**/*.test.ts`、`tests/react-main/*.test.mjs`、`tests/support/*.test.mjs` 和 `tests/test-plan.mjs` 中的仓库内测试。`test:ui-rewrite`、`test:support`、`test:repository` 保留分组入口，全部使用 Vitest。

`test:workspace` 使用独立配置执行原有跨游戏源码集成测试，需要相应游戏子仓库与工具链。`scripts/check.mjs` 的仓库内与工作区测试调度也统一调用 Vitest。浏览器、Python、需要私有原始资源的命令行验收和 monorepo 的独立服务测试沿用各自的专项入口。

旧 `node:assert/strict` 断言保留原意；测试注册、生命周期钩子、假计时器和用例结束清理由 Vitest 管理。原有动态子测试改为原生测试用例，公告检查也逐项报告。文件以独立子进程顺序运行，避免全局 DOM、缓存、发布夹具和监听端口相互污染。

`test:unit` 和应用类型检查先生成 Paraglide。工具类型检查同时覆盖两份 Vitest 配置与 TypeScript 测试。CI 的 UI 任务执行 Biome、词条检查和统一的 `bun run test`。独立测试配置不挂载 React Router 发布插件；发布构建测试仍在自己的临时目录中运行真实打包器。

启动器服务冷编译会将 Paraglide 和 JSON 词库打包为共享翻译模块，并将消息文件纳入缓存身份。修改译文后，服务模块与 React 页面使用同一组编译消息。

## Biome 的边界

`biome.json` 列出 React 前端、消息文件和本轮迁移工具的检查范围。生成的 Paraglide、缓存、构建产物、旧测试夹具和游戏源码不进入前端格式化；原站 `public/styles.css` 保留原文。

检查使用当前推荐规则。已有生命周期所有者的 Hook 依赖与部分组合控件的语义规则保留为警告，供后续按组件审查；不会自动增删依赖数组而重启 Runtime、抽屉或事件订阅。装饰 SVG 标明 `aria-hidden`，可访问名称由所属按钮提供。

项目的设置层叠依赖 CSS `!important` 和既有选择器顺序，因此关闭对应两条样式偏好规则。受控输入编辑器保留 `autoFocus`，协议文本解析保留控制字符正则。已知的内联 SVG、启动脚本与样式在指定文件内保留原有 HTML 通道；帮助正文和译文仍经过内容清洗器。Runtime 检查的 `finally` 负责关闭本轮检查实例，其清理异常规则保留为警告。

## 词条与内容回归

Vitest 覆盖三语键集合、插值、语言隔离、已知错误翻译、连接状态、帮助正文结构与链接、玩法标签交互、富文本清洗、兼容帮助的 ES5 输出及硬编码审计。

`i18n:audit` 解析 TSX，检查 JSX 正文、可访问标签和中文字符串。品牌、原作标题、键位、单位、备案号及协议分类标识有明确豁免；用户输入与原生错误诊断按各自来源处理。已知协议错误在展示边界映射到 Paraglide，不改动校验字符串或联机协议。Runtime 就绪超时使用独立错误类型标识，中英日的导入恢复与错误展示顺序一致，不依赖提示文字。

远程浏览器验收：

```sh
bun run test:content-i18n:browser -- <候选地址>
bun run test:paraglide:browser -- <候选地址>
```

构建与测试环境使用 Bun 1.4.4，通过 `bun install --frozen-lockfile` 安装依赖。主锁文件为 `bun.lock`，版本固定在 `.bun-version` 和 `packageManager`。

## Bun 运行时与依赖锁

安装依赖使用 `bun install --frozen-lockfile`，运行测试使用 `bun run test`。包内命令通过 `bun --bun run` 强制 Vitest、React Router、Biome 和 TypeScript 的 JavaScript 入口使用 Bun。测试会检查 worker 的实际运行时，防止 CLI 的 Node shebang 把执行环境切回 Node。

主启动器改用 `bun.lock`。自托管包也提供 `bun.lock`，Bun 安装校验记录锁文件、清单、运行时版本和所需依赖；既有 npm 自托管包的兼容校验保留，用原有回归验证。

JavaScript 语法检查使用 `Bun.Transpiler` 纯解析，另有不会执行源码的回归用例。契约构建的锁在进程退出时同步释放，覆盖 Bun 在前端产物加载失败时提前退出的路径。
