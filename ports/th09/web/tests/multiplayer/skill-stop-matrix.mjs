// Run retained baseline and both charge policies through the same stop checks.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..'),label=process.env.RUN_LABEL;
assert.match(label||'',/^[\w-]+$/,'Set a fresh RUN_LABEL');
const builds=Object.fromEntries(['BASELINE','CONFIRMED','LOCAL'].map(key=>{
  const path=process.env[`STOP_${key}_BUILD`];
  assert.ok(path,`Set STOP_${key}_BUILD`);
  assert.ok(existsSync(resolve(path,'th09-presentation.wasm')),`Missing ${key} development build`);
  return [key,resolve(path)];
}));
const out=resolve(root,'artifacts/multiplayer-tests',label);
assert.ok(!existsSync(out),'Retain previous matrix reports');mkdirSync(out,{recursive:true});
const cases=[
  {name:'baseline',build:'BASELINE',env:{EXPECT_REGRESSION:'1'}},
  {name:'confirmed',build:'CONFIRMED'},
  {name:'confirmed-chain',build:'CONFIRMED',env:{FRAMES:'9600',CHAINED_SKILLS:'1'}},
  {name:'local',build:'LOCAL',env:{LOCAL_CHARGE:'1'}},
  {name:'local-chain',build:'LOCAL',env:{LOCAL_CHARGE:'1',FRAMES:'9600',CHAINED_SKILLS:'1'}},
  {name:'local-replay-chain',build:'LOCAL',script:'local-charge-browser.mjs',env:{FRAMES:'9600',CHAINED_SKILLS:'1'}},
  {name:'local-replay-pause',build:'LOCAL',script:'local-charge-browser.mjs',env:{PAUSE_CYCLE:'1'}},
  {name:'local-disabled',build:'LOCAL',script:'local-charge-compatibility.mjs',env:{BASELINE_BUILD:builds.BASELINE}},
];
const results=[];
for(const test of cases){
  const runLabel=`${label}-${test.name}`;
  assert.ok(!existsSync(resolve(out,'..',runLabel)),`Report already exists: ${runLabel}`);
  const env={...process.env,FRAMES:'2400',PACKET_DELAY:'3',PACKET_JITTER:'4',SKILL_WARMUP:'0',
    CHAINED_SKILLS:'0',PAUSE_CYCLE:'0',LOCAL_CHARGE:'0',EXPECT_REGRESSION:'0',
    ...test.env,PC_BUILD:builds[test.build],RUN_LABEL:runLabel};
  console.log(`Running ${test.name}`);
  const code=await new Promise((accept,reject)=>{
    const child=spawn(process.execPath,[resolve(import.meta.dirname,test.script||'skill-stop-browser.mjs')],
      {cwd:root,env,windowsHide:true,stdio:['ignore','pipe','pipe']});
    let output='';for(const stream of [child.stdout,child.stderr])stream.on('data',chunk=>{output+=chunk;});
    child.on('error',reject);child.on('exit',code=>{
      writeFileSync(resolve(out,`${test.name}.log`),output);accept(code);
    });
  });
  const report=resolve(out,'..',runLabel,'report.json');
  results.push({name:test.name,code,report,identity:existsSync(report)?JSON.parse(readFileSync(report,'utf8')).identity:null});
  writeFileSync(resolve(out,'report.json'),JSON.stringify({builds,results},null,2)+'\n');
  assert.equal(code,0,`Failed ${test.name}. See ${resolve(out,`${test.name}.log`)}`);
}
console.log(JSON.stringify({passed:true,cases:results.length,report:resolve(out,'report.json')}));
