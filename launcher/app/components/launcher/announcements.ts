import type {UiMessageKey} from '../../i18n';

interface AnnouncementIdentity {
  id: string;
  author: string;
  authorUsername?: string | null;
  published: string;
}

export type Announcement = AnnouncementIdentity & (
  {body: string} |
  {groups: readonly {type?: 'Feature' | 'Fix'; points: readonly UiMessageKey[]}[]}
);

export const announcements: readonly Announcement[] = [{
  id: 'performance-incident-20261127',
  author: 'Goan114',
  authorUsername: 'goan114',
  published: '2026-11-27T15:49:30Z',
  body: `目前由于未知原因，导致目前站内游戏出现严重的性能问题。或是长时间严重卡顿，或是帧数严重不稳定。

目前，受影响的游戏有：

- 东方红魔乡（PC、手机）
- 东方妖妖梦（PC、手机）
- 东方风神录（PC）
- 东方星莲船（PC、手机）
- 其他可能受影响的游戏

我们正在尽全力去寻找站内游戏出现性能问题的原因，目前我们正在尽力抢救，而且不知道严重掉帧、卡顿的根本原因。各位用户请耐心等待。

--Goan114`,
}, {
  id: 'th075-multiplayer-20261125',
  author: 'Goan114',
  authorUsername: 'goan114',
  published: '2026-11-25T07:39:21Z',
  body: `我现在才发现这么方便的功能！That's awesome!
Fix:
- 修复了萃梦想的旁观错误显示加载的问题
- 修复了萃梦想旁观无法正常中途加入的问题
- 修复了风神录观战无法换面的问题
Feature:
- 更新了萃梦想的联机大厅，添加比分
- 将萃梦想的选人和环境更换机制放入游戏内
- 一局游戏结束后现在可以直接继续萃梦想游戏`,
}];
