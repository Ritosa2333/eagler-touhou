import {readdir, readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {printRuntimeInputs} from './runtime-inputs.mjs';

// Include checked-in runtime cores in the assembly fingerprint. Private disks
// and music are inventoried separately by build-eagler's package receipt.
export async function printPC98Inputs({port, game}) {
  const inputs = ['source-import.json'];
  async function visit(directory, prefix) {
    let entries;
    try { entries = await readdir(directory, {withFileTypes:true}); }
    catch (error) { if (error.code === 'ENOENT') return; throw error; }
    for (const entry of entries) {
      if (entry.isSymbolicLink()) throw Error('Source symlinks are not accepted');
      const name = `${prefix}/${entry.name}`;
      if (entry.isDirectory()) {
        if (!/^(?:node_modules|\.cache|build|reports|__pycache__)$/.test(entry.name))
          await visit(resolve(directory,entry.name),name);
      } else if (/\.(?:m?js|cjs|html|css|json|cpp|c|h|hpp|inc|asm|nasm|wat|wasm|py|sh|rs|toml|lock)$/.test(entry.name)) inputs.push(name);
    }
  }
  for (const directory of ['web','scripts','patches','tools']) await visit(resolve(port,directory),directory);
  await visit(resolve(port,'../../common/browser'),'../../common/browser');
  if(game!=='th01'){
    await visit(resolve(port,'../../common/pc98'),'../../common/pc98');
    inputs.push('../../tools/build/pc98-pmd.mjs');
  }
  inputs.push('../../tools/build/pc98-inputs.mjs','../../tools/build/runtime-inputs.mjs');
  const baseline = JSON.parse(await readFile(resolve(port,'source-import.json'),'utf8'));
  await printRuntimeInputs({workspace:port, root:port, sources:[], headers:inputs,
    flags:[`game=${game}`, 'assembly=managed-import', game==='th05'?'native-core=external-precompiled':'native-core=checked-in-precompiled', `source-baseline=${baseline.commit}`]});
}
