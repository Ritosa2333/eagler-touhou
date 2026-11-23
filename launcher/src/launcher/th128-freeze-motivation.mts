export const TH128_FREEZE_MOTIVATION_MULTIPLIERS = [2.5, 2, 1, 0.75, 0.5] as const;
export type Th128FreezeMotivationMultiplier = typeof TH128_FREEZE_MOTIVATION_MULTIPLIERS[number];
export function validTh128FreezeMotivationMultiplier(value: unknown): value is Th128FreezeMotivationMultiplier {
  return TH128_FREEZE_MOTIVATION_MULTIPLIERS.some((multiplier) => multiplier === value);
}
