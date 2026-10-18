import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';
test('TH11 stage resource sessions load every original stage bundle',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),session=c.resources_create(),stage=c.stage_resources_create();memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(session,data,raw.length),1);
 let checks=0,bossPrograms=0,messageBytes=0;
 try{for(let n=1;n<=7;n++){assert.equal(c.resources_stage(session,n,stage),1,'stage '+n);for(const k of [0,1,2,3])assert.ok(c.stage_value(stage,k)>0,`stage ${n} field ${k}`);assert.equal(c.stage_value(stage,5),6);for(let i=0;i<6;i++){const size=c.stage_message_size(stage,i);assert.ok(size>0);messageBytes+=size;}bossPrograms+=c.stage_value(stage,4);checks+=1;}
 report('stage-resources',{passed:true,checks,stages:7,bossPrograms,messageBytes,scope:'All seven original TH11 stage bundles: background/logo/enemy ANM, timeline ECL, six dialogue files per stage and present boss ECL files. Gameplay scheduling and dialogue rendering remain separate.'});
 }finally{c.stage_resources_delete(stage);c.resources_delete(session);c.release(data);}
});
