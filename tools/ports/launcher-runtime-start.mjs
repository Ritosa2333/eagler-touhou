import assert from 'node:assert/strict';
import {matchesMusicLaunchConsent} from './launcher-music-journey.mjs';

/** One real launch decision flow, shared by installed-package acceptance journeys. */
export async function runLauncherRuntimeStart(io,{game,previousEpoch=0,music='none',timeoutMs=180000}={}){
  const e={source:'Launcher #launch',previousEpoch,music,consents:[],transitions:[]},deadline=io.now()+timeoutMs;let previous;
  await io.launch();
  do{
    const s=await io.read();assert.ok(Number.isSafeInteger(s.epoch)&&s.epoch>=0);
    const key=JSON.stringify([s.epoch,!!s.ready,!!s.first,s.prompt.open,s.errors,s.blockingUi]);
    if(key!==previous){e.transitions.push(s);assert.ok(e.transitions.length<=64,'Launcher startup changed state too many times');previous=key;}
    assert.deepEqual(s.errors,[],'Native startup returned an error');
    if(s.blockingUi?.importRequired||s.blockingUi?.startupError)throw Object.assign(Error('Launcher launch blocked: '+JSON.stringify(s.blockingUi)),{evidence:e});
    if(s.prompt.open){
      assert.equal(matchesMusicLaunchConsent(music,s.prompt),true,'Unexpected Launcher launch decision');assert.equal(e.consents.length,0,'Music launch consent repeated');
      await io.confirm();e.consents.push({...s.prompt,confirmed:true});await io.waitPromptClosed();
    }else if(s.epoch>previousEpoch&&s.ready&&s.first){
      for(const event of [s.ready,s.first]){assert.equal(event.game,game);assert.equal(event.epoch,s.epoch);assert.equal(event.protocol,'eagler-touhou/1');}
      assert.ok(Number.isSafeInteger(s.first.presentation.nativePresentCount)&&s.first.presentation.nativePresentCount>0,'Launcher first frame lacks a native present');
      e.runtime=s;return e;
    }
    await io.wait(100);
  }while(io.now()<deadline);
  throw Object.assign(Error('Launcher did not present a new Runtime before its deadline'),{evidence:e});
}

export async function launchLauncherRuntime(page,{journal={global:'__offlineRuntimeEvents'},...options}={}){
  assert.match(journal.global,/^[A-Za-z_$][\w$]*$/);if(journal.field!==undefined)assert.match(journal.field,/^[A-Za-z_$][\w$]*$/);
  const evidence=await runLauncherRuntimeStart({now:Date.now,launch:()=>page.locator('#launch').click(),
    read:()=>page.evaluate(journal=>{
      const frame=document.getElementById('gameFrame'),url=frame?.src??'about:blank',epoch=Number(new URL(url,location.href).searchParams.get('runtimeEpoch')),
        owner=window[journal.global],events=(journal.field?owner?.[journal.field]:owner)?.filter(event=>event.epoch===epoch);
      if(!Array.isArray(events))throw Error('Launcher startup has no declared protocol event journal');
      return{url,epoch,ready:events.find(event=>event.event==='ready')??null,first:events.find(event=>event.event==='first-frame')??null,
        errors:events.filter(event=>event.ok===false||['error','abort'].includes(event.event)),blockingUi:{
          importRequired:document.getElementById('gameDataImportWindow')?.hidden===false,
          startupError:document.getElementById('startupError')?.hidden===false?document.getElementById('startupErrorText')?.textContent:null,
          importReason:document.getElementById('gameDataImportWindow')?.hidden===false?document.getElementById('gameDataImportReason')?.textContent:null},prompt:{open:document.getElementById('decisionDialog')?.open===true,
          message:document.getElementById('decisionMessage')?.textContent??'',confirm:document.getElementById('decisionConfirm')?.textContent??''}};
    },journal),
    confirm:()=>page.locator('#decisionConfirm').click(),waitPromptClosed:()=>page.waitForFunction(()=>document.getElementById('decisionDialog')?.open!==true,null,{timeout:15000}),
    wait:ms=>page.waitForTimeout(ms),
  },options);
  return{frame:await(await page.$('#gameFrame')).contentFrame(),evidence};
}
