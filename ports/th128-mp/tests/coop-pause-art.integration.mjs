// Offline: node tests/coop-pause-art.integration.mjs <author DAT/zh package.zip> <managed runtime directory>
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {join,resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
import {readZipDirectory,readEntryData} from '../../th128/site/runtime/zip-reader.mjs';
import {openTh128Dat} from '../../th128/site/runtime/th128-dat.mjs';
import {openManagedPauseArt} from '../runtime/coop-pause-art.mjs';
import {openCoopReplaySave} from '../runtime/coop-replay-save.mjs';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';

const [pkg,root]=process.argv.slice(2),digest=b=>createHash('sha256').update(b).digest('hex');
const entries=bytes=>new Map(readZipDirectory(bytes).entries.map(e=>[e.name,e]));
const zip=entries(await readFile(pkg)),get=name=>readEntryData(zip.get(name));
const descriptor=JSON.parse(await get('package.json'));
async function asset(id){const pin=descriptor.files[id],b=await get(pin.source);assert.equal(digest(b),pin.sha256);return b;}
const zh=entries(await asset('language:lang_zh-hans')),dat=openTh128Dat(await asset('game-data'));
const {files}=JSON.parse(await readFile(join(root,'runtime/program.json')));
async function program(pin){const b=await readFile(join(root,pin.path));assert.equal(b.length,pin.bytes);assert.equal(digest(b),pin.sha256);return b;}
const wasm=await program(files.wasm);await program(files.core);
const factory=(await import(pathToFileURL(resolve(root,files.core.path)))).default,life=new AbortController();
const art=await openManagedPauseArt({factory,signal:life.signal,resources:{read(key){assert.equal(key,'wasm');return wasm;},raw:name=>zh.has('raw/'+name)?readEntryData(zh.get('raw/'+name)):dat.read(name)}});
let time=0;const state=(selection,confirm=false,yes=false,kind="pause",help=null)=>({selection,confirm,yes,kind,help});
function present(s,n=1,frame=51){let p;for(let i=0;i<n;i++)p=art.frame(s,frame,time+=1000/60);return p;}
try{
  assert.equal(present(null),null);
  const main=present(state('resume'),25);assert.deepEqual(main.records.map(r=>r.script).sort((a,b)=>a-b),[84,85,87,88,89,90,121]);
  assert.ok(main.records.every(r=>r.bank===5&&r.drawPass==='original-pause-front'&&r.width>0));
  assert.deepEqual(main.records.filter(r=>[87,88,90].includes(r.script)).map(r=>r.sprite),[67,68,70]);
  assert.equal(main.snapshot.backdrop.sourceFrame,51);
  assert.ok(main.originalRecords.some(r=>r.bank===6&&r.script===82&&r.drawPass==='original-pause-shade'));
  assert.ok(main.originalRecords.some(r=>r.bank===6&&r.entry===1&&r.drawPass==='original-pause-backdrop'));
  const end=present(state('end'),10);assert.notEqual(end.records.find(r=>r.script===88).color,main.records.find(r=>r.script===88).color);
  const no=present(state('end',true,false),20),yes=present(state('end',true,true),10);
  assert.deepEqual(no.records.filter(r=>r.script>=96&&r.script<=98).map(r=>r.sprite),[73,69,71]);
  assert.notEqual(no.records.find(r=>r.script===97).color,yes.records.find(r=>r.script===97).color);assert.ok(yes.revision>no.revision);
  assert.equal(yes.snapshot.serial,main.snapshot.serial);const cancel=present(state('end'),10);assert.ok(!cancel.records.some(r=>r.script>=96&&r.script<=98));
  const again=present(state('restart',true,false),25);assert.equal(again.records.filter(r=>r.script===97).length,1);
  assert.equal(present(null),null);const reopened=present(state('resume'),20,3);assert.ok(reopened.snapshot.serial>again.snapshot.serial);assert.equal(reopened.snapshot.backdrop.sourceFrame,3);
  const gameOver=present(state('end',false,false,'game-over'),25,117);
  assert.deepEqual(gameOver.records.map(r=>r.script).sort((a,b)=>a-b),[102,103,105,106,107,108]);
  assert.equal(gameOver.records.find(r=>r.script===105).y,297,'original return row stays in its original location');
  const help=present(state('manual',false,false,'game-over',{topic:0,page:false}),25,117);
  assert.ok(help.records.some(r=>r.bank===46&&r.script===0));
  const page=present(state('manual',false,false,'game-over',{topic:1,page:true}),25,117);
  assert.ok(page.records.some(r=>r.bank===46&&r.script===9));
  const coopWasm=await program(files.coopWasm);await program(files.coopCore);
  const coFactory=(await import(pathToFileURL(resolve(root,files.coopCore.path)))).default;
  const build={coreSHA256:digest(coopWasm),resourceSHA256:descriptor.files['game-data'].sha256};
  const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'real-native-postrun-menu',epoch:1,players:2,
    route:'A',difficulty:1,challengeMode:false,seed:1,inputDelay:0,rollbackWindow:0,...build});
  const tape=await createConfirmedHistory({rules,allowedMask:763});
  for(let frame=0;frame<8;frame++)tape.append({frame,inputs:[0,1].map(player=>({player,held:0,pressed:0,released:0,predicted:false}))});
  tape.seal('completed');const bytes=tape.replayBytes(),writes=[],sounds=[];
  const replay=await openCoopReplaySave({bytes,postrun:true,signal:life.signal,resources:{build,read:()=>coopWasm},factory:coFactory,sound:id=>sounds.push(id),
    shelf:{async list(){return [];},async save(file,{slot,playerName}){
      assert.deepEqual(new Uint8Array(await file.arrayBuffer()),bytes);writes.push({slot,playerName});
      const result=await readMultiplayerReplay(new Uint8Array(await file.arrayBuffer()),build);
      assert.equal(result.status().end,'completed');return {slot,playerName,frames:8,route:'A',players:2,difficulty:1,savedAt:Date.now()};}}});
  const redraw=(n=1)=>{let save,out;for(let i=0;i<n;i++){time+=1000/60;save=replay.frame(time);out=art.postrunFrame(save,time);}return {save,out};};
  const tap=code=>{replay.key({code,down:true});replay.key({code,down:false});return redraw(3);};
  const slots=redraw(25);
  assert.ok(slots.out.some(r=>r.bank===24&&r.sprite>=0),'original title112 replay backdrop has visible art');
  assert.ok(slots.out.some(r=>r.bank===11&&r.x===58&&r.y===80),'original No.01 at the standalone replay screen position');
  assert.equal(tap('ArrowDown').save.slot,1);assert.equal(tap('Enter').save.phase,2);
  const name=redraw(20);assert.ok(name.out.some(r=>r.bank===11&&r.x===212&&r.y===360),'original postrun keyboard position');
  tap('KeyZ');tap('ArrowUp');tap('ArrowLeft');assert.equal(tap('Enter').save.phase,3);
  assert.deepEqual(writes,[],'pending owner awaits actual file bytes and storage');
  for(let n=0;n<10;n++)await new Promise(resolve=>setImmediate(resolve));
  assert.deepEqual(writes,[{slot:1,playerName:'A'}]);assert.equal(redraw().save.phase,5);assert.ok(sounds.includes(18));
  console.log('PASS: original pause, Game Over, postrun title112 and named multiplayer Replay saved through native keyboard');
}finally{life.abort();art.close();}
