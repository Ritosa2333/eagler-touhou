import {componentFileIds} from './package-generation.mjs';

// Multiplayer title previews in TH02 and TH04 use the solo disk as well.
export function pc98RequiredBlockFiles(descriptor, {game, language, multiplayer, replay = false}) {
  if (!['th02', 'th03', 'th04'].includes(game) || descriptor?.game !== game) throw Error('Invalid PC-98 game');
  const japanese = language === 'ja';
  let paths = game === 'th02'
    ? [`th02-solo-${japanese ? 'jp' : 'cn'}.hdi.gz`, ...(multiplayer ? [`th02-local-${japanese ? 'jp' : 'cn'}.hdi.gz`] : [])]
    : game === 'th03' ? [`disks/3-${japanese ? 'jp' : 'cn'}.hdi.gz`, ...(multiplayer && japanese ? ['disks/3-cn.hdi.gz'] : [])]
      : [`th04-solo${japanese ? '-jp' : ''}.hdi.gz`, ...(multiplayer ? [`th04-coop${japanese ? '-jp' : ''}.hdi.gz`] : [])];
  // A replay can carry a different language from the launcher's selection.
  if (replay) paths = game === 'th02' ? ['th02-solo-jp.hdi.gz', 'th02-solo-cn.hdi.gz']
    : game === 'th03' ? ['disks/3-jp.hdi.gz', 'disks/3-cn.hdi.gz']
      : ['th04-solo-jp.hdi.gz', 'th04-solo.hdi.gz'];
  const ids = ['pc98-disk-manifest'];
  const manifest = descriptor.components?.['pc98-manifest'];
  if (manifest?.type !== 'pc98-disk' || descriptor.files?.[ids[0]]?.target !== '/pc98/disks.json' || !componentFileIds(descriptor, 'pc98-manifest').includes(ids[0])) throw Error('PC-98 disk manifest is missing');
  for (const path of paths) {
    const componentId = `pc98-disk:${path.replaceAll('/', ':')}`;
    const component = descriptor.components?.[componentId];
    if (component?.type !== 'pc98-disk') throw Error(`PC-98 disk component is missing: ${path}`);
    const files = componentFileIds(descriptor, componentId);
    if (!files.length || files.some(id => !/^pc98-block:[a-f0-9]{64}$/.test(id) ||
      descriptor.files?.[id]?.target !== `/pc98/disk-blocks/${id.slice('pc98-block:'.length)}.gz`)) throw Error(`Invalid PC-98 disk component: ${path}`);
    ids.push(...files);
  }
  return [...new Set(ids)];
}
