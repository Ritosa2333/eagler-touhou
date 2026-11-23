// Native draw regression: no substitute assets, synthetic damage or debug writes.
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {prepareA13Preview} from '../runtime/preview-world.mjs';
import {cooperativeArt} from '../runtime/coop-hud.mjs';
const [pin,core,wasmPath,only]=process.argv.slice(2),config=JSON.parse(await readFile(pin,'utf8'));
const hash=b=>createHash('sha256').update(b).digest('hex'),read=async p=>new Uint8Array(await readFile(p));
const dat=await read(config.dat),wasm=await read(wasmPath);assert.equal(hash(dat),config.datSHA256);
const paths={...config,bullet9:config.type9,bullet32:config.type32,...Object.fromEntries(Object.entries(config.bullets).map(([k,v])=>['bullet'+k,v]))};
const factory=(await import(pathToFileURL(resolve(core)))).default,build={coreSHA256:hash(wasm),resourceSHA256:hash(dat)};
const prepared=await prepareA13Preview({resources:{build,read:k=>k==='dat'?dat:k==='wasm'?wasm:read(paths[k])},factory,onFactory(){},onWorldStart(){},onTick(){}});
const heads=(draws,i)=>draws.filter(r=>r.id>=12000000+i*10&&r.id<12000010+i*10);
const headText=rs=>rs.map(r=>r.sprite===269?'%':String.fromCharCode(r.sprite-210)).join('');
const alpha=record=>record?.color>>>24;
async function run(players,which,challenge=false){
 const game=await prepared.openCoopStage({players,stageId:3,difficulty:1,mode:'same-stage-coop',seed:34772,rollbackWindow:2,challengeMode:challenge,...build});let f=0;
 const advance=(fn=()=>0)=>{game.step({frame:f,inputs:Array.from({length:players},(_,player)=>({player,held:fn(player,f)}))});f++;return game.state();};
 try{if(which==='head'){
  const trigger=players-1; let diverged=false;
  for(let j=0;j<300;j++){const st=advance((i,t)=>i!==trigger?0:t<22?128:t>=80&&t<148?1:0);if(![21,100,147,150,170,220,250].includes(j))continue;
   const ds=game.draws();for(const s of st.seats){const rows=heads(ds,s.seat),word=`${Math.trunc(s.icePower/100)}%`;assert.equal(headText(rows),word,`P${s.seat+1} head frame ${f}`);
    const scale=rows[0].sx;assert.equal(rows[0].bank,11);assert.ok(Math.abs(rows[0].x-(320+s.rawX/128-word.length*9*scale/2))<1.01);assert.ok(Math.abs(rows[0].y-(s.rawY/128-8-10*scale))<1.01);}
   if(st.seats[trigger].icePower!==st.seats[0].icePower)diverged=true;
  }assert.ok(diverged,'a real Z release must distinguish that seat');
 }else if(which==='focus'){
  // Detect the authored records by ANM, not the new per-seat ID encoding.
  const marker=r=>r.bank===0&&r.sprite===444&&r.layer===16&&(r.script===79||r.script===80);
  const all=()=>game.draws().filter(marker);
  assert.deepEqual(all(),[]);
  const sleep=(frames,keys)=>{let s;for(let j=0;j<frames;j++)s=advance(keys);return s;};
  for(let i=players-1;i>=0;--i){
   const s=sleep(12,seat=>seat===i?8:0),native=game.draws(),marks=native.filter(marker),p=s.seats[i];
   assert.deepEqual(marks.map(r=>r.script).sort(),[79,80],`P${i+1} can activate bullet80/79 without P1`);
   assert.ok(marks.every(r=>r.x===320+p.rawX/128&&r.y===16+p.rawY/128),`P${i+1} marker follows its native body`);
   for(const {seat} of s.seats)assert.deepEqual(cooperativeArt(native,s,seat).filter(marker),seat===i?marks:[]);
   sleep(12,()=>0);assert.deepEqual(all(),[],`P${i+1} release finishes the native animation`);
  }
  sleep(12,()=>8);
  const meet=seat=>8|(seat===0?128:seat===players-1?64:0);
  const s=sleep((players-1)*8,meet),native=game.draws(),marks=native.filter(marker);
  assert.equal(new Set(s.seats.map(p=>`${p.rawX},${p.rawY}`)).size,1,'actual player centers touch');
  assert.equal(new Set(marks.map(r=>r.id)).size,2*players,'native focus IDs remain unique with everybody focused');
  const displayed=new Set();
  for(const {seat} of s.seats){
   const art=cooperativeArt(native,s,seat), own=art.filter(marker);
   assert.equal(own.length,2,`P${seat+1} sees exactly one marker`);
   assert.ok(own.every(r=>!displayed.has(r.id)),'no teammate marker appears in this seat');
   own.forEach(r=>displayed.add(r.id));
   assert.deepEqual(s.seats.flatMap(p=>heads(art,p.seat)),heads(native,seat));
  }
  const markState=()=>JSON.stringify(game.draws().filter(marker)), checkpoint=game.checkpoint();
  sleep(12,()=>0);assert.deepEqual(all(),[]);game.restore(checkpoint);f-=12;
  const original=JSON.stringify(marks);assert.equal(markState(),original,'rollback restores all visible focus animations');
  sleep(12,()=>0);assert.deepEqual(all(),[]);game.release(checkpoint);
 }else if(which==='area'){
  for(let j=0;j<490;j++){const st=advance((i,t)=>i===players-1?(t<32?16:t>=190&&t<250?1:0):0);if(!st.teamPixels)continue;
   assert.ok(Math.abs(st.firepower-st.teamPixels*(players===2?80:75)/172032)<0.0001);
   const area=game.draws().filter(r=>r.drawPass==='original-percent-hud'&&r.y===358).map(r=>r.sprite===255?'%':String(r.sprite-243)).join('');assert.equal(area,`${Math.trunc(st.firepower)}%`,'authored frozen area must be the actual team shot-level value');
   console.log({players,frame:f,raw:st.teamPixels,area});return;
  }assert.fail('no freeze was settled');
 }else{
  const observed=new Set(),lastPositions=new Map(),drifted=new Set(),deathAt=new Map(),ordinary=new Set();let isolated=false;
  for(let j=0;j<12000;j++){const st=advance();for(const s of st.seats){if(s.retired&&!deathAt.has(s.seat))deathAt.set(s.seat,f);if(s.state===1&&s.respawns)ordinary.add(s.seat);}
   if(challenge){if(ordinary.size<players)continue;for(const s of st.seats){assert.equal(s.ghost,false);assert.ok(s.motivation>=0);}return;}
   if(!deathAt.size||f%5)continue;const ds=game.draws();for(const s of st.seats){const age=f-(deathAt.get(s.seat)??Infinity);if(age<31)continue;
    const body=ds.find(r=>r.id===4000000+10*s.seat);assert.ok(body,`missing ghost P${s.seat+1}, frame ${f}`);assert.equal(alpha(body),128,`P${s.seat+1} ghost body must be half opaque; frame ${f}`);assert.equal(body.bank,7);assert.equal(body.drawPass,'player');assert.equal(body.blend,0);assert.equal(s.ghost,true);
    assert.equal(heads(ds,s.seat).length,0);assert.ok(!ds.some(r=>r.id===4000001+10*s.seat),'no opaque outline around a ghost');
    const pos=[body.x,body.y].join(',');if(lastPositions.has(s.seat)&&lastPositions.get(s.seat)!==pos)drifted.add(s.seat);lastPositions.set(s.seat,pos);observed.add(s.seat);
   }if(st.seats.some(s=>s.ghost)&&st.seats.some(s=>!s.ghost&&s.state===1)){const live=st.seats.find(s=>!s.ghost&&s.state===1);if(alpha(ds.find(r=>r.id===4000000+10*live.seat))===255)isolated=true;}
   if(drifted.size===players){assert.ok(isolated,'ghost did not affect a living ally');console.log({players,ordinary:[...ordinary],ghosts:[...observed],drifted:[...drifted],frame:f});return;}
  }assert.fail('real deaths/drift not observed for every seat');
 }console.log({players,which,challenge,frame:f});}finally{game.close();}
}
for(const n of [2,3])for(const k of ['head','focus','area','ghost','challenge'])if(!only||k===only)await run(n,k,k==='challenge');
