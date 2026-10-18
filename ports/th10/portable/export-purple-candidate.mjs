// Export review evidence without staging, committing or including game DATA.
import {execFileSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const root=resolve(import.meta.dirname,'..'),out=resolve(root,'th10_web/artifacts/purple-check');
mkdirSync(out,{recursive:true});
const git=args=>execFileSync('git',args,{cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:20*1024*1024});
let patch=git(['diff','--no-ext-diff','--binary','HEAD','--','portable','th10_web/cpp','th10_web/sdl-runtime']);
for(const file of git(['ls-files','--others','--exclude-standard','-z','--','portable','th10_web/cpp','th10_web/sdl-runtime']).split('\0').filter(Boolean)){
 try{git(['diff','--no-index','--no-ext-diff','--binary','--','NUL',file]);}
 catch(error){if(error.status!==1)throw error;patch+=String(error.stdout).replaceAll('a/NUL','a/'+file);}
}
writeFileSync(resolve(out,'candidate.patch'),patch);
const sha256=createHash('sha256').update(readFileSync(resolve(out,'candidate.patch'))).digest('hex');
const record={base:git(['rev-parse','HEAD']).trim(),branch:git(['branch','--show-current']).trim(),sha256,bytes:Buffer.byteLength(patch),status:'reviewed-source-candidate-not-deployed'};
writeFileSync(resolve(out,'candidate.json'),JSON.stringify(record,null,2)+'\n');console.log(JSON.stringify(record));
