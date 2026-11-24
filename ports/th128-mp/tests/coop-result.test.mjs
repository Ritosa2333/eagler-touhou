import assert from 'node:assert/strict';
import test from 'node:test';
import {readFile} from 'node:fs/promises';
import {bindCoopResult} from '../runtime/coop-result.mjs';

function fixture(){
  const el=()=>Object.assign(new EventTarget(),{focus(){this.focused=true;}});
  const ui=Object.fromEntries(['panel','caption','save','download','review','open','back'].map(k=>[k,el()]));
  const calls=[],validations=[],life=new AbortController();
  const result=bindCoopResult(ui,{signal:life.signal,makeFile:bytes=>new Promise((resolve,reject)=>validations.push({bytes,resolve,reject})),
    save:file=>calls.push(['save',file]),exportFile:file=>calls.push(['download',file]),enterReplay:file=>calls.push(['enter',file]),returnRoom:()=>calls.push(['room'])});
  return {ui,result,calls,validations,life,click:k=>ui[k].dispatchEvent(new Event('click'))};
}

test('shell has one result-owned save action, no HTML in-game pause/HUD and no file offer on title presentation',async()=>{
  const source=async path=>readFile(new URL(path,import.meta.url),'utf8');
  const [html,entry,css]=await Promise.all(['../site/th128.html','../site/managed-entry.mjs','../site/managed.css'].map(source));
  assert.doesNotMatch(html+css,/coop-pause|coop-hud|coop-viewport|保存最近一局多人录像/);
  assert.doesNotMatch(entry,/bindSeatHud|renderSeats|saveReplay\.hidden/);
  const panel=html.match(/<section id="coop-result"[^>]*>[\s\S]*?<\/section>/)?.[0];
  assert.ok(panel?.includes('id="save-replay"'));assert.ok(panel.includes('id="coop-open-replay"'));
  assert.equal((html.match(/id="save-replay"/g)??[]).length,1);
  const preview=entry.slice(entry.indexOf('async presentTitle('),entry.indexOf('async launchRoom('));
  assert.doesNotMatch(preview,/replay\.(enter|enableTitle|open)\(/);
});

test('sealed result owns save, review, explicit file flow, return focus and re-entry',async()=>{
  const {ui,result,calls,validations,click}=fixture();
  await result.show({active:true,result:'cleared'});assert.equal(ui.panel.hidden,true);
  result.show({active:true,paused:true});assert.equal(ui.panel.hidden,true);click('save');click('open');assert.deepEqual(calls,[]);
  const validated=result.show({active:true,epoch:1,result:'left',replayBytes:new Uint8Array([1])});
  assert.equal(ui.panel.hidden,false);assert.equal(ui.caption.textContent,'本局已结束');assert.equal(ui.save.disabled,true);
  click('save');click('review');assert.deepEqual(calls,[]);
  const file=new File(['sealed'],'one.th128mp');validations[0].resolve(file);await validated;
  click('save');click('review');assert.deepEqual(calls,[['save',file],['enter',file]]);assert.equal(ui.panel.hidden,true);
  click('save');click('open');click('back');assert.equal(calls.length,2,'hidden result cannot act');
  result.restore();assert.equal(ui.review.focused,true);click('open');assert.deepEqual(calls.at(-1),['enter',null]);
  result.restore();assert.equal(ui.open.focused,true);click('save');assert.deepEqual(calls.at(-1),['save',file]);click('back');assert.deepEqual(calls.at(-1),['room']);
});

test('archive from another epoch, retired result or delayed previous room cannot become this result’s file',async()=>{
  const {result,validations,ui,click,calls,life}=fixture();
  const old=result.show({active:true,epoch:1,result:'completed',replayBytes:new Uint8Array([1])});
  result.hide();result.restore();assert.equal(ui.panel.hidden,true);
  const current=result.show({active:true,epoch:1,result:'completed',replayBytes:new Uint8Array([2])}); // new room: even the epoch may repeat
  validations[0].resolve({name:'old'});await old;assert.equal(ui.save.disabled,true);click('save');assert.deepEqual(calls,[]);
  const file={name:'new'};validations[1].resolve(file);await current;click('save');assert.deepEqual(calls,[['save',file]]);
  result.show({active:true,epoch:2,result:null});assert.equal(ui.panel.hidden,true);click('review');click('save');result.restore();assert.deepEqual(calls,[['save',file]]);
  life.abort();assert.equal(ui.panel.hidden,true);
});

test('result saving stays local; a separate explicit action exports the same tape',async()=>{
  const {result,ui,validations,calls,click}=fixture();
  const reading=result.show({active:true,result:'left',replayBytes:Uint8Array.of(7)});
  const file=new File(['input'],'one.th128mp');validations[0].resolve(file);await reading;
  result.suspend(true);assert.equal(ui.panel.hidden,true);click('download');click('save');assert.deepEqual(calls,[]);
  result.suspend(false);assert.equal(ui.panel.hidden,false);
  click('save');assert.deepEqual(calls,[['save',file]]);
  click('download');assert.deepEqual(calls,[['save',file],['download',file]]);
});
