import {verifyInGameReplaySave} from './coop-browser-original.mjs';
import {observeCoopPalettes,verifyRecordedPalettes} from './coop-browser-palettes.mjs';
import assert from 'node:assert/strict';import {mkdir,readdir,readFile,writeFile} from 'node:fs/promises';import {join} from 'node:path';import {createHash} from 'node:crypto';import {setTimeout as sleep} from 'node:timers/promises';
// A new, independent author Launcher participant spectates; no data is seeded and no player-ready/message callback is stubbed.
export async function verifyBrowserResult({watcher,players,seats,invite,child,until,work,inspect}){
 const nativeSave=process.env.TH128_TEST_IN_GAME_SAVE==='1';
 const proof={seats,operations:[]};const note=async op=>{proof.operations.push(op);await writeFile(join(work,'result-progress.json'),JSON.stringify(proof,null,2));};
 const waitChild=(p,pred,what,ms=45000)=>until(what,async()=>{const val=await child(p);return val&&await pred(val)?val:null;},ms);
 const action=async(p,key)=>{const {f}=await waitChild(p,v=>v.status.scene==='match','interactive game');await f.focus('#game');await p.keyboard.press(key);};
 const ui=async(p,selector)=>{const {f}=await waitChild(p,async v=>v.f.evaluate(s=>{const e=document.querySelector(s);return e&&e.getClientRects().length&&!e.disabled;},selector),'available '+selector);await f.click(selector);return f;};
 if(watcher){const duringPause=process.env.TH128_TEST_ADMISSION_PAUSE==='1';if(duringPause){await action(players[0],'Escape');await waitChild(players.at(-1),v=>v.status.paused,'observer admission while common timeline held');}await watcher.goto(invite(seats),{waitUntil:'domcontentloaded'});await watcher.waitForFunction(()=>{const e=document.querySelector('#mpSpectatorJoin');return e&&!e.hidden&&!e.disabled;},{timeout:35000});await inspect(watcher,'06-room-viewer');await watcher.evaluate(()=>{const buttons=[...document.querySelectorAll('#mpSpectatorJoin,#mpSpectatorPreviewJoin')];const visible=buttons.find(b=>b.getClientRects().length>0);if(visible&&/加入旁观|Join spectator/i.test(visible.textContent))visible.click();});
  const initial=await waitChild(watcher,({status})=>status.scene==='match'&&status.frame>=10,'actual spectator first frame',105000);proof.joinedAt=initial.status.frame;await note('late spectator saw native game');if(duringPause){await action(players[0],'Escape');await waitChild(players[0],v=>!v.status.paused,'players resumed after late spectator joined');}
  const toggle=await watcher.waitForSelector('#cooperativeSpectatorToggle',{visible:true,timeout:10000});if(await toggle.evaluate(e=>e.getAttribute('aria-expanded')!=='true'))await toggle.click();await watcher.click('#cooperativeSpectatorPause');await watcher.waitForFunction(()=>/继续|恢复|Resume/i.test(document.querySelector('#cooperativeSpectatorPause')?.textContent??''),{timeout:12000});await sleep(220);const frozen=(await child(watcher)).status.frame,hostStart=(await child(players[0])).status.frame;
  await waitChild(players[0],v=>v.status.frame>=hostStart+8,'players advance during observer pause');await sleep(150);assert.equal((await child(watcher)).status.frame,frozen);proof.pausedAt=frozen;await note('viewer pause did not stop players');
  // Return to live through the real author spectator controls; keyboard playback is independent of the game players.
  await watcher.click('#cooperativeSpectatorLive');await waitChild(watcher,v=>v.status.frame>frozen,'observer resumed');
 }
 if(process.env.TH128_TEST_PALETTE)proof.cirnoPalettes=await observeCoopPalettes({players,watcher,child,until,work});
 await waitChild(players.at(-1),v=>v.status.scene==='match'&&!v.status.paused&&v.status.frame>=45,'fresh recorded inputs');
 await action(players.at(-1),'Escape');await waitChild(players[0],v=>v.status.paused,'common pause for end');const ender=players.at(-1);for(const key of nativeSave?['ArrowDown','ArrowDown','Enter']:['ArrowDown','Enter'])await action(ender,key);const confirm=await waitChild(ender,v=>v.status.confirm==='no'&&v.status.menu===(nativeSave?'save':'end'),'original end confirmation');await (await confirm.f.$('#game')).screenshot({path:join(work,'end-confirm.png')});for(const key of ['ArrowUp','Enter'])await action(ender,key);
 if(nativeSave){await verifyInGameReplaySave({page:ender,frame:confirm.f,until,work});proof.inGameSave=true;}
 const party=[...players,...(watcher?[watcher]:[])];const titles=await Promise.all(party.map(p=>waitChild(p,async v=>v.f.evaluate(()=>!document.querySelector('#coop-result').hidden&&!document.querySelector('#coop-review').disabled&&document.querySelector('#coop-result-label').innerText==='本局已结束'),'finished local history',100000)));await note('players and spectator received sealed result');
 async function download(p,frame,name){const dir=join(work,'recordings-'+name);await mkdir(dir);const cdp=await p.createCDPSession();await cdp.send('Browser.setDownloadBehavior',{behavior:'allow',downloadPath:dir});await frame.click('#coop-download-replay');const filename=await until('download full '+name,async()=>(await readdir(dir)).find(x=>x.endsWith('.th128mp')));await cdp.detach();return {file:join(dir,filename),bytes:await readFile(join(dir,filename))};}
 const first=await download(players[0],titles[0].f,'P1'),record=JSON.parse(first.bytes);assert.equal(record.schema,'th128-coop-replay/1');assert.equal(record.header.rules.players,seats);assert.equal(record.tail.reason,'left');assert.ok(record.tail.frameCount>20);assert.equal(record.held.length,record.tail.frameCount*seats);proof.frames=record.tail.frameCount;proof.replaySHA256=createHash('sha256').update(first.bytes).digest('hex');
 if(watcher){const observed=await download(watcher,titles.at(-1).f,'observer');assert.deepEqual(observed.bytes,first.bytes);proof.identicalObserverFile=true;}await note('downloaded confirmed original game-input recording');
 const reviewIndex=nativeSave?seats-1:watcher?seats:0,reviewer=party[reviewIndex],frame=titles[reviewIndex].f;
 if(!nativeSave)await ui(reviewer,'#save-replay');
 await ui(reviewer,'#coop-review');await waitChild(reviewer,v=>v.status.scene==='replay','post-run archive opened');
 await waitChild(reviewer,v=>v.f.evaluate(()=>!document.querySelector('#replay-shelf').hidden&&document.querySelector('#replay-slots').options.length>0),'real local replay archive');
 if(nativeSave)assert.equal(await frame.$eval('#replay-slots',list=>list.options[0]?.textContent),'No.02 · A');
 await frame.click('#replay-slot-open');await waitChild(reviewer,v=>v.status.scene==='replay','load saved browser replay slot');proof.localArchive=true;
 // Reopen the physical file just downloaded via the normal Replay picker, not a synthetic byte roundtrip.
 const input=await frame.$('#replay-file');await input.uploadFile(first.file);await frame.waitForFunction(()=>document.querySelector('#game').dataset.scene==='replay'&&!document.querySelector('#replay-toggle').disabled,{timeout:25000});
 await frame.click('#replay-toggle');await waitChild(reviewer,v=>v.status.scene==='replay'&&v.status.frame>=4,'reopened recording advances');
 if(proof.cirnoPalettes)await verifyRecordedPalettes(record,proof.cirnoPalettes,frame,until);
 if(process.env.TH128_TEST_FREEZE_MULTIPLIER){const multiplier=record.header.rules.freezeMotivationMultiplier??1;assert.equal(multiplier,Number(process.env.TH128_TEST_FREEZE_MULTIPLIER));proof.freezeMotivationMultiplier=multiplier;}
 if(seats===3){await frame.click('#replay-seat');await frame.click('#replay-seat');assert.match(await frame.$eval('#replay-seat',b=>b.textContent),/P1/);}
 await frame.click('#replay-return');await waitChild(reviewer,v=>v.status.scene==='title','native title after recorded file');await note('file reopened, replayed, and returned to title');
 if(process.env.TH128_DIALOGUE_TAPE){
   await ui(reviewer,'#coop-open-replay');const picker=await frame.$('#replay-file');await picker.uploadFile(process.env.TH128_DIALOGUE_TAPE);
   await waitChild(reviewer,v=>v.status.scene==='replay','authored dialogue input tape');
   await frame.evaluate(()=>{const bar=document.querySelector('#replay-seek');bar.value=bar.max;bar.dispatchEvent(new Event('change',{bubbles:true}));});
   const played=await waitChild(reviewer,v=>v.f.evaluate(()=>{const c=document.querySelector('#stage-text');if(c.hidden||Number(c.dataset.messageLines)<1)return false;
     const pixels=c.getContext('2d').getImageData(0,0,c.width,c.height).data;let opaque=0;for(let i=3;i<pixels.length;i+=4)if(pixels[i]>90)opaque++;return opaque>100;
   }),'real original dialogue pixels in the multiplayer bubble',45000);
   await (await played.f.$('main')).screenshot({path:join(work,'original-dialogue.png')});proof.dialoguePixels=true;
   await frame.click('#replay-return');await waitChild(reviewer,v=>v.status.scene==='title','title after original dialogue replay');
 }
 for(const p of [reviewer,...party.filter(p=>p!==reviewer)]){await ui(p,'#coop-back-room');await waitChild(p,async v=>v.status.scene==='title'&&v.f.evaluate(()=>document.querySelector('#coop-result').hidden),'result released to original room title');await p.waitForFunction(()=>{const back=document.querySelector('#mpLeaveRoom');return !!back&&back.getBoundingClientRect().width>0&&!!document.querySelector('#mpSeatStage');},{timeout:45000});}
 await note('everyone returned independently through author room owner');return proof;
}
