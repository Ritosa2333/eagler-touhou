import type {UiMessageKey} from '../../i18n';

export interface Announcement {
  id: string;
  author: string;
  published: string;
  groups: readonly {type?: 'Feature' | 'Fix'; points: readonly UiMessageKey[]}[];
}

export const announcements: readonly Announcement[] = [
  {
    id: '2026-11-19-launcher',
    author: 'Ritosa',
    published: '2026-11-19T18:09:19+08:00',
    groups: [{type: 'Fix', points: ['announcements.titleBarGap', 'announcements.adaptationNotice']}],
  },
  {
    id: '2026-11-19-registration',
    author: 'Ritosa',
    published: '2026-11-19T14:11:00+08:00',
    groups: [{type: 'Feature', points: ['announcements.registrationCaptcha']}],
  },
];
