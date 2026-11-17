import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {Pc98RollbackCore} from './rollback-client.mjs';
const module=await WebAssembly.compile(await readFile(new URL('./rollback-core.wasm',import.meta.url)));
const core=new Pc98RollbackCore(module,{players:2,slot:0,limit:8,held:15,directions:15,directionFrames:2});
const input=buttons=>({buttons,mode:0,x:0,y:0,flags:0});
core.submit(1,0,input(8));
for(let frame=0;frame<4;frame++){core.submit(0,frame,input(0));const row=core.prepare(frame);assert(row);assert.equal(row[1].buttons,frame===3?0:8);core.commit(frame);}
core.submit(1,1,input(8));assert.equal(core.dirty,3);core.rewind(3);assert.equal(core.prepare(3)[1].buttons,8);core.close();
console.log('PASS PC-98 WASM bridge prediction correction');
