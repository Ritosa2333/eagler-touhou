const {chromium}=require('C:/Users/14915/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules/playwright');
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict');
const root=path.resolve(__dirname,'..'),web=path.join(root,'web'),origin='http://127.0.0.1:20404';
const runtime=process.env.RUNTIME||'main',language=process.env.LANGUAGE||'jp',entry=process.env.ENTRY||'stage',stage=Number(process.env.STAGE||0),phase=Number(process.env.PHASE||0),record=process.env.RECORD==='1';
const character=Number(process.env.CHARACTER||0),branch=process.env.BRANCH||'auto';
const power=Number(process.env.POWER||128);
// Test-only native memory probe, installed by route in both execution modes.
// The optional pickup creates a real item for the native collision/award path.
const resourceProbe=`function(type){
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer),d=emulator.practice.debug.d;
  if(type!==undefined){
    const item=d+0xaf34;h.fill(0,item,item+20);h[item]=1;h[item+14]=type;v.setUint16(item+16,44+type,true);
    for(const [source,targets]of [[0x464e,[2,6]],[0x4650,[4,8]]])for(const target of targets)v.setUint16(item+target,v.getUint16(d+source,true),true);
  }
  const table=v.getUint16(d+0x449c,true),level=h[d+0x4665],shots=[];
  for(let i=0;i<68;i++){const a=d+0xb55e+i*18;if(h[a])shots.push({flag:h[a],damage:h[a+16],pat:v.getUint16(a+14,true)});}
  const hazards={};for(const [name,offset,stride,count]of [['bullets',0x5a22,26,440],['lasers',0x42f0,24,2],['gather',0x9292,42,16],['items',0xaf34,20,32],['circles',0x9594,10,16],['custom',0xb204,26,32]]){
    hazards[name]=[];for(let i=0;i<count;i++)if(h[d+offset+stride*i])hazards[name].push(i);
  }
  return {power:h[d+0x4664],level,shotFunction:v.getUint16(d+0x449a,true),expectedShotFunction:v.getUint16(d+table+2*level,true),dream:v.getUint16(d+0xbccc,true)*10,dreamItems:h[d+0x4676],shots,hazards,cleanup:emulator.practice.debug.entryCleanup,phaseFrame:v.getUint16(d+0x53da,true),midFrame:v.getUint16(d+0x53c6,true)};
}`;
const report={runtime,language,entry,stage,phase,character,branch,power,record,passed:false,errors:[]},stem=`practice-${runtime}-${language}-${stage}-${entry}-${phase}-${character}-${branch}${record?'-rep':''}${process.env.OUTCOME?'-'+process.env.OUTCOME:''}${process.env.CPU_CYCLES?'-cpu'+process.env.CPU_CYCLES:''}${process.env.HELPERS?'-helpers':''}${process.env.SPEED_TEST?'-speed':''}${process.env.RETRY?'-retry':''}${process.env.LAYOUT_TEST?'-layout':''}${process.env.TIMELOCK_TEST?'-timelock':''}${process.env.INPUT_TEST?'-input':''}${process.env.POWER?'-power'+power:''}${process.env.RESOURCE_TEST?'-resources':''}`;
const mime={'.html':'text/html','.js':'text/javascript','.mjs':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css','.bmp':'image/bmp','.png':'image/png'};
(async()=>{let browser,page;try{
  browser=await chromium.launch({executablePath:'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true});
  const context=await browser.newContext({acceptDownloads:true});
  await context.route(origin+'/**',async route=>{
    const p=path.resolve(web,'.'+decodeURIComponent(new URL(route.request().url()).pathname));
    if(!p.startsWith(web+path.sep)||!fs.existsSync(p)||!fs.statSync(p).isFile())return route.fulfill({status:404,body:p});
    let body=fs.readFileSync(p);
    if(p===path.join(web,'solo-worker.js'))body=Buffer.from(body.toString().replace("if(method==='soloFault')",`if(method==='practiceResources')return (${resourceProbe})(args[0]);if(method==='soloFault')`));
    if(p===path.join(web,'practice-engine.js')&&process.env.PREP_SPEED)body=Buffer.from(body.toString().replace(/setSpeed\(8\)/g,`setSpeed(${Number(process.env.PREP_SPEED)})`));
    if(p===path.join(web,'practice-engine.js')&&process.env.TRACE)body=Buffer.from(body.toString().replace('locate(packed);',`locate(packed);globalThis.practiceTrace??=[];if(port===0x7fe1){const s=status(),id=[s.stage,s.phase,s.bossUpdate,s.bossHp,s.phaseEndHp].join(':');if(globalThis.practiceTrace.at(-1)?.id!==id){globalThis.practiceTrace.push({id,frame:s.frames,phaseFrame:s.phaseFrame});if(globalThis.practiceTrace.length>100)globalThis.practiceTrace.shift();}}`));
    if(p===path.join(web,'dosbox-disk.js')&&process.env.CPU_CYCLES)body=Buffer.from(body.toString().replace('practice?16000',`practice?${Number(process.env.CPU_CYCLES)}`));
    if(p===path.join(web,'solo.js'))body=Buffer.from(body.toString()+`\nglobalThis.practiceQA={read:()=>({state:readSoloState(),practice:emulator?.isWorker?emulator.snapshot.practice:emulator?.practice?.status,debug:emulator?.practice?.debug,status:$('status').textContent,replayStatus:$('replay-status').textContent,record:recorder?.state,core:emulator?.state,playback,frames:emulator?.module.localFrameCount,music:soloMusic?{context:soloMusic.context.state,track:soloMusic.player.active?.id,generation:soloMusic.player.generation,rate:soloMusic.player.active?.source?.playbackRate.value,status:soloMusic.player.status,sequence:soloMusic.reader?.sequence}:null}),core:()=>emulator,file:()=>recordBlob};`);
    if(p===path.join(web,'solo.js'))body=Buffer.from(body.toString()+`\nglobalThis.practiceQA.resources=type=>emulator.isWorker?emulator.call('practiceResources',[type]):(${resourceProbe})(type);`);
    return route.fulfill({contentType:mime[path.extname(p)]||'application/octet-stream',body});
  });
  page=await context.newPage();page.on('pageerror',e=>report.errors.push(e.stack));page.on('console',m=>{if(m.type()==='error')console.log('CONSOLE',m.text());});
  await page.goto(origin+'/solo.html');await page.waitForFunction(()=>globalThis.practiceQA);
  await page.selectOption('#language',language);await page.selectOption('#runtime-mode',runtime);await page.selectOption('#audio-mode','independent');
  await page.locator('#record-enabled').setChecked(record);
  await page.click('#practice-open');
  await page.selectOption('[data-jump=character]',String(character));await page.selectOption('[data-jump=stage]',String(stage));await page.selectOption('[data-jump=entry]',entry);await page.selectOption('[data-jump=branch]',branch,{force:true});if(entry!=='stage')await page.selectOption('[data-jump=phase]',String(phase));
  if(process.env.HP)await page.fill('[data-jump=hp]',process.env.HP);
  await page.fill('[data-jump=power]',String(power));
  await page.click('[data-practice-jump]');
  if(process.env.PROGRESS)for(let i=0;i<4;i++){await page.waitForTimeout(5000);console.log('PROGRESS',JSON.stringify(await page.evaluate(()=>practiceQA.read())));if((await page.evaluate(()=>practiceQA.read())).practice?.ready)break;}
  await page.waitForFunction(()=>practiceQA.read().practice?.ready||practiceQA.read().practice?.error||/启动失败/.test(practiceQA.read().status),null,{timeout:Number(process.env.TIMEOUT||90000)});
  report.arrival=await page.evaluate(()=>practiceQA.read());console.log('ARRIVAL',JSON.stringify(report.arrival));
  assert(report.arrival.practice?.ready,'practice must reach target');
  await page.waitForFunction(()=>practiceQA.read().state?.mode===2,null,{timeout:10000});
  await page.locator('canvas').screenshot({path:path.join(root,'reports',stem+'-arrival.png')});
  assert.equal(report.arrival.practice.lives,0);assert.equal(report.arrival.practice.bombs,0);
  report.resources=await page.evaluate(()=>practiceQA.resources());
  assert.equal(report.resources.power,power);assert.equal(report.resources.level,[6,12,16,24,32,48,72,96,128].filter(n=>power>=n).length);
  assert.equal(report.resources.shotFunction,report.resources.expectedShotFunction);assert(report.resources.shotFunction>0);
  assert.equal(report.resources.dream,0);assert.equal(report.resources.dreamItems,0);
  if(entry!=='stage'){
    for(const pool of ['bullets','lasers','gather','items','circles'])assert.deepEqual(report.resources.hazards[pool],[],`Arrival must remove old ${pool}`);
    const allowed=stage===3&&entry==='boss'&&character<2?[0,1,2,3]:[];
    assert(report.resources.hazards.custom.every(i=>allowed.includes(i)),'Arrival must remove old custom emitters');
    assert(report.resources.cleanup,'Arrival cleanup callback must have run');
  }
  if(process.env.ARRIVAL_ONLY){report.passed=true;console.log('PASS arrival',stem);return;}
  if(process.env.HAZARD_TEST)await page.evaluate(()=>practiceQA.core().practiceOptions({invincible:true,enemyInvincible:true,lockTime:true}));
  await page.locator('canvas').focus();await page.keyboard.press('z',{delay:140});
  await page.waitForFunction(()=>practiceQA.read().state?.mode===1&&!practiceQA.read().practice.waiting,null,{timeout:15000});
  report.resumed=await page.evaluate(()=>practiceQA.read());
  assert.equal(report.resumed.practice.invincibility,0);
  await page.waitForFunction(()=>{const m=practiceQA.read().music;return m?.rate===1&&m.context==='running';},null,{timeout:30000});
  report.music=await page.evaluate(()=>practiceQA.read().music);assert.equal(report.music.context,'running');
  if(process.env.HAZARD_TEST){
    assert(!record,'Hazard observation uses assisted play, not the REP flow');
    for(let i=0;i<180;i++){
      report.attackResources=await page.evaluate(()=>practiceQA.resources());
      const h=report.attackResources.hazards;
      if(process.env.ENTITY_TEST?(stage===3?h.custom.length===4:h.custom.includes(31)):h.bullets.length+h.lasers.length+h.gather.length+h.custom.filter(i=>!(stage===3&&character<2&&i<4)).length)break;
      await page.waitForTimeout(200);
    }
    const h=report.attackResources.hazards;
    assert(h.bullets.length+h.lasers.length+h.gather.length+h.custom.filter(i=>!(stage===3&&character<2&&i<4)).length>0,'Selected attack must produce its own native projectiles after resume');
    if(process.env.ENTITY_TEST)assert(stage===3?h.custom.length===4:h.custom.includes(31),'Native Boss entities must still spawn');
    report.attack=await page.evaluate(()=>practiceQA.read());assert.equal(report.attack.practice.outcome,null);
    if(phase)assert.equal(entry==='mid'?report.attack.practice.midPhase:report.attack.practice.phase,phase);
    await page.locator('canvas').screenshot({path:path.join(root,'reports',stem+'-attack.png')});
    assert.deepEqual(report.errors,[]);report.passed=true;console.log('PASS clean entry and native attack',JSON.stringify({cleanup:report.resources.cleanup,arrival:report.resources.hazards,attack:h}));return;
  }
  if(process.env.RESOURCE_TEST){
    assert(!record,'Native pickup injection is not a recorded gameplay test');
    await page.evaluate(()=>practiceQA.core().practiceOptions({invincible:true,enemyInvincible:true,lockTime:true}));
    await page.keyboard.down('z');await page.waitForTimeout(700);report.firing=await page.evaluate(()=>practiceQA.resources());await page.keyboard.up('z');
    assert(report.firing.shots.length>0,'Selected shot function must actually produce native shots');
    assert.equal(report.firing.shotFunction,report.resources.expectedShotFunction);
    if(power<128){
      await page.evaluate(()=>practiceQA.resources(0));await page.waitForTimeout(300);report.pickup=await page.evaluate(()=>practiceQA.resources());
      assert.equal(report.pickup.power,power+1);assert.equal(report.pickup.level,[6,12,16,24,32,48,72,96,128].filter(n=>power+1>=n).length);
      assert.equal(report.pickup.shotFunction,report.pickup.expectedShotFunction);
      await page.evaluate(()=>practiceQA.core().practiceOptions({lockPower:true}));await page.waitForTimeout(100);
      await page.evaluate(()=>practiceQA.resources(0));await page.waitForTimeout(300);report.locked=await page.evaluate(()=>practiceQA.resources());
      assert.equal(report.locked.power,report.pickup.power);assert.equal(report.locked.level,report.pickup.level);assert.equal(report.locked.shotFunction,report.pickup.shotFunction);
      await page.evaluate(()=>practiceQA.core().practiceOptions({lockPower:false}));
    }
    await page.evaluate(()=>practiceQA.resources(2));await page.waitForTimeout(300);report.dreamPickup=await page.evaluate(()=>practiceQA.resources());
    assert.equal(report.dreamPickup.dreamItems,1);assert.equal(report.dreamPickup.dream,1000,'Dream must grow naturally after the zero initial state');
    await page.locator('canvas').screenshot({path:path.join(root,'reports',stem+'-resources.png')});
    assert.deepEqual(report.errors,[]);report.passed=true;console.log('PASS native resources',JSON.stringify({power,resources:report.resources,pickup:report.pickup,locked:report.locked,dream:report.dreamPickup.dream}));return;
  }
  await page.keyboard.press('F1');await page.waitForTimeout(200);assert.equal((await page.evaluate(()=>practiceQA.read())).practice.options.invincible,false);
  await page.keyboard.press('Backspace');await page.keyboard.press('F1');await page.keyboard.press('Backspace');await page.waitForTimeout(300);
  assert.equal((await page.evaluate(()=>practiceQA.read())).practice.options.invincible,true);
  if(process.env.INPUT_TEST){
    await page.evaluate(()=>practiceQA.core().practiceOptions({lockBombs:true,disableBomb:true,disableShot:true,forceFocus:true}));
    await page.keyboard.down('z');await page.keyboard.down('x');await page.keyboard.down('Shift');await page.waitForTimeout(700);
    report.disabledInputs=await page.evaluate(()=>practiceQA.read());assert.equal(report.disabledInputs.practice.keys&48,0);assert.equal(report.disabledInputs.practice.bombsUsed,0);assert.equal(report.disabledInputs.practice.focus,1);
    await page.evaluate(()=>practiceQA.core().practiceOptions({disableFocus:true}));await page.waitForTimeout(500);report.disabledFocus=await page.evaluate(()=>practiceQA.read());assert.equal(report.disabledFocus.practice.focus,0);
    for(const key of ['z','x','Shift'])await page.keyboard.up(key);await page.evaluate(()=>practiceQA.core().practiceOptions({lockBombs:false,disableBomb:false,disableShot:false,forceFocus:false,disableFocus:false}));
  }
  if(process.env.TIMELOCK_TEST){
    assert.equal(stage,0);assert([2,3].includes(phase));await page.evaluate(()=>practiceQA.core().practiceOptions({lockTime:true,speed:2}));
    const required=phase===3?1700:3000,start=(await page.evaluate(()=>practiceQA.read())).practice.elapsed;
    await page.waitForFunction(v=>practiceQA.read().practice.elapsed>=v,start+required,{timeout:150000});
    report.timeLocked=await page.evaluate(()=>practiceQA.read());assert.equal(report.timeLocked.practice.phase,phase);assert.equal(report.timeLocked.practice.outcome,null);assert.equal(report.timeLocked.practice.midPhase,255,'Boss-only practice must not activate a stage midboss');
    assert(report.timeLocked.practice.phaseFrame<(phase===3?1501:65536));await page.evaluate(()=>practiceQA.core().practiceOptions({speed:1}));
  }
  if(process.env.SPEED_TEST){
    report.speeds=[];const music=await page.evaluate(()=>practiceQA.read().music);
    for(const speed of [1,.5,2]){
      await page.evaluate(value=>practiceQA.core().practiceOptions({speed:value,lockTime:true}),speed);await page.waitForTimeout(300);
      const a=await page.evaluate(()=>({p:practiceQA.read().practice,at:performance.now()}));await page.waitForTimeout(2500);const b=await page.evaluate(()=>({p:practiceQA.read().practice,at:performance.now(),music:practiceQA.read().music}));
      report.speeds.push({speed,frames:b.p.elapsed-a.p.elapsed,ms:b.at-a.at,music:b.music});assert.equal(b.music.rate,1);assert.equal(b.music.generation,music.generation);
    }
    console.log('SPEEDS',JSON.stringify(report.speeds));const rates=report.speeds.map(n=>n.frames/n.ms);assert(rates[1]/rates[0]>.3&&rates[1]/rates[0]<.8);assert(rates[2]/rates[0]>1.4);await page.evaluate(()=>practiceQA.core().practiceOptions({speed:1}));
  }
  await page.keyboard.press('F12');await page.waitForTimeout(300);assert.equal((await page.evaluate(()=>practiceQA.read())).state.mode,2);
  report.layout=await page.evaluate(()=>{let s=document.querySelector('.solo-screen').getBoundingClientRect(),p=document.querySelector('.practice-panel').getBoundingClientRect();return {screen:[s.x,s.y,s.width,s.height],panel:[p.x,p.y,p.width,p.height]};});
  await page.locator('#game').screenshot({path:path.join(root,'reports',stem+'.png')});
  if(process.env.LAYOUT_TEST){
    const box=()=>page.evaluate(()=>{const s=document.querySelector('.solo-screen').getBoundingClientRect(),p=document.querySelector('.practice-panel').getBoundingClientRect(),n=document.querySelector('.practice-panel');return {screen:[s.x,s.y,s.width,s.height],panel:[p.x,p.y,p.width,p.height],scroll:n.scrollHeight,client:n.clientHeight};});
    report.layouts=[];for(const size of [{width:390,height:844},{width:844,height:390},{width:1280,height:720}]){
      await page.setViewportSize(size);await page.waitForTimeout(250);const b=await box();report.layouts.push({size,...b});for(let i=0;i<4;i++)assert(Math.abs(b.screen[i]-b.panel[i])<2);assert(b.scroll>b.client);
      await page.locator('[data-practice="speed"]').scrollIntoViewIfNeeded();await page.locator('#game').screenshot({path:path.join(root,'reports',stem+'-'+size.width+'.png')});
    }
    await page.locator('[data-jump="power"]').focus();await page.keyboard.press('F12');assert.equal(await page.locator('.practice-panel').evaluate(n=>n.hidden),true);
    await page.keyboard.press('F12');await page.evaluate(()=>document.getElementById('game').requestFullscreen());await page.waitForTimeout(300);report.fullscreen=await box();for(let i=0;i<4;i++)assert(Math.abs(report.fullscreen.screen[i]-report.fullscreen.panel[i])<2);await page.evaluate(()=>document.exitFullscreen());
  }
  await page.click('[data-close-practice]');await page.keyboard.press('z',{delay:140});
  if(process.env.OUTCOME==='clear')await page.keyboard.down('z');
  else {await page.keyboard.press('Backspace');await page.keyboard.press('F1');await page.keyboard.press('Backspace');}
  if(process.env.HELPERS){
    await page.keyboard.press('Backspace');await page.keyboard.press('F2');await page.keyboard.press('F3');await page.keyboard.press('Backspace');
    await page.waitForFunction(()=>{const p=practiceQA.read().practice;return p.misses>0&&p.missTimer>0&&p.missTimer<=32;},null,{timeout:45000});report.death=await page.evaluate(()=>practiceQA.read());
    await page.waitForFunction(()=>{const p=practiceQA.read().practice;return p.misses>0&&!p.missTimer&&p.invincibility>0;},null,{timeout:45000});report.respawn=await page.evaluate(()=>practiceQA.read());
    assert.equal(report.respawn.practice.lives,0);assert.equal(report.respawn.practice.bombs,0);assert(report.respawn.practice.power<128);assert.equal(report.respawn.practice.outcome,null);
    await page.keyboard.press('x',{delay:300});await page.waitForFunction(()=>practiceQA.read().practice.bombsUsed>0,null,{timeout:10000});report.bomb=await page.evaluate(()=>practiceQA.read());assert.equal(report.bomb.practice.bombs,0);
    await page.keyboard.press('Backspace');for(const key of ['F1','F4','F5','u'])await page.keyboard.press(key);await page.keyboard.press('Backspace');
    const miss=report.bomb.practice.misses;await page.waitForFunction(n=>{const p=practiceQA.read().practice;return p.misses>n&&!p.invincibility&&!p.missTimer;},miss,{timeout:90000});report.consumedHit=await page.evaluate(()=>practiceQA.read());assert.equal(report.consumedHit.practice.outcome,null);
    const hp=report.consumedHit.practice.bossHp;await page.keyboard.down('z');await page.waitForTimeout(1500);await page.keyboard.up('z');assert.equal((await page.evaluate(()=>practiceQA.read())).practice.bossHp,hp);
  }
  if(process.env.OUTCOME||record){
    await page.waitForFunction(()=>practiceQA.read().practice?.outcome,null,{timeout:150000});report.ended=await page.evaluate(()=>practiceQA.read());console.log('ENDED',JSON.stringify(report.ended));
    await page.waitForFunction(()=>document.getElementById('practice-result-dialog').open);await page.keyboard.press('ArrowUp');
    assert.equal(await page.locator('[data-practice-other]').evaluate(n=>n.classList.contains('selected')),true);
    if(record){
      await page.waitForFunction(()=>practiceQA.read().record?.ready||practiceQA.read().record?.failed,null,{timeout:60000});report.sealed=await page.evaluate(()=>practiceQA.read());assert(report.sealed.record.ready,report.sealed.replayStatus);
      assert.equal(await page.locator('[data-practice-other]').evaluate(n=>n.classList.contains('selected')),true,'sealing must preserve selection');
      const bytes=await page.evaluate(async()=>Array.from(new Uint8Array(await practiceQA.file().arrayBuffer()))),file=path.join(root,'reports',stem+'.th04r');fs.writeFileSync(file,Buffer.from(bytes));report.fileBytes=bytes.length;
      const viewer=await context.newPage();viewer.on('pageerror',e=>report.errors.push(e.stack));await viewer.goto(origin+'/solo.html');await viewer.setInputFiles('#replay-file',file);await viewer.waitForFunction(()=>!document.getElementById('replay-play').disabled);await viewer.click('#replay-play');
      await viewer.waitForFunction(()=>/校验一致|不一致|播放失败|启动失败/.test(practiceQA.read().status+' '+practiceQA.read().replayStatus),null,{timeout:180000});report.playback=await viewer.evaluate(()=>practiceQA.read());assert.match(report.playback.status,/终态内存校验一致/);await viewer.close();
    }
    if(process.env.RETRY){
      await page.click('[data-practice-other]');await page.waitForTimeout(700);assert.equal(await page.locator('#practice-result-dialog').evaluate(n=>n.open),false);
      await page.selectOption('[data-jump=stage]','1');await page.click('[data-close-practice]');await page.keyboard.press('Backspace');await page.keyboard.press('Shift+f');
      await page.waitForFunction(()=>{const p=practiceQA.read().practice;return p?.waiting&&!p.outcome;},null,{timeout:90000});report.retried=await page.evaluate(()=>practiceQA.read());assert.equal(report.retried.practice.setup.stage,stage);assert.equal(report.retried.practice.setup.phase,phase);
    }
  }
  assert.deepEqual(report.errors,[]);report.passed=true;console.log('PASS',stem);
}catch(error){report.failure=error.stack;console.error(error);if(page){report.last=await page.evaluate(()=>globalThis.practiceQA?.read()).catch(()=>null);report.trace=await page.evaluate(()=>globalThis.practiceTrace);report.logs=await page.evaluate(()=>practiceQA.core()?.logs);report.fault=await page.evaluate(async()=>{const c=practiceQA.core();return c.isWorker?c.call('soloFault'):(await import('/solo-fault.js')).readSoloFault(c.module.HEAPU8,c.module);}).catch(()=>null);await page.evaluate(()=>{const n=document.querySelector('.practice-preparing');if(n)n.hidden=true;});await page.screenshot({path:path.join(root,'reports',stem+'-failure.png')});}}
finally{fs.mkdirSync(path.join(root,'reports'),{recursive:true});fs.writeFileSync(path.join(root,'reports',stem+'.json'),JSON.stringify(report,null,2));await browser?.close();if(!report.passed)process.exitCode=1;}})();
