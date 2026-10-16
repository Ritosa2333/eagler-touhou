import {readFile,writeFile} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
const root=fileURLToPath(new URL('../../',import.meta.url));
const source=await readFile(new URL('../browser/browser-gamepad-input.hpp',import.meta.url),'utf8');
const check=process.argv.includes('--check');
for(const game of ['th06','th07','th08','th09','th10','th11','th15','th20']){
  const path=resolve(root,`${game}-eagler`,game==='th06'||game==='th07'?'src':`${game}_web/cpp/sdl`,'browser-gamepad-input.hpp');
  if(check){if(await readFile(path,'utf8')!==source)throw Error(`${game} gamepad boundary is stale`);}
  else await writeFile(path,source);
}
console.log(`Shared gamepad input ${check?'check':'sync'}: PASS (8 adapters)`);
