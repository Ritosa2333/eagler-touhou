import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,rm,readFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {createTelemetryStore,validateSample,startTelemetryServer} from '../src/netplay-telemetry.mjs';

const sample={version:1,id:'independent-client-123456',seq:1,product:'th08mp',players:2,role:'player',automated:false,seconds:0,route:'direct',links:[]};
test('shared product rules, bounded elapsed time and public identity removal',()=>{
  let now=Date.UTC(2026,9,18);const store=createTelemetryStore({},()=>now);
  assert.equal(validateSample({...sample,product:'unknown'}),null);
  assert(store.accept({...sample,ip:'private-ip',playerName:'private-name'}));
  now+=15000;
  assert(store.accept({...sample,seq:2,seconds:15,links:[{path:'direct',seconds:15,tx:100,rx:200,txPackets:1,rxPackets:2}]}));
  assert.equal(store.accept({...sample,seq:3,seconds:90}),false);
  const publicData=JSON.stringify(store.snapshot());
  for(const value of [sample.id,'private-ip','private-name'])assert.equal(publicData.includes(value),false);
});
test('independent HTTP service validates origin and persists only aggregates',async()=>{
  const directory=await mkdtemp(resolve(tmpdir(),'eagler-independent-telemetry-')),stateFile=resolve(directory,'state.json');let service;
  try {
    service=await startTelemetryServer({port:0,origin:'https://shell.example',stateFile});
    const base='http://127.0.0.1:'+service.server.address().port+'/netplay-stats';
    const post=origin=>fetch(base+'/sample',{method:'POST',headers:{Origin:origin,'Content-Type':'application/json'},body:JSON.stringify({...sample,ip:'do-not-store'})});
    assert.equal((await post('https://wrong.example')).status,403);
    assert.equal((await post('https://shell.example')).status,204);
    const publicData=await(await fetch(base+'/summary')).text();
    assert.equal(publicData.includes('do-not-store'),false);assert.equal(publicData.includes(sample.id),false);
    await service.close();service=null;
    assert.equal((await readFile(stateFile,'utf8')).includes(sample.id),false);
  }finally{await service?.close();await rm(directory,{recursive:true,force:true});}
});
