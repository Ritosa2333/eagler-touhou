const {readdir,readFile}=require('node:fs/promises');
const {resolve,join}=require('node:path');
const {pathToFileURL}=require('node:url');
const assert=require('node:assert/strict');
const create=require(resolve(process.argv[2]));const root=resolve(process.argv[3]),ui=resolve(process.argv[4]);
async function copy(m,dir,dst){for(const e of await readdir(dir,{withFileTypes:true})){const p=dst+'/'+e.name;if(e.isDirectory()){m.FS.mkdir(p);await copy(m,join(dir,e.name),p);}else if(e.isFile())m.FS.writeFile(p,await readFile(join(dir,e.name)));}}
(async()=>{
 const {createRollbackMatch}=await import(pathToFileURL(resolve('ports/th03/web/native-match-rollback.mjs')));
 const {createConfirmedDriver}=await import(pathToFileURL(resolve('ports/th03/web/native-confirmed-driver.mjs')));
 const {packTouch}=await import(pathToFileURL(resolve('ports/th03/web/touch-input.js')));
 const modules=await Promise.all([create(),create(),create()]);
 for(const m of modules){m.FS.mkdir('/content');m.FS.mkdir('/user');m.FS.mkdir('/user/REPLAY');await copy(m,root,'/content');m.FS.mkdir('/content/PODDUI');await copy(m,ui,'/content/PODDUI');assert.equal(m._th03_classic_create(),1);m._th03_classic_start_versus();}
 const spectator=createConfirmedDriver(modules[2]);let compared=0;
 const queues=[[],[]], channels=[{},{}], errors=[],played=[[],[]],confirmedInputs=[[],[]];let hostTick=0;
 channels.forEach((ch,i)=>ch.send=message=>queues[1-i].push({message,due:hostTick+(hostTick>700&&i===1?3:0)}));
 const matches=channels.map((channel,seat)=>createRollbackMatch({seat,channel,engine:modules[seat],sample:()=>{const bits=hostTick<500?0:hostTick<650?(hostTick%60<5?512:0):(hostTick%40<20?(seat?8:4):0)|(hostTick%80<20?512:0);const active=hostTick>=800&&hostTick<900;return packTouch(bits,{active,begin:hostTick===800,x:active?(seat?-16:16):0,y:active?8:0});},onConfirmedInput:(frame,inputs,metadata)=>confirmedInputs[seat].push([frame,...inputs,metadata.replaySamples,metadata.charge]),onFrame:(frame,pcm)=>{let hash=2166136261;for(const value of pcm)hash=Math.imul(hash^value,16777619);played[seat].push([frame,pcm.length,hash>>>0]);},onError:e=>errors.push(e),now:()=>hostTick*5}));
 for(hostTick=0;hostTick<1600;hostTick++){
  matches.forEach(m=>m.tick());
  for(let i=0;i<2;i++)while(queues[i].length&&queues[i][0].due<=hostTick)channels[i].onmessage({data:queues[i].shift().message});
  if(errors.length)throw errors[0];
  while(spectator.frame<confirmedInputs[0].length){const row=confirmedInputs[0][spectator.frame];spectator.step(row.slice(1,3),row[4]);}
  if(matches[0].frame===matches[0].confirmed&&spectator.frame===matches[0].frame&&spectator.frame>750){
    assert.equal(modules[2]._th03_classic_scene(),modules[0]._th03_classic_scene());
    if(modules[0]._th03_classic_scene()===9)assert.equal(modules[2]._th03_semantic_state_hash(),modules[0]._th03_semantic_state_hash());
    const a=modules[0]._th03_classic_pixels(),b=modules[2]._th03_classic_pixels();assert.deepEqual(modules[2].HEAPU8.slice(b,b+640*400*4),modules[0].HEAPU8.slice(a,a+640*400*4));compared++;
  }
 }
 assert.ok(compared>0,'confirmed spectator re-simulation matched native player');
 assert.ok(matches[0].rollbacks>0,'late input caused real rollback');
 assert.ok(matches.every(m=>m.snapshotStats.captures>0&&m.snapshotStats.restores>0),'native semantic snapshots were captured and restored');
 const common=Math.min(played[0].length,played[1].length);
 assert.ok(common>0,'both peers confirmed frames');
 assert.deepEqual(played[0].slice(0,common),played[1].slice(0,common),'shared confirmed frames and audio must match');
 assert.deepEqual(confirmedInputs[0].slice(0,common),confirmedInputs[1].slice(0,common),'confirmed input stream must match after rollback');
 assert.ok(confirmedInputs[0].slice(0,common).some((row,i)=>row[0]===i&&row[1]>2**40),'confirmed stream includes touch data');
 console.log('Native peers corrected delayed inputs, confirmed audio exactly once',matches.map(m=>({frame:m.frame,confirmed:m.confirmed,rollbacks:m.rollbacks,snapshots:m.snapshotStats})));
})().catch(e=>{console.error(e);process.exitCode=1;});
