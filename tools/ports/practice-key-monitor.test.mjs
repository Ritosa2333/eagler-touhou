import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {TH08_PRACTICE_OPTIONS as options} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {assertPracticeKeyMonitor} from './practice-key-monitor.mjs';

function fixture(){
  const c=options.keyMonitor,f=c.fields,e={schema:'eagler-touhou/practice-key-monitor/1',arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},cleanupErrors:[],rows:[],pulses:[]};
  const identity={app:123,timeOrigin:45,url:'https://localhost/runtime'};let csv=c.header;
  for(let i=0;i<110;i++){
    const input=i>=5&&i<=10?1:i>=85&&i<=90?132:i>=95&&i<=98?256:0;
    const mask=input===1?16:input===132?520:input===256?256:0;
    const aps=i<5?0:i<11?1:i<65?2:i<71?1:i<85?0:i<91?2:i<95?4:i<99?5:6;
    const words=Array(1024).fill(0);words.splice(0,8,1,1,i+1000,i+1000,0,1,0,0);words[16]=1;words[17]=words[18]=i+1000;words[19]=input;
    words[f.recording]=1;words[f.count]=words[f.apsCount]=i+1;words[f.keys]=mask;words[f.aps]=aps;
    const scene={available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
      active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
      writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
      timelineCount:1,withheldTimelineUpdates:i+1000,enemies:0,bullets:0,lasers:0,stage:0,enemyFrame:i+1000,controlFrame:i+1000,requestGeneration:1};
    e.rows.push({raw:{available:true,words},identity,scene});
    csv+=[i+1,aps,...Array.from({length:10},(_,bit)=>mask&(1<<bit)?'O':'-')].join(',')+'\n';
  }
  e.before=structuredClone(e.rows[0]);e.before.raw.words[f.recording]=e.before.raw.words[f.count]=0;
  e.stopped=structuredClone(e.rows.at(-1));e.stopped.raw.words[f.recording]=0;
  e.pulses=c.pulses.map((p,i)=>({keys:p.keys,samples:[[1009,1010],[1089,1090],[1097,1098]][i],released:[1080,1094,1109][i]}));
  const bytes=Buffer.from(csv);e.csv={bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};return{e,bytes};
}
test('Native keyboard record, 60-tick APS rollover and the actual downloaded CSV are independently compared',()=>{
  const {e,bytes}=fixture();assert.equal(assertPracticeKeyMonitor(e,options,{csvBytes:bytes}).rows,110);
  for(const change of [e=>e.rows[65].raw.words[42]++,e=>e.rows[71].raw.words[42]=1,e=>e.rows[9].raw.words[41]=0,
    e=>e.rows[3].raw.words[39]++,e=>e.rows[4].identity={...e.rows[4].identity,app:124},e=>e.stopped.raw.words[38]=1,
    e=>e.rows[9].raw.words[19]=0,e=>e.pulses[0].samples=[1001,1002],e=>e.csv.sha256='0'.repeat(64)]){
    const bad=structuredClone(e);change(bad);assert.throws(()=>assertPracticeKeyMonitor(bad,options,{csvBytes:bytes}));
  }
  assert.throws(()=>assertPracticeKeyMonitor(e,options,{csvBytes:Buffer.from('made up CSV')}));
});
