import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,readdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand} from './production-driver.mjs';
import {nativeReplay} from '../web/replay.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(a=>{const at=a.indexOf('=');return [a.slice(2,at),a.slice(at+1)];}));
if(!args.replays||!args.output)throw Error('需要 --replays 和 --output');
const output=resolve(args.output);mkdirSync(output,{recursive:true});
const baseline=args.baseline?JSON.parse(readFileSync(resolve(args.baseline),'utf8')):null;
const report={schema:'thkkd/production-replay-regression/1',checkpointInterval:1024,cases:[]};
const requested=args.cases?new Set(args.cases.split(',')):null;
const names=readdirSync(args.replays).filter(n=>/^corpus-\d+\.rpy$/.test(n)&&(!requested||requested.has(n.slice(0,-4)))).sort();
assert.equal(names.length,requested?.size??17,'选定的每份录像均存在');
const d=await productionBrowser({url:args.url,report,protocolTimeout:1200000});
let frame;
try{
 await d.library();frame=await d.start();
 await frame.evaluate(()=>{
  const r=__thkkdRuntime,w=address=>r.core._thkkd_word(address),step=r.core._thkkd_step;
  function snapshot(){
   let hash=2166136261,second=5381,bullets=0,enemies=0;
   const word=value=>{for(let i=0;i<4;i++){const byte=(value>>>(i*8))&255;hash=Math.imul(hash^byte,16777619)>>>0;second=(Math.imul(second,33)^byte)>>>0;}};
   for(const address of [0xa74c08,0xb3e2ec,0xb3e2e8,0xa77390,0xa733c0,0xa78e2c,0xa73060])word(w(address));
   for(let i=0;i<60;i++)word(w(0xa8cd68+i*4));
   // 记录游戏字段，平台资源句柄不参与摘要。
   for(let i=0;i<80;i++){const base=0xa87fe8+i*248;if(!w(base))continue;enemies++;word(i);for(const offset of [0,4,8,12,20,36,56,60,64,68,72,76,80,84])word(w(base+offset));}
   for(let i=0;i<3000;i++){const base=0xa8e660+i*240;if(!w(base))continue;bullets++;word(i);for(const offset of [0,4,8,12,16,64,68,72,76,80,84,88,92])word(w(base+offset));}
   for(const [base,count,stride]of [[0xa87be0,20,24],[0xa79090,80,56]])for(let i=0;i<count;i++)for(let j=0;j<stride;j+=4)word(w(base+i*stride+j));
   return {cursor:w(0xb43110),stage:w(0xb3e2ec),frame:w(0xb3e2e8),part:w(0xa77390),phase:w(0xa733c0),scene:w(0xa78e2c),random:w(0xa74c08),bullets,enemies,digest:hash.toString(16).padStart(8,'0')+second.toString(16).padStart(8,'0')};
  }
  window.__replayRegression={target:0,checkpoints:[],last:-1,enabled:false,snapshot};
  r.core._thkkd_step=(...a)=>{const result=step(...a),test=__replayRegression,cursor=w(0xb43110);if(test.enabled&&cursor!==test.last&&(cursor%1024===0||cursor>=test.target)){test.checkpoints.push(snapshot());test.last=cursor;}return result;};
 });
 for(const name of names){
  const bytes=readFileSync(resolve(args.replays,name)),parsed=nativeReplay(bytes),target=parsed.count-1,id=name.slice(0,-4),began=Date.now();
  console.log(JSON.stringify({id,status:'starting',frames:parsed.count}));
  await frame.evaluate(target=>Object.assign(__replayRegression,{enabled:false,target,checkpoints:[],last:-1}),target);
  await runtimeCommand(d.page,'write',{path:'replay/regression.rpy',bytes:[...bytes]});
  await runtimeCommand(d.page,'playback',{action:'open',path:'replay/regression.rpy'});
  await runtimeCommand(d.page,'playback',{action:'pause'});
  await frame.evaluate(()=>{__replayRegression.enabled=true;});
  await runtimeCommand(d.page,'playback',{action:'seek',value:target});
  await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)>=target||document.querySelector('#error').textContent,{timeout:1200000,polling:1000},target);
  const result=await frame.evaluate(()=>({checkpoints:__replayRegression.checkpoints,final:__replayRegression.snapshot(),error:document.querySelector('#error').textContent}));
  assert.equal(result.error,'');assert.equal(result.final.cursor,target);
  const row={id,inputSha256:createHash('sha256').update(bytes).digest('hex'),inputFrames:parsed.count,seconds:(Date.now()-began)/1000,...result};
  if(baseline){const expected=baseline.cases.find(c=>c.id===id);assert.ok(expected,'基线中存在 '+id);assert.equal(row.inputSha256,expected.inputSha256);assert.deepEqual(row.checkpoints,expected.checkpoints,id+' 首个状态检查点差异');assert.deepEqual(row.final,expected.final,id+' 结束状态差异');row.result='passed';}
  else row.result='captured';
  report.cases.push(row);writeFileSync(resolve(output,'progress.json'),JSON.stringify(report,null,2));
  console.log(JSON.stringify({id,result:row.result,frames:row.inputFrames,checkpoints:row.checkpoints.length,seconds:row.seconds,final:row.final}));
 }
 assert.equal(report.cases.length,names.length);assert.equal(report.errors.length,0);report.result=baseline?'passed':'captured';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{await d.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await d.browser.close();}
