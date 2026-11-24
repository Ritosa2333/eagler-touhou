import assert from 'node:assert/strict';import test from 'node:test';import {createCoopTitleMusic} from '../runtime/coop-title-music.mjs';
test('OGG room preview owns the original title loop; gesture, stop, return and music none use one decode',async()=>{
 const signal=new AbortController(),events=new EventTarget(),calls=[],states=[];let release;
 const music={load(_rows,_base,_resolve,{tracks}){calls.push(tracks);return new Promise(r=>release=r);},start(){calls.push('start');},stopSource(){calls.push('stop');},dispose(){calls.push('dispose');}};
 const ctx={state:'suspended',destination:{},createGain:()=>({connect(){},disconnect(){}}),resume(){this.state='running';return Promise.resolve();},close(){this.state='closed';return Promise.resolve();}};
 const owner=createCoopTitleMusic({resources:{musicTracks:[]},signal:signal.signal,eventTargets:[events],createContext:()=>ctx,makeMusic:()=>music,onState:s=>states.push(s)});
 owner.show(true);owner.hide();release();await new Promise(r=>setImmediate(r));assert.deepEqual(calls,[['th128_07.wav']]);
 owner.show(false);events.dispatchEvent(new Event('pointerdown'));assert.deepEqual(calls,[['th128_07.wav']]);
 owner.show(true);events.dispatchEvent(new Event('keydown'));await new Promise(r=>setImmediate(r));assert.equal(states.at(-1),'playing');
 owner.hide();assert.equal(states.at(-1),'off');owner.show(true);await new Promise(r=>setImmediate(r));assert.equal(states.at(-1),'playing');
 signal.abort();assert.deepEqual(calls,[['th128_07.wav'],'start','stop','start','stop','dispose']);
});
