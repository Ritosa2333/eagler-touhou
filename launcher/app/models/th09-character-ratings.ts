import {z} from 'zod';
import {th09CharacterLoadout} from '../../src/launcher/th09-room-stats.mjs';
import type {Translate, UiMessageKey} from '../i18n';

export const th09CharacterIds = Array.from({length:16}, (_, index) => index);
const positive = z.number().int().positive();
export const th09RatingPoint = z.object({at:z.number().int().nonnegative().max(8.64e15), rating:z.number().int()});
export const th09CharacterRating = z.object({
  character:z.number().int().min(0).max(th09CharacterIds.length-1), rating:z.number().int(),
  interval:z.number().int().nonnegative(), games:positive, rank:positive.optional(),
  history:z.array(th09RatingPoint).optional(),
});
export function th09CharacterName(character:number, t:Translate) {
  const loadout = th09CharacterLoadout(character);
  return loadout ? t(loadout.labelKey as UiMessageKey) : t('profile.loadoutNumber',{number:character+1});
}
