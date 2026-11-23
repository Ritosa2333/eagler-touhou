// Offline: node tests/coop-hud-native.integration.mjs <world-input.json> <coop-core.mjs> <coop-core.wasm>
// Uses the caller's original DAT in memory, including its own sprite tables (no PNG fixture or deployment).
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {nativeQuadColors,orderDrawLayers} from '../../th128/site/runtime/draw.mjs';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
import {cooperativeArt} from '../runtime/coop-hud.mjs';
import {bounds,runs,seatGroups,style0,style4} from './coop-hud-readback.mjs';

const [pin,core,wasmPath]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const sha=b=>createHash('sha256').update(b).digest('hex');
const dat=await readFile(config.dat),wasm=await readFile(wasmPath);
assert.equal(sha(dat),config.datSHA256);
const archive=openTh128Dat(dat), paths={...config,bullet9:config.type9,bullet32:config.type32,
  ...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const build={coreSHA256:sha(wasm),resourceSHA256:config.datSHA256};
const resources={build,raw:k=>archive.read(k),read:k=>k==='wasm'?wasm:readFile(paths[k])};
const factory=(await import(pathToFileURL(resolve(core)).href)).default;
const prepared=await prepareA13Preview({resources,factory,onFactory(){},onWorldStart(){},onTick(){}});
const banks=new Map([[5,'front.anm'],[11,'ascii.anm']].map(([id,name])=>{
  const bytes=archive.read(name);return [id,new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength)];
}));
function compareSprite(r) {
  const bank=banks.get(r.bank);assert.ok(bank);assert.equal(bank.getUint32(0,true),7);assert.equal(r.entry,0);
  assert.ok(r.sprite>=0 && r.sprite<bank.getUint16(4,true));
  const at=bank.getUint32(64+r.sprite*4,true),[x,y,w,h]=[4,8,12,16].map(n=>bank.getFloat32(at+n,true));
  const W=bank.getUint16(10,true),H=bank.getUint16(12,true);
  assert.deepEqual([r.width,r.height,r.u0,r.v0,r.u1,r.v1],[w,h,x/W,y/H,(x+w)/W,(y+h)/H],`original bank ${r.bank} sprite ${r.sprite}`);
}
const nativeHead=r=>r.bank===11&&r.id>=12000000&&r.id<12000030;
function verifyHeads(native,art,seat){
  const expected=native.filter(r=>nativeHead(r)&&Math.floor((r.id-12000000)/10)===seat);
  assert.deepEqual(art.filter(nativeHead),expected,`P${seat+1} receives only its own unchanged native gauge`);
  return expected.length;
}
const percent=n=>`${Math.floor(n/100)}%`;
function verify(g) {
  const s=g.state(),native=g.draws(),art=cooperativeArt(native,s,0),ordered=orderDrawLayers(art,{world:true});
  for(const r of native.filter(r=>r.bank===5&&[2,3,4,5,6,7,9,66+s.rank].includes(r.script) ||
    r.drawPass==='original-percent-hud' && [16,358].includes(r.y) || r.drawPass==='player')) assert.ok(art.includes(r));
  assert.ok(art.some(r=>r.bank===5&&r.script===9&&r.y===336));
  assert.ok(art.some(r=>r.bank===11&&r.drawPass==='original-percent-hud'&&r.y===358));
  verifyHeads(native,art,0);
  const groups=seatGroups(native,art,s.seats.length),back=ordered.findIndex(r=>r.id===7000003);
  for(const [i,group] of groups.entries()) {
    const p=s.seats[i],words=[`P${i+1}`],numbers=[];
    if(s.challengeMode){words.push('DEATHS');numbers.push(String(p.deaths));}

    else numbers.push(percent(p.motivation));
    numbers.push(percent(p.icePower));if(!s.challengeMode)numbers.push(percent(p.charge));
    assert.deepEqual(runs(group,style0),words);assert.deepEqual(runs(group,style4),numbers);
    assert.equal(group.filter(r=>r.bank===5&&r.sprite===10).length,s.challengeMode?0:Math.floor(p.charge/10000));
    for(const r of group){compareSprite(r);nativeQuadColors(r);const b=bounds(r);
      assert.ok(b.left>=512&&b.right<=640&&b.top>=36&&b.bottom<=332);assert.ok(ordered.indexOf(r)>back);}
  }
  return groups;
}
for (const players of [2,3]) for (const challengeMode of [false,true]) {
  const g=await prepared.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode,...build});
  try {
    verify(g);
    const starts=g.draws(), state=g.state();
    for(const {seat} of state.seats)assert.ok(verifyHeads(starts,cooperativeArt(starts,state,seat),seat)>0);
    let separateZ=false, separateLife=false;const samples=new Set([188,250,361,500,800]);
    for(let frame=0;frame<1200;frame++){
      const packet={frame,inputs:Array.from({length:players},(_,player)=>({player,held:player===players-1?(frame>=190&&frame<250?1:0):512}))};
      if(frame===250){const checkpoint=g.checkpoint();g.step(packet);const first=verify(g);
        g.restore(checkpoint);g.step(packet);assert.deepEqual(verify(g),first,'replaying the same confirmed row produces the identical HUD');g.release(checkpoint);}
      else g.step(packet);
      const s=g.state(),different=key=>new Set(s.seats.map(p=>p[key])).size>1;
      if(!separateZ&&different('icePower')){verify(g);separateZ=true;}
      if(!separateLife&&different(challengeMode?'deaths':'motivation')){verify(g);separateLife=true;}
      if(samples.has(frame))verify(g);
      if(separateZ&&separateLife&&frame>=800)break;
    }
    assert.ok(separateZ&&separateLife,'exercise differing real seat resources, not just identical start values');
    console.log(JSON.stringify({players,challengeMode,frame:g.state().frame,independentZ:separateZ,independentLifeOrDeaths:separateLife,drawPacketAndAtlas:true,rollbackReadback:true}));
  } finally {g.close();}
}
