import {readFile, access} from 'node:fs/promises';
import {resolve} from 'node:path';
const root = resolve(import.meta.dirname, '../..');
const workspace = JSON.parse(await readFile(resolve(root, 'launcher/config/workspace.json'), 'utf8'));
const builds = JSON.parse(await readFile(resolve(root, 'launcher/config/runtime-builds.json'), 'utf8'));
const affected = new Set(JSON.parse(process.env.EAGLER_CI_AFFECTED || '[]'));
let checked = 0;
for (const [game, entry] of Object.entries(builds.games)) for (const [variant, definition] of Object.entries(entry.variants)) {
  if (!['normal', 'multiplayer'].includes(variant)) continue;
  const directory = workspace.repositories[definition.workspaceRepository ?? entry.workspaceRepository];
  if (!affected.has(directory)) continue;
  const recipe = {...entry.sourceBuild, ...definition.sourceBuild};
  await access(resolve(root, directory, recipe.kind === 'cmake' ? 'CMakeLists.txt' : recipe.build[0]));
  for (const input of [recipe.commonMarker ?? 'include/eagler/input/DialogueGesture.hpp', ...(recipe.commonInputs ?? [])])
    await access(resolve(root, workspace.repositories.common, input));
  if (variant === 'multiplayer') await access(resolve(root, workspace.repositories.common, 'include/eagler/netplay/SessionChannel.hpp'));
  checked++;
}
console.log(JSON.stringify({sourceEntries: checked, scope: 'source/input contracts; not compilation or gameplay'}));
