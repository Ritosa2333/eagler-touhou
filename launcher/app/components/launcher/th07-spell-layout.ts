import type {SaveProgress} from '../../services/cloud-saves';

type CardLayout = readonly [number: number, nameLength: number];
const levels = ['easy','normal','hard','lunatic','extra','phantasm'] as const;
const midbossRanges = [[1,2],[11,14],[27,28],[69,72],[89,92],[117,118],[129,130]] as const;

// Card numbers, difficulties and visible name lengths follow ThScoreFileConverter's th07 Definitions.
// The displayed Japanese name is read from the player's CATK record.
const stages: readonly (readonly (readonly CardLayout[])[])[] = [
  [[[3,21],[7,22]], [[4,14],[8,15]], [[1,12],[5,21],[9,15]], [[2,22],[6,24],[10,13]], [], []],
  [[[11,14],[15,15],[19,15],[23,15]], [[12,7],[16,8],[20,8],[24,8]], [[13,8],[17,8],[21,9],[25,8]], [[14,18],[18,8],[22,10],[26,8]], [], []],
  [[[29,19],[33,18],[37,17],[41,19]], [[30,12],[34,11],[38,10],[42,12]], [[27,8],[31,19],[35,12],[39,11],[43,19]], [[28,18],[32,14],[36,22],[40,9],[44,11]], [], []],
  [[[45,21],[49,23],[53,19],[57,20],[61,21],[65,24]], [[46,14],[50,16],[54,12],[58,13],[62,14],[66,17]], [[47,15],[51,14],[55,14],[59,15],[63,16],[67,18]], [[48,25],[52,17],[56,24],[60,25],[64,26],[68,18]], [], []],
  [[[69,19],[73,19],[77,19],[81,16],[85,17]], [[70,12],[74,12],[78,12],[82,9],[86,10]], [[71,10],[75,10],[79,9],[83,9],[87,9]], [[72,12],[76,10],[80,19],[84,9],[88,9]], [], []],
  [[[89,17],[93,16],[97,15],[101,13],[105,22],[109,17],[113,11]], [[90,10],[94,12],[98,15],[102,16],[106,22],[110,17],[114,11]], [[91,17],[95,14],[99,15],[103,18],[107,22],[111,17],[115,11]], [[92,20],[96,12],[100,15],[104,16],[108,22],[112,17],[116,11]], [], []],
  [[], [], [], [], [[117,8],[118,10],[119,8],[120,10],[121,12],[122,14],[123,22],[124,16],[125,15],[126,5],[127,10],[128,10]], []],
  [[], [], [], [], [], [[129,11],[130,10],[131,9],[132,10],[133,10],[134,16],[135,11],[136,11],[137,9],[138,7],[139,10],[140,10],[141,9]]]
];

export function th07SpellStages(difficulty: SaveProgress['difficulty']) {
  const level = levels.findIndex(value => value === difficulty);
  if (level < 0) return [];
  return stages.flatMap((layouts, stage) => {
    const cards = layouts[level];
    if (!cards?.length) return [];
    return [{
      stage: stage + 1,
      cards: cards.map(([number, nameLength]) => ({
        number, nameLength, midboss: midbossRanges.some(([first,last]) => number >= first && number <= last),
      })),
    }];
  });
}
