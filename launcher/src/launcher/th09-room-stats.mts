export interface Th09PlayerStats { wins: number; losses: number; minutes: number; }
export function parseTh09PlayerStats(value: unknown): Th09PlayerStats | undefined {
  if (!value || typeof value !== 'object' || Array.isArray(value)) return;
  const item = value as Record<string, unknown>;
  if (![item.wins,item.losses,item.minutes].every(n => Number.isSafeInteger(n) && (n as number) >= 0)) return;
  return {wins:item.wins as number,losses:item.losses as number,minutes:item.minutes as number};
}

export interface Th09AccountSummary { games: number; wins: number }

export function parseTh09AccountSummary(value: unknown): Th09AccountSummary | undefined {
  if (!value || typeof value !== 'object') return;
  const {games, wins} = value as Partial<Th09AccountSummary>;
  if (typeof games !== 'number' || typeof wins !== 'number' ||
      !Number.isSafeInteger(games) || !Number.isSafeInteger(wins) || wins < 0 || games < wins) return;
  return {games, wins};
}

export function formatTh09WinRate({games,wins}: Th09AccountSummary, locale: string) {
  return games ? new Intl.NumberFormat(locale,{style:'percent',maximumFractionDigits:0}).format(wins / games) : null;
}
