import assert from 'node:assert/strict';
import {assertControlledSceneArmed,assertControlledSceneWorld} from '../../tests/adaptation/adapters/scene.mjs';

function value(state,key){
  assert.ok(state.practice.applied,'Practice control lost its accepted native gameplay owner');
  if(key.kind==='cheat-mask')return Boolean(state.practice.applied.cheats&key.mask);
  assert.equal(key.kind,'everlasting-bgm');return state.practice.applied.everlastingBgm;
}
function item(state,key){
  const matches=state.ui.items.filter(item=>item.key===key);assert.equal(matches.length,1);
  const row=matches[0];assert.equal(row.generation,state.ui.renderedGeneration);assert.equal(state.ui.inputGeneration,state.ui.renderedGeneration);
  assert.ok(row.id>0);return row;
}
function visiblePanel(state,name){
  const panel=state.ui.panels[name];assert.equal(panel.exists,true);assert.equal(panel.active,true);assert.equal(panel.hidden,false);
  assert.ok(panel.vertexCount>0&&panel.rect.width>0&&panel.rect.height>0);return panel;
}
function panelReady(state,name){const panel=state.ui.panels[name];return panel.exists&&panel.active&&!panel.hidden&&panel.vertexCount>0&&panel.rect.width>0&&panel.rect.height>0;}
export function nativeWidgetTouchCandidates(rect){
  return[.5,.75,.25,.9,.1].map(fraction=>({x:rect.x+rect.width*fraction,y:rect.y+rect.height/2}));
}
function assertTouch(touch,rect,identity){
  assert.equal(touch.source,'browser-touch');assert.deepEqual(touch.rect,rect);const logical=touch.logical;
  assert.ok(nativeWidgetTouchCandidates(rect).some(point=>point.x===logical.x&&point.y===logical.y),'Touch lies outside declared native widget targets');
  const {canvas,requestedPage,frameBox,frameViewport}=touch;
  for(const box of [canvas,frameBox]){for(const field of ['x','y','width','height'])assert.ok(Number.isFinite(box[field]));assert.ok(box.width>0&&box.height>0);}
  assert.ok(frameViewport.width>0&&frameViewport.height>0);
  const scale=Math.min(canvas.width/640,canvas.height/480),expected={
    x:canvas.x+(canvas.width-640*scale)/2+logical.x*scale,y:canvas.y+(canvas.height-480*scale)/2+logical.y*scale};
  assert.deepEqual(requestedPage,expected);
  assert.equal(touch.journals.length,2);assert.ok(touch.journals.every(row=>row.drops===0));
  const events=touch.journals.flatMap(row=>row.events);assert.equal(events.length,2);
  assert.deepEqual(events,[touch.down,touch.up]);
  for(const [i,event] of events.entries()){
    assert.equal(event.type,i?'pointerup':'pointerdown');assert.equal(event.pointerType,'touch');assert.equal(event.isTrusted,true);
    assert.equal(event.pointerId,touch.down.pointerId);assert.ok(Number.isInteger(event.pointerId)&&event.pointerId>0);
    assert.equal(event.realm,'runtime');assert.equal(event.url,identity.url);assert.equal(event.timeOrigin,identity.timeOrigin);
    const point={x:frameBox.x+event.clientX*frameBox.width/frameViewport.width,y:frameBox.y+event.clientY*frameBox.height/frameViewport.height};
    for(const axis of ['x','y'])assert.ok(Number.isFinite(point[axis])&&Math.abs(point[axis]-expected[axis])<=1,'Actual trusted touch missed the submitted native widget');
  }
}

export const practiceControlSamples=e=>[e.before,e.menuOpened,e.mobileOpened,e.cheatsExpanded,...e.trials.flatMap(row=>[row.before,row.keyboard,row.pointer]),
  ...Object.values(e.keyboardPanels).flatMap(row=>[row.before,row.opened,row.closed]),...Object.values(e.mobilePanels),e.final];

export function assertPracticeControlScene(e,keys){
  assert.equal(e.schema,'eagler-touhou/practice-control-scene/1');assert.deepEqual(e.trials.map(row=>row.key),keys.map(row=>row.key));
  assertControlledSceneArmed(e.arm);
  assert.equal(e.before.ui.menuOpen,false);assert.equal(e.menuOpened.ui.menuOpen,true);
  for(const row of [e.mobileOpened,e.cheatsExpanded]){assert.equal(row.ui.mobileEnabled,true);assert.equal(row.ui.mobileVisible,true);}
  assert.equal(visiblePanel(e.mobileOpened,'cheats').expanded,false);assert.equal(visiblePanel(e.cheatsExpanded,'cheats').expanded,true);
  let previous=e.cheatsExpanded;
  for(const [index,trial] of e.trials.entries()){
    const key=keys[index];assert.equal(value(trial.before,key),value(previous,key));
    assert.equal(value(trial.keyboard,key),!value(trial.before,key));assert.equal(item(trial.keyboard,key.key).value,value(trial.keyboard,key));
    assert.equal(trial.keyboard.held[key.key],false,'Function key was not released');
    assert.equal(value(trial.pointer,key),value(trial.before,key));assert.equal(item(trial.pointer,key.key).value,value(trial.pointer,key));
    assert.equal(trial.pointer.ui.pointerDown,false);assert.equal(trial.pointer.ui.pendingPointerEdges,0);
    assertTouch(trial.touch,item(trial.keyboard,key.key).rect,trial.keyboard.identity);
    for(const other of keys.filter(candidate=>candidate.key!==key.key)){
      assert.equal(value(trial.keyboard,other),value(trial.before,other));assert.equal(value(trial.pointer,other),value(trial.before,other));
    }
    previous=trial.pointer;
  }
  for(const [name,field,key] of [['tracker','trackerOpen','Tab'],['advanced','advancedOpen','F12']]){
    const row=e.keyboardPanels[name];assert.equal(row.before.ui[field],false);assert.equal(row.opened.ui[field],true);
    assert.equal(row.closed.ui[field],false);assert.equal(row.closed.held[key],false);
    assert.equal(visiblePanel(e.mobilePanels[name],name).expanded,true);
  }
  assert.equal(e.final.ui.menuOpen,false);assert.equal(e.final.ui.mobileVisible,false);assert.equal(e.final.ui.pointerDown,false);
  assert.ok(Object.values(e.final.held).every(value=>value===false));assert.deepEqual(e.cleanupErrors,[]);
  const identity=e.before.identity;
  assert.ok(identity.app>0&&Number.isFinite(identity.timeOrigin));
  let previousScene;
  for(const row of practiceControlSamples(e)){
    assert.deepEqual(row.identity,identity,'Practice control scene changed native document or GameApplication');
    assertControlledSceneWorld(row.scene);assert.equal(row.scene.requestGeneration,e.arm.requestGeneration);
    if(previousScene){
      assert.equal(row.scene.stage,previousScene.stage);
      for(const field of ['controlFrame','enemyFrame','withheldTimelineUpdates'])assert.ok(row.scene[field]>=previousScene[field],`Practice ${field} moved backwards`);
      assert.equal(row.scene.writeReceipt.committedBytes,previousScene.writeReceipt.committedBytes);
    }
    previousScene=row.scene;
  }
  assert.ok(e.final.scene.controlFrame>e.before.scene.controlFrame,'Practice control scene did not advance the native world');
}

/** Titles own native meanings and UI reads; operations and invariants remain shared. */
export async function runPracticeControlScene(io,keys){
  const e={schema:'eagler-touhou/practice-control-scene/1',arm:io.arm,trials:[],keyboardPanels:{},mobilePanels:{},cleanupErrors:[]};let failure;
  const wait=(label,predicate)=>io.waitUntil(label,predicate),save=()=>io.save(e);
  try{
    e.before=await io.read();assert.equal(e.before.ui.menuOpen,false);await io.press('Backspace');
    e.menuOpened=await wait('native Mod menu key consumed',s=>s.ui.menuOpen);await save();
    await io.toggleMobile();e.mobileOpened=await wait('native mobile panel entry visible',s=>s.ui.mobileVisible&&panelReady(s,'cheats'));
    e.openCheats=await io.touch(visiblePanel(e.mobileOpened,'cheats').rect);
    e.cheatsExpanded=await wait('actual native Cheat panel expanded',s=>panelReady(s,'cheats')&&s.ui.panels.cheats.expanded&&s.ui.items.length===keys.length&&s.ui.inputGeneration===s.ui.renderedGeneration);await save();
    for(const key of keys){
      const trial={key:key.key,before:await io.read()};e.trials.push(trial);const before=value(trial.before,key);
      await io.press(key.key);trial.keyboard=await wait('native keyboard toggles '+key.key,s=>value(s,key)!==before&&!s.held[key.key]&&
        s.ui.renderedGeneration===s.ui.inputGeneration&&s.ui.items.some(item=>item.key===key.key&&item.value!==before));await save();
      trial.touch=await io.touch(item(trial.keyboard,key.key).rect);
      trial.pointer=await wait('actual native touch widget restores '+key.key,s=>value(s,key)===before&&!s.ui.pointerDown&&s.ui.pendingPointerEdges===0&&
        s.ui.renderedGeneration===s.ui.inputGeneration&&s.ui.items.some(item=>item.key===key.key&&item.value===before));await save();
    }
    for(const [name,field,key] of [['tracker','trackerOpen','Tab'],['advanced','advancedOpen','F12']]){
      const row={before:await io.read()};e.keyboardPanels[name]=row;assert.equal(row.before.ui[field],false);
      await io.press(key);row.opened=await wait('native '+name+' key opens its owner',s=>s.ui[field]);
      await io.press(key);row.closed=await wait('native '+name+' key closes its owner',s=>!s.ui[field]&&!s.held[key]);await save();
    }
    for(const name of ['tracker','advanced']){
      const before=await wait('native '+name+' header ready',s=>panelReady(s,name)),panel=visiblePanel(before,name);assert.equal(panel.expanded,false);
      await io.touch(panel.rect);e.mobilePanels[name]=await wait('actual native '+name+' panel expanded',s=>panelReady(s,name)&&s.ui.panels[name].expanded&&s.ui.panels[name].vertexCount>panel.vertexCount);await save();
    }
    await io.toggleMobile();await wait('native mobile panels close',s=>!s.ui.mobileVisible);
    await io.press('Backspace');e.final=await wait('native Mod menu closes and input releases',s=>!s.ui.menuOpen&&!s.ui.pointerDown&&Object.values(s.held).every(value=>value===false));
    assertPracticeControlScene(e,keys);await save();return e;
  }catch(error){failure=error;if(error.touchEvidence)e.failedTouch=error.touchEvidence;error.evidence=e;throw error;}
  finally{try{await io.release();}catch(error){e.cleanupErrors.push(String(error));}await save();
    if(e.cleanupErrors.length&&!failure)throw Object.assign(Error('Practice control input cleanup failed'),{evidence:e});}
}
