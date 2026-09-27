#!/usr/bin/env node
import {spawnSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {dirname,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

const here=dirname(fileURLToPath(import.meta.url)),repo=resolve(here,'../..'),web=resolve(repo,'th09_web');
const caseIndex=process.argv.indexOf('--case'),selected=caseIndex<0?null:Number(process.argv[caseIndex+1]);
if(selected!==null&&![0,1,2].includes(selected))throw Error('--case must be 0, 1, or 2');
const env={...process.env,TH09_REPLAY_VERIFIER_LIGHT:'1',...(selected===null?{}:{TH09_DEMO_CASE:String(selected)})};
for(const test of ['tests/cpp/replay-session.test.mjs','tests/cpp/session-replays.test.mjs']){
 const run=spawnSync(process.execPath,['--test',test],{cwd:web,env,stdio:'inherit',windowsHide:true});
 if(run.error)throw run.error;if(run.status!==0)process.exit(run.status??1);
}
const report=JSON.parse(readFileSync(resolve(web,'artifacts/cpp/verification/session-replays.json'),'utf8'));
const verifier=report.verifier;
const expectedCount=selected===null?3:1;
if(!verifier?.passed||!Array.isArray(verifier.results)||verifier.results.length!==expectedCount)throw Error('TH09 common Replay verifier report is incomplete');
const out=resolve(web,'artifacts/replay-verifier');mkdirSync(out,{recursive:true});
for(const result of verifier.results)writeFileSync(resolve(out,`${result.id}.json`),`${JSON.stringify(result,null,2)}\n`);
for(const result of verifier.results)process.stdout.write(`${result.id}: ${result.status} (${result.comparedTicks} ticks)\n`);
process.stdout.write(`TH09 Replay verifier${selected===null?'':` demo${selected}`}: PASS (${verifier.results.reduce((sum,result)=>sum+result.comparedTicks,0)} fixed ticks)\n`);
