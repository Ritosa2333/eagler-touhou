# 历史规格 | 已停止原生维护

2026-11-27：本页功能改用正式站网页。以下是归档前记录，不是当前 Flutter 开发或验收要求。

# 帐户、公告与私信

[网页登录](../../../docs/evidence/web/mobile-login.png) | [网页注册](../../../docs/evidence/web/mobile-register.png) | [窄屏注册](../../../docs/evidence/web/mobile-narrow-register.png) | [网页版公告](../../../docs/evidence/web/mobile-announcements-guest.png) | [公告 Pencil](../../../docs/evidence/pencil/mobile-announcement.png)。

## 登录与注册

| 编号 | 元素 | 布局与用途 | 状态与验收 |
| --- | --- | --- | --- |
| AUTH-01 | 返回 | 手机在全宽面板上方，与表单分行；桌面沿用导航框架。 | 从手机主页、抽屉、私信和云存档登录入口进入，记录返回目标。 |
| AUTH-02 | 背景和表单 | 桌面全视口背景，示意图上放右侧整高面板；手机面板与视口等宽。表单标签与输入框左对齐。 | 320 px、390 px 和桌面亮暗主题。缩放、软键盘、系统上下栏变化下，最后一个动作可滚到视口内。 |
| AUTH-03 | 用户名或邮箱 | 登录首栏，标签常驻，接受用户名或邮箱。 | 键盘下一项进入密码，登录失败保留账号输入，并显示翻译后的错误。 |
| AUTH-04 | 密码 | 标签与忘记密码同行，输入框跨两列，眼睛位于输入框右侧。 | 显示和隐藏保留输入。切换登录和注册时清理登录密码。 |
| AUTH-05 | 登录与切换 | 登录按钮占满表单，注册入口在按钮下方。 | 登录中禁止重复提交，成功导航先验证入口提供的返回地址。 |
| AUTH-06 | 注册身份输入 | 用户名、昵称、邮箱依次排列。用户名和昵称说明图标紧随标签。 | 检查用户名字符和长度、昵称长度、邮箱格式和已有账号提示。 |
| AUTH-07 | 密码确认 | 注册密码和确认密码使用同一字段组件。 | 提交前验证长度和一致性。错误信息放在字段附近。 |
| AUTH-08 | 同意条款 | 确认密码下方、发送验证码上方。选中时打开条款阅读，取消和确认是独立动作。 | 关闭或取消仍为未同意。确认后启用提交。弹窗关闭把焦点交回条款入口。 |
| AUTH-09 | 邮箱验证 | 发送后在注册表单增加验证码栏、重发动作和倒计时。保存发送时绑定的数据。 | 滑块关闭、邮箱请求失败、重发冷却、验证码错误、过期后重新编辑和成功注册。 |
| AUTH-10 | 找回密码 | 从登录密码标签旁进入，顶部返回登录。先验证邮箱，随后输入验证码、新密码和确认密码。 | 走通发送、重发、失败、成功返回。新密码按账户接口提交。 |
| AUTH-11 | 隐私与语言 | 注册表单底部有隐私条款入口。走查三语和亮暗主题，保存主动选择。 | 跟踪系统默认语言、用户主动选择和更新之后的重新打开。 |
| AUTH-12 | 会话生命周期 | 启动时校验本机安全存储中的会话。账号区域显示检查进度，失败时提供重试和手动登录。访问已登录页面时以服务确认的身份为准。 | 同一账号头像用于侧栏、个人主页和账号菜单。核对重开、会话轮换、断连重试、撤销、退出及站点隔离。 |
| AUTH-13 | 邮件安全验证 | 服务开启邮件验证时，注册发码、注册重发与找回密码发码先打开原生拼图窗。支持等待、换图、取消和重试。 | 从正常入口加载正式站下发的拼图，核对原图和拼块的缩放。验证完成后的票据只用于当前邮件请求。 |

注册后读取会话。服务已建立会话时返回进入注册前的页面，否则返回登录并预填邮箱。找回密码成功后显示登录入口，旧密码和验证码从表单清除。桌面在复用的站点插画右侧显示表单，手机采用可滚动的全宽表单。

账户表单沿用发布版 `account-page.css` 的排版：540 px 及以下使用 24 px 边距和 24 px 标题，较宽手机使用 32 px 边距，桌面使用 28 px 标题。标题与字段组相隔 24 px，字段间隔 16 px，标签与输入框相隔至少 8 px。说明与找回密码动作保留原生按钮的命中空间。登录、注册、找回密码与账户安全复用字段组件。[390 px 注册前](../../../docs/evidence/flutter/mobile-register-layout-before.png) | [调整后](../../../docs/evidence/flutter/mobile-register-layout-after.png) | [320 px 英文注册](../../../docs/evidence/flutter/mobile-narrow-register-layout-after.png)

启动器只向安全存储写入按站点区分的会话 Cookie。重开时通过 `/api/auth/get-session` 读取已验证身份。网络错误允许重查，退出登录和服务撤销会清除本机 Cookie。改密等身份操作收到轮换 Cookie 时立即更新。Android 的备份与换机迁移排除会话文件。 [Flutter Secure Storage](https://pub.dev/packages/flutter_secure_storage)

Windows 用隔离账号连续启动三次进程，验证从本机安全存储恢复登录，退出后再次打开显示游客。手机主页在 320 px 下验证检查、失败和恢复三种状态。重试图标与登录、注册同行，待检查和失败状态的启动器设置位置一致。 [检查](../../../docs/evidence/flutter/mobile-session-checking.png) | [重试](../../../docs/evidence/flutter/mobile-session-retry.png) | [已登录](../../../docs/evidence/flutter/mobile-session-verified.png)

服务条款和隐私条款取仓库的三语原文。独立页和注册弹窗共用同一份对应语言的正文。Flutter 用 Markdown 控件渲染，确认和取消由原生按钮处理。 [Flutter Markdown Plus](https://pub.dev/packages/flutter_markdown_plus) | [Flutter URL Launcher](https://pub.dev/packages/url_launcher)

## 三语截图和回归

英文与日文采用发布版本的原始词库、服务条款和隐私正文。网页与 Flutter 使用相同视口，沿用各自的返回控件。中文版见本页开头和总对照。

| 语言 | 界面 | 1440 px | 390 px | 320 px |
| --- | --- | --- | --- | --- |
| 英语 | 登录 | [网页](../../../docs/evidence/web/desktop-login-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-login-en.png) | [网页](../../../docs/evidence/web/mobile-login-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-login-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-login-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-login-en.png) |
| 英语 | 注册 | [网页](../../../docs/evidence/web/desktop-register-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-register-en.png) | [网页](../../../docs/evidence/web/mobile-register-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-register-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-register-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-register-en.png) |
| 英语 | 找回密码 | [网页](../../../docs/evidence/web/desktop-reset-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-reset-en.png) | [网页](../../../docs/evidence/web/mobile-reset-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-reset-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-reset-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-reset-en.png) |
| 英语 | 条款弹窗 | [网页](../../../docs/evidence/web/desktop-registration-terms-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-registration-terms-en.png) | [网页](../../../docs/evidence/web/mobile-registration-terms-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-registration-terms-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-registration-terms-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-registration-terms-en.png) |
| 英语 | 隐私条款 | [网页](../../../docs/evidence/web/desktop-privacy-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-privacy-en.png) | [网页](../../../docs/evidence/web/mobile-privacy-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-privacy-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-privacy-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-privacy-en.png) |
| 日语 | 登录 | [网页](../../../docs/evidence/web/desktop-login-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-login-ja.png) | [网页](../../../docs/evidence/web/mobile-login-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-login-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-login-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-login-ja.png) |
| 日语 | 注册 | [网页](../../../docs/evidence/web/desktop-register-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-register-ja.png) | [网页](../../../docs/evidence/web/mobile-register-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-register-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-register-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-register-ja.png) |
| 日语 | 找回密码 | [网页](../../../docs/evidence/web/desktop-reset-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-reset-ja.png) | [网页](../../../docs/evidence/web/mobile-reset-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-reset-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-reset-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-reset-ja.png) |
| 日语 | 条款弹窗 | [网页](../../../docs/evidence/web/desktop-registration-terms-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-registration-terms-ja.png) | [网页](../../../docs/evidence/web/mobile-registration-terms-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-registration-terms-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-registration-terms-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-registration-terms-ja.png) |
| 日语 | 隐私条款 | [网页](../../../docs/evidence/web/desktop-privacy-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-privacy-ja.png) | [网页](../../../docs/evidence/web/mobile-privacy-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-privacy-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-privacy-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-privacy-ja.png) |

320 px 英文注册在填写表单后打开条款。取消时保留输入和未同意状态，确认后可以发送验证码，重名提示使用英文。日文找回密码在模拟键盘占用 240 px 的视口内走过验证码加载失败、取消、请求、错误码和重置成功。英文、日文账户安全从实际账户菜单进入，验证邮箱格式、验证码倒计时、反馈和返回。

## 账户安全

| 编号 | 元素 | 布局与验收 |
| --- | --- | --- |
| SECURITY-01 | 入口与权限 | 桌面账户菜单的“账户设置”进入安全页，手机从主页账户组的“账户安全”进入。宽屏左列呈现账户栏目。表单在右侧或手机全宽滚动区。返回原入口，会话失效后只提供登录入口。 |
| SECURITY-02 | 新邮箱和验证码 | 新邮箱上方，验证码与获取动作同一行。服务启用拼图时复用原生验证窗。发出后倒数 60 秒，验证码绑定到发送时的地址。修改地址清空验证码。 |
| SECURITY-03 | 修改邮箱 | 验证码字段只接受六位数字。收到服务器确认后清空输入并显示结果。失败就地提示，下次可重试。 |
| SECURITY-04 | 修改密码 | 依次输入当前密码、新密码和确认密码。密码默认隐藏。客户端验证长度和确认输入。提交要求服务撤销其他会话，成功清空密码并更新当前会话。 |
| SECURITY-05 | 等待、错误和返回 | 提交中禁重复操作，工具栏保留进度位置。消息对读屏设备播报。系统返回和顶部返回都走实际页面栈。提交期间，账户栏目及顶栏单机／联机也等待请求结束，结束后正常离开。 |
| SECURITY-06 | 账户栏目 | 安全、个人资料和游戏设置使用共用栏目导航，其中游戏设置进入云存档。桌面顶栏位于栏目切换之外，三个栏目使用相同侧栏和正文边线。切换栏目后返回原入口。显示偏好属于个人资料表单。 |

安全页恢复平铺分组与分隔线。桌面沿用主顶栏，外框最大 1080 px，侧栏 176 px，与正文相隔 48 px；正文最大 800 px。字段标签宽 116 px，与输入框相隔 20 px，提交动作沿输入框左侧对齐。手机保留 52 px 原生返回栏、20 px 内容边距，标签排在输入框上方。验证码输入与按钮相隔 8 px，按钮按获取与倒计时中较宽的文字预留空间。账户输入框采用 44 px 最小高度；错误和大字号可自然增高。

三个账户栏目复用同一布局，主标题与分隔线位置一致。云存档刷新动作与分组标题同行，手机使用原生工具栏动作。资料编辑与云存档沿用各自的草稿放弃确认。桌面实际入口回归核对了切换前后的顶栏实例与坐标，并验证经其他栏目返回书架。

[桌面调整前](../../../docs/evidence/flutter/desktop-security-flat-before.png) | [调整后](../../../docs/evidence/flutter/desktop-security-flat-after.png) | [390 px](../../../docs/evidence/flutter/mobile-security-flat-after.png) | [320 px 英文](../../../docs/evidence/flutter/mobile-narrow-security-flat-after.png)。同一隔离账号用三语在 1440、390 和 320 px 核对位置。跨桌面、手机排布后，密码草稿与焦点保持同一实例；发码倒计时不改变验证码输入框及按钮宽度。

网页和 Flutter 的安全页使用同一隔离会话。回归验证了验证码取消、错误、地址变更、倒计时、密码失败与成功、会话更新和返回。请求测试核对服务器接收的数据和会话轮换。Pencil 有手机账户组截图，安全页详细表单待补。正式账户改密、真实邮箱收码和 Android 真机待验。 [截图对照](../../../docs/comparison.md) | [Better Auth 邮件验证码](https://better-auth.com/docs/plugins/email-otp) | [Better Auth 密码变更](https://better-auth.com/docs/authentication/email-password)

## 公告与私信

入口、未读、正文、发布权限、会话与头像统一按[消息中心规格](../../../docs/inbox.md)验收。

## 联调证据

正式网页的游客公告与作者头像已通过真实接口复测。侧栏和个人主页的网页证据使用模拟会话及真实公开资料。Flutter 的注册与找回使用隔离账户样例验证条款取消和确认、用户名冲突、60 秒重发、验证码过期、错误验证码、密码重置以及返回。320 px 在真实入口读取并渲染正式站滑块响应，关闭后回到找回密码的邮箱表单。正式账号手动拼图、邮件收取及 Android 真机待验。 [对照图](../../../docs/comparison.md)

