import {z} from 'zod';
import type {UiMessageKey} from '../i18n';
import type {ProfileStatistics} from '../services/cloud-saves';

const overviewKeys = ['career', 'versus', 'records', 'content', 'friends', 'favorites'] as const;
const display = z.object({
  order: z.array(z.enum(overviewKeys)),
  visible: z.object({career:z.boolean(), versus:z.boolean().optional(), records:z.boolean(),
    content:z.boolean(), friends:z.boolean(), favorites:z.boolean().optional()}),
});
type Display = z.infer<typeof display>;
export type ProfileDisplay = Display & {visible: Display['visible'] & {versus: boolean}};

export const profileSectionLabels = {
  career: 'profile.tab.career',
  versus: 'profile.versusCareer',
  records: 'profile.tab.records',
} as const satisfies Record<string, UiMessageKey>;
export type ProfileSection = keyof typeof profileSectionLabels;
export type ProfileSaveSection = Exclude<ProfileSection, 'versus'>;

export function isProfileSection(key: Display['order'][number]): key is ProfileSection {
  return Object.hasOwn(profileSectionLabels, key);
}

export function normalizeProfileDisplay(value: unknown): ProfileDisplay {
  const {data} = display.safeParse(value);
  const visible: Display['visible'] = data?.visible ?? {career:true, records:true, content:true, friends:true};
  const savedOrder = data?.order;
  const order = savedOrder && new Set(savedOrder).size === savedOrder.length &&
    (savedOrder.length === overviewKeys.length || savedOrder.length === overviewKeys.length - 1 && !savedOrder.includes('versus'))
    ? [...savedOrder] : [...overviewKeys];
  if (!order.includes('versus')) order.splice(order.indexOf('career') + 1, 0, 'versus');
  return {visible:{...visible, versus:visible.versus ?? visible.records}, order};
}

export function hasProfileSaveSection(data: ProfileStatistics | undefined, key: ProfileSaveSection) {
  if (!data?.[key]) return false;
  return key === 'career' ? data.games.some(game => game.statistics.progress.length > 0) :
    data.games.some(game => game.statistics.playtimeSeconds !== null || game.statistics.playCount !== null);
}
