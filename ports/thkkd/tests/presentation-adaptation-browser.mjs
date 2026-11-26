import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/adaptation-20261127/presentation');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/presentation-adaptation-browser/1',checks:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report});const {browser,page}=driver;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
  await page.evaluate(()=>{
    for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(key=>key.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.keyboardPreferences){session.settings.setOption('frameLimit60Enabled',false);return;}fiber=fiber.return;}}
    throw Error('Launcher session missing');
  });
  await page.click('#libraryBack');const frame=await driver.start({expectedBuildId:JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId});
  const replay=replayFixture({count:360,bombFrame:-1});for(let i=0;i<360;i++)replay[i]=32|(Math.floor(i/45)%2?2:4);
  await runtimeCommand(page,'write',{path:'replay/presentation-check.rpy',bytes:[...replay]});
  await frame.evaluate(()=>{const renderer=__thkkdRuntime.platform.renderer,present=renderer.present.bind(renderer);window.__presentationAlphas=[];renderer.present=(alpha=0,...rest)=>{__presentationAlphas.push(alpha);return present(alpha,...rest);};});
  await runtimeCommand(page,'playback',{action:'open',path:'replay/presentation-check.rpy'});await wait(400);await runtimeCommand(page,'playback',{action:'pause'});
  report.replayAlphas=await frame.evaluate(()=>__presentationAlphas.slice(-80));assert.ok(report.replayAlphas.some(alpha=>alpha>0&&alpha<=1));
  await runtimeCommand(page,'playback',{action:'seek',value:70});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===70);
  const visual=await frame.evaluate(async()=>{
    const r=__thkkdRuntime,renderer=r.platform.renderer,canvas=document.querySelector('canvas');
    const before=r.core.HEAPU8.slice();renderer.present(0);const current=canvas.toDataURL();renderer.present(.5);const interpolated=canvas.toDataURL();
    for(let i=0;i<100;i++)renderer.present((i%10)/10);
    const after=r.core.HEAPU8.slice();const digest=async bytes=>Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),byte=>byte.toString(16).padStart(2,'0')).join('');
    return {current,interpolated,memoryBefore:await digest(before),memoryAfter:await digest(after),identities:[...new Set(renderer.commands.map(command=>command.identity).filter(Boolean))],commands:renderer.commands.length};
  });
  assert.equal(visual.memoryBefore,visual.memoryAfter);assert.notEqual(visual.current,visual.interpolated);
  for(const name of ['current','interpolated'])writeFileSync(resolve(output,name+'.png'),Buffer.from(visual[name].split(',')[1],'base64'));
  report.visual={...visual,current:undefined,interpolated:undefined};report.checks.push('DISPLAY-03-extra-presents-preserve-complete-WASM-memory','DISPLAY-04-replay-presentation-uses-current-motion');
  report.cadences=await frame.evaluate(async replay=>{
    const {createLocalReplay}=await import(new URL('./local-replay.mjs',location.href)),{decodeReplay}=await import(new URL('./replay.mjs',location.href));
    const r=__thkkdRuntime,description=(await decodeReplay(Uint8Array.from(replay))).description,rows=[];
    const signature=()=>{let hash=2166136261;const word=value=>{hash=Math.imul(hash^value,16777619)>>>0;};for(const address of [0xa74c08,0xb3e2e8,0xb43110,0xa78e2c,0xa8ce58])word(r.core._thkkd_word(address));for(let i=0;i<60;i++)word(r.core._thkkd_word(0xa8cd68+i*4));for(let i=0;i<80;i++)for(let j=0;j<62;j++)word(r.core._thkkd_word(0xa87fe8+i*248+j*4));for(let i=0;i<3000;i++)for(let j=0;j<60;j++)word(r.core._thkkd_word(0xa8e660+i*240+j*4));return hash.toString(16);};
    for(const hz of [60,120,144,240]){
      let time=0,next=0,steps=0,presents=0;const queue=new Map();r.platform.setSeeking(true);
      const state=()=>{const cursor=r.core._thkkd_word(0xb43110);return {frame:cursor,total:359,ended:cursor>=359,stage:0,startStage:0};};
      const bridge=createLocalReplay({inspect:async()=>description,open:async()=>{
        r.input.clear();r.input.replaying=true;r.input.replayMotion=null;r.core._thkkd_stop();r.platform.resetGame();if(r.core._thkkd_start()<0)throw Error('Native start failed');
        r.core.FS.writeFile('/savesthkkd/replay/pacing.rpy',Uint8Array.from(replay));r.core.HEAPU8.set(new TextEncoder().encode('replay/pacing.rpy\0'),r.core._thkkd_replay_path());if(r.core._thkkd_replay_open()<0)throw Error('Native Replay bootstrap failed');
      },step:()=>{steps++;return r.core._thkkd_step()===0;},state,stopLoop(){},audioState:()=>true,raf:callback=>{queue.set(++next,callback);return next;},cancel:id=>queue.delete(id),now:()=>time,hidden:()=>false,onError:error=>{throw error;}});
      await bridge.command({action:'open',path:'replay/pacing.rpy'});
      for(let tick=0;tick<hz*8&&!state().ended;tick++){
        time=tick*1000/hz;const callbacks=[...queue.values()];queue.clear();for(const callback of callbacks)callback(time);r.platform.renderer.present(.5);presents++;
      }
      const before=signature();for(let i=0;i<1000;i++)r.core._thkkd_word(0xa8cd68+(i%60)*4);const after=signature();
      rows.push({hz,steps,presents,...state(),signature:after,observerReadOnly:before===after});bridge.close();
    }
    r.platform.setSeeking(false);return rows;
  },[...replay]);
  for(const row of report.cadences){assert.equal(row.ended,true);assert.equal(row.frame,359);assert.equal(row.signature,report.cadences[0].signature);assert.equal(row.steps,report.cadences[0].steps);assert.equal(row.observerReadOnly,true);}
  report.checks.push('REPLAY-07-simulated-60-120-144-240-Hz-complete-native-Replay-states-match','VERIFY-07-normal-runtime-observer-keeps-game-state');
  const fixtures=[];for(let character=0;character<3;character++)for(let shot=0;shot<3;shot++)fixtures.push({character,shot,bytes:[...replayFixture({character,shot,practice:true,count:480,bombFrame:-1})]});
  report.objects=await frame.evaluate(fixtures=>{
    const r=__thkkdRuntime,rows=[];r.platform.setSeeking(true);
    for(const fixture of fixtures){
      r.input.clear();r.input.replaying=true;r.core._thkkd_stop();r.platform.resetGame();if(r.core._thkkd_start()<0)throw Error('Native start failed');
      r.core.FS.writeFile('/savesthkkd/replay/objects.rpy',Uint8Array.from(fixture.bytes));r.core.HEAPU8.set(new TextEncoder().encode('replay/objects.rpy\0'),r.core._thkkd_replay_path());if(r.core._thkkd_replay_open()<0)throw Error('Native Replay bootstrap failed');
      const families={},continuous={},born={};let birthsChecked=0,retiredChecked=0;const renderer=r.platform.renderer;
      for(let tick=0;tick<479;tick++){
        if(r.core._thkkd_step()!==0)throw Error(r.core.UTF8ToString(r.core._thkkd_error()));
        const presentIds=new Set(renderer.commands.map(command=>command.identity).filter(Boolean));
        for(const id of renderer.previous.keys())if(!presentIds.has(id))retiredChecked++;
        renderer.commands.forEach(command=>{
          if(!command.identity)return;const family=Number.isFinite(Number(command.identity.split(':')[0]))?Math.floor(Number(command.identity.split(':')[0])/100000):'overlay',previous=renderer.previous.get(command.identity);
          families[family]=(families[family]||0)+1;
          if(previous&&previous.texture.texture===command.texture.texture&&previous.points.some((value,i)=>value!==command.points[i]))continuous[family]=(continuous[family]||0)+1;
          if(!previous)born[family]=(born[family]||0)+1;
        });
        if(tick%30===0){renderer.present(.5);renderer.commands.forEach((command,index)=>{if(command.identity&&!renderer.previous.has(command.identity)){const offset=index*6*(renderer.vertexStride||10);if(Math.abs(renderer.vertices[offset]-command.points[0])>.001||Math.abs(renderer.vertices[offset+1]-command.points[1])>.001||renderer.vertexStride>10&&renderer.vertices.slice(offset+10,offset+14).some(value=>value!==0))throw Error('New object used stale position');birthsChecked++;}});}
      }
      renderer.present(.5);rows.push({character:fixture.character,shot:fixture.shot,families,continuous,born,birthsChecked,retiredChecked,cursor:r.core._thkkd_word(0xb43110),image:document.querySelector('canvas').toDataURL()});
    }
    r.platform.setSeeking(false);return rows;
  },fixtures);
  for(const row of report.objects){writeFileSync(resolve(output,`objects-${row.character}-${row.shot}.png`),Buffer.from(row.image.split(',')[1],'base64'));delete row.image;assert.equal(row.cursor,479);assert.ok(row.families[6]>0,'通用自机子弹具有独立显示标识');assert.ok(row.birthsChecked>0);}
  assert.ok(report.objects.some(row=>row.continuous[7]>0),'武器子弹连续位置');assert.ok(report.objects.some(row=>row.continuous[8]>0),'附属物连续位置');
  report.checks.push('DISPLAY-04-nine-loadouts-shot-option-and-laser-identities','DISPLAY-05-native-object-births-use-current-position-and-retired-commands-leave');
  await driver.collect();assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,cadences:report.cadences}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
