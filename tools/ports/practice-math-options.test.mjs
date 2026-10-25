import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {TH08_PRACTICE_OPTIONS as options} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {assertPracticeMathOptions} from './practice-math-options.mjs';

test('Math options compare actual seeds, vector outputs, table bytes and native identities',async()=>{
  const c=options.math,reference=await c.loadReference(process.cwd()),original=Buffer.alloc(c.tableBytes),imported=Buffer.alloc(c.tableBytes);
  for(let i=0;i<original.length;i+=4){original.writeUInt32LE(0x3f800000,i);imported.writeUInt32LE(0x3f800001,i);}
  const files={original,imported,exported:imported},e={schema:'eagler-touhou/practice-math/1',
    arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},trials:[],cleanupErrors:[],reference:reference.source,
    files:Object.fromEntries(Object.entries(files).map(([key,b])=>[key,{bytes:b.length,sha256:createHash('sha256').update(b).digest('hex')}]))};
  for(const mode of c.modes){const trial={id:mode.id,samples:[]};e.trials.push(trial);
    for(let i=0;i<8;i++){
      const w=Array(1024).fill(0),f=c.fields;w.splice(0,8,1,1,i+1,i+1,0,1,0,0);
      w[f.option]=1;w[f.selected]=mode.index;w[f.sequence]=100+i;w[f.enabled]=1;w[f.cpu]=mode.code;
      w[f.square]=c.bits(25);w[f.seed]=reference.seed(w[f.square],mode.index,mode.id==='custom-imported'?imported:original);
      w.splice(f.input,3,...[3,4,0].map(c.bits));const r=Math.fround,s=c.float(w[f.seed]),scale=r(r(.5*s)*r(3-r(r(25*s)*s)));
      w[f.scale]=c.bits(scale);w.splice(f.output,3,...[3,4,0].map(n=>c.bits(r(n*scale))));
      trial.samples.push({raw:{available:true,words:w},identity:{app:1000,documentTimeOrigin:123,url:'https://localhost/runtime'},
        scene:{available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
          active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
          writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
          timelineCount:1,withheldTimelineUpdates:100+i,enemies:0,bullets:0,lasers:0,stage:0,enemyFrame:100+i,controlFrame:100+i,requestGeneration:1}});
    }
  }
  assert.equal(assertPracticeMathOptions(e,options,{reference,...files}).actualNormalization,true);
  for(const mutate of [e=>e.trials[0].samples[0].raw.words[68]++,e=>e.trials[1].samples[1].raw.words[72]++,
    e=>e.trials[2].samples[1].raw.words[64]=100,e=>e.trials[3].samples[0].identity.app++,e=>e.trials.pop()]){
    const bad=structuredClone(e);mutate(bad);assert.throws(()=>assertPracticeMathOptions(bad,options,{reference,...files}));
  }
  assert.throws(()=>assertPracticeMathOptions(e,options,{reference,...files,exported:original}));
});
