// Requires the main Launcher with th02,th04 development packages enabled.
// Exercise the actual browser controllers and native practice engines together.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';
const require = createRequire(new URL('../../launcher/package.json', import.meta.url));
const puppeteer = require('puppeteer-core');
const base = process.env.PC98_LAUNCHER_URL || 'http://127.0.0.1:18330/';
const games = process.env.PC98_PRACTICE_GAMES?.split(',') || ['th02','th04'];
const mobile = process.env.PC98_PRACTICE_MOBILE === '1';
const targetStage=Number(process.env.PC98_PRACTICE_STAGE||0),latePhase=process.env.PC98_PRACTICE_LATE_PHASE==='1';
const browser = await puppeteer.launch({executablePath:await findChromiumExecutable(), headless:true,
  args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
const wait = async (check, label, timeout = 90000) => {
  const deadline = Date.now() + timeout;
  while (Date.now() < deadline) { if (await check()) return; await new Promise(r => setTimeout(r,100)); }
  throw Error('Timeout: ' + label);
};
try {
  for (const game of games) {
    const page = await browser.newPage(), events = [], errors = [];
    const out = resolve(import.meta.dirname, '../../ports', game, '.cache/practice-browser');
    await mkdir(out, {recursive:true}); await page.setViewport({width:1360,height:900});
    if (mobile) {
      await page.setUserAgent('Mozilla/5.0 (Linux; Android 14) AppleWebKit/537.36 Chrome/140.0.0.0 Mobile Safari/537.36');
      await page.setViewport({width:900,height:700,isMobile:true,hasTouch:true});
    }
    page.on('pageerror', error => { errors.push(error.message); console.error(game,error.message); });
    await page.exposeFunction('recordPracticeEvent', event => events.push(event));
    await page.evaluateOnNewDocument(() => addEventListener('message', event => {
      if (event.data?.protocol === 'eagler-touhou/1') window.recordPracticeEvent(event.data);
    }));
    let frame;
    const read = () => frame.evaluate(async () => (await import('./managed-solo.mjs')).managedAPI.practiceState());
    const press = async code => { await frame.$eval('canvas', node => node.focus()); await page.keyboard.press(code,{delay:100}); };
    const click = async selector => {
      // Scroll the original panel like a user, keeping controls below its sticky header.
      await frame.$eval(selector,node=>node.scrollIntoView({block:'center',inline:'nearest'}));
      if(mobile)await frame.tap(selector);else await frame.click(selector);
    };
    try {
      await page.goto(base + '?test='+game);
      await page.waitForFunction(() => window.__eaglerBoot?.done === true, {timeout:45000});
      await page.evaluate(() => document.querySelector('#firstUseNoticeDialog[open] #firstUseNoticeClose')?.click());
      // Catalog hydration may replace the initial static rail. The first card
      // activation selects a cover; activating the selected cover opens tools.
      await wait(async () => {
        if (await page.$('.tools.is-open')) return true;
        await page.$eval(`.game[data-game="${game}"]:not([data-product])`, node => node.click());
        return false;
      },game+' card selection',15000);
      await page.waitForSelector('.tools.is-open',{visible:true});
      await page.$eval('.tools.is-open', async node => Promise.all(node.getAnimations().map(a => a.finished.catch(() => {}))));
      await page.select('#musicSelect',process.env.PC98_PRACTICE_MUSIC||'none');
      if (mobile) await page.$eval('#touchToggle', node => {if(node.getAttribute('aria-checked') !== 'true')node.click();});
      await page.waitForFunction(() => document.querySelector('#launch')?.disabled === false);
      await page.click('#launch');
      await wait(async () => {
        await page.evaluate(() => { document.querySelector('#decisionDialog[open] #decisionConfirm')?.click(); document.querySelector('#touchHelpClose')?.click(); });
        if (events.some(e => e.event === 'error')) throw Error(JSON.stringify(events.filter(e => e.event === 'error')));
        return events.some(e => e.event === 'first-frame');
      },game+' main Launcher start',120000);
      frame = page.frames().find(f => f.url().includes(game+'.html') && f.url().includes('runtimeEpoch='));
      await frame.waitForSelector('.managed-practice-open:not([hidden])',{visible:true});
      assert.equal((await read()).configuration.mode,'full');
      if (mobile) {
        assert.equal(await page.$eval('#touchDirectSurface',node => node.hidden),true,'host movement surface leaves browser controls clickable');
        await frame.tap('.managed-practice-open');
      } else await frame.click('.managed-practice-open');
      await frame.waitForSelector('#practice-panel.practice-full:not([hidden])',{visible:true});
      if(mobile){
        await page.waitForFunction(()=>document.querySelector('#player').classList.contains('browser-practice-open'));
        assert.equal(await page.$eval('#touchHud',node=>getComputedStyle(node).visibility),'hidden','host movement HUD cannot cover browser controllers');
      }
      console.log(game,'browser practice entry visible');
      await frame.select('[data-jump=character]','1');
      await frame.select('[data-jump=stage]',String(targetStage));
      await frame.select('[data-jump=entry]','boss');
      const phase = await frame.$$eval('[data-jump=phase] option', (nodes,late) => nodes.filter(n => n.value !== '0').at(late?-1:0).value,latePhase);
      await frame.select('[data-jump=phase]',phase);
      await click('[data-jump=power]');
      await page.keyboard.down('Control'); await page.keyboard.press('KeyA'); await page.keyboard.up('Control'); await page.keyboard.type('32');
      assert.equal(await frame.$eval('[data-jump=power]', node => node.value),'32','browser form keeps numeric typing');
      assert.equal(await frame.evaluate(() => __eaglerPC98Input.sample()),0,'form navigation never reaches the guest');
      await click('[data-practice=invincible]');
      await click('[data-practice=alwaysPoint]');
      const targetStarted=performance.now();await click('[data-practice-jump]');
      await wait(async () => {
        const state = await read();
        if (state.native?.practice?.error) throw Error(state.native.practice.error);
        return state.native?.practice?.ready && state.native?.practice?.waiting;
      },game+' actual target practice',180000);
      const target = await read();
      assert.equal(target.native.practice.setup.character,1);
      assert.equal(target.native.practice.setup.stage,targetStage);
      assert.equal(target.native.practice.setup.entry,'boss');
      assert.equal(target.native.practice.setup.phase,Number(phase));
      assert.equal(target.native.practice.power,32);
      assert.equal(target.native.practice.options.invincible,true);
      assert.equal(target.native.practice.options.alwaysPoint,true);
      console.log(game,'native target ready',JSON.stringify(target.native.practice.setup),'loadMs',Math.round(performance.now()-targetStarted));
      await press('Enter');
      await wait(async () => (await read()).native?.mode === 1,game+' resume native pause',15000);
      if(process.env.PC98_PRACTICE_FAILURE==='1'){
        await press('Backspace');await press('F1');await press('Backspace');
        await wait(async()=>!!(await read()).native?.practice?.outcome,game+' natural practice miss',180000);
        await frame.waitForSelector('#practice-result-dialog[open]',{visible:true,timeout:10000});
        assert.equal((await read()).native.practice.outcome,'failed');
        await click('[data-practice-again]');
        await wait(async()=>{const p=(await read()).native?.practice;return p?.ready&&p.waiting&&!p.outcome;},game+' restart after game over',120000);
        assert.equal(await frame.$eval('#practice-result-dialog',node=>node.open),false,'result dialog closes on retry');
        console.log('PASS',game,'natural miss, result and retry');continue;
      }
      await frame.waitForFunction(()=>/^FPS: [1-9]\d*$/.test(document.querySelector('#pc98-game-fps')?.textContent||''),{timeout:15000});
      const fps=await frame.$eval('#pc98-game-fps',node=>({text:node.textContent,style:getComputedStyle(node).pointerEvents}));
      assert.equal(fps.style,'none');assert(Number(fps.text.slice(5))>0,'running game advances its own frame counter');
      const before = (await read()).native.x;
      await frame.$eval('canvas', node => node.focus());
      await page.keyboard.down('ArrowRight'); await new Promise(r => setTimeout(r,300)); await page.keyboard.up('ArrowRight');
      assert((await read()).native.x > before,'canonical keyboard moves the native player');
      await press('Backspace');
      await frame.waitForSelector('#practice-panel:not([hidden]):not(.practice-full)',{visible:true});
      await press('F4');
      await wait(async () => (await read()).native?.practice?.options.lockPower === true,game+' live browser assist',10000);
      await press('Tab');
      await frame.waitForSelector('.practice-overlay:not([hidden])',{visible:true});
      const compact = await frame.evaluate(() => {
        const rect = selector => {const r=document.querySelector(selector).getBoundingClientRect();return {x:r.x,y:r.y,w:r.width,h:r.height};};
        return {screen:rect('.practice-screen'),panel:rect('#practice-panel'),overlay:rect('.practice-overlay'),viewport:{w:innerWidth,h:innerHeight}};
      });
      for (const name of ['panel','overlay']) {
        const r=compact[name],s=compact.screen;
        if(name==='panel')assert(r.x<s.x+s.w*.1,'compact menu follows TH05 top-left placement');
        else assert(r.x>=s.x+s.w*.65,'information stays on the native information side');
        assert(r.x+r.w <= s.x+s.w+2 && r.y >= s.y-2 && r.y+r.h <= s.y+s.h+2,name+' stays inside the picture');
      }
      assert(Math.abs(compact.screen.w/compact.screen.h-1.6)<.01,'original 640x400 picture ratio');
      if(mobile){
        const position=(await read()).native.x,header=await frame.$('#practice-panel .practice-switches label'),box=await header.boundingBox(),touch=await page.createCDPSession();
        const x=box.x+10,y=box.y+box.height/2;
        assert(await frame.$eval('#practice-panel .practice-switches label',node=>{const r=node.getBoundingClientRect();return !!document.elementFromPoint(r.x+10,r.y+r.height/2)?.closest('#practice-panel');}),'controller receives the drag at its visible header');
        await touch.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:1}]});
        await touch.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:x+25,y,id:1}]});
        await touch.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
        await new Promise(r=>setTimeout(r,200));
        assert.equal((await read()).native.x,position,'dragging a browser controller never moves the native player');
        await touch.detach();
      }
      await page.screenshot({path:resolve(out,'compact.png')});
      assert.equal((await read()).retry.state,'idle','no background emulator is created');
      // Draft setup edits must not replace the committed target on quick retry.
      await press('F12');
      await frame.select('[data-jump=stage]','1');
      await press('Backspace');
      await frame.$eval('canvas', node => node.focus());
      await page.keyboard.down('Shift'); await page.keyboard.press('KeyF',{delay:100}); await page.keyboard.up('Shift');
      await wait(async () => {
        const state = await read();
        return state.native?.practice?.ready && state.native.practice.waiting && state.native.practice.elapsed === 0;
      },game+' quick retry preserves target',90000);
      const retry = await read();
      assert.equal(retry.native.practice.setup.stage,targetStage);
      assert.equal(retry.native.practice.setup.phase,Number(phase));
      assert.equal(retry.native.practice.options.lockPower,true);
      assert.equal(retry.native.practice.options.invincible,true);
      assert.equal(retry.retry.state,'idle','retry does not leave a standby emulator');
      const parked = {x:retry.native.x,y:retry.native.y,elapsed:retry.native.practice.elapsed};
      await new Promise(r => setTimeout(r,700));
      const stillWaiting = (await read()).native;
      assert.deepEqual({x:stillWaiting.x,y:stillWaiting.y,elapsed:stillWaiting.practice.elapsed},parked,'prepared start stays frozen in original pause until resumed');
      console.log(game,'retry without background instance');
      await press('F12');
      await frame.waitForSelector('#practice-panel.practice-full:not([hidden])',{visible:true});
      assert.equal(await frame.$('[data-practice=preparedRetry]'),null);
      await page.setViewport({width:480,height:800,isMobile:mobile,hasTouch:mobile});
      await frame.$eval('#practice-panel',node=>{node.scrollTop=0;});
      const bounds = await frame.$eval('#practice-panel', node => {
        const r = node.getBoundingClientRect(), s = node.parentElement.getBoundingClientRect();
        return {panel:[r.x,r.y,r.width,r.height],screen:[s.x,s.y,s.width,s.height],viewport:[innerWidth,innerHeight]};
      });
      assert(Math.abs(bounds.panel[0]-(bounds.screen[0]+bounds.screen[2]*.40625))<3,'setup follows TH05 placement');
      assert(Math.abs(bounds.panel[2]-bounds.screen[2]*.5156)<3,'setup follows TH05 width');
      const [sx,sy,sw,sh]=bounds.screen;
      assert(sx>=-2&&sy>=-2&&sx+sw<=bounds.viewport[0]+2&&sy+sh<=bounds.viewport[1]+2,'picture stays visible inside the iframe viewport');
      assert(Math.abs(sw/sh-1.6)<.01,'narrow screen retains the original picture ratio');
      await page.screenshot({path:resolve(out,'practice-narrow.png')});
      await click('[data-close-practice]');
      await page.waitForFunction(()=>!document.querySelector('#player').classList.contains('browser-practice-open'));
      // The practice recording uses the existing recorder and main file store.
      await page.setViewport({width:mobile?900:1360,height:mobile?700:900,isMobile:mobile,hasTouch:mobile});
      await press('Enter');
      await wait(async () => (await read()).native?.mode === 1,game+' retry resumes',15000);
      if(mobile){
        const canvas=await frame.$('canvas'),box=await canvas.boundingBox(),touch=await page.createCDPSession();
        const x=box.x+box.width*.35,y=box.y+box.height*.75,beforeTouch=(await read()).native.x;
        await touch.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{x,y,id:1}]});
        await touch.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[{x:x+box.width*.08,y,id:1}]});
        await wait(async () => (await read()).native.x>beforeTouch,game+' native canvas touch movement',15000);
        await touch.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
        await touch.detach();
      }
      await frame.evaluate(async () => (await import('./managed-solo.mjs')).managedAPI.finishRecording());
      const saved = () => frame.evaluate(game => [...__eaglerPC98Files.memory].filter(([name]) => name.endsWith('.'+game+'r')).map(([name,bytes]) => ({name,size:bytes.length})),game);
      await wait(async () => {
        const status=await frame.evaluate(()=>document.querySelector('#replay-status,#record-status')?.textContent||'');
        if(status.includes('录制已停止'))throw Error(status);
        return (await saved()).length>0;
      },game+' practice REP in main storage',45000);
      const files = await saved();
      if(process.env.PC98_EXPORT_REPLAY==='1'){const bytes=await frame.evaluate(name=>Array.from(__eaglerPC98Files.memory.get(name)),files[0].name);await writeFile(resolve(out,game+'-sample.rep'),new Uint8Array(bytes));}
      await frame.evaluate(async () => (await import('./managed-solo.mjs')).managedAPI.saveRecording());
      assert.deepEqual(await saved(),files,'saving again keeps the same practice file');
      console.log(game,'practice REP stored',JSON.stringify(files));
      assert.deepEqual(errors,[]);
      assert.deepEqual(events.filter(e => e.event === 'error'),[]);
      await writeFile(resolve(out,mobile?'result-mobile.json':'result.json'),JSON.stringify({passed:true,target,retry,compact,bounds,files,errors},null,2));
      console.log('PASS',game,'main entry, native target, form isolation, hotkeys, assists, retry and narrow layout');
    } catch (error) {
      await page.screenshot({path:resolve(out,'failure.png')});
      await writeFile(resolve(out,'failure.json'),JSON.stringify({events,errors,state:frame ? await read().catch(e => ({error:e.message})) : null},null,2));
      throw error;
    } finally { await page.close(); }
  }
} finally { await browser.close(); }
