import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';
test('TH11 session resource bundle prepares core and stage ownership',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(raw.length),source=c.resources_create(),session=c.session_resources_create();memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 try{assert.equal(c.session_resources_core(session,source),1);for(const k of [0,1,2,3,4,5])assert.ok(c.session_resources_value(session,k)>0,`core ${k}`);for(let stage=1;stage<=7;stage++){assert.equal(c.session_resources_stage(session,source,stage),1);assert.equal(c.session_resources_value(session,6),stage);}report('session-resources',{passed:true,checks:7,coreAnm:6,coreSht:6,defaultEcl:true,stages:7,scope:'Owned core resource bundle and replaceable stage bundle loaded through the TH11 named archive boundary; no live frame, input, title or rendering loop is included.'});}
 finally{c.session_resources_delete(session);c.resources_delete(source);c.release(data);}
});
