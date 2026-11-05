import {spawn} from 'node:child_process';
import {readFileSync,createWriteStream,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
const repo=fileURLToPath(new URL('../../',import.meta.url));
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const corpus=JSON.parse(readFileSync(new URL('./corpus.json',import.meta.url))),id=option('--case'),lane=option('--lane')||'all';
const ids=id?[id]:lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];if(!ids)throw Error('Unknown suite');
const out=resolve(option('--output')||resolve(repo,'artifacts/replay-verifier/oracle'));mkdirSync(out,{recursive:true});
async function run(script,args,logName){
 const log=createWriteStream(resolve(out,logName+'.log')),child=spawn(process.execPath,[resolve(repo,'tools/replay-verifier',script),...args],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe']});
 child.stdout.pipe(log,{end:false});child.stderr.pipe(log,{end:false});
 const code=await new Promise((accept,reject)=>{child.on('error',reject);child.on('exit',accept);});await new Promise(accept=>log.end(accept));
 if(code!==0)throw Error('Provider failed. See '+resolve(out,logName+'.log'));
}
for(const name of ids){
 if(!corpus.cases.some(c=>c.id===name))throw Error('Unknown case '+name);
 console.log('Original:',name);await run('capture-original.mjs',['--case',name,'--output',out],name+'.original');
 console.log('Candidate:',name);await run('capture-candidate.mjs',['--case',name,'--output',out],name+'.candidate');
}
