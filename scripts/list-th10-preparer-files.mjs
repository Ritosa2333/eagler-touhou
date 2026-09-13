import {resolve} from 'node:path';
import {workspacePath} from '../lib/workspace-layout.mjs';
import {localModuleClosure} from '../lib/browser-module-graph.mjs';

const repository=workspacePath('th10'),site=resolve(repository,'site');
const modules=await localModuleClosure({root:site,entries:['runtime/retail-music-layout.mjs']});
console.log(JSON.stringify([
  {source:resolve(repository,'scripts/prepare-eagler-content.mjs'),target:'th10-runtime/scripts/prepare-eagler-content.mjs'},
  ...modules.map(name=>({source:resolve(site,name),target:'th10-runtime/'+name})),
  {source:resolve(site,'vendor/th10-game.wasm'),target:'th10-runtime/vendor/th10-game.wasm'},
]));
