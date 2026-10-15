export interface Th09ReplayMatch {
  schema: 'eagler-touhou/th09-match-replay/1';
  names: [string, string];
  startedAt: number;
  frames: number;
  winner: 0 | 1;
}
export function parseTh09ReplayMatch(value: unknown): Th09ReplayMatch | null {
  if (!value || typeof value !== 'object') return null;
  const m = value as Record<string, unknown>;
  if (m.schema !== 'eagler-touhou/th09-match-replay/1' || !Array.isArray(m.names) || m.names.length !== 2 ||
      m.names.some(name => typeof name !== 'string' || name.length > 64) ||
      !Number.isSafeInteger(m.startedAt) || Number(m.startedAt) < 0 ||
      !Number.isSafeInteger(m.frames) || Number(m.frames) < 30 || ![0, 1].includes(Number(m.winner)) || typeof m.winner !== 'number') return null;
  return {schema: m.schema, names: m.names.map((name, side) => name || `P${side + 1}`) as [string, string],
    startedAt: Number(m.startedAt), frames: Number(m.frames), winner: m.winner as 0 | 1};
}
export const th09ReplayMatchLabel = (match: Th09ReplayMatch): string => match.names.join(' vs ');
// Preserve actual archive directories; never synthesize groups from sidecars.
export function th09ReplayImportDirectory(path: string): string {
  const parts = path.split('/');
  if (parts.length !== 3 || parts[0] !== 'replay') return 'replay';
  const folder = parts[1]!.replace(/[<>:"/\\|?*\u0000-\u001f\u007f]/g, '_').slice(0, 128).replace(/[. ]+$/g, '');
  return folder && folder !== '.' && folder !== '..' ? `replay/${folder}` : 'replay';
}

export function isTh09ReplayDirectory(path: string): boolean {
  if (path === 'replay') return true;
  const parts = path.split('/'), name = parts[1];
  return parts.length === 2 && parts[0] === 'replay' && !!name && name.length <= 128 &&
    name !== '.' && name !== '..' && !/[<>:"\\|?*\u0000-\u001f\u007f]/.test(name) && !/[. ]$/.test(name);
}
