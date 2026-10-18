import {spawn} from 'node:child_process';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {sections} from '../th10_web/sdl-runtime/practice-sections.mjs';
const root=resolve(import.meta.dirname,'..'),out=resolve(root,'th10_web/artifacts/purple-check');
const cases=sections.map(s=>s.id);[5,5,7,8,6,4,6].forEach((count,stage)=>{for(let part=1;part<=count;part++)cases.push(10000+(stage+1)*100+part);});
const results=[];
async function worker(){while(cases.length){const id=cases.shift();const result=await new Promise(done=>{const p=spawn(process.execPath,['portable/check-purple-browser.mjs'],{cwd:root,env:{...process.env,TH10_PRACTICE_SECTION:String(id)},windowsHide:true});let stdout='',stderr='';p.stdout.on('data',d=>stdout+=d);p.stderr.on('data',d=>stderr+=d);p.on('error',e=>done({id,passed:false,error:String(e)}));p.on('exit',code=>done({id,passed:code===0,stdout,stderr}));});results.push(result);console.log(JSON.stringify({id,passed:result.passed,remaining:cases.length}));}}
await Promise.all(Array.from({length:3},worker));mkdirSync(out,{recursive:true});writeFileSync(resolve(out,'matrix.json'),JSON.stringify({passed:results.every(r=>r.passed),count:results.length,results},null,2)+'\n');
if(results.some(r=>!r.passed))throw Error('Retail practice cases failed: '+results.filter(r=>!r.passed).map(r=>r.id));
