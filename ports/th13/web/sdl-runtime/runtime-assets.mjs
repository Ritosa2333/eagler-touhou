// Original stage-table resources and ECL ANIM/include dependencies. Both
// browser entry points and the asset preparation command use this list.
export const runtimeAssets = [
  'text.anm', 'sig.anm', 'ascii.anm', 'effect.anm', 'title.anm', 'title_v.anm', 'help.anm',
  'front.anm', 'enemy.anm', 'bullet.anm', 'astral.anm', 'default.ecl', 'staff.msg',
  // Ending 场景（0x416130 家族）：结局 CG（e##.anm，op7 装载槽 20+idx）+
  // 滚屏脚本（e##.msg，名表 0x4bba20）+ 名单 CG（staff.anm，op12 接 staff.msg）。
  'staff.anm',
  ...Array.from({length: 12}, (_, index) => `e${String(index + 1).padStart(2, '0')}.anm`),
  ...Array.from({length: 12}, (_, index) => `e${String(index + 1).padStart(2, '0')}.msg`),
  ...Array.from({length: 4}, (_, character) => `pl0${character}.anm`),
  ...Array.from({length: 4}, (_, character) => `pl0${character}a.sht`),
];

for (let stage = 1; stage <= 7; ++stage) {
  const prefix = `st0${stage}`;
  runtimeAssets.push(`${prefix}.ecl`, `${prefix}bs.ecl`, `${prefix}.std`,
                    `${prefix}wl.anm`, `${prefix}enm.anm`, `${prefix}logo.anm`,
                    ...['a', 'b', 'c', 'd'].map(character => `${prefix}${character}.msg`));
  if ([2, 3, 4, 5, 7].includes(stage)) runtimeAssets.push(`${prefix}mbs.ecl`);
  if ([3, 4, 5, 7].includes(stage)) runtimeAssets.push(`${prefix}menm.anm`);
  if (stage === 6) runtimeAssets.push('st06enm2.anm', 'st06enm3.anm');
}
