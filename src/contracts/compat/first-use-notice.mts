const keys=['eagler-touhou-new-player-notice-seen-v1','eagler-touhou-changelog-seen-v2','eagler-touhou-changelog-seen-20260822-1'];
/** Any nonempty value, including values other than "1", was the acknowledgement contract. */
export function legacyFirstUseNoticeSeen(storage: Pick<Storage,'getItem'> | null) {
  return keys.some(key=>!!storage?.getItem(key));
}
