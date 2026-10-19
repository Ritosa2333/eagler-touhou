import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {runInNewContext} from 'node:vm';
import {createRuntimeFixtureSite,protocolHostPage} from './runtime-fixture-site.mjs';

test('protocol host rejects messages from another origin, frame, game, protocol or epoch',async()=>{
  const listeners=new Set(),sent=[],origin='http://127.0.0.1:8139';
  const contentWindow={postMessage:message=>sent.push(message)};
  const context={location:{origin},document:{querySelector:()=>({contentWindow})},
    addEventListener:(_type,listener)=>listeners.add(listener),removeEventListener:(_type,listener)=>listeners.delete(listener),
    crypto:{randomUUID:()=> 'fixed-request'},setTimeout,clearTimeout,fetch:()=>{throw Error('Not used');}};
  context.window=context;
  const html=protocolHostPage({target:{id:'th06-normal',game:'th06',entry:'index.html'},epoch:3});
  runInNewContext(html.match(/<script>([\s\S]*?)<\/script>/)[1],context);
  const correct={origin,source:contentWindow,data:{protocol:'eagler-touhou/1',game:'th06',epoch:3,event:'ready'}};
  const deliver=event=>{for(const listener of [...listeners])listener(event);};
  const invalid=[{...correct,origin:'https://other.test'}, {...correct,source:{}},
    ...[{protocol:'other'},{game:'th07'},{epoch:2}].map(change=>({...correct,data:{...correct.data,...change}}))];
  for(const event of invalid)deliver(event);
  assert.equal(context.events.length,0);
  deliver(correct);assert.equal(context.events.length,1);
  const response=context.rpc('configure');assert.equal(sent.length,1);
  for(const event of invalid)deliver({...event,data:{...event.data,request:'fixed-request',ok:true}});
  assert.equal(listeners.size,2,'An unrelated response must not complete the RPC');
  deliver({...correct,data:{...correct.data,request:'fixed-request',ok:true}});
  assert.equal((await response).ok,true);assert.equal(listeners.size,1);
});

test('fixture serves injected JavaScript bytes and rejects a Runtime without identity preflight',async()=>{
  const site=await createRuntimeFixtureSite({repoRoot:process.cwd(),targets:[{id:'fixture',entry:'fixture.html'}],
    files:new Map([['/injected.js',Buffer.from('window.fixture=true;')]])});
  try{
    const script=await fetch(site.base+'/injected.js');assert.equal(script.status,200);
    assert.equal(script.headers.get('content-type'),'text/javascript');assert.equal(await script.text(),'window.fixture=true;');
    const host=await fetch(site.base+'/host/fixture');assert.equal(host.status,500);
    assert.match(await host.text(),/has not passed preflight/);
    const runtime=await fetch(site.base+'/runtime/fixture/fixture.html');assert.equal(runtime.status,500);
  }finally{await site.close();}
});
test('fixture detects shared resource byte replacement at the same path',async()=>{
  const directory=await mkdtemp(resolve(tmpdir(),'eagler-fixture-shared-')),font=resolve(directory,'font.bin');
  await writeFile(font,Buffer.from([1,2,3]));
  const site=await createRuntimeFixtureSite({repoRoot:process.cwd(),targets:[{id:'fixture'}],shared:{'font.bin':font}});
  try{
    const original=await fetch(site.base+'/shared/font.bin');assert.deepEqual([...new Uint8Array(await original.arrayBuffer())],[1,2,3]);
    await writeFile(font,Buffer.from([1,4,3]));
    await assert.rejects(site.verifyShared(),/Shared resource changed/);
    const changed=await fetch(site.base+'/shared/font.bin');assert.equal(changed.status,500);assert.match(await changed.text(),/Shared resource changed/);
    assert.equal(site.httpFailures.length,1);
  }finally{await site.close();assert.ok(directory.startsWith(resolve(tmpdir(),'eagler-fixture-shared-')));await rm(directory,{recursive:true,force:true});}
});
