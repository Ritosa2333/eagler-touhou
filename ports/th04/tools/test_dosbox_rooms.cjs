const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
(async()=>{const reports=[],rtc=process.env.TRANSPORT==='rtc'||process.env.RTC_OFF==='1',rollback=process.env.RTC_OFF!=='1'&&process.env.ROLLBACK!=='0',browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});try{
 const base=process.env.ROOM_URL||'http://127.0.0.1:9868';
 for(const players of (process.env.ROOM_PLAYERS?JSON.parse(process.env.ROOM_PLAYERS):rollback?[2,3]:[2])){
  const pages=[],errors=[];
  for(let slot=0;slot<players;slot++){
   const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>{errors.push(e.stack||e.message);console.log('PAGEERROR',slot,e.message);});page.on('requestfailed',r=>console.log('REQUEST FAILED',slot,r.url(),r.failure()));
   await page.route('**/netplay/room.js',async route=>{const r=await route.fetch();await route.fulfill({response:r,body:await r.text()+`\nglobalThis.roomQA={read:()=>({running,stopped,stopReason,frame:queue?.frame,confirmed:queue?.confirmed,active:queue?.active,slot:queue?.slot,reason:runtime?.predictionBlockReason(),offlineMask:queue?.offlineMask(queue.frame),hashes:[...hashes].slice(-6).map(([tick,s])=>({tick,hash:s.hash})),s:runtime?.snapshot(),check:runtime?.checksum(),rollbacks:rollbackCount,replayed:resimulated,info:runtime?.rollbackInfo(),hz,averageCost,details:$('diagnostics').textContent}),stop:()=>{stopped=true;running=false;runtime?.stop();},keys};const qaSend=broadcast;broadcast=packet=>{if(window.qaDelay&&packet.type==='input')setTimeout(()=>qaSend(packet),window.qaDelay);else qaSend(packet);};`});});
   await require('./native_scheduler_qa.cjs').install(page);
   if(process.env.FULL_FIRE==='1')await page.route('**/netplay/runtime.js',async route=>{
    const response=await route.fetch();await route.fulfill({response,body:await response.text()+`
     const fullPowerStep=th04Sync.step;th04Sync.step=(inputs,options)=>{
      if(mailbox>=0){const d=gameDataBase(),m=emulator.module,h=m.HEAPU8;
       m.thDirtyMark?.(d+0x4664,2);m.thDirtyMark?.(mailbox+609,48);
       h[d+0x4664]=128;h[d+0x4665]=9;
       for(let s=0;s<3;s++){h[mailbox+609+s*16]=128;h[mailbox+610+s*16]=9;}
      }return fullPowerStep(inputs,options);
     };`});
   });
   await page.goto(base+'/lan.html?network=lan&transport='+(rtc?'rtc':'ws'));await page.waitForFunction(()=>window.roomQA);
  }
  const host=pages[0];await host.click('#create');await host.locator('#lobby').waitFor({state:'visible'});await host.selectOption('#players',String(players));if(rollback)await host.locator('#rollback').check();else await host.locator('#rollback').uncheck();if(rtc)await host.selectOption('#language','jp');
  const code=(await host.locator('#room-info').textContent()).match(/房间 (\d{4})/)[1];
  for(let i=1;i<players;i++){await pages[i].fill('#room',code);await pages[i].click('#join');await pages[i].locator('#lobby').waitFor({state:'visible'});}
  // Host takes P2 to verify host authority is independent of seat.
  const seats=players===2?[1,0]:[1,2,0];for(let i=0;i<players;i++)await pages[i].click('#seat'+seats[i]);
  if(process.env.ART_DUPLICATES==='1')for(let i=0;i<players;i++){
   const loadout=seats[i]%2;await pages[i].click('[data-loadout="'+loadout+'"]');
   await pages[i].waitForFunction(value=>document.querySelector('[data-loadout="'+value+'"]').getAttribute('aria-pressed')==='true',loadout);
  }
  for(const page of pages)await page.click('#ready');await host.waitForFunction(()=>!document.querySelector('#start').disabled);await host.click('#start');
  try{for(const page of pages)await page.waitForFunction(rollback=>{const s=window.roomQA.read();return s.stopped||(s.s?.gameFrame>100&&(!rollback||s.active));},rollback,{timeout:60000});}catch(error){for(let i=0;i<pages.length;i++){console.log('ROOM FAILURE',i,await pages[i].evaluate(()=>({qa:window.roomQA.read(),status:document.querySelector('#status').textContent,startup:document.querySelector('#startup-progress').textContent})));await pages[i].screenshot({path:path.resolve(__dirname,`../reports/dosbox-room-failure-${i}.png`)});}throw error;}
  const boot=await Promise.all(pages.map(p=>p.evaluate(()=>window.roomQA.read())));for(let i=0;i<boot.length;i++){const s=boot[i];if(!s.running||s.stopped){for(const f of pages[i].frames())console.log('BOOT FRAME',i,f.url(),await f.evaluate(()=>({details:document.querySelector('#details')?.textContent,warning:document.querySelector('#page-warning')?.textContent})).catch(()=>null));}assert(s.running&&!s.stopped,s.stopReason);}console.log('rooms boot',players,boot.map(s=>({frame:s.frame,active:s.active})));
  const before=await host.evaluate(()=>window.roomQA.read());
  if(process.env.FULL_FIRE==='1'){
   for(const page of pages)await page.keyboard.down('KeyZ');
   await host.waitForTimeout(2500);
   for(const page of pages)await page.keyboard.up('KeyZ');
  }
  let outage=null;
  if(process.env.ROOM_OUTAGE==='1'){
   const route='**/api/events?*',block=r=>r.fulfill({status:503,contentType:'application/json',body:JSON.stringify({error:'QA temporary room service outage'})});
   for(const page of pages)await page.route(route,block);
   await host.waitForTimeout(8500);outage=await Promise.all(pages.map(p=>p.evaluate(()=>window.roomQA.read())));
   for(const s of outage){assert(s.running&&!s.stopped,s.stopReason);assert(s.frame>before.frame+30);assert(s.details.includes('QA temporary room service outage'));}
   for(const page of pages)await page.unroute(route,block);
   for(const page of pages)await page.waitForFunction(()=>!window.roomQA.read().details.includes('QA temporary room service outage'));
   console.log('PASS room HTTP outage > 5 s while player channels keep running; recovery',players);
  }
  for(const page of pages)await page.evaluate(()=>window.qaDelay=65);
  // Observe movement while the key remains held. Comparing only after five
  // alternating holds can legitimately land back at the starting position.
  for(const page of pages)await page.keyboard.down('ArrowRight');
  let movement;
  try{
   await host.waitForFunction(start=>window.roomQA.read().s?.players.some((xy,i)=>Math.abs(xy[0]-start.players[i][0])>4),before.s,{timeout:20000});
   movement=await host.evaluate(()=>window.roomQA.read());
  }finally{for(const page of pages)await page.keyboard.up('ArrowRight');}
  for(let k=0;k<5;k++){
   for(let i=0;i<players;i++)await pages[i].keyboard.down(k%2?'ArrowLeft':'ArrowRight');
   await host.waitForTimeout(180);
   for(let i=0;i<players;i++){await pages[i].keyboard.up(k%2?'ArrowLeft':'ArrowRight');await pages[i].keyboard.press('KeyZ',{delay:40});}
   await host.waitForTimeout(180);
  }
  await host.waitForTimeout(1200);const moved=await Promise.all(pages.map(p=>p.evaluate(()=>window.roomQA.read())));
  if(rollback)assert(moved.some(s=>s.rollbacks>before.rollbacks),'delayed real input causes rollback');else for(const s of moved){assert.equal(s.rollbacks,0);assert.equal(s.active,false);}
  // Save one common protocol checksum by observing the peers' confirmed hashes.
  for(const page of pages)await page.evaluate(()=>window.qaDelay=0);await host.waitForTimeout(3000);
  const final=await Promise.all(pages.map(p=>p.evaluate(()=>window.roomQA.read())));
  console.log('rooms final',players,final.map(s=>({frame:s.frame,active:s.active,rollbacks:s.rollbacks,info:s.info})));
  for(const s of final){assert(s.running&&!s.stopped,s.stopReason);assert(s.frame>before.frame+50);assert(s.details.includes('相同状态步'));if(rollback)assert.equal(s.info.storage,'page undo');else assert(s.details.includes('回滚已关闭'));}
  const common=final[0].hashes.filter(h=>final.every(s=>s.hashes.some(other=>other.tick===h.tick)));assert(common.length,'common confirmed state');for(const h of common)for(const s of final)assert.equal(s.hashes.find(other=>other.tick===h.tick).hash,h.hash);
  assert(movement.s.players.some((xy,i)=>Math.abs(xy[0]-before.s.players[i][0])>4),'native movement through room');
  await pages[1].keyboard.press('Escape',{delay:120});for(const page of pages)await page.waitForFunction(()=>window.roomQA.read().reason==='同步暂停菜单',{},{timeout:15000});
  await host.waitForTimeout(500);const paused=await host.evaluate(()=>window.roomQA.read());await host.waitForTimeout(300);assert.equal((await host.evaluate(()=>window.roomQA.read())).s.gameFrame,paused.s.gameFrame);
  await pages[1].keyboard.press('Escape',{delay:120});for(const page of pages)await page.waitForFunction(()=>window.roomQA.read().reason===''&&window.roomQA.read().s.gameFrame>100,{},{timeout:15000});await host.waitForTimeout(600);assert((await host.evaluate(()=>window.roomQA.read())).s.gameFrame>paused.s.gameFrame);
  let departed=null;if(players===3){await pages[2].close();const beforeDeparture=await host.evaluate(()=>window.roomQA.read());await host.waitForFunction(()=>!!(window.roomQA.read().offlineMask&1),{},{timeout:20000});await host.waitForTimeout(600);departed=await host.evaluate(()=>window.roomQA.read());assert(departed.running&&!departed.stopped);assert(departed.frame>beforeDeparture.frame);}
  assert.deepEqual(errors,[]);reports.push({players,seats,before,outage,movement,moved,final,commonHashes:common,paused,departed,errors});
  const downloadPromise=host.waitForEvent('download');await host.locator('[data-fault-download]').evaluate(b=>b.click());const download=await downloadPromise;
  const faultFile=path.resolve(__dirname,`../reports/room-fault-download-${players}p.json`);await download.saveAs(faultFile);const fault=JSON.parse(fs.readFileSync(faultFile));
  assert.equal(fault.schema,'touhou-network-fault/1');assert.equal(fault.runtime.cpu.nativeWarning.kind,'idle');assert(fault.runtime.game.gameFrame>100);
  for(let i=0;i<players;i++)if(!pages[i].isClosed()){await pages[i].screenshot({path:path.resolve(__dirname,`../reports/dosbox-room-${players}p-${i}.png`)});await pages[i].evaluate(()=>window.roomQA.stop());await pages[i].close();}
  console.log('PASS real room',players,'players; rollback counts',final.map(s=>s.rollbacks));
 }
 fs.writeFileSync(path.resolve(__dirname,process.env.OUTPUT|| (rtc?'../reports/dosbox-rtc-lockstep.json':'../reports/dosbox-rooms.json')),JSON.stringify(reports,null,2));
 }finally{await browser.close();}})().catch(e=>{console.error(e);process.exitCode=1});
