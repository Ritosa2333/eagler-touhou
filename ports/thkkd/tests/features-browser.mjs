import assert from 'node:assert/strict';
import puppeteer from '../../../launcher/node_modules/puppeteer-core/lib/puppeteer/puppeteer-core.js';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
const output=fileURLToPath(new URL('../build/features-browser/',import.meta.url));mkdirSync(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:process.env.CHROME_PATH||'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',headless:true,args:['--autoplay-policy=no-user-gesture-required'],defaultViewport:{width:960,height:720}});
const report={browser:await browser.version(),checks:[],errors:[]};
try{
 const page=await browser.newPage();page.on('pageerror',e=>report.errors.push(e.message));
 await page.evaluateOnNewDocument(()=>{let id=0;const callbacks=new Map();window.requestAnimationFrame=f=>{callbacks.set(++id,f);return id;};window.cancelAnimationFrame=i=>callbacks.delete(i);window.__pump=t=>{const batch=[...callbacks.values()];callbacks.clear();for(const callback of batch)callback(t);};});
 await page.goto('http://127.0.0.1:8173/runtime/thkkd.html?runtimeEpoch=1&standalone=1&data=../build/web-content/thkkd.data');await page.waitForFunction(()=>!!globalThis.__thkkdRuntime,{timeout:60000,polling:50});
 await page.evaluate(()=>{
   const r=__thkkdRuntime,w=a=>r.core._thkkd_word(a);
   window.steps=n=>{for(let i=0;i<n;i++)if(r.core._thkkd_step()!==0)throw Error('原生步进失败');};
   window.key=(code,n=2)=>{r.input.key(code,true,'test');steps(n);r.input.key(code,false,'test');steps(40);};
   window.startGame=()=>{steps(120);for(let i=0;i<4;i++)key('KeyZ');if(w(0xa8ce58)!==100)throw Error('菜单没有进入游戏 '+JSON.stringify([w(0xa8ce58),w(0xa87f48),w(0xa87f40)]));};
   window.state=()=>({rng:w(0xa74c08),frame:w(0xb3e2e8),cursor:w(0xb43110),x:[w(0xa8cdf8),w(0xa8cdfc)],y:[w(0xa8ce00),w(0xa8ce04)],score:w(0xa8cd78),life:w(0xa8cd7c),bomb:w(0xa8cd80),pools:[...Array.from({length:80},(_,i)=>w(0xa87fe8+i*248)),...Array.from({length:3000},(_,i)=>w(0xa8e660+i*240))]});
 });
 report.cadence=[];
 for(const hz of [60,120,144])for(const limited of [false,true]){
   const result=await page.evaluate(async({hz,limited})=>{const r=__thkkdRuntime;await r.stop();await r.command({command:'configure',music:'ogg',options:{limitPresentationTo60:limited,alwaysHitbox:true}});await r.command({command:'launch'});startGame();r.input.key('ArrowRight',true,'test');let displays=0;const original=r.platform.renderer.present.bind(r.platform.renderer);r.platform.renderer.present=(...args)=>{displays++;return original(...args);};for(let i=0;i<=hz*2;i++)__pump(i*1000/hz);r.platform.renderer.present=original;r.input.clear();return {state:state(),displays,error:document.querySelector('#error').textContent};},{hz,limited});
   assert.equal(result.error,'');assert.deepEqual(result.state,report.cadence[0]?.state??result.state);assert.ok(Math.abs(result.displays-(limited?121:hz*2+1))<=1);report.cadence.push({hz,limited,...result});
 }
 report.checks.push('60/120/144-Hz-identical-native-state-and-60-Hz-limit');
 const purity=await page.evaluate(async()=>{const r=__thkkdRuntime,before=JSON.stringify(state()),files=JSON.stringify(await r.command({command:'list'})),audio=[...r.platform.sounds.values()].map(s=>[s.playing,s.offset,s.started]);for(let i=0;i<100;i++)r.platform.renderer.present(i/100);return {state:before===JSON.stringify(state()),files:files===JSON.stringify(await r.command({command:'list'})),audio:JSON.stringify(audio)===JSON.stringify([...r.platform.sounds.values()].map(s=>[s.playing,s.offset,s.started])),marker:r.platform.renderer.commands.some(c=>c.identity?.startsWith('always-hitbox:'))};});
 assert.deepEqual(purity,{state:true,files:true,audio:true,marker:true});report.purity=purity;report.checks.push('extra-presentations-preserve-state-files-and-audio-events');
 report.recordings=[];
 for(const extended of [false,true]){
   const result=await page.evaluate(async extended=>{
     const r=__thkkdRuntime,w=a=>r.core._thkkd_word(a);await r.stop();await r.command({command:'configure',music:'ogg',options:{touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:200}});await r.command({command:'launch'});startGame();
     r.input.key('KeyZ',true,'keyboard');r.input.command({command:'touch-controls',fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:200});steps(12);
     r.input.key('KeyZ',false,'keyboard');const keys=new Uint8Array(256);r.input.sample(keys);if(!keys[44])throw Error('多来源持有丢失');
     if(extended){r.input.command({command:'direct-touch',type:'down',id:1,x:.3,y:.7});r.input.command({command:'direct-touch',type:'move',id:1,x:.42,y:.61});steps(1);r.input.command({command:'direct-touch',type:'up',id:1,x:.42,y:.61});}
     steps(25);r.input.clear();
     const final=state(),motion=[...r.input.recordedMotion];key('Escape');if(w(0xa78e2c)!==1)throw Error('暂停失败');key('KeyR');if(w(0xa78e2c)!==0)throw Error('R 重开失败');
     // 再开一局录制，保留需要逐帧比较的状态，按原作保存询问、槽位和命名流程保存。
     steps(20);r.input.key('KeyZ',true,'keyboard');steps(10);if(extended){r.input.player={x:192,y:384};r.input.command({command:'direct-touch',type:'down',id:2,x:.3,y:.7});r.input.command({command:'direct-touch',type:'move',id:2,x:.4,y:.6});steps(1);r.input.command({command:'direct-touch',type:'up',id:2,x:.4,y:.6});}steps(15);r.input.clear();
     // 原作 ReplayPauseEntry 在 cursor+1==count 时停片；多录一帧后比较这个原生结束点。
     const recorded=state();steps(1);key('Escape');key('ArrowDown');key('KeyZ');if(w(0xa8ce58)!==16||w(0xb41215)!==3)throw Error('没有进入原作录像保存询问');key('KeyZ');key('KeyZ');key('ArrowLeft');key('KeyZ');await r.command({command:'sync'});
     const files=(await r.command({command:'list'})).files.filter(f=>f.path.endsWith(extended?'.thkkdr':'.rpy'));if(!files.length)throw Error('原作保存器没有写出录像');const name=files.at(-1).path;
     const bytes=(await r.command({command:'read',path:name})).bytes;
     await r.command({command:'playback',action:'open',path:name});await r.command({command:'playback',action:'pause'});await r.command({command:'playback',action:'seek',value:recorded.cursor});
     return {extended,name,bytes:bytes.length,recorded,nativeCount:r.core._thkkd_word(0xb43a08),firstMotion:motion,final};
   },extended);
   for(let i=0;i<100;i++){if(await page.evaluate(cursor=>__thkkdRuntime.core._thkkd_word(0xb43110)===cursor,result.recorded.cursor))break;await page.evaluate(time=>__pump(time),i*16.666667);await new Promise(resolve=>setTimeout(resolve,5));}
   const restored=await page.evaluate(()=>state());result.restored=restored;report.recordings.push(result);console.log(JSON.stringify({extended,cursor:result.recorded.cursor,nativeCount:result.nativeCount,restored:restored.cursor,diagnostic:await page.evaluate(()=>({error:document.querySelector('#error').textContent,scene:__thkkdRuntime.core._thkkd_word(0xa78e2c),outer:__thkkdRuntime.core._thkkd_word(0xa8ce58)}))}));assert.deepEqual(restored,result.recorded);
 }
 report.checks.push('merged-native-recording-and-versioned-movement-replay-roundtrip','pause-and-native-R-restart');
 assert.equal(report.errors.length,0);report.result='passed';await page.screenshot({path:output+'features.png'});console.log(JSON.stringify({result:report.result,checks:report.checks}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);}
finally{writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
