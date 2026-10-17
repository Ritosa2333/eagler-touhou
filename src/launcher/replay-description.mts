export interface ReplayStageDescription {id: number; label?: string; score: number | null; frames?: number}
export interface ReplayDescription {
  name: string; date?: string; timestamp?: number; score?: number; slowdown?: number;
  character: number; shot?: number; difficulty: number; lastStage?: number;
  spell?: string; spellName?: string; mode?: string; stages: ReplayStageDescription[];
}

/** Only native, validated metadata may select a playable stage. */
export function parseReplayDescription(value: unknown): ReplayDescription {
  if (!value || typeof value !== 'object') throw Error('录像信息无效');
  const d = value as Record<string, unknown>;
  if (!Array.isArray(d.stages) || !d.stages.length || d.stages.length > 20 ||
      !Number.isSafeInteger(d.character) || !Number.isSafeInteger(d.difficulty)) throw Error('录像信息无效');
  const ids = new Set<number>();
  const stages = d.stages.map((value: unknown): ReplayStageDescription => {
    if (!value || typeof value !== 'object') throw Error('录像关卡无效');
    const s = value as Record<string, unknown>;
    if (!Number.isSafeInteger(s.id) || Number(s.id) < 0 || ids.has(Number(s.id)) ||
        s.score !== null && (typeof s.score !== 'number' || !Number.isFinite(s.score) || s.score < 0) ||
        s.frames !== undefined && (!Number.isSafeInteger(s.frames) || Number(s.frames) < 0)) throw Error('录像关卡无效');
    ids.add(Number(s.id));
    return {id: Number(s.id), score: s.score as number | null, ...(typeof s.label === 'string' ? {label: s.label.slice(0, 128)} : {}), ...(s.frames !== undefined ? {frames: Number(s.frames)} : {})};
  });
  let name = typeof d.name === 'string' ? d.name.slice(0, 128) : '';
  if (Array.isArray(d.nameBytes) && d.nameBytes.length <= 128 && d.nameBytes.every(n => Number.isInteger(n) && n >= 0 && n <= 255))
    name = new TextDecoder('shift_jis').decode(new Uint8Array(d.nameBytes)).replace(/\0.*$/, '').trim();
  const result: ReplayDescription = {name, character: Number(d.character), difficulty: Number(d.difficulty), stages};
  for (const key of ['score', 'slowdown', 'timestamp', 'lastStage', 'shot'] as const)
    if (typeof d[key] === 'number' && Number.isFinite(d[key])) result[key] = d[key];
  for (const key of ['date', 'spell', 'spellName', 'mode'] as const) if (typeof d[key] === 'string') result[key] = d[key].slice(0, 256);
  if (Array.isArray(d.spellNameBytes) && d.spellNameBytes.length <= 128 && d.spellNameBytes.every(n => Number.isInteger(n) && n >= 0 && n <= 255))
    result.spellName = new TextDecoder('shift_jis').decode(new Uint8Array(d.spellNameBytes)).replace(/\0.*$/, '').trim();
  return result;
}
