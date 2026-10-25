import assert from 'node:assert/strict';
import {runPracticeControlScene,nativeWidgetTouchCandidates} from './practice-ui-scene.mjs';
import {practiceSnapshotRequest,readPracticeSnapshotRaw} from '../../tests/adaptation/adapters/practice.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
import {controlValidationScene,validationSceneRequest,assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';
import {createPracticeUiInput} from './practice-ui-input.mjs';

/** Installed before either document's production input handlers. */
export function installPracticeTouchJournal(){
  window.__practiceTouchEvents=[];window.__practiceTouchDrops=0;
  const capture=event=>{if(event.pointerType!=='touch')return;if(window.__practiceTouchEvents.length>=256){window.__practiceTouchDrops++;return;}
    window.__practiceTouchEvents.push({type:event.type,pointerType:event.pointerType,pointerId:event.pointerId,isTrusted:event.isTrusted,
      clientX:event.clientX,clientY:event.clientY,target:event.target?.id??null,realm:window===window.top?'host':'runtime',url:location.href,timeOrigin:performance.timeOrigin});};
  for(const type of ['pointerdown','pointerup','pointercancel'])addEventListener(type,capture,true);
  window.__releasePracticeTouchJournal=()=>{for(const type of ['pointerdown','pointerup','pointercancel'])removeEventListener(type,capture,true);};
}

/** Bind the common native-widget scene to the existing formal Launcher journey. */
export async function runLauncherPracticeControls({page,frame,context,target,adapter,save}){
  const keys=adapter.practice.controlKeys,allKeys=[...keys.map(row=>row.key),'Backspace','Tab','F12'];
  const nativeInput=createPracticeUiInput({page,frame,adapter}),physical=nativeInput.press;
  const observe=()=>frame.evaluate(readRuntimeObservation,runtimeObservationRequest(target));
  const until=async(label,predicate,read)=>{const deadline=Date.now()+20000;let sample;do{sample=await read();if(predicate(sample))return sample;await page.waitForTimeout(50);}while(Date.now()<deadline);throw Error(label+': '+JSON.stringify(sample));};
  if(await page.locator('#touchHelp').isVisible())await page.locator('#touchHelpClose').click();
  const readPractice=async()=>adapter.practice.snapshot.decode(await frame.evaluate(readPracticeSnapshotRaw,
    practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice})));
  const sceneRequest=validationSceneRequest(target),arm=assertControlledSceneArmed(await frame.evaluate(controlValidationScene,
    validationSceneRequest({...target,action:'arm'})));
  await adapter.practice.enterMenu({read:readPractice,waitUntil:(label,predicate)=>until(label,predicate,readPractice),press:physical,waitForPracticeInput:nativeInput.waitForPracticeInput,
    waitForGameplay:()=>until('native practice gameplay ready',state=>resolveTitleFlow(target).isGameplayReady(state,'desktop'),observe),
    readInitial:()=>frame.evaluate(readPracticeSnapshotRaw,practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice,kind:'gameplayInitial'})),
    readIdentity:()=>frame.evaluate(runtime=>({app:window[runtime.global][runtime.app],documentTimeOrigin:performance.timeOrigin,url:location.href}),adapter.runtime)});
  assertControlledSceneWorld(await frame.evaluate(controlValidationScene,sceneRequest));
  const fire=page.locator('#touchFire');if(await fire.getAttribute('aria-pressed')==='true')await fire.click();
  await until('native input neutral before Practice UI scene',s=>s.input.value===0,observe);
  const uiRequest=practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice,kind:'ui'}),nativeRequest=practiceSnapshotRequest({runtime:adapter.runtime,contract:adapter.practice});
  const read=async()=>{const uiRaw=await frame.evaluate(readPracticeSnapshotRaw,uiRequest),nativeRaw=await frame.evaluate(readPracticeSnapshotRaw,nativeRequest);
    return{uiRaw,nativeRaw,scene:assertControlledSceneWorld(await frame.evaluate(controlValidationScene,sceneRequest)),ui:adapter.practice.uiSnapshot.decode(uiRaw),practice:adapter.practice.snapshot.decode(nativeRaw),
    ...await frame.evaluate(({runtime,keys})=>({identity:{url:location.href,timeOrigin:performance.timeOrigin,app:window[runtime.global][runtime.app]},
      held:Object.fromEntries(keys.map(key=>[key,window.__eaglerKeyboardInputBridge.keyDown(key)]))}),{runtime:adapter.runtime,keys:allKeys})};};
  const cdp=await context.newCDPSession(page);let activeTouch=false;
  for(const document of [page,frame])assert.equal(await document.evaluate(()=>Array.isArray(window.__practiceTouchEvents)),true,'Native touch journal must be installed before production input');
  try{
    return await runPracticeControlScene({arm,read,press:physical,waitUntil:(label,predicate)=>until(label,predicate,read),save:async e=>{e.nativeInput=nativeInput.evidence;await save(e);},
      toggleMobile:async()=>{const button=page.locator('#touchThpracBackspace');assert.equal(await button.isVisible(),true);await button.click();},
      touch:async rect=>{
        const canvas=await frame.locator('canvas').boundingBox();assert.ok(canvas&&canvas.width>0&&canvas.height>0);
        const scale=Math.min(canvas.width/640,canvas.height/480),frameBox=await (await frame.frameElement()).boundingBox(),
          frameViewport=await frame.evaluate(()=>({width:innerWidth,height:innerHeight}));
        const targets=[];let point,logical;
        for(const candidate of nativeWidgetTouchCandidates(rect)){
          if(candidate.x<0||candidate.x>=640||candidate.y<0||candidate.y>=480)continue;
          const pagePoint={x:canvas.x+(canvas.width-640*scale)/2+candidate.x*scale,y:canvas.y+(canvas.height-480*scale)/2+candidate.y*scale};
          const hostHit=await (await frame.frameElement()).evaluate((element,p)=>document.elementFromPoint(p.x,p.y)===element,pagePoint),
            nativeHit=await frame.evaluate(p=>document.elementFromPoint(p.x,p.y)===document.querySelector('canvas'),
              {x:(pagePoint.x-frameBox.x)*frameViewport.width/frameBox.width,y:(pagePoint.y-frameBox.y)*frameViewport.height/frameBox.height});
          targets.push({logical:candidate,requestedPage:pagePoint,hostHit,nativeHit});
          if(hostHit&&nativeHit){point=pagePoint;logical=candidate;break;}
        }
        assert.ok(point,'Native practice widget has no unobstructed browser touch point: '+JSON.stringify({rect,targets}));
        const start=await Promise.all([page,frame].map(document=>document.evaluate(()=>window.__practiceTouchEvents.length)));
        await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{id:0,x:point.x,y:point.y,radiusX:1,radiusY:1,force:1}]});activeTouch=true;
        await page.waitForTimeout(90);await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});activeTouch=false;
        await until('native touch click released',s=>!s.ui.pointerDown&&s.ui.pendingPointerEdges===0,read);
        const journals=await Promise.all([page,frame].map((document,i)=>document.evaluate(start=>({events:window.__practiceTouchEvents.slice(start),drops:window.__practiceTouchDrops}),start[i])));
        const events=journals.flatMap(row=>row.events),down=events.filter(row=>row.type==='pointerdown'),up=events.filter(row=>row.type==='pointerup');
        const receipt={source:'browser-touch',rect,targets,logical,requestedPage:point,canvas,journals,frameBox,frameViewport,down:down[0],up:up[0]};
        try{assert.ok(journals.every(row=>row.drops===0));assert.equal(down.length,1);assert.equal(up.length,1);assert.equal(down[0].pointerId,up[0].pointerId);
          assert.equal(down[0].isTrusted,true);assert.equal(up[0].isTrusted,true);}catch(error){error.touchEvidence=receipt;throw error;}
        return receipt;
      },
      release:async()=>{if(activeTouch){await cdp.send('Input.dispatchTouchEvent',{type:'touchCancel',touchPoints:[]});activeTouch=false;}
        await frame.locator('canvas').focus();for(const key of allKeys)await page.keyboard.up(key);},
    },keys);
  }finally{for(const document of [page,frame])await document.evaluate(()=>window.__releasePracticeTouchJournal?.());await cdp.detach();}
}
