import {existsSync} from 'node:fs';
import {copyFile,mkdir,readFile} from 'node:fs/promises';
import {resolve,dirname} from 'node:path';

const contracts=['product-catalog','netplay-timing','multiplayer-quick-chat','public-nickname'];
const shared=[...contracts,'th15-chapter-reward'];
const root=project=>resolve(project,'../common/contracts');

export function sharedContractInputs(project) {
  const directory=root(project);
  if(!existsSync(directory))return [];
  return shared.flatMap(name=>['.mjs','.d.mts'].map(extension=>resolve(directory,name+extension)));
}

// Preserve the existing flat browser publication URLs. These are build outputs
// copied from the same neutral authority used directly by standalone services.
export async function copySharedContractOutputs(project,assets,{contractsOnly=false}={}) {
  const directory=root(project);
  if(!existsSync(directory))return;
  for(const name of contractsOnly?contracts:shared) {
    const component=contracts.includes(name)?'contracts':'launcher';
    const facade=await readFile(resolve(project,'src',component,name+'.mts'),'utf8');
    const authored=facade.replace(/\/\/[^\n]*/g,'').trim();
    if(authored!==`export * from '../../../common/contracts/${name}.mjs';`)throw Error('Shared contract facade must remain a pure re-export: '+name);
    const target=resolve(assets,component,name+'.mjs');
    await mkdir(dirname(target),{recursive:true});
    await copyFile(resolve(directory,name+'.mjs'),target);
  }
}
