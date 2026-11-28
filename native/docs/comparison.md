# 网页、Pencil 与 Flutter 对照

[当前核心截图](evidence/current/README.md)已于 2026-11-27 在账号网页迁移后重拍并记录源码校验值。本页其余 Flutter 图片是对应改动的历史对照，不能直接代表当前实现。

使用深色主题。语言对照分别使用简体中文、英语和日语，其余截图使用简体中文。资料编辑与账户安全使用同一隔离账户样例，公开页面使用游客会话。大厅联机与断连对照使用同一组公开房间样例。房间对照使用双人等待室样例。最近游玩和最后运行日期使用同一组七部作品与时间记录，Pencil 的历史记录用其设计样例。网页取正式站提交 `85941f0d56`。Flutter 图像取相同逻辑宽度，由控件测试渲染。Pencil 采用既有设计导出，示例账号、封面数量和近期记录随当时的演示数据而变。

[账户英日文对照](../archive/web-only/docs/account.md#三语截图和回归)包含登录、注册、找回密码、条款弹窗和隐私独立页。[帮助与联机指南英日文对照](../archive/web-only/docs/site-content.md#英语与日语)覆盖 13 个阅读页。

[联机大厅与房间](room.md)提供英日文实拍、前后对照和坐标。[房间规则](room.md#房间规则表单)、[联机时序](room.md#联机时序)、[游戏设置](room.md#房间内游戏设置)、[限速提示](room.md#限速房间的触控设置)与[常用发言](room.md#常用发言)均有三种宽度的同状态对照。

[桌面顶栏与共用侧栏](library.md#桌面顶栏对齐)提供前后对照、亮暗主题及像素坐标。

[触控工作台三语](game.md#触控工作台三语)包括桌面 th06、390 px th075 和 320 px th06 的入口实拍与交互回归。

## 桌面 1440 × 734

| 状态 | 网页 | Pencil | Flutter |
| --- | --- | --- | --- |
| 书架 | [截图](evidence/web/desktop-library.png) | [设计](evidence/pencil/desktop-library.png) | [截图](evidence/flutter/desktop-library.png) |
| 界面语言 | [截图](evidence/web/desktop-interface-language.png) | 待补 | [截图](evidence/flutter/desktop-interface-language.png) |
| 英文书架 | [截图](evidence/web/desktop-library-en.png) | 待补 | [截图](evidence/flutter/desktop-library-en.png) |
| 日文书架 | [截图](evidence/web/desktop-library-ja.png) | 待补 | [截图](evidence/flutter/desktop-library-ja.png) |
| 最近游玩 | [截图](evidence/web/desktop-library-recent.png) | [设计](evidence/pencil/desktop-library.png) | [截图](evidence/flutter/desktop-library-recent.png) |
| th06 最后运行日期 | [截图](evidence/web/desktop-game-last-played.png) | 待补 | [截图](evidence/flutter/desktop-game-last-played.png) |
| 书架列表 | [截图](evidence/web/desktop-library-list.png) | 待补 | [截图](evidence/flutter/desktop-library-list.png) |
| 封面编号倒序 | [截图](evidence/web/desktop-library-reverse.png) | 待补 | [截图](evidence/flutter/desktop-library-reverse.png) |
| 列表编号倒序 | [截图](evidence/web/desktop-library-list-reverse.png) | 待补 | [截图](evidence/flutter/desktop-library-list-reverse.png) |
| 常见问题 | [截图](evidence/web/desktop-site-faq.png) | 待补 | [截图](evidence/flutter/desktop-site-faq.png) |
| 关于本站 | [截图](evidence/web/desktop-site-about.png) | 待补 | [截图](evidence/flutter/desktop-site-about.png) |
| 联机玩法介绍 | [截图](evidence/web/desktop-site-guide.png) | 待补 | [截图](evidence/flutter/desktop-site-guide.png) |
| 协作通用规则 | [截图](evidence/web/desktop-site-guide-coop.png) | 待补 | [截图](evidence/flutter/desktop-site-guide-coop.png) |
| 服务状态 | [截图](evidence/web/desktop-site-uptime.png) | 待补 | [截图](evidence/flutter/desktop-site-uptime.png) |
| 支持面板 | [截图](evidence/web/desktop-site-donation.png) | 待补 | [截图](evidence/flutter/desktop-site-donation.png) |
| 联机大厅 | [截图](evidence/web/desktop-lobby-live.png) | 待补 | [截图](evidence/flutter/desktop-lobby-live.png) |
| 超量目录单选 | [截图](evidence/web/desktop-lobby-capped.png) | 待补 | [截图](evidence/flutter/desktop-lobby-capped.png) |
| 退出旧房间 | [截图](evidence/web/desktop-lobby-membership.png) | 待补 | [截图](evidence/flutter/desktop-lobby-membership.png) |
| 花映塚大厅评分卡 | [截图](evidence/web/desktop-lobby-rating-card.png) | 待补 | [截图](evidence/flutter/desktop-lobby-rating-card.png) |
| 花映塚等待室评分卡 | [截图](evidence/web/desktop-room-rating-card.png) | 待补 | [截图](evidence/flutter/desktop-room-rating-card.png) |
| 花映塚排行榜评分卡 | [截图](evidence/web/desktop-board-rating-card.png) | 待补 | [截图](evidence/flutter/desktop-board-rating-card.png) |
| 大厅网络检查 | [截图](evidence/web/desktop-network-check.png) | 待补 | [截图](evidence/flutter/desktop-network-check.png) |
| 联机大厅 \| 断连 | [截图](evidence/web/desktop-lobby-disconnected.png) | 待补 | [截图](evidence/flutter/desktop-lobby-disconnected.png) |
| 联机房间 \| 等待开局 | [截图](evidence/web/desktop-room-waiting.png) | 待补 | [截图](evidence/flutter/desktop-room-waiting.png) |
| 联机房间 \| 聊天 | [截图](evidence/web/desktop-room-chat.png) | 待补 | [截图](evidence/flutter/desktop-room-chat.png) |
| th06 详情 | [截图](evidence/web/desktop-game-detail.png) | [设计](evidence/pencil/desktop-game-detail.png) | [截图](evidence/flutter/desktop-game-detail.png) |
| 游客通关表 | [截图](evidence/web/desktop-game-progress-guest.png) | 待补 | [截图](evidence/flutter/desktop-game-progress-guest.png) |
| th07 通关表 | [截图](evidence/web/desktop-game-progress.png) | 待补 | [截图](evidence/flutter/desktop-game-progress.png) |
| th07 好友游玩记录 | [截图](evidence/web/desktop-game-friend-activity.png) | 待补 | [截图](evidence/flutter/desktop-game-friend-activity.png) |
| 符卡收取图 | [截图](evidence/web/desktop-spell-map.png) | [设计](evidence/pencil/desktop-spell-map.png) | [截图](evidence/flutter/desktop-spell-map.png) |
| 符卡列表 | [截图](evidence/web/desktop-spell-list.png) | [设计](evidence/pencil/desktop-spell-list.png) | [截图](evidence/flutter/desktop-spell-list.png) |
| 游戏设置 | [截图](evidence/web/desktop-game-settings.png) | [设计](evidence/pencil/desktop-game-settings.png) | [截图](evidence/flutter/desktop-game-settings.png) |
| 显示设置展开 | [截图](evidence/web/desktop-game-settings-display.png) | 待补 | [截图](evidence/flutter/desktop-game-settings-display.png) |
| 键盘改键 | [截图](evidence/web/desktop-game-keyboard.png) | 待补 | [截图](evidence/flutter/desktop-game-keyboard.png) |
| 手柄改键 | [截图](evidence/web/desktop-game-gamepad.png) | 待补 | [截图](evidence/flutter/desktop-game-gamepad.png) |
| th09 本地双人 | [截图](evidence/web/desktop-th09-local-duel.png) | 待补 | [截图](evidence/flutter/desktop-th09-local-duel.png) |
| th09 存档与录像设置 | [截图](evidence/web/desktop-game-files.png) | 待补 | [截图](evidence/flutter/desktop-game-files.png) |
| th09 录像管理空态 | 待补 | 待补 | [截图](evidence/flutter/desktop-replay-manager-empty.png) |
| th09 录像操作菜单 | 待补 | 待补 | [截图](evidence/flutter/desktop-replay-manager-actions.png) |
| th06 触控布局 | [截图](evidence/web/desktop-touch-layout.png) | 待补 | [截图](evidence/flutter/desktop-touch-layout.png) |
| th075 触控布局 | [截图](evidence/web/desktop-touch-layout-fighting.png) | 待补 | [截图](evidence/flutter/desktop-touch-layout-fighting.png) |
| 导入游戏资源 | [截图](evidence/web/desktop-game-data-import.png) | 待补 | [截图](evidence/flutter/desktop-game-data-import.png) |
| 登录 | [截图](evidence/web/desktop-login.png) | 待补 | [截图](evidence/flutter/desktop-login.png) |
| 注册账号 | [截图](evidence/web/desktop-register.png) | 待补 | [截图](evidence/flutter/desktop-register.png) |
| 重置密码 | [截图](evidence/web/desktop-reset.png) | 待补 | [截图](evidence/flutter/desktop-reset.png) |
| 注册同意条款 | [截图](evidence/web/desktop-registration-terms.png) | 待补 | [截图](evidence/flutter/desktop-registration-terms.png) |
| 注册隐私条款 | [截图](evidence/web/desktop-privacy.png) | 待补 | [截图](evidence/flutter/desktop-registration-privacy.png) |
| 公告预览 | [截图](evidence/web/desktop-announcement-preview.png) | 待补 | [截图](evidence/flutter/desktop-announcement-preview.png) |
| 私信游客预览 | [截图](evidence/web/desktop-inbox-preview-guest.png) | 待补 | [截图](evidence/flutter/desktop-inbox-preview-guest.png) |
| 私信登录态 | [列表](evidence/web/desktop-inbox-member.png)、[对话](evidence/web/desktop-inbox-thread.png) | [列表](evidence/pencil/desktop-inbox.png)、[对话](evidence/pencil/desktop-inbox-thread.png) | [列表](evidence/flutter/desktop-inbox-member.png)、[对话](evidence/flutter/desktop-inbox-thread.png) |
| 公告 | [截图](evidence/web/desktop-announcements-guest.png) | [设计](evidence/pencil/desktop-message-center.png) | [截图](evidence/flutter/desktop-announcements-guest.png) |
| 个人主页 \| 花映塚 | [截图](evidence/web/desktop-profile-public.png) | 待补 | [截图](evidence/flutter/desktop-profile-public.png) |
| WHR 走势 | [截图](evidence/web/desktop-profile-whr.png) | [设计](evidence/pencil/desktop-whr.png) | [截图](evidence/flutter/desktop-profile-whr.png) |
| 机体分对照 | [截图](evidence/web/desktop-profile-whr-comparison.png) | 待补 | [截图](evidence/flutter/desktop-profile-whr-comparison.png) |
| 逐局录像 | [截图](evidence/web/desktop-profile-duel-replays.png) | 待补 | [截图](evidence/flutter/desktop-profile-duel-replays.png) |
| 花映塚排行榜 | [截图](evidence/web/desktop-th09-board.png) | 待补 | [截图](evidence/flutter/desktop-th09-board.png) |
| 个人主页 \| 全部作品 | [截图](evidence/web/desktop-profile-all-games.png) | [设计](evidence/pencil/desktop-profile.png) | [截图](evidence/flutter/desktop-profile-all-games.png) |
| 云存档 | [截图](evidence/web/desktop-cloud-saves.png) | 待补 | [截图](evidence/flutter/desktop-cloud-saves.png) |
| 粉丝列表 | [截图](evidence/web/desktop-profile-social-followers.png) | 待补 | [截图](evidence/flutter/desktop-profile-social-followers.png) |
| 个人资料编辑 | [截图](evidence/web/desktop-profile-edit.png) | 待补 | [截图](evidence/flutter/desktop-profile-edit.png) |

## 手机 390 × 844

| 状态 | 网页 | Pencil | Flutter |
| --- | --- | --- | --- |
| 书架 | [截图](evidence/web/mobile-library.png) | [设计](evidence/pencil/mobile-library.png) | [截图](evidence/flutter/mobile-library.png) |
| 界面语言 | [截图](evidence/web/mobile-interface-language.png) | 待补 | [截图](evidence/flutter/mobile-interface-language.png) |
| 游客主页 | [截图](evidence/web/mobile-home-guest.png) | [设计](evidence/pencil/mobile-home-guest.png) | [截图](evidence/flutter/mobile-home-guest.png) |
| 主页语言设置 | [截图](evidence/web/mobile-home-language.png) | [设计](evidence/pencil/mobile-home-expanded.png) | [截图](evidence/flutter/mobile-home-language.png) |
| 英文书架 | [截图](evidence/web/mobile-library-en.png) | 待补 | [截图](evidence/flutter/mobile-library-en.png) |
| 日文书架 | [截图](evidence/web/mobile-library-ja.png) | 待补 | [截图](evidence/flutter/mobile-library-ja.png) |
| 最近游玩 | [截图](evidence/web/mobile-library-recent.png) | [设计](evidence/pencil/mobile-library.png) | [截图](evidence/flutter/mobile-library-recent.png) |
| th06 最后运行日期 | [截图](evidence/web/mobile-game-last-played.png) | 待补 | [截图](evidence/flutter/mobile-game-last-played.png) |
| 书架列表 | [截图](evidence/web/mobile-library-list.png) | 待补 | [截图](evidence/flutter/mobile-library-list.png) |
| 封面编号倒序 | [截图](evidence/web/mobile-library-reverse.png) | 待补 | [截图](evidence/flutter/mobile-library-reverse.png) |
| 列表编号倒序 | [截图](evidence/web/mobile-library-list-reverse.png) | 待补 | [截图](evidence/flutter/mobile-library-list-reverse.png) |
| 站点菜单 | [截图](evidence/web/mobile-library-drawer.png) | [设计](evidence/pencil/mobile-drawer-guest.png) | [截图](evidence/flutter/mobile-library-drawer.png) |
| 常见问题 | [截图](evidence/web/mobile-site-faq.png) | 待补 | [截图](evidence/flutter/mobile-site-faq.png) |
| 关于本站 | [截图](evidence/web/mobile-site-about.png) | 待补 | [截图](evidence/flutter/mobile-site-about.png) |
| 联机玩法介绍 | [截图](evidence/web/mobile-site-guide.png) | 待补 | [截图](evidence/flutter/mobile-site-guide.png) |
| 协作通用规则 | [截图](evidence/web/mobile-site-guide-coop.png) | 待补 | [截图](evidence/flutter/mobile-site-guide-coop.png) |
| 服务状态 | [截图](evidence/web/mobile-site-uptime.png) | 待补 | [截图](evidence/flutter/mobile-site-uptime.png) |
| 联机大厅 | [截图](evidence/web/mobile-lobby-live.png) | 待补 | [截图](evidence/flutter/mobile-lobby-live.png) |
| 超量目录单选 | [截图](evidence/web/mobile-lobby-capped.png) | 待补 | [截图](evidence/flutter/mobile-lobby-capped.png) |
| 退出旧房间 | [截图](evidence/web/mobile-lobby-membership.png) | 待补 | [截图](evidence/flutter/mobile-lobby-membership.png) |
| 花映塚大厅评分卡 | [截图](evidence/web/mobile-lobby-rating-card.png) | 待补 | [截图](evidence/flutter/mobile-lobby-rating-card.png) |
| 花映塚等待室评分卡 | [截图](evidence/web/mobile-room-rating-card.png) | 待补 | [截图](evidence/flutter/mobile-room-rating-card.png) |
| 花映塚排行榜评分卡 | [截图](evidence/web/mobile-board-rating-card.png) | 待补 | [截图](evidence/flutter/mobile-board-rating-card.png) |
| 大厅网络检查 | [截图](evidence/web/mobile-network-check.png) | 待补 | [截图](evidence/flutter/mobile-network-check.png) |
| 联机大厅 \| 断连 | [截图](evidence/web/mobile-lobby-disconnected.png) | 待补 | [截图](evidence/flutter/mobile-lobby-disconnected.png) |
| 联机房间 \| 等待开局 | [截图](evidence/web/mobile-room-waiting.png) | 待补 | [截图](evidence/flutter/mobile-room-waiting.png) |
| 联机房间 \| 聊天 | [截图](evidence/web/mobile-room-chat.png) | 待补 | [截图](evidence/flutter/mobile-room-chat.png) |
| th06 详情 | [截图](evidence/web/mobile-game-detail.png) | [设计](evidence/pencil/mobile-game-detail.png) | [截图](evidence/flutter/mobile-game-detail.png) |
| 游客通关表 | [截图](evidence/web/mobile-game-progress-guest.png) | 待补 | [截图](evidence/flutter/mobile-game-progress-guest.png) |
| th07 通关表 | [截图](evidence/web/mobile-game-progress.png) | 待补 | [截图](evidence/flutter/mobile-game-progress.png) |
| th07 好友游玩记录 | [截图](evidence/web/mobile-game-friend-activity.png) | 待补 | [截图](evidence/flutter/mobile-game-friend-activity.png) |
| 符卡收取图 | [截图](evidence/web/mobile-spell-map.png) | [设计](evidence/pencil/mobile-spell-map.png) | [截图](evidence/flutter/mobile-spell-map.png) |
| 符卡列表 | [截图](evidence/web/mobile-spell-list.png) | [设计](evidence/pencil/mobile-spell-list.png) | [截图](evidence/flutter/mobile-spell-list.png) |
| 游戏设置 | [截图](evidence/web/mobile-game-settings.png) | [设计](evidence/pencil/mobile-game-settings.png) | [截图](evidence/flutter/mobile-game-settings.png) |
| 键盘改键 | [截图](evidence/web/mobile-game-keyboard.png) | 待补 | [截图](evidence/flutter/mobile-game-keyboard.png) |
| 手柄改键 | [截图](evidence/web/mobile-game-gamepad.png) | 待补 | [截图](evidence/flutter/mobile-game-gamepad.png) |
| th09 本地双人 | [截图](evidence/web/mobile-th09-local-duel.png) | 待补 | [截图](evidence/flutter/mobile-th09-local-duel.png) |
| th09 存档与录像设置 | [截图](evidence/web/mobile-game-files.png) | 待补 | [截图](evidence/flutter/mobile-game-files.png) |
| th09 录像管理空态 | 待补 | 待补 | [截图](evidence/flutter/mobile-replay-manager-empty.png) |
| th09 录像操作菜单 | 待补 | 待补 | [截图](evidence/flutter/mobile-replay-manager-actions.png) |
| th06 触控布局 | [截图](evidence/web/mobile-touch-layout.png) | 待补 | [截图](evidence/flutter/mobile-touch-layout.png) |
| th075 触控布局 | [截图](evidence/web/mobile-touch-layout-fighting.png) | 待补 | [截图](evidence/flutter/mobile-touch-layout-fighting.png) |
| 导入游戏资源 | [截图](evidence/web/mobile-game-data-import.png) | 待补 | [截图](evidence/flutter/mobile-game-data-import.png) |
| 登录 | [截图](evidence/web/mobile-login.png) | 待补 | [截图](evidence/flutter/mobile-login.png) |
| 注册账号 | [截图](evidence/web/mobile-register.png) | 待补 | [截图](evidence/flutter/mobile-register.png) |
| 重置密码 | [截图](evidence/web/mobile-reset.png) | 待补 | [截图](evidence/flutter/mobile-reset.png) |
| 注册同意条款 | [截图](evidence/web/mobile-registration-terms.png) | 待补 | [截图](evidence/flutter/mobile-registration-terms.png) |
| 注册隐私条款 | [截图](evidence/web/mobile-privacy.png) | 待补 | [截图](evidence/flutter/mobile-registration-privacy.png) |
| 公告 | [截图](evidence/web/mobile-announcements-guest.png) | [设计](evidence/pencil/mobile-announcement.png) | [截图](evidence/flutter/mobile-announcements-guest.png) |
| 个人主页 \| 花映塚 | [截图](evidence/web/mobile-profile-public.png) | 待补 | [截图](evidence/flutter/mobile-profile-public.png) |
| WHR 走势 | [截图](evidence/web/mobile-profile-whr.png) | [设计](evidence/pencil/mobile-whr.png) | [截图](evidence/flutter/mobile-profile-whr.png) |
| 机体分对照 | [截图](evidence/web/mobile-profile-whr-comparison.png) | 待补 | [截图](evidence/flutter/mobile-profile-whr-comparison.png) |
| 逐局录像 | [截图](evidence/web/mobile-profile-duel-replays.png) | [设计](evidence/pencil/mobile-match-record.png) | [截图](evidence/flutter/mobile-profile-duel-replays.png) |
| 花映塚排行榜 | [截图](evidence/web/mobile-th09-board.png) | 待补 | [截图](evidence/flutter/mobile-th09-board.png) |
| 个人主页 \| 全部作品 | [截图](evidence/web/mobile-profile-all-games.png) | [设计](evidence/pencil/mobile-profile.png) | [截图](evidence/flutter/mobile-profile-all-games.png) |
| 云存档 | [截图](evidence/web/mobile-cloud-saves.png) | 待补 | [截图](evidence/flutter/mobile-cloud-saves.png) |
| 粉丝列表 | [截图](evidence/web/mobile-profile-social-followers.png) | 待补 | [截图](evidence/flutter/mobile-profile-social-followers.png) |
| 个人资料编辑 | [截图](evidence/web/mobile-profile-edit.png) | 待补 | [截图](evidence/flutter/mobile-profile-edit.png) |

## 手机窄屏 320 × 640

| 状态 | 网页 | Flutter |
| --- | --- | --- |
| 书架 | [截图](evidence/web/mobile-narrow-library.png) | [截图](evidence/flutter/mobile-narrow-library.png) |
| 界面语言 | [截图](evidence/web/mobile-narrow-interface-language.png) | [截图](evidence/flutter/mobile-narrow-interface-language.png) |
| 游客主页 | [截图](evidence/web/mobile-narrow-home-guest.png) | [截图](evidence/flutter/mobile-narrow-home-guest.png) |
| 主页语言设置 | [截图](evidence/web/mobile-narrow-home-language.png) | [截图](evidence/flutter/mobile-narrow-home-language.png) |
| 英文书架 | [截图](evidence/web/mobile-narrow-library-en.png) | [截图](evidence/flutter/mobile-narrow-library-en.png) |
| 日文书架 | [截图](evidence/web/mobile-narrow-library-ja.png) | [截图](evidence/flutter/mobile-narrow-library-ja.png) |
| 最近游玩 | [截图](evidence/web/mobile-narrow-library-recent.png) | [截图](evidence/flutter/mobile-narrow-library-recent.png) |
| th06 最后运行日期 | [截图](evidence/web/mobile-narrow-game-last-played.png) | [截图](evidence/flutter/mobile-narrow-game-last-played.png) |
| 书架列表 | [截图](evidence/web/mobile-narrow-library-list.png) | [截图](evidence/flutter/mobile-narrow-library-list.png) |
| 封面编号倒序 | [截图](evidence/web/mobile-narrow-library-reverse.png) | [截图](evidence/flutter/mobile-narrow-library-reverse.png) |
| 列表编号倒序 | [截图](evidence/web/mobile-narrow-library-list-reverse.png) | [截图](evidence/flutter/mobile-narrow-library-list-reverse.png) |
| 站点菜单 | [截图](evidence/web/mobile-narrow-library-drawer.png) | [截图](evidence/flutter/mobile-narrow-library-drawer.png) |
| 常见问题 | [截图](evidence/web/mobile-narrow-site-faq.png) | [截图](evidence/flutter/mobile-narrow-site-faq.png) |
| 关于本站 | [截图](evidence/web/mobile-narrow-site-about.png) | [截图](evidence/flutter/mobile-narrow-site-about.png) |
| 联机玩法介绍 | [截图](evidence/web/mobile-narrow-site-guide.png) | [截图](evidence/flutter/mobile-narrow-site-guide.png) |
| 协作通用规则 | [截图](evidence/web/mobile-narrow-site-guide-coop.png) | [截图](evidence/flutter/mobile-narrow-site-guide-coop.png) |
| 服务状态 | [截图](evidence/web/mobile-narrow-site-uptime.png) | [截图](evidence/flutter/mobile-narrow-site-uptime.png) |
| 联机大厅 | [截图](evidence/web/mobile-narrow-lobby-live.png) | [截图](evidence/flutter/mobile-narrow-lobby-live.png) |
| 超量目录单选 | [截图](evidence/web/mobile-narrow-lobby-capped.png) | [截图](evidence/flutter/mobile-narrow-lobby-capped.png) |
| 退出旧房间 | [截图](evidence/web/mobile-narrow-lobby-membership.png) | [截图](evidence/flutter/mobile-narrow-lobby-membership.png) |
| 花映塚大厅评分卡 | [截图](evidence/web/mobile-narrow-lobby-rating-card.png) | [截图](evidence/flutter/mobile-narrow-lobby-rating-card.png) |
| 花映塚等待室评分卡 | [截图](evidence/web/mobile-narrow-room-rating-card.png) | [截图](evidence/flutter/mobile-narrow-room-rating-card.png) |
| 花映塚排行榜评分卡 | [截图](evidence/web/mobile-narrow-board-rating-card.png) | [截图](evidence/flutter/mobile-narrow-board-rating-card.png) |
| 大厅网络检查 | [截图](evidence/web/mobile-narrow-network-check.png) | [截图](evidence/flutter/mobile-narrow-network-check.png) |
| 联机大厅 \| 断连 | [截图](evidence/web/mobile-narrow-lobby-disconnected.png) | [截图](evidence/flutter/mobile-narrow-lobby-disconnected.png) |
| 联机房间 \| 等待开局 | [截图](evidence/web/mobile-narrow-room-waiting.png) | [截图](evidence/flutter/mobile-narrow-room-waiting.png) |
| 联机房间 \| 聊天 | [截图](evidence/web/mobile-narrow-room-chat.png) | [截图](evidence/flutter/mobile-narrow-room-chat.png) |
| th06 详情 | [截图](evidence/web/mobile-narrow-game-detail.png) | [截图](evidence/flutter/mobile-narrow-game-detail.png) |
| 游客通关表 | [截图](evidence/web/mobile-narrow-game-progress-guest.png) | [截图](evidence/flutter/mobile-narrow-game-progress-guest.png) |
| th07 通关表 | [截图](evidence/web/mobile-narrow-game-progress.png) | [截图](evidence/flutter/mobile-narrow-game-progress.png) |
| th07 好友游玩记录 | [截图](evidence/web/mobile-narrow-game-friend-activity.png) | [截图](evidence/flutter/mobile-narrow-game-friend-activity.png) |
| 符卡收取图 | [截图](evidence/web/mobile-narrow-spell-map.png) | [截图](evidence/flutter/mobile-narrow-spell-map.png) |
| 符卡列表 | [截图](evidence/web/mobile-narrow-spell-list.png) | [截图](evidence/flutter/mobile-narrow-spell-list.png) |
| 游戏设置 | [截图](evidence/web/mobile-narrow-game-settings.png) | [截图](evidence/flutter/mobile-narrow-game-settings.png) |
| 键盘改键 | [截图](evidence/web/mobile-narrow-game-keyboard.png) | [截图](evidence/flutter/mobile-narrow-game-keyboard.png) |
| 手柄改键 | [截图](evidence/web/mobile-narrow-game-gamepad.png) | [截图](evidence/flutter/mobile-narrow-game-gamepad.png) |
| th09 本地双人 | [截图](evidence/web/mobile-narrow-th09-local-duel.png) | [截图](evidence/flutter/mobile-narrow-th09-local-duel.png) |
| th09 存档与录像设置 | [截图](evidence/web/mobile-narrow-game-files.png) | [截图](evidence/flutter/mobile-narrow-game-files.png) |
| th09 录像管理空态 | 待补 | [截图](evidence/flutter/mobile-narrow-replay-manager-empty.png) |
| th09 录像操作菜单 | 待补 | [截图](evidence/flutter/mobile-narrow-replay-manager-actions.png) |
| th06 触控布局 | [截图](evidence/web/mobile-narrow-touch-layout.png) | [截图](evidence/flutter/mobile-narrow-touch-layout.png) |
| th075 触控布局 | [截图](evidence/web/mobile-narrow-touch-layout-fighting.png) | [截图](evidence/flutter/mobile-narrow-touch-layout-fighting.png) |
| 导入游戏资源 | [截图](evidence/web/mobile-narrow-game-data-import.png) | [截图](evidence/flutter/mobile-narrow-game-data-import.png) |
| 登录 | [截图](evidence/web/mobile-narrow-login.png) | [截图](evidence/flutter/mobile-narrow-login.png) |
| 注册账号 | [截图](evidence/web/mobile-narrow-register.png) | [截图](evidence/flutter/mobile-narrow-register.png) |
| 重置密码 | [截图](evidence/web/mobile-narrow-reset.png) | [截图](evidence/flutter/mobile-narrow-reset.png) |
| 注册同意条款 | [截图](evidence/web/mobile-narrow-registration-terms.png) | [截图](evidence/flutter/mobile-narrow-registration-terms.png) |
| 注册隐私条款 | [截图](evidence/web/mobile-narrow-privacy.png) | [截图](evidence/flutter/mobile-narrow-registration-privacy.png) |
| 公告 | [截图](evidence/web/mobile-narrow-announcements-guest.png) | [截图](evidence/flutter/mobile-narrow-announcements-guest.png) |
| 个人主页 \| 花映塚 | [截图](evidence/web/mobile-narrow-profile-public.png) | [截图](evidence/flutter/mobile-narrow-profile-public.png) |
| WHR 走势 | [截图](evidence/web/mobile-narrow-profile-whr.png) | [截图](evidence/flutter/mobile-narrow-profile-whr.png) |
| 机体分对照 | [截图](evidence/web/mobile-narrow-profile-whr-comparison.png) | [截图](evidence/flutter/mobile-narrow-profile-whr-comparison.png) |
| 逐局录像 | [截图](evidence/web/mobile-narrow-profile-duel-replays.png) | [截图](evidence/flutter/mobile-narrow-profile-duel-replays.png) |
| 花映塚排行榜 | [截图](evidence/web/mobile-narrow-th09-board.png) | [截图](evidence/flutter/mobile-narrow-th09-board.png) |
| 个人主页 \| 全部作品 | [截图](evidence/web/mobile-narrow-profile-all-games.png) | [截图](evidence/flutter/mobile-narrow-profile-all-games.png) |
| 云存档 | [截图](evidence/web/mobile-narrow-cloud-saves.png) | [截图](evidence/flutter/mobile-narrow-cloud-saves.png) |
| 粉丝列表 | [截图](evidence/web/mobile-narrow-profile-social-followers.png) | [截图](evidence/flutter/mobile-narrow-profile-social-followers.png) |
| 个人资料编辑 | [截图](evidence/web/mobile-narrow-profile-edit.png) | [截图](evidence/flutter/mobile-narrow-profile-edit.png) |

Flutter 书架采用网页实际展示的 11 部作品与封面。大厅数据随现场房间变化。详情已接入资源导入和游戏设置。导入对照中，网页接受多个封装和转换流程，Flutter 当前校验发布清单对应的 DATA。游戏详情使用共用通关表，旧个人主页对照已归档。通关、游玩时长与符卡对照用同一份隔离云存档样例。Pencil 与样例的数据量不同，用于对齐布局。最后运行日期见同一组本机历史对照。好友列及本机存档预览已接入账号服务。本页账户、个人主页及裁切图片为已放弃维护的原生实现历史对照。当前入口统一打开正式站应用内网页，见[账号契约](account.md)。

th09 的存档与录像组已有三个宽度的网页实拍。录像管理空态及 Flutter 文件校验结果见[改键与录像管理三语](#改键与录像管理三语)。

Pencil 的个人主页图使用登录后的演示账号。网页与 Flutter 实拍使用同一位公开玩家，资料、对局和统计量随服务端变化。

[网页截图与元素坐标总清单](evidence/web/manifest.json) | [Pencil 导出清单](evidence/pencil/manifest.json)
| 账户安全 | [截图](evidence/web/desktop-account-security.png) | 待补 | [截图](evidence/flutter/desktop-account-security.png) |
| 账户安全 \| 密码区 | [截图](evidence/web/desktop-account-security-password.png) | 待补 | [截图](evidence/flutter/desktop-account-security-password.png) |
| 账户安全 | [截图](evidence/web/mobile-account-security.png) | 待补 | [截图](evidence/flutter/mobile-account-security.png) |
| 账户安全 \| 密码区 | [截图](evidence/web/mobile-account-security-password.png) | 待补 | [截图](evidence/flutter/mobile-account-security-password.png) |
| 账户安全 | [截图](evidence/web/mobile-narrow-account-security.png) | [截图](evidence/flutter/mobile-narrow-account-security.png) |
| 账户安全 \| 密码区 | [截图](evidence/web/mobile-narrow-account-security-password.png) | [截图](evidence/flutter/mobile-narrow-account-security-password.png) |


## 游戏设置三语

同一固定版本的 th06，在相同界面语言下分别展开显示与存档分组。中文与 Pencil 的对照见上方按视口排列的表格。

| 视口 | 网页显示设置 | Flutter 显示设置 | 网页存档与录像 | Flutter 存档与录像 |
| --- | --- | --- | --- | --- |
| 1440 × 734 英文 | [截图](evidence/web/desktop-game-settings-display-en.png) | [截图](evidence/flutter/desktop-game-settings-display-en.png) | [截图](evidence/web/desktop-game-files-en.png) | [截图](evidence/flutter/desktop-game-files-en.png) |
| 390 × 844 日文 | [截图](evidence/web/mobile-game-settings-display-ja.png) | [截图](evidence/flutter/mobile-game-settings-display-ja.png) | [截图](evidence/web/mobile-game-files-ja.png) | [截图](evidence/flutter/mobile-game-files-ja.png) |
| 320 × 640 英文 | [截图](evidence/web/mobile-narrow-game-settings-display-en.png) | [截图](evidence/flutter/mobile-narrow-game-settings-display-en.png) | [截图](evidence/web/mobile-narrow-game-files-en.png) | [截图](evidence/flutter/mobile-narrow-game-files-en.png) |

## 改键与录像管理三语

使用 th09。网页和 Flutter 从游戏设置打开输入弹窗，在同一弹窗内切换键盘、手柄和本地双人。手柄图使用未连接设备的状态。Flutter 测量切换前后的弹窗边界，并在延迟保存期间检查切换和返回。关闭后焦点回到发起的设置行。录像对照从空目录取景，网页播放器显示游戏资源加载进度。

三个视口下的 Flutter 弹窗分别为 560 × 608、366 × 608 和 296 × 608。各面板共享边界，选项栏固定。将键盘配置滚到底部后仍可切换设备，切回时从顶部开始，并显示刚保存的键位。

| 视口 | 键盘 | 手柄 | 本地双人 | 录像空态 |
| --- | --- | --- | --- | --- |
| 1440 × 734 英文 | [网页](evidence/web/desktop-game-keyboard-en.png) / [Flutter](evidence/flutter/desktop-game-keyboard-en.png) | [网页](evidence/web/desktop-game-gamepad-en.png) / [Flutter](evidence/flutter/desktop-game-gamepad-en.png) | [网页](evidence/web/desktop-th09-local-duel-en.png) / [Flutter](evidence/flutter/desktop-th09-local-duel-en.png) | [网页](evidence/web/desktop-replay-manager-empty-en.png) / [Flutter](evidence/flutter/desktop-replay-manager-empty-en.png) |
| 390 × 844 日文 | [网页](evidence/web/mobile-game-keyboard-ja.png) / [Flutter](evidence/flutter/mobile-game-keyboard-ja.png) | [网页](evidence/web/mobile-game-gamepad-ja.png) / [Flutter](evidence/flutter/mobile-game-gamepad-ja.png) | [网页](evidence/web/mobile-th09-local-duel-ja.png) / [Flutter](evidence/flutter/mobile-th09-local-duel-ja.png) | [网页](evidence/web/mobile-replay-manager-empty-ja.png) / [Flutter](evidence/flutter/mobile-replay-manager-empty-ja.png) |
| 320 × 640 英文 | [网页](evidence/web/mobile-narrow-game-keyboard-en.png) / [Flutter](evidence/flutter/mobile-narrow-game-keyboard-en.png) | [网页](evidence/web/mobile-narrow-game-gamepad-en.png) / [Flutter](evidence/flutter/mobile-narrow-game-gamepad-en.png) | [网页](evidence/web/mobile-narrow-th09-local-duel-en.png) / [Flutter](evidence/flutter/mobile-narrow-th09-local-duel-en.png) | [网页](evidence/web/mobile-narrow-replay-manager-empty-en.png) / [Flutter](evidence/flutter/mobile-narrow-replay-manager-empty-en.png) |

320 px 英文实拍另记录[空文件导入提示](evidence/flutter/mobile-narrow-replay-manager-invalid-en.png)和[导入真实录像后的菜单](evidence/flutter/mobile-narrow-replay-manager-actions-en.png)。Pencil 当前导出缺少这四个子页的同状态画面。

## 云存档三语

网页从个人主页的账号游戏设置展开云存档。Flutter 从同一入口进入独立页面，也可从游戏设置带入已选作品。同一份隔离账号样例提供 th07 的云端版本。现有 Pencil 页面未包含对应状态。

| 场景 | 网页 | Flutter |
| --- | --- | --- |
| 桌面英文 | [网页](evidence/web/desktop-cloud-saves-en.png) | [Flutter](evidence/flutter/desktop-cloud-saves-en.png) |
| 390 px 日文 | [网页](evidence/web/mobile-cloud-saves-ja.png) | [Flutter](evidence/flutter/mobile-cloud-saves-ja.png) |
| 320 px 英文 | [网页](evidence/web/mobile-narrow-cloud-saves-en.png) | [Flutter](evidence/flutter/mobile-narrow-cloud-saves-en.png) |
| 桌面英文展开文件 | [网页](evidence/web/desktop-cloud-files-en.png) | [Flutter](evidence/flutter/desktop-cloud-files-en.png) |
| 390 px 日文展开文件 | [网页](evidence/web/mobile-cloud-files-ja.png) | [Flutter](evidence/flutter/mobile-cloud-files-ja.png) |
| 320 px 英文展开文件 | [网页](evidence/web/mobile-narrow-cloud-files-en.png) | [Flutter](evidence/flutter/mobile-narrow-cloud-files-en.png) |

文件清单默认收起。展开后两端都显示 `score.dat` 和 16 B。桌面两张比较卡等宽等高，手机使用纵向排列以便阅读。版本冲突后可见刷新后的云端版本和原草稿。 [英文](evidence/flutter/mobile-narrow-cloud-conflict-en.png) | [日文](evidence/flutter/mobile-cloud-conflict-ja.png)

## 通关与符卡三语

网页和 Flutter 使用同一份 th07 云存档样例，选择灵梦 A 和 Normal。Flutter 在 320 px 英文下先收缩符卡色块，第 4 关和第 6 关的六个 Boss 格各自保持同行。长图例可换行，符卡名沿用日文原名。Pencil 已有中文符卡设计导出，英日语同状态稿待补。

| 视口 | 通关进度 | 符卡收取图 | 符卡列表 |
| --- | --- | --- | --- |
| 320 × 640 中文 | [网页](evidence/web/mobile-narrow-game-progress.png) / [Flutter](evidence/flutter/mobile-narrow-game-progress.png) | [网页](evidence/web/mobile-narrow-spell-map.png) / [Flutter](evidence/flutter/mobile-narrow-spell-map.png) | [网页](evidence/web/mobile-narrow-spell-list.png) / [Flutter](evidence/flutter/mobile-narrow-spell-list.png) |
| 320 × 640 英文 | [网页](evidence/web/mobile-narrow-game-progress-en.png) / [Flutter](evidence/flutter/mobile-narrow-game-progress-en.png) | [网页](evidence/web/mobile-narrow-spell-map-en.png) / [Flutter](evidence/flutter/mobile-narrow-spell-map-en.png) | [网页](evidence/web/mobile-narrow-spell-list-en.png) / [Flutter](evidence/flutter/mobile-narrow-spell-list-en.png) |
| 390 × 844 日文 | [网页](evidence/web/mobile-game-progress-ja.png) / [Flutter](evidence/flutter/mobile-game-progress-ja.png) | [网页](evidence/web/mobile-spell-map-ja.png) / [Flutter](evidence/flutter/mobile-spell-map-ja.png) | [网页](evidence/web/mobile-spell-list-ja.png) / [Flutter](evidence/flutter/mobile-spell-list-ja.png) |
th15 使用模拟云端成绩验证 320 px 下传统模式和绀珠传模式切换。难度色块先收缩，完整显示「铃仙·优昙华院·因幡」，切换后通关进度标题的位置一致。 [传统模式](evidence/flutter/mobile-narrow-th15-legacy.png) | [绀珠传模式](evidence/flutter/mobile-narrow-th15-pointdevice.png)

### 通关表刷新与符卡焦点

th07 的云存档样例从第 3 版更新到第 4 版，灵梦 A 的 Easy 最高分从 1,500,000 变为 1,600,000。320 px 回归核对当前机体、难度、成绩和通关表位置。 [刷新后的通关表](evidence/flutter/mobile-narrow-refreshed.png)

桌面与 320 px 英文竖屏回归核对聚焦和 Tab 切换显示的编号、原名、次数及提示边距，Esc 关闭提示。网页基线的交互依据见固定版本的符卡控件规格，Pencil 仅有收取图静态状态。 [桌面焦点](evidence/flutter/desktop-spell-keyboard.png) | [窄屏焦点](evidence/flutter/mobile-narrow-en-spell-keyboard.png)

## 真实存档通关状态

网页与 Flutter 使用同一版本账号解析器导出的存档统计，并经各自的详情页云存档入口显示。输入来自本机原作存档，读取和截图在隔离会话完成。这组存档尚无同状态 Pencil 稿。

| 作品和视口 | 网页 | Flutter | 核对结果 |
| --- | --- | --- | --- |
| th08，320 × 640 英文 | [网页](evidence/web/mobile-narrow-real-progress-th08.png) | [Flutter](evidence/flutter/real-progress-th08.png) | 结界组的 Lunatic 有游玩记录。12 个机体或组合的英文名可以完整显示。 |
| th10，1440 × 734 英文 | [网页](evidence/web/desktop-real-progress-th10.png) | [Flutter](evidence/flutter/real-progress-th10.png) | 两位角色各有 A/B/C 列。灵梦 A 的 Easy 为一命通关，点按显示实际最高分。 |
| th11，320 × 640 英文 | [网页](evidence/web/mobile-narrow-real-progress-th11.png) | [Flutter](evidence/flutter/real-progress-th11.png) | 所有机体的游玩次数为零。网页将默认榜分标为未通关，Flutter 标为未尝试。 |
| th12，390 × 844 日文 | [网页](evidence/web/mobile-real-progress-th12.png) | [Flutter](evidence/flutter/real-progress-th12.png) | 三位角色各有 A/B。初始档标为未尝试。 |


## th08 符卡收取

th07 的网页与 Pencil 收取图提供控件与布局基准。th08 的符卡区使用账号解析器输出的独立样例。实测原作空档含 222 个 CATK 条目，均无尝试记录。构造档在同一机体、同一张卡中分别记录正作 4/7、练习 6/11，并提供 Last Word 1/19。场景使用相同格式和读取入口，不对应实际游玩成绩。固定版网页与现有 Pencil 导出缺少 th08 逐卡视图。

| 视口 | 符卡练习 | Last Word |
| --- | --- | --- |
| 320 × 640 英文 | [收取图](evidence/flutter/mobile-narrow-th08-spell-practice.png) | [收取图](evidence/flutter/mobile-narrow-th08-last-word.png) / [列表](evidence/flutter/mobile-narrow-th08-last-word-list.png) |
| 390 × 844 日文 | [收取图](evidence/flutter/mobile-th08-spell-practice.png) | [Last Word](evidence/flutter/mobile-th08-last-word.png) |
| 1440 × 734 中文 | [收取图](evidence/flutter/desktop-th08-spell-practice.png) | [Last Word](evidence/flutter/desktop-th08-last-word.png) |

## 通关表亮暗层次

通关表、符卡收取图和符卡列表使用 Material 3 的细边框卡片。表底色与页面一致，使未读取和未尝试的中性色块可见；未通关增加边界。游戏详情、个人主页及全部作品共用通关状态配色。层次参考固定网页版本的 `clear-matrix.css`，测试使用同一份 th07 云存档样例，并走过刷新、选格、提示与收取图／列表切换。

| 视口 | 暗色未读取 | 亮色未读取 | 亮色记录与符卡 |
| --- | --- | --- | --- |
| 320 × 640 | [截图](evidence/flutter/mobile-narrow-game-progress-guest.png) | [截图](evidence/flutter/mobile-narrow-light-game-progress-guest.png) | [截图](evidence/flutter/mobile-narrow-light-game-progress.png) |
| 390 × 844 | [截图](evidence/flutter/mobile-game-progress-guest.png) | [截图](evidence/flutter/mobile-light-game-progress-guest.png) | [截图](evidence/flutter/mobile-light-game-progress.png) |
| 1440 × 734 | [截图](evidence/flutter/desktop-game-progress-guest.png) | [截图](evidence/flutter/desktop-light-game-progress-guest.png) | [截图](evidence/flutter/desktop-light-game-progress.png) |

Windows Release 使用游客 th06 从书架进入，实测[桌面配色](evidence/windows/progress-light.jpg)及[竖屏收缩](evidence/windows/portrait-progress-light.jpg)。竖屏保留机体全名、五列色块与同行操作按钮，返回后回到书架。

## 游戏资源导入三语

以正式站同一版本的 th06 对照导入和下载地址。Flutter 在同一弹窗内显示下载地址，关闭该视图或用系统返回即可继续选文件。两个动作等宽，共用字号和基线。当前 Pencil 导出缺少这两个状态。

Flutter 的失败和重试截图使用同一份清单和 th06 安装包，网络错误由隔离输入复现。 [三语截图和返回验收](game.md#游戏资源导入)

| 视口 | 导入 | 备用下载地址 |
| --- | --- | --- |
| 320 × 640 英文 | [网页](evidence/web/mobile-narrow-game-package-import-en.png) / [Flutter](evidence/flutter/mobile-narrow-game-package-import.png) | [网页](evidence/web/mobile-narrow-game-package-reference-en.png) / [Flutter](evidence/flutter/mobile-narrow-game-package-reference.png) |
| 390 × 844 日文 | [网页](evidence/web/mobile-game-package-import-ja.png) / [Flutter](evidence/flutter/mobile-game-package-import.png) | [网页](evidence/web/mobile-game-package-reference-ja.png) / [Flutter](evidence/flutter/mobile-game-package-reference.png) |
| 1440 × 734 中文 | [网页](evidence/web/desktop-game-package-import-zh.png) / [Flutter](evidence/flutter/desktop-game-package-import.png) | [网页](evidence/web/desktop-game-package-reference-zh.png) / [Flutter](evidence/flutter/desktop-game-package-reference.png) |

## 详情首屏整改

从书架实际点开 th07，使用相同发布目录、中文、云存档和好友样例采集整改前后。网页清单确认版本为 `85941f0d564cf10378d2af6b14d41a000bd27612`。Pencil 的账户和成绩不同，用于核对布局与层级。以下截图供审查，尚未获得用户视觉认可。

| 视口与主题 | 网页 | 整改前 | 当前整改 | 测量 |
| --- | --- | --- | --- | --- |
| 1440 × 734 暗色 | [截图](evidence/web/desktop-game-detail-entry.png) | [截图](evidence/flutter/desktop-detail-layout-before.png) | [截图](evidence/flutter/desktop-detail-layout-after.png) | [坐标](evidence/flutter/desktop-detail-layout.json) |
| 1440 × 734 亮色 | [截图](evidence/web/desktop-game-detail-entry-light.png) | [截图](evidence/flutter/desktop-light-detail-layout-before.png) | [截图](evidence/flutter/desktop-light-detail-layout-after.png) | [坐标](evidence/flutter/desktop-light-detail-layout.json) |
| 390 × 844 暗色 | [截图](evidence/web/mobile-game-detail-entry.png) | [截图](evidence/flutter/mobile-detail-layout-before.png) | [截图](evidence/flutter/mobile-detail-layout-after.png) | [坐标](evidence/flutter/mobile-detail-layout.json) |
| 390 × 844 亮色 | [截图](evidence/web/mobile-game-detail-entry-light.png) | [截图](evidence/flutter/mobile-light-detail-layout-before.png) | [截图](evidence/flutter/mobile-light-detail-layout-after.png) | [坐标](evidence/flutter/mobile-light-detail-layout.json) |
| 320 × 640 暗色 | [截图](evidence/web/mobile-narrow-game-detail-entry.png) | [截图](evidence/flutter/mobile-narrow-detail-layout-before.png) | [截图](evidence/flutter/mobile-narrow-detail-layout-after.png) | [坐标](evidence/flutter/mobile-narrow-detail-layout.json) |
| 320 × 640 亮色 | [截图](evidence/web/mobile-narrow-game-detail-entry-light.png) | [截图](evidence/flutter/mobile-narrow-light-detail-layout-before.png) | [截图](evidence/flutter/mobile-narrow-light-detail-layout-after.png) | [坐标](evidence/flutter/mobile-narrow-light-detail-layout.json) |

1440 px 的封面原先从 x = 313 开始，统计区从 x = 348.5 开始。调整后共用 x = 353，封面从 144 px 恢复至 174 px，标题从 24 px 恢复至 48 px。通关、符卡和好友标题共用 19 px 字号。头部恢复本作封面背景，亮暗主题都使用该背景对应的浅色文字，避免外层亮色主题使运行数据变黑。

手机收藏位置及「导入」文案沿用后续要求，覆盖旧稿中的位置与文案。控制高度共用 44 px。网页以小数小时展示时长，客户端按本地化的小时和分钟展示。正常字号下，320 px 的日期与时长仍在同一行。Windows 渲染中的手机视口用于布局检查，不代表移动设备字体或性能验收。

目录中的 11 部作品逐一通过实际搜索和封面入口打开，覆盖 320、390、1440 px 与亮暗主题。检查方形封面、统一左边线、同一行的启动／导入／设置、手机右上收藏与桌面动作尾部收藏，以及返回书架。长文字、放大字号和减少动画另由对应回归覆盖。
