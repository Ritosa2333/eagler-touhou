import {spawn} from 'node:child_process';
import {createWriteStream,readFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const repo=fileURLToPath(new URL('../../',import.meta.url)),corpus=JSON.parse(readFileSync(new URL('./corpus.json',import.meta.url)));
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const id=option('--case'),lane=option('--lane')||'all',out=resolve(option('--output')||resolve(repo,'artifacts/replay-verifier/oracle'));
const ids=id?[id]:lane==='all'?Object.values(corpus.suites).flat():corpus.suites[lane];if(!ids)throw Error('Unknown suite');
mkdirSync(out,{recursive:true});
const sha=b=>createHash('sha256').update(b).digest('hex'),core=resolve(repo,'th11_web/artifacts/cpp/game-core-test.wasm'),coreSha256=sha(readFileSync(core));
for(const name of ids){
  const fixture=corpus.cases.find(c=>c.id===name);if(!fixture)throw Error('Unknown case '+name);
  const replay=resolve(repo,fixture.source);if(fixture.replaySha256&&sha(readFileSync(replay))!==fixture.replaySha256)throw Error('Replay identity mismatch');
  const env={...process.env,TH11_WORLD_REPLAYS:String(fixture.demo||0),TH11_ORACLE_COUNT_INSTRUCTIONS:'0',TH11_WORLD_DRAW_CLOCKS:'1',TH11_VERIFIER_CASE:name,TH11_VERIFIER_CAPTURE:out,TH11_CORE_TEST_FILE:core};
  for(const key of ['TH11_WORLD_LIMIT','TH11_WORLD_RESUME','TH11_WORLD_NATIVE_RESUME','TH11_WORLD_CHECKPOINT','TH11_WORLD_FILE','TH11_WORLD_STAGE','TH11_WORLD_CAMPAIGN'])delete env[key];
  if(fixture.kind!=='demo')Object.assign(env,{TH11_WORLD_FILE:replay,TH11_WORLD_STAGE:String(fixture.kind==='extra'?7:1),TH11_WORLD_CAMPAIGN:'1'});
  console.log('Original/Candidate:',name);const log=createWriteStream(resolve(out,name+'.log'));
  const child=spawn(process.execPath,['--test','th11_web/tests/cpp/world-replays.test.mjs'],{cwd:repo,env,windowsHide:true,stdio:['ignore','pipe','pipe']});
  child.stdout.pipe(log,{end:false});child.stderr.pipe(log,{end:false});
  const code=await new Promise((accept,reject)=>{child.on('error',reject);child.on('exit',accept);});await new Promise(accept=>log.end(accept));
  if(sha(readFileSync(core))!==coreSha256)throw Error('Core changed during oracle run');
  if(code!==0)throw Error(name+' failed. See '+resolve(out,name+'.log'));
}
