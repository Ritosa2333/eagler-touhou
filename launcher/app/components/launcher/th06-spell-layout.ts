import type {SaveProgress} from '../../services/cloud-saves';

// Stage ranges are the native one-based CATK numbers. Difficulty membership
// also matches the Purple statistics table, excluding its practice counter.
const stageEnds = [3, 7, 14, 32, 40, 51, 64];
const midboss = new Set([1, 8, 9, 33, 34, 41, 52, 53, 54]);
const hard = [1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 34, 38, 39, 40, 41, 47, 48, 49, 50, 51];
const layout: Partial<Record<SaveProgress['difficulty'], readonly number[]>> = {
  easy: [2, 3, 4, 6, 8, 10, 12, 15, 16, 17, 18, 28, 29, 30, 32, 33, 35, 36, 37],
  normal: [2, 3, 4, 6, 7, 8, 10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 28, 29, 30, 31, 32, 33, 35, 36, 37, 41, 42, 43, 44, 45, 46],
  hard, lunatic: hard,
  extra: Array.from({length: 13}, (_, index) => index + 52),
};

export function th06SpellStages(difficulty: SaveProgress['difficulty']) {
  const numbers = layout[difficulty] ?? [];
  return stageEnds.flatMap((end, index) => {
    const start = stageEnds[index - 1] ?? 0;
    const cards = numbers.filter(number => start < number && number <= end)
      .map(number => ({number, midboss:midboss.has(number), nameLength:3}));
    return cards.length ? [{stage:index + 1, cards}] : [];
  });
}
