import type {SaveProgress} from './cloud-saves';
import {readLocalSaveFiles} from './local-save-storage';

const difficulties = ['easy', 'normal', 'hard', 'lunatic', 'extra'] as const;
const settingsBytes = (9 + 5) * 4;
const progressBytes = 5 * 3 * 3 * 7 * 4;

/** 原生存档为每种武器保存七面的结果：0 未通关、1 续关通关、2 不续关通关。 */
export function parseThkkdProgress(bytes: Uint8Array): SaveProgress[] {
  if (bytes.byteLength < settingsBytes + progressBytes) throw new Error('Incomplete thkkd save');
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const progress: SaveProgress[] = [];
  for (let difficulty = 0; difficulty < 5; difficulty++) {
    for (let character = 0; character < 3; character++) {
      for (let shot = 0; shot < 3; shot++) {
        const finalStage = difficulty === 4 ? 6 : 5;
        const offset = settingsBytes + (difficulty * 63 + character * 21 + shot * 7 + finalStage) * 4;
        const value = view.getUint32(offset, true);
        progress.push({
          difficulty: difficulties[difficulty], character, shot,
          cleared: value <= 2 ? value > 0 : null,
          oneCreditClear: value <= 2 ? value === 2 : null,
          clearCount: null, playCount: null, bestScore: null, playtimeSeconds: null,
        });
      }
    }
  }
  return progress;
}

export async function readThkkdProgress(factory: IDBFactory) {
  const [file] = await readLocalSaveFiles('thkkd', ['th_kaikeidou01.dat'], 'ja', factory);
  if (!file) return;
  return {progress: parseThkkdProgress(new Uint8Array(await file.arrayBuffer())), modifiedAt: file.lastModified};
}
