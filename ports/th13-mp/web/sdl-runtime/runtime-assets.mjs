export const runtimeAssets = [
  'text.anm', 'sig.anm', 'ascii.anm', 'effect.anm', 'title.anm', 'title_v.anm', 'help.anm',
  ...Array.from({length: 8}, (_, index) => `help_${String(index + 1).padStart(2, '0')}.png`),
  'front.anm', 'enemy.anm', 'bullet.anm', 'astral.anm', 'default.ecl', 'staff.msg',
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

export const audioAssets = [
  'thbgm.fmt',
  'se_plst00.wav', 'se_enep00.wav', 'se_pldead00.wav', 'se_power0.wav', 'se_power1.wav',
  'se_tan00.wav', 'se_tan01.wav', 'se_tan02.wav', 'se_ok00.wav', 'se_cancel00.wav',
  'se_select00.wav', 'se_gun00.wav', 'se_cat00.wav', 'se_lazer00.wav', 'se_lazer01.wav',
  'se_enep01.wav', 'se_damage00.wav', 'se_item00.wav', 'se_kira00.wav', 'se_kira01.wav',
  'se_kira02.wav', 'se_timeout.wav', 'se_graze.wav', 'se_powerup.wav', 'se_pause.wav',
  'se_cardget.wav', 'se_damage01.wav', 'se_timeout2.wav', 'se_invalid.wav', 'se_slash.wav',
  'se_ch00.wav', 'se_ch01.wav', 'se_extend.wav', 'se_nep00.wav', 'se_bonus.wav',
  'se_bonus2.wav', 'se_enep02.wav', 'se_lazer02.wav', 'se_nodamage.wav', 'se_boon00.wav',
  'se_don00.wav', 'se_boon01.wav', 'se_ch02.wav', 'se_ch03.wav', 'se_extend2.wav',
  'se_pin00.wav', 'se_pin01.wav', 'se_lgods1.wav', 'se_lgods2.wav', 'se_lgods3.wav',
  'se_lgods4.wav', 'se_lgodsget.wav', 'se_msl.wav', 'se_msl2.wav', 'se_pldead01.wav',
  'se_heal.wav', 'se_astralup.wav', 'se_fault.wav', 'se_noise.wav',
];

export const musicShardSize = 1048576;
export const musicShardCount = 30;
export const musicFinalShardSize = 150104;
export const musicShardAssets = Array.from(
  {length: musicShardCount},
  (_, index) => `thbgm-${String(index).padStart(2, '0')}.part`);
