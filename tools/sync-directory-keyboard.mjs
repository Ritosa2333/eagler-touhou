import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

// Self-contained directory module: title pins need no submodule update.
const owners=(await readFile(new URL('../browser/keyboard-owners.mjs',import.meta.url),'utf8')).replaceAll('\r\n','\n');
const bridge=(await readFile(new URL('../browser/directory-keyboard.mjs',import.meta.url),'utf8')).replaceAll('\r\n','\n').replace(/^import .*;\n/,'');
const source='// Generated from eagler-common/browser/{keyboard-owners,directory-keyboard}.mjs.\n'+owners+'\n'+bridge;
const args=process.argv.slice(2),check=args.includes('--check'),targets=args.filter(arg=>arg!=='--check');
if(!targets.length)throw Error('usage: node tools/sync-directory-keyboard.mjs [--check] DIRECTORY-KEYBOARD.mjs ...');
for(const target of targets){
  const path=resolve(target);
  if(check){if((await readFile(path,'utf8')).replaceAll('\r\n','\n')!==source)throw Error('Shared keyboard drift: '+path);}
  else await writeFile(path,source);
  console.log(`${check?'PASS':'SYNC'} directory keyboard: ${path}`);
}
