export interface Th09PlayerStats { wins: number; losses: number; minutes: number; }
export function parseTh09PlayerStats(value: unknown): Th09PlayerStats | undefined {
  if (!value || typeof value !== 'object' || Array.isArray(value)) return;
  const item = value as Record<string, unknown>;
  if (![item.wins,item.losses,item.minutes].every(n => Number.isSafeInteger(n) && (n as number) >= 0)) return;
  return {wins:item.wins as number,losses:item.losses as number,minutes:item.minutes as number};
}
