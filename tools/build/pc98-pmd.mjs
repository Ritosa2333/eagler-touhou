import {execFileSync} from 'node:child_process';
import {copyFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';

export async function installPc98Pmd(repo,output,runtime){
  const target=resolve(repo,'.cache/pc98-pmd');
  execFileSync(process.env.CARGO||'cargo',['build','--release','--locked','--target','wasm32-unknown-unknown',
    '--manifest-path',resolve(repo,'common/pc98/pmd-player/Cargo.toml'),'--target-dir',target,'-p','pc98-pmd-web'],{stdio:'inherit'});
  await mkdir(output,{recursive:true});
  const files=[['common/pc98/pmd-archives.mjs','pmd-archives.mjs'],['common/pc98/pmd-player.mjs','pmd-player.mjs'],
    ['common/pc98/pmd-worklet.js','pmd-worklet.js'],['common/pc98/pmd-player/LICENSE','pmd-player.LICENSE.txt'],
    ['common/pc98/pmd-player/NOTICE','pmd-player.NOTICE.txt'],['common/pc98/pmd-player/UPSTREAM','pmd-player.UPSTREAM.txt'],
    [resolve(target,'wasm32-unknown-unknown/release/pc98_pmd_web.wasm'),'pmd-player.wasm']];
  for(const [source,name] of files){await copyFile(resolve(repo,source),resolve(output,name));runtime.push(name);}
}
