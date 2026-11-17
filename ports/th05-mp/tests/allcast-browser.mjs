import assert from 'node:assert/strict';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
import {launchBrowser} from '../../../tools/browser/launch.mjs';

// Pass a diagnostic build via TH05_TEST_RUNTIME. The same scene and original
// DATA are exercised in the normal runtime and in the multiplayer runtime.
const multiplayer=process.env.TH05_ALLCAST_MULTIPLAYER!=='0';
const fixture=await multiplayerFixture(1),browser=await launchBrowser(),page=await browser.newPage();
try{
 await page.goto(fixture.url);
 await page.waitForFunction(()=>events.some(e=>e.event==='ready'));
 for(const [command,payload] of [
  ['configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:{}}],
  [multiplayer?'prepare-room':'launch',{}],
 ])assert.equal((await page.evaluate(([name,payload])=>window.command(0,name,payload),[command,payload])).ok,true);
 const results=await page.evaluate(()=>{
  const m=document.querySelector('iframe').contentWindow.Module,originalMeasure=m.eaglerAudio.measure;
  const check=(value,message)=>{if(!value)throw Error(message);};
  const probe=field=>m._th05_native_allcast(field);
  const tick=(count=1)=>{for(let i=0;i<count;i++)check(m._th05_tick(0),'TH05 scene tick');};
  const start=character=>check(probe(-character-1)===1,'All Cast initialized with original art');
  const scene=()=>m._th05_scene();
  const clockCases=[];
  try{
   // This uses the packaged EXED note samples and the original Ogg's loop
   // bounds, including the jump in the recorded measure near the first wrap.
   for(const mode of ['loop','stalled','silent']){
    let seconds=0,age=0,previous=-1;
    m.eaglerAudio.measure=()=>mode==='stalled'?0:mode==='silent'?-1:
     m.eaglerMusicNotes('EXED.M2',seconds)?.measure??-1;
    start(0);
    const screens=new Set();
    while(scene()===10&&age<15000){
     seconds=age++/56;
     if(seconds>=93.01)seconds=0.849+(seconds-93.01)%(93.01-0.849);
     tick();
     if(scene()===10){
      check(probe(2)>=previous,'The cast clock must not move backward');
      previous=probe(2);
      screens.add(probe(0));
     }
    }
    check(scene()===11&&screens.size===7,mode+' traverses every page and opens the verdict');
    if(mode==='loop')check(age>93.01*56&&age<160*56,'Playback advances through the wrap at the original tempo');
    clockCases.push({mode,frames:age,screens:[...screens]});
   }
   m.eaglerAudio.measure=()=>-1;
   for(let character=0;character<4;character++){
    // A held confirm carried from the ending cannot skip the credits.
    m._th05_key(90,1);start(character);tick(180);
    check(probe(0)===0&&probe(1)>=2,'Carried confirm did not advance the first page');
    m._th05_key(90,0);tick();
    // A press on an odd tick still counts, even though animation runs on even ticks.
    if(probe(3)%2)tick();
    m._th05_key(90,1);tick();m._th05_key(90,0);tick(80);
    check(probe(0)===1,'Confirm advances one page');
    m._th05_key(27,1);tick();m._th05_key(27,0);tick(65);
    check(scene()===11,'Escape finishes All Cast through the normal verdict transition');
   }
   return clockCases;
  }finally{m._th05_clear();m.eaglerAudio.measure=originalMeasure;}
 });
 assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
 console.log(JSON.stringify({multiplayer,clockCases:results,manualCharacters:4}));
}finally{await browser.close();await fixture.close();}
