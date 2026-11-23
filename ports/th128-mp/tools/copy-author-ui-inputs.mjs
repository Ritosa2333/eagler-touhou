// Some cross-game UI dependencies belong to the author Launcher, even in a TH128-only Host.
// Read omitted sparse-checkout files from their existing Git blob; never checkout/write another port.
import {execFileSync} from 'node:child_process';import {readFile,writeFile,mkdir} from 'node:fs/promises';import {dirname,join} from 'node:path';
export async function copyAuthorUiInputs(repo,out){for(const file of ['docs/TOUHOU_PROGRESS.md','common/browser/th09-replay-upload.mjs','ports/th09/web/sdl-runtime/motion-replay.mjs']){
 let bytes;try{bytes=await readFile(join(repo,file));}catch(err){if(err.code!=='ENOENT')throw err;bytes=execFileSync('git',['-C',repo,'show','HEAD:'+file],{windowsHide:true,env:{...process.env,GIT_NO_LAZY_FETCH:'1'}});}
 const dest=join(out,file);await mkdir(dirname(dest),{recursive:true});await writeFile(dest,bytes,{flag:'wx'});
}}
