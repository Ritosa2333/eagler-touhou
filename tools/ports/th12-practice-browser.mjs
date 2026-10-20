// Actual purple Wasm/ImGui UI and shared font. Synthetic game surface; no retail
// assets/gameplay are used or claimed by this browser input/layout check.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../browser/launch.mjs';
const root=resolve(import.meta.dirname,'../..'),runtime=resolve(root,'ports/th12/build-eagler');
const output=resolve(root,'artifacts/th12-purple-ui');await mkdir(output,{recursive:true});
const font=resolve(process.env.TH12_TEST_FONT??resolve(root,'dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:20px;background:#181820;color:white}canvas#game{width:640px;height:480px;background:linear-gradient(30deg,#1c2434,#3a2c40);display:block}</style><canvas id="game" width="640" height="480"></canvas><script type="module">
import createCore from '/core.mjs';import {PracticeBridge} from '/bridge.mjs';import {PracticeMenu} from '/menu.mjs';import {PracticeOverlay} from '/overlay.mjs';
window.core=await createCore();window.actions=[];window.context={scene:'playing',replay:false,menuReady:true};
window.ui=new PracticeOverlay({bridge:new PracticeBridge(core),canvas:document.getElementById('game'),onMenuAction:action=>actions.push(action)});
ui.configure({enabled:true,locale:'zh-CN'});await ui.initialize(new Map([['/unifont.otf',await (await fetch('/unifont.otf')).blob()]]));
window.openPractice=()=>{window.context={scene:'practice',menu:new PracticeMenu({locale:'zh-CN'}),menuReady:true};};
window.routed=[];for(const name of ['keydown','keyup'])addEventListener(name,event=>{routed.push({code:event.code,down:name==='keydown',consumed:ui.key(event.code,name==='keydown')});event.preventDefault();});
for(const name of ['pointerdown','pointermove','pointerup','pointercancel'])addEventListener(name,event=>{ui.pointer({id:event.pointerId,type:name==='pointercancel'?'cancel':name.slice(7),x:event.clientX,y:event.clientY,pointerType:event.pointerType,button:event.button});});
addEventListener('wheel',event=>{if(ui.wheel(event))event.preventDefault();},{passive:false});
window.ticks=0;let last=0;function frame(now){if(now-last>=1000/60-.5){ui.update(context);ui.tick(0);last=now;window.ticks++;}ui.render();requestAnimationFrame(frame);}requestAnimationFrame(frame);window.ready=true;
</script>`;
const server=createServer(async(request,response)=>{
  try{
    const pathname=new URL(request.url,'http://localhost').pathname;
    if(pathname==='/'){response.setHeader('Content-Type','text/html;charset=utf-8');response.end(html);return;}
    const file=pathname==='/unifont.otf'?font:resolve(runtime,'.'+decodeURIComponent(pathname));
    if(file!==font&&!file.startsWith(runtime+sep))throw Error('Invalid runtime path');
    response.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.otf':'font/otf'})[extname(file)]??'application/octet-stream');response.end(await readFile(file));
  }catch(error){response.statusCode=404;response.end(error.message);}
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await launchBrowser();const page=await browser.newPage({viewport:{width:900,height:650}}),errors=[];
page.on('pageerror',error=>errors.push(error.message));
try{
  await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>window.ready,undefined,{timeout:120000});
  await page.keyboard.press('F1');assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),0);
  await page.keyboard.press('Backspace');await page.waitForFunction(()=>core._th12_practice_ui_flags()&1);
  const shown=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,shown);
  await page.mouse.click(50,46);await page.waitForFunction(()=>ui.bridge.snapshot().tools&1);
  await page.screenshot({path:resolve(output,'desktop-cheats.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.keyboard.press('F1');await page.waitForFunction(()=>!(ui.bridge.snapshot().tools&1));
  await page.keyboard.press('KeyU');await page.waitForFunction(()=>ui.bridge.snapshot().tools&256);
  await page.keyboard.press('Tab');await page.waitForFunction(()=>core._th12_practice_ui_flags()&4);
  const trackerVertices=()=>page.evaluate(()=>{
    const header=new Uint32Array(core.HEAPU8.buffer,core._th12_practice_ui_draw(),8),view=new DataView(core.HEAPU8.buffer);let count=0;
    for(let i=0;i<header[3];i++){const at=header[0]+i*header[6],x=view.getFloat32(at,true),y=view.getFloat32(at+4,true);if(x>=425&&y>=338&&y<=480)count++;}
    return count;
  });
  const settle=async()=>{const before=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+3,before);};
  await settle();const tracker=await trackerVertices();assert.ok(tracker>100);
  for(const scene of ['pause','pause-confirm']){
    await page.evaluate(scene=>{context.scene=scene;},scene);await settle();assert.equal(await trackerVertices(),tracker,scene+' retains tracker geometry');
    for(const [code,bit] of [['F1',1],['F2',2],['F3',4],['F4',8],['F5',16],['F6',32],['F7',64],['KeyU',256]]){
      const tools=await page.evaluate(()=>ui.bridge.snapshot().tools);
      await page.keyboard.down(code);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools^bit,scene+' toggles '+code);
      await page.keyboard.down(code);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools^bit,'held key does not toggle repeatedly');
      await page.keyboard.up(code);await page.keyboard.press(code);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools);
      assert.ok(await page.evaluate(code=>routed.filter(e=>e.code===code).slice(-4).every(e=>e.consumed),code),'THPrac consumes keydown and keyup');
    }
    const tools=await page.evaluate(()=>ui.bridge.snapshot().tools);await page.mouse.click(50,46);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools^1,'paused checkbox remains clickable');
    await page.mouse.click(50,46);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools);
    await page.evaluate(()=>{context.replay=true;});await settle();await page.keyboard.press('F1');await page.mouse.click(50,46);await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),tools,'paused replay remains read only');
    assert.ok(await page.evaluate(()=>routed.filter(e=>e.code==='F1').slice(-2).every(e=>e.consumed)),'readonly panel still consumes F1');
    await page.evaluate(()=>{context.replay=false;});await settle();
  }
  await page.screenshot({path:resolve(output,'paused-tracker.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.evaluate(()=>{context.scene='title';});await settle();assert.equal(await trackerVertices(),0);
  const titleTools=await page.evaluate(()=>ui.bridge.snapshot().tools);await page.keyboard.press('F1');await settle();assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),titleTools);assert.ok(await page.evaluate(()=>routed.filter(e=>e.code==='F1').slice(-2).every(e=>!e.consumed)),'title retains manual shortcut');
  await page.evaluate(()=>{context.scene='playing';});await settle();assert.equal(await trackerVertices(),tracker);
  await page.keyboard.press('Tab');await page.waitForFunction(()=>!(core._th12_practice_ui_flags()&4));
  await page.keyboard.press('F12');await page.waitForFunction(()=>ui.capturesGameplay);
  await page.mouse.click(100,92); // Native Gameplay group.
  const expanded=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,expanded);
  await page.screenshot({path:resolve(output,'desktop-advanced.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.keyboard.press('F12');await page.waitForFunction(()=>!ui.capturesGameplay);
  await page.evaluate(()=>{context.replay=true;});
  await page.waitForFunction(()=>ui.replay);const before=await page.evaluate(()=>ui.bridge.snapshot().tools);
  await page.keyboard.press('F1');assert.equal(await page.evaluate(()=>ui.bridge.snapshot().tools),before);
  await page.evaluate(()=>{context.replay=false;ui.mobilePanels({thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true});});
  await page.waitForFunction(()=>!ui.replay&&!ui.capturesGameplay);
  await page.screenshot({path:resolve(output,'mobile-collapsed.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.keyboard.press('Enter');assert.equal(await page.evaluate(()=>ui.capturesGameplay),false);
  const routed=await page.evaluate(()=>[
    ui.pointer({id:'game-finger',type:'down',x:650,y:490}),
    ui.pointer({id:'game-finger',type:'move',x:200,y:42}),
    ui.pointer({id:'game-finger',type:'up',x:200,y:42}),
  ]);assert.deepEqual(routed,[false,false,false]);
  await page.mouse.click(200,42);await page.waitForFunction(()=>ui.capturesGameplay);
  await page.screenshot({path:resolve(output,'mobile-advanced.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.evaluate(()=>{ui.mobilePanels({thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:false});ui.mobilePanels({thpracMobilePanelsEnabled:true,thpracMobilePanelsVisible:true});});
  await page.waitForFunction(()=>!ui.capturesGameplay);
  await page.evaluate(()=>{ui.mobilePanels({thpracMobilePanelsEnabled:false,thpracMobilePanelsVisible:false});openPractice();});
  await page.waitForFunction(()=>ui.scene==='practice'&&ui.capturesGameplay);
  const practice=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,practice);
  await page.screenshot({path:resolve(output,'practice-options.png'),clip:{x:20,y:20,width:640,height:480}});
  await page.mouse.click(400,113); // Open the native Mode combo.
  const popup=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,popup);
  await page.keyboard.press('Escape');
  const closed=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,closed);
  assert.equal(await page.evaluate(()=>actions.length),0,'Escape closes the combo before cancelling Practice');
  await page.keyboard.down('Control');await page.mouse.click(400,190);await page.keyboard.up('Control');
  const editing=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,editing);
  await page.keyboard.press('Control+a');const selected=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+1,selected);
  await page.keyboard.type('6');const digit=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+1,digit);await page.keyboard.press('Enter');
  const typed=await page.evaluate(()=>ticks);await page.waitForFunction(value=>ticks>value+2,typed);
  assert.equal(await page.evaluate(()=>new Int32Array(core.HEAPU8.buffer,core._th12_practice_ui_parameters(),20)[8]),6,'native temporary numeric input');
  assert.equal(await page.evaluate(()=>actions.length),0,'Enter commits the numeric field before starting Practice');
  await page.keyboard.press('Escape');await page.waitForFunction(()=>actions.length>0);assert.equal(await page.evaluate(()=>actions.at(-1).type),'cancel');
  assert.deepEqual(errors,[]);
  const report={status:'PASS',runtime:'actual TH12 Wasm + common ImGui + shared Unifont',checks:['native checkbox mutation','original hotkey gating','enemy invincibility','paused tracker geometry','pause-confirm tracker geometry','paused F1-F7/U mutation and routing','paused checkbox mutation','held shortcut edge detection','paused replay read only','title manual routing','title HUD isolation','replay cheat isolation','mobile folded reopen','pointer-only headers','game drag ownership','combo cancel isolation','numeric keyboard editing','native practice cancel'],retailAssets:false,screenshots:6};
  await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
}catch(error){
  console.error({errors,state:await page.evaluate(()=>({ready:window.ready,flags:window.core?._th12_practice_ui_flags(),enabled:window.ui?.enabled,initialized:window.ui?.initialized})).catch(()=>null)});
  await page.screenshot({path:resolve(output,'failure.png')});throw error;
}finally{
  await browser.close();await new Promise(resolve=>server.close(resolve));
}
