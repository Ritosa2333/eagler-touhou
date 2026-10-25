import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {TH08_PRACTICE_OPTIONS as options} from '../../tests/adaptation/adapters/titles/th08-practice-options.mjs';
import {assertPracticeDisplay} from './practice-display-options.mjs';
const fnv=text=>{let n=2166136261;for(const b of Buffer.from(text))n=Math.imul(n^b,16777619)>>>0;return n;};

test('Display options require live projectiles, real geometry, visible license bytes and bound images',async()=>{
  const c=options.display,f=c.fields,license=await c.loadLicense(process.cwd()),identity={app:1,documentTimeOrigin:2,url:'https://localhost/runtime'};
  const raw=()=>{const words=Array(1024).fill(0);words.splice(0,8,1,1,10,10,0,1,0,0);words[f.generation]=10;return{available:true,words};};
  const e={schema:'eagler-touhou/practice-display-options/1',enabled:true,
    arm:{available:true,armAccepted:true,armStatus:1,requestPending:true,requestGeneration:1},hit:{accepted:true},cleanupErrors:[],samples:[],images:[]};
  for(let i=0;i<6;i++){const row={raw:raw(),identity,projectile:{available:true,complete:true,request:{status:1},projectile:{liveCount:1}},
    scene:{available:true,accepted:true,acceptedBeforeFirstUpdate:true,firstTimelineWithheld:true,updateStarted:true,sessionWriteLocked:true,
      active:true,ready:true,replay:false,assisted:true,writePolicy:{initialization:'ram-score-overlay'},
      writeReceipt:{locked:true,loadScope:false,attempts:1,ramOverrides:1,discardedBytes:64,committedBytes:0,rejectedWrites:0},
      timelineCount:1,withheldTimelineUpdates:100+i,enemies:0,bullets:1,lasers:0,stage:0,enemyFrame:100+i,controlFrame:100+i,requestGeneration:1}};
    const w=row.raw.words;w[f.option]=w[f.bullets]=w[f.age]=1;w[f.vertices]=12;w[f.x]=192;w[f.y]=384;w[f.hitX]=w[f.hitY]=8;w[f.playerWidth]=4;w.splice(f.left,4,218,394,230,406);e.samples.push(row);}
  e.license={before:{raw:raw(),identity},hidden:{raw:raw(),identity},shown:{raw:raw(),identity},source:license.source};
  const w=e.license.shown.raw.words;w[f.licenseFlag]=1;w[f.aboutLines]=4;w[f.licenseBytes]=Buffer.byteLength(license.text);w[f.licenseHash]=fnv(license.text);w[f.licenseVertices]=100;
  const png=Buffer.alloc(1200);png.set([137,80,78,71,13,10,26,10]);const sha256=createHash('sha256').update(png).digest('hex');
  e.images=[{path:'one',bytes:png.length,sha256,identity},{path:'two',bytes:png.length,sha256,identity}];const inputs={license,images:[png,png]};
  assert.equal(assertPracticeDisplay(e,options,inputs).license,true);
  for(const mutate of [e=>e.samples[0].raw.words[f.left]++,e=>e.samples[1].raw.words[f.vertices]=0,e=>e.samples[2].projectile.projectile.liveCount=0,
    e=>e.license.shown.raw.words[f.licenseHash]++,e=>e.license.shown.raw.words[f.licenseVertices]=0,e=>e.license.hidden.raw.words[f.licenseFlag]=1,e=>e.images[0].sha256='0'.repeat(64)]){
    const bad=structuredClone(e);mutate(bad);assert.throws(()=>assertPracticeDisplay(bad,options,inputs));}
  const disabled=structuredClone(e);disabled.enabled=false;for(const row of disabled.samples){row.raw.words[f.option]=0;row.raw.words[f.bullets]=0;row.raw.words[f.vertices]=0;}
  assert.equal(assertPracticeDisplay(disabled,options,inputs).license,false);disabled.samples[0].raw.words[f.bullets]=1;assert.throws(()=>assertPracticeDisplay(disabled,options,inputs));
});
