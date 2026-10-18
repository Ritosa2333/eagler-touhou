import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
const option=name=>{const i=process.argv.indexOf(name);return i<0?null:process.argv[i+1];};
const root=option('--capture-root'),report=option('--report');if(!root||!report)throw Error('Use --capture-root original-directory --report report.json');
const path=resolve('th15_web/artifacts/native-process-replay/native-replay-frames.bin'),raw=readFileSync(path),sha=b=>createHash('sha256').update(b).digest('hex');
assert.equal(sha(raw),'f1a6b014632a699fd016111612915e88781e975029ed54844103423c2d4d396c','Pinned independent original-process capture');
const stages=new Map();let owner=1,clock=0,last=null,frozenRows=0,loadingRows=0;
for(let at=0;at<raw.length;at+=64){
  const words=Array.from({length:16},(_,i)=>raw.readUInt32LE(at+i*4)),label=words[0],frame=words[1],state=words.slice(1,14);
  if((words[15]&0x800)||frame===0){loadingRows++;if(label!==owner){owner=label;clock=0;last=null;}continue;}
  // Retail increments the requested stage inside the terminal old-stage tick.
  // The clock resets only when the incoming stage actually owns calculation.
  assert.ok(label===owner||(label===owner+1&&frame===clock+1),'Observed stage ownership');
  if(frame===clock){assert.deepEqual(state,last,'Frozen observation cannot change declared state');frozenRows++;continue;}
  assert.equal(frame,clock+1,'No missing active original-process frame');
  if(!stages.has(owner))stages.set(owner,[]);stages.get(owner).push(state);clock=frame;last=state;
}
const results=[];
for(const [stage,expected] of stages){
  const source=resolve(root,`marisa-lunatic-stage${stage}.rows.jsonl`);
  try{
    const actual=readFileSync(source,'utf8').trim().split('\n').map(JSON.parse);let compared=0,firstDifference=null;
    for(let i=0;i<Math.min(expected.length,actual.length);i++){
      const a=actual[i].words.slice(0,13),b=expected[i];
      if(JSON.stringify(a)!==JSON.stringify(b)){firstDifference={stage,clock:b[0],expected:b,actual:a};break;}compared++;
    }
    results.push({stage,comparedTicks:compared,referenceTicks:expected.length,observedTicks:actual.length,firstDifference});
  }catch(error){results.push({stage,error:String(error)});}
}
const result={schema:'th15/retail-backend-cross-check/v1',status:results.some(r=>r.firstDifference)?'DIVERGED':results.every(r=>!r.error&&r.comparedTicks===r.referenceTicks)?'REFERENCE-MATCH':'INCOMPLETE',
  referenceSha256:sha(raw),fields:13,loadingRows,frozenRows,results,
  scope:'Independent original-process state versus the original x86 provider. Stage labels are normalized by observed clock ownership. Reference stops before the final clear tick, so this is backend support evidence, not a complete oracle/golden gate.'};
writeFileSync(report,JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result,null,2));if(result.status!=='REFERENCE-MATCH')process.exitCode=1;
