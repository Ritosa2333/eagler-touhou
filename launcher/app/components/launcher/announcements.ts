import type {UiMessageKey} from '../../i18n';

export interface Announcement {
  id: string;
  author: string;
  published: string;
  groups: readonly {type?: 'Feature' | 'Fix'; points: readonly UiMessageKey[]}[];
}

export const announcements: readonly Announcement[] = [];
