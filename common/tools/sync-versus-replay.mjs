import {readFileSync,writeFileSync} from 'node:fs';

// The packaged module keeps its title's sibling imports and native ABI.
// The port copy also supports serving the original development shell.
const source=new URL('../browser/versus-replay-th09.mjs',import.meta.url);
const target=new URL('../../ports/th09/web/sdl-runtime/replay-player.mjs',import.meta.url);
const bytes=readFileSync(source);
if(!bytes.equals(readFileSync(target))){
  if(process.argv.includes('--check'))throw Error('Stale th09 Replay development copy');
  writeFileSync(target,bytes);
}
console.log('CHECK th09 versus Replay | shared source and development copy match');
