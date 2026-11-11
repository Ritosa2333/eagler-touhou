const assert=require('node:assert/strict');
const {existsSync,mkdirSync,readFileSync}=require('node:fs');
const {resolve}=require('node:path');
const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
(async()=>{
  const browser=await chromium.launch({headless:true,executablePath:[
    chromium.executablePath(),'C:/Program Files/Google/Chrome/Application/chrome.exe',
    'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'].find(existsSync)});
  const base=process.env.TH03_URL||'http://127.0.0.1:9870';
  const errors=[];
  const hostSeat=Number(process.env.TH03_HOST_SEAT||0);
  try{
    const api=async(path,data)=>{const res=await fetch(base+'/api/'+path,{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({protocol:'th03-lan/1',...data})});const value=await res.json();assert(res.ok,JSON.stringify(value));return value;};
    const host=await api('create',{mode:process.env.TH03_TRANSPORT||'relay'});
    const guest=await api('join',{room:host.room});
    const ids=[host,guest];
    await api('settings',{...host,settings:{...host.state.settings,hostSeat,language:'cn',clock:Number(process.env.TH03_CLOCK||32),rollback:process.env.TH03_ROLLBACK==='1'}});
    await api('ready',host);await api('ready',guest);await api('start',host);
    const contexts=[await browser.newContext({viewport:{width:1280,height:900}}),
      await browser.newContext({viewport:{width:390,height:844},deviceScaleFactor:2})];
    const pages=await Promise.all(contexts.map(c=>c.newPage()));
    for(const page of pages)await page.route('**/app.js',route=>{
      if(route.request().resourceType()!=='script')return route.continue();
      const source=readFileSync(resolve('web/app.js'),'utf8').replace('const heap=emulator.module.HEAPU8;let h=2166136261;',
        'const heap=emulator.module.HEAPU8;window.syncTestHeap=heap.slice();let h=2166136261;')
        .replace('syncFrame=frame;','syncFrame=frame;window.syncTestInputs=inputs;')
        .replace('await netplay.synchronize(syncIdentity);','window.syncTestNetwork=netplay;window.syncTestMusic=()=>({id:localBgm?.active?.id,time:localBgm?.context.currentTime,sequence:nativeMusic?.sequence});await netplay.synchronize(syncIdentity);');
      return route.fulfill({contentType:'text/javascript',body:source});
    });
    for(const page of pages)await page.route('**/lockstep.js',route=>{
      if(route.request().resourceType()!=='script')return route.continue();
      const source=readFileSync(resolve('web/lockstep.js'),'utf8')
        .replace('this.onState?.(`双方校验不同', 'console.error(`双方校验不同')
        .replace('this.budget-=TICK_MS;', 'this.budget-=TICK_MS;if(window.syncTestPacing)syncTestPacing.push({pump:started,at:performance.now(),frame:this.queue.frame});');
      return route.fulfill({contentType:'text/javascript',body:source});
    });
    for(let i=0;i<2;i++){
      pages[i].on('pageerror',e=>errors.push(e.message));
      pages[i].on('console',m=>{if(m.type()==='error'){console.log('browser',i,m.text());if(m.text().includes('校验不同'))errors.push(m.text());}});
      const params=new URLSearchParams({net:host.state.mode,role:i?'guest':'host',room:host.room,token:ids[i].token,fullHash:'1',rollback:'1'});
      await pages[i].goto(base+'/local.html?'+params);
      await pages[i].locator('#start').click();
    }
    for(const page of pages)await page.waitForFunction(()=>window.th03SyncState?.frame>10,null,{timeout:120000});
    for(let i=0;i<2;i++){
      assert.equal(await pages[i].evaluate(()=>syncTestNetwork.localSeat),hostSeat^i);
      assert.equal(await pages[i].locator('.player-label').textContent(),`${(hostSeat^i)+1}P`);
    }
    // The scenarios below are indexed by game seat, independently of room role.
    if(hostSeat===1)pages.reverse();
    for(let round=0;round<30;round++){
      await pages[0].waitForTimeout(1000);
      const states=await Promise.all(pages.map(p=>p.evaluate(()=>({state:window.th03SyncState,status:document.getElementById('status').textContent}))));
      console.log(JSON.stringify(states));
      if(states.some(s=>s.status.includes('错误'))){
        const hashes=await Promise.all(pages.map(p=>p.evaluate(()=>{const heap=window.syncTestHeap,result=[];for(let at=0;at<heap.length;at+=4096){let h=2166136261;for(let i=at;i<at+4096&&i<heap.length;i++)h=Math.imul(h^heap[i],16777619);result.push(h>>>0);}return result;})));
        const blocks=hashes[0].flatMap((h,i)=>h!==hashes[1][i]?[i]:[]);
        console.log('Different heap blocks:',blocks);
        for(const block of blocks.slice(0,40)){
          const data=await Promise.all(pages.map(p=>p.evaluate(at=>Array.from(syncTestHeap.subarray(at,at+4096)),block*4096)));
          const diffs=data[0].flatMap((v,i)=>v!==data[1][i]?[[block*4096+i,v,data[1][i]]]:[]);
          console.log('DIFF',block,JSON.stringify(diffs.slice(0,70)));
        }
        throw Error('Synchronization failed');
      }
      if(states.every(s=>s.state?.selecting))break;
    }
    const selecting=await Promise.all(pages.map(p=>p.evaluate(()=>window.th03SyncState)));
    assert(selecting.every(s=>s.selecting),'Both games must reach native character select');
    for(const page of pages)await page.waitForFunction(()=>syncTestMusic().id==='select-m26');
    await pages[0].screenshot({path:resolve('reports/select-before.png'),fullPage:true});
    for(const page of pages)await page.evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[0].bringToFront();await pages[0].evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[0].locator('#canvas').focus();await pages[0].keyboard.down('ArrowDown');
    for(const page of pages)await page.waitForFunction(()=>syncTestInputs[0].actions.includes('down')&&!syncTestInputs[1].actions.includes('down'));
    await pages[0].keyboard.up('ArrowDown');
    await pages[1].bringToFront();await pages[1].evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[1].locator('#canvas').focus();await pages[1].keyboard.down('ArrowDown');
    for(const page of pages)await page.waitForFunction(()=>syncTestInputs[1].actions.includes('down')&&!syncTestInputs[0].actions.includes('down'));
    await pages[1].keyboard.up('ArrowDown');
    await pages[0].waitForTimeout(800);
    for(const page of pages){await page.bringToFront();await page.evaluate(()=>window.dispatchEvent(new Event('focus')));
      await page.locator('#canvas').focus();await page.keyboard.down('KeyZ');await page.waitForTimeout(1000);
      console.log('PRESS',JSON.stringify(await page.evaluate(()=>({input:syncTestInputs,state:th03SyncState}))));
      await page.keyboard.up('KeyZ');await page.waitForTimeout(800);}
    await pages[0].waitForTimeout(2000);
    await pages[0].screenshot({path:resolve('reports/select-after.png'),fullPage:true});
    console.log('AFTER SELECT',await Promise.all(pages.map(p=>p.evaluate(()=>({state:window.th03SyncState,status:document.getElementById('status').textContent})))));
    for(const page of pages)await page.waitForFunction(()=>window.th03SyncState?.phase===1,null,{timeout:60000}).catch(async e=>{
      console.log('PHASE TIMEOUT',await Promise.all(pages.map(p=>p.evaluate(()=>({state:window.th03SyncState,inputs:window.syncTestInputs})))));
      throw e;
    });
    for(const page of pages)await page.waitForFunction(()=>/^0[0-8]mm-m26$/.test(syncTestMusic().id));
    const songs=await Promise.all(pages.map(p=>p.evaluate(()=>syncTestMusic().id)));
    assert.equal(songs[0],songs[1],'Confirmed music must agree');
    for(const page of pages)await page.evaluate(()=>window.syncTestPacing=[]);
    // Stall a real native game's browser task, then inspect every new-frame pump.
    await pages[1].evaluate(()=>{const until=performance.now()+240;while(performance.now()<until){}});
    await pages[0].waitForTimeout(1200);
    for(const page of pages){
      const pacing=await page.evaluate(()=>{const groups=new Map();for(const frame of syncTestPacing)groups.set(frame.pump,(groups.get(frame.pump)||0)+1);
        return {newFrames:syncTestPacing.length,maxNewFrames:Math.max(0,...groups.values()),state:th03SyncState};});
      assert(pacing.newFrames>=8,'Both native games must recover after the injected stall');
      assert.equal(pacing.maxNewFrames,1,'Stall recovery must not fast-forward multiple new gameplay frames in one pump');
      console.log('PACING',JSON.stringify(pacing));
    }
    for(let n=0;n<12;n++){
      const page=pages[n%2];await page.locator('#canvas').focus();
      await page.bringToFront();await page.evaluate(()=>window.dispatchEvent(new Event('focus')));
      await page.keyboard.down(n%2?'ArrowLeft':'ArrowRight');await page.keyboard.down('KeyZ');
      await page.waitForTimeout(300);await page.keyboard.up('KeyZ');await page.keyboard.up(n%2?'ArrowLeft':'ArrowRight');
    }
    await pages[0].waitForTimeout(4000);
    await pages[1].bringToFront();await pages[1].evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[1].locator('.touch-help-open').click();
    if(await pages[1].locator('[data-touch]').getAttribute('aria-pressed')!=='true')await pages[1].locator('[data-touch]').click();
    await pages[1].locator('[data-unlimited]').check();await pages[1].locator('[data-always-point]').check();
    await pages[1].locator('[data-save]').click();
    await pages[1].waitForTimeout(200);
    assert.equal(await pages[0].locator('.hit-points i:not([hidden])').count(),0);
    assert.equal(await pages[1].locator('.hit-points i:not([hidden])').count(),1);
    const presentation=await pages[1].evaluate(()=>({points:syncTestInputs.map(i=>i.points),touch:syncTestInputs.map(i=>i.touch)}));
    assert(presentation.points.every(v=>v===false)&&presentation.touch.every(v=>v<2**42),'Local marker preferences must not enter network frames');
    const surface=await pages[1].locator('.touch-surface').boundingBox();
    await pages[1].mouse.move(surface.x+surface.width*.65,surface.y+surface.height*.55);await pages[1].mouse.down();
    await pages[1].mouse.move(surface.x+surface.width*.75,surface.y+surface.height*.65,{steps:10});
    await pages[1].waitForTimeout(250);await pages[1].mouse.up();
    await pages[1].waitForTimeout(300);
    const fire=await pages[1].locator('[data-charge]').boundingBox();
    await pages[1].mouse.move(fire.x+fire.width/2,fire.y+fire.height/2);await pages[1].mouse.down();
    await pages[1].waitForTimeout(1000);await pages[1].mouse.up();await pages[1].waitForTimeout(500);
    // Local settings and DOM focus must never influence shared pause state.
    await pages[0].locator('#controls').click();
    assert(await pages[0].locator('#focus-enabled-0').isDisabled());
    await pages[0].locator('#focus-points-0').uncheck();
    await pages[1].bringToFront();await pages[1].evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[1].locator('#canvas').focus();await pages[1].keyboard.press('Escape');
    for(const page of pages)await page.waitForFunction(()=>th03SyncState.pauseSeat===1);
    assert.equal(await pages[1].locator('#pause-title').textContent(),'2P 暂停');
    assert.equal(await pages[1].locator('[data-choice=resume]').textContent(),'继续');
    assert.equal(await pages[1].locator('[data-choice=match]').textContent(),'背水阵');
    const ticks=await pages[0].evaluate(()=>th03SyncState.tick);
    const musicBefore=await pages[0].evaluate(()=>syncTestMusic());
    await pages[1].locator('#pause-menu [data-choice=round]').focus();
    await pages[0].waitForTimeout(600);
    assert.equal(await pages[0].evaluate(()=>th03SyncState.tick),ticks,'Pause must stop native time');
    const musicAfter=await pages[0].evaluate(()=>syncTestMusic());
    assert.equal(musicAfter.id,musicBefore.id);assert(musicAfter.time>musicBefore.time+.4);
    await pages[1].keyboard.press('ArrowDown');
    for(const page of pages)await page.waitForFunction(()=>document.querySelector('[data-choice=round]').dataset.selected==='true');
    await pages[1].evaluate(()=>document.activeElement.blur());
    await pages[1].keyboard.press('ArrowUp');
    for(const page of pages)await page.waitForFunction(()=>document.querySelector('[data-choice=resume]').dataset.selected==='true');
    await pages[1].evaluate(()=>document.activeElement.blur());
    await pages[1].keyboard.press('Enter');
    for(const page of pages)await page.waitForFunction(t=>th03SyncState.pauseSeat===null&&th03SyncState.tick>t,ticks);
    await pages[0].evaluate(()=>document.getElementById('settings').close());
    // Ordered delay plus a one-off stall: both instances must wait and recover.
    await pages[1].evaluate(()=>{
      const channel=syncTestNetwork.channel,send=channel.send.bind(channel);let pending=Promise.resolve(),first=true;
      channel.send=value=>{const delay=first?900:35;first=false;
        pending=pending.then(()=>new Promise(r=>setTimeout(r,delay))).then(()=>send(value));};
    });
    await pages[0].waitForTimeout(1800);
    await pages[1].evaluate(()=>syncTestNetwork.command('hidden'));
    for(const page of pages)await page.waitForFunction(()=>th03SyncState.hidden.includes(1),null,{timeout:15000});
    const hiddenTick=await pages[0].evaluate(()=>th03SyncState.tick);
    await pages[0].waitForTimeout(500);
    assert.equal(await pages[0].evaluate(()=>th03SyncState.tick),hiddenTick);
    await pages[1].evaluate(()=>syncTestNetwork.command('visible'));
    for(const page of pages)await page.waitForFunction(t=>!th03SyncState.hidden.length&&th03SyncState.tick>t,hiddenTick,{timeout:15000});
    await pages[1].bringToFront();await pages[1].evaluate(()=>window.dispatchEvent(new Event('focus')));
    await pages[1].locator('#canvas').focus();await pages[1].keyboard.press('Escape');
    for(const page of pages)await page.waitForFunction(()=>th03SyncState.pauseSeat===1);
    await pages[1].locator('#pause-menu [data-choice=match]').click();
    for(const page of pages)await page.waitForFunction(()=>th03SyncState.pauseSeat===null&&th03SyncState.phase!==1,null,{timeout:20000});
    await pages[0].waitForTimeout(5000);
    const states=await Promise.all(pages.map(p=>p.evaluate(()=>({state:window.th03SyncState,status:document.getElementById('status').textContent}))));
    assert(states.every(s=>!s.status.includes('错误')),JSON.stringify(states));
    assert(states.every(s=>s.state.frame>1200),'Game must advance beyond startup');
    assert.deepEqual(errors,[]);
    const out=resolve('reports/sync-check');mkdirSync(out,{recursive:true});
    for(let i=0;i<2;i++)await pages[i].screenshot({path:resolve(out,i===hostSeat?'host.png':'guest.png'),fullPage:true});
    await api('leave',host);
    console.log('PASS: two real NP21 instances, different viewport/DPR, character select, gameplay, touch/charge/point, local settings, pause/resume, delayed transport, visibility and match surrender with full-state checks');
  }finally{await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
