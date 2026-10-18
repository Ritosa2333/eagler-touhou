import {spawnSync} from 'node:child_process';
import path from 'node:path';
const root=path.resolve(import.meta.dirname,'..');
for(const script of ['build.mjs','build-audio.mjs','package-eagler.mjs']){
  const result=spawnSync(process.execPath,[path.join(import.meta.dirname,script)],{cwd:root,stdio:'inherit',windowsHide:true,env:{...process.env,TH12_THPRAC:process.env.TH12_THPRAC??'1'}});
  if(result.error)throw result.error;if(result.status!==0)process.exit(result.status??1);
}
