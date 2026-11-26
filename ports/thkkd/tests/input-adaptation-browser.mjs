import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,nativeState,wait} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/adaptation-20261127/input');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/input-adaptation-browser/1',checks:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report,touch:true});
const {browser,page,key}=driver;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
  await page.evaluate(()=>{
    for(const element of document.querySelectorAll('*')){
      let fiber=element[Object.keys(element).find(key=>key.startsWith('__reactFiber$'))];
      while(fiber){const session=fiber.memoizedProps?.session;if(session?.keyboardPreferences){window.__adaptSession=session;session.settings.setOption('touchEnabled',true);session.settings.setOption('alwaysHitbox',true);session.settings.setOption('restartButtonEnabled',true);session.settings.setOption('touchMovementMode','touch');session.settings.setOption('touchFocusMode','toggle-button');return;}fiber=fiber.return;}
    }throw Error('Launcher session missing');
  });
  await page.click('#libraryBack');const frame=await driver.start({expectedBuildId:JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId});await page.evaluate(()=>__adaptSession.closeTouchHelp());
  for(let i=0;i<9&&(await nativeState(frame)).outer!==100;i++)await key('z');
  assert.equal((await nativeState(frame)).outer,100);await wait(150);
  await page.click('#touchFocus');await frame.waitForFunction(()=>{const r=__thkkdRuntime;return r.core._thkkd_word(0xa72208+r.core._thkkd_word(0xa733f4)*4)>0;});
  await wait(150);assert.equal(await page.$eval('#touchFocus',element=>element.getAttribute('aria-pressed')),'true');
  await page.click('#touchFocus');await frame.waitForFunction(()=>{const r=__thkkdRuntime;return r.core._thkkd_word(0xa72208+r.core._thkkd_word(0xa733f4)*4)===0;});
  report.checks.push('TOUCH-06-shared-toggle-focus-persists-and-releases-native-input');
  report.native=await frame.evaluate(()=>{
    const r=__thkkdRuntime,input=r.input,w=address=>r.core._thkkd_word(address),savedOptions=input.options,savedControls={...input.controls},checks=[];
    const real=address=>{const data=new DataView(new ArrayBuffer(8));data.setUint32(0,w(address),true);data.setUint32(4,w(address+4),true);return data.getFloat64(0,true);};
    const position=()=>({x:real(0xa8cdf8),y:real(0xa8ce00)});
    const step=(n=1)=>{for(let i=0;i<n;i++){const result=r.core._thkkd_step();if(result!==0)throw Error('Native step failed: '+r.core.UTF8ToString(r.core._thkkd_error()));}};
    const check=(id,ok,data)=>{checks.push({id,passed:!!ok,...data});if(!ok)throw Error(id+': '+JSON.stringify(data));};
    const controls=extra=>input.command({command:'touch-controls',fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100,...extra});
    const reset=(mode='touch',extra={})=>{input.clear();input.configure({touchEnabled:true,touchMovementMode:mode,...extra});controls({touchSensitivity:extra.touchSensitivity||100});step(2);};
    const touch=(type,id=1,x=.5,y=.5)=>input.command({command:'direct-touch',type,id,x,y});
    const delta=(before,after)=>({x:after.x-before.x,y:after.y-before.y});
    try{
      reset();let before=position();touch('down');touch('move',1,.55,.5);step();let after=position();const fast=after.x-before.x;
      check('TOUCH-01-limited-native-speed',fast>0&&fast<=real(0xa8ce18)+.0001,{before,after,fast});
      reset('touch');controls({focusEnabled:true});before=position();touch('down');touch('move',1,.45,.5);step();after=position();const slow=before.x-after.x;
      check('TOUCH-05-focus-native-speed',slow>0&&slow<fast,{slow,fast});
      check('RUNTIME-06-native-focus-marker',r.platform.hitbox()===null,{});
      controls({focusEnabled:false});touch('up',1,.45,.5);step(2);
      const marker=r.platform.hitbox();check('RUNTIME-06-always-marker-follows-player',!!marker&&r.platform.graphs.has(marker.graph)&&Math.abs(marker.x-position().x-32)<1,{marker,player:position()});
      for(const sensitivity of [100,200,300]){
        reset('touch-unlimited',{touchSensitivity:sensitivity});before=position();touch('down');touch('move',1,.49,.5);step();after=position();
        check('TOUCH-14-sensitivity-'+sensitivity,Math.abs(before.x-after.x-6.4*sensitivity/100)<.001,{before,after});
      }
      reset('touch-unlimited');before=position();touch('down');touch('move',1,.6,.5);step();after=position();
      check('TOUCH-02-unlimited-native-motion',Math.abs(after.x-before.x-64)<.001&&input.unlimitedUsed,{before,after});
      reset('touch');before=position();touch('down');touch('move',1,.4,.5);step();after=position();
      check('TOUCH-02-return-to-limited',Math.abs(after.x-before.x)<=fast+.001,{before,after});
      for(const mode of ['joystick','joystick-free'])for(const [x,y]of [[1,0],[-1,0],[0,-1],[0,1],[1,-1],[-1,1],[-1,-1],[1,1]]){
        reset(mode);before=position();controls({joystickX:x,joystickY:y});step();after=position();const d=delta(before,after);
        check((mode==='joystick'?'TOUCH-03':'TOUCH-04')+`-${x}-${y}`,Math.sign(d.x)===x&&Math.sign(d.y)===y,{delta:d});
      }
      reset('joystick-free');before=position();controls({joystickX:.3,joystickY:.4});step();after=position();const d=delta(before,after);
      check('TOUCH-04-analog-magnitude',Math.abs(Math.hypot(d.x,d.y)-fast*.5)<.001,{delta:d});
      reset('touch',{touchFocusMode:'two-finger'});touch('down',1);touch('down',2,.6,.5);step();
      const slowBinding=w(0xa733f4),slowDown=w(0xa72208+slowBinding*4);touch('up',2,.6,.5);step();
      check('TOUCH-07-release-one-finger',slowDown>0&&w(0xa72208+slowBinding*4)===0&&input.pointers.size===1,{});
      reset();input.key('ArrowRight',true,'keyboard');touch('down');touch('move',1,.51,.5);step();input.command({command:'touch-cancel'});before=position();step();after=position();
      check('INPUT-05-TOUCH-15-keyboard-owner-survives-cancel',after.x>before.x&&input.owners.get('ArrowRight')?.has('keyboard')&&input.pointers.size===0,{before,after});
      reset('touch',{doubleTapBombEnabled:true});const bombs=w(0xa8cd80);touch('down');touch('move',1,.55,.5);touch('up',1,.55,.5);touch('down',2,.55,.5);step();
      check('TOUCH-11-drag-is-not-double-tap',w(0xa8cd80)===bombs,{bombsAfter:w(0xa8cd80),bombs});
      reset('touch',{doubleTapBombEnabled:true});touch('down',1,.2,.6);touch('move',1,.21,.6);touch('down',2,.7,.7);touch('up',2,.7,.7);touch('down',3,.7,.7);step();
      check('TOUCH-11-native-bomb-retains-movement',w(0xa8cd80)===bombs-1&&w(0xa8cd94)>0&&input.pointers.has(1),{bombs:w(0xa8cd80),timer:w(0xa8cd94)});
      return {checks,frame:w(0xb3e2e8),recordedCoordinates:input.recordedMotion.size};
    }finally{input.clear();input.configure(savedOptions);input.command(savedControls);}
  });
  report.checks.push(...report.native.checks.map(check=>check.id));
  await page.click('#touchEscape');await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xa78e2c)===1);
  const paused=await nativeState(frame);await page.click('#touchRestart');await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xa78e2c)===0&&__thkkdRuntime.core._thkkd_word(0xb3e2e8)<50);
  report.restart={paused,after:await nativeState(frame)};report.checks.push('TOUCH-08-shared-pause-button','TOUCH-18-shared-restart-button');
  report.heldRespawn=await frame.evaluate(()=>{
    const r=__thkkdRuntime,input=r.input,motion=input.motion.bind(input);let dead=false,resumed=false,afterRespawn=0,violation=null;const samples=[];
    input.clear();input.configure({touchEnabled:true,touchMovementMode:'touch'});input.command({command:'touch-controls',fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100});
    r.core._thkkd_step();input.command({command:'direct-touch',type:'down',id:90,x:.5,y:.7});
    input.motion=state=>{
      const value=motion(state);
      if(state.blocked)dead=true;
      else if(dead){resumed=true;afterRespawn++;if(samples.length<8)samples.push({state,value});if(value&&Math.hypot(value.x-state.oldX,value.y-state.oldY)>state.speed+.001)violation={state,value};}
      return value;
    };
    let movedAfterRespawn=false;
    try{
      for(let step=0;step<6000&&afterRespawn<45&&!violation;step++){const result=r.core._thkkd_step();if(result!==0)throw Error('Native death scenario stopped');}
      if(resumed&&!violation){const before=r.core._thkkd_word(0xa8cdfc);input.command({command:'direct-touch',type:'move',id:90,x:.51,y:.7});r.core._thkkd_step();movedAfterRespawn=r.core._thkkd_word(0xa8cdfc)!==before;}
    }
    finally{input.motion=motion;}
    const held=input.pointers.has(90);input.command({command:'touch-cancel'});return {dead,resumed,held,afterRespawn,violation,samples,movedAfterRespawn};
  });
  assert.ok(report.heldRespawn.dead&&report.heldRespawn.resumed&&report.heldRespawn.held,'真实死亡和复活保留同一触点');assert.equal(report.heldRespawn.violation,null,'复活过程不因触控超过原生移动速度');
  assert.equal(report.heldRespawn.movedAfterRespawn,true,'保留的触点继续拖动可以移动');
  report.checks.push('TOUCH-09-same-contact-through-native-death-and-respawn');
  await driver.library();await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
  await page.evaluate(()=>{for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(key=>key.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.settings){session.settings.setOption('alwaysHitbox',false);return;}fiber=fiber.return;}}throw Error('Settings session missing');});
  await page.click('#libraryBack');const withoutMarker=await driver.start();
  for(let i=0;i<9&&(await nativeState(withoutMarker)).outer!==100;i++)await key('z');
  assert.equal((await nativeState(withoutMarker)).outer,100);assert.equal(await withoutMarker.evaluate(()=>__thkkdRuntime.platform.hitbox()),null);
  report.checks.push('RUNTIME-06-always-marker-can-be-disabled');
  await page.screenshot({path:resolve(output,'restarted.png')});await driver.collect();assert.equal(report.errors.length,0);report.result='passed';
  console.log(JSON.stringify({result:report.result,checks:report.checks}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
