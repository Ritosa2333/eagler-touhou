import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {runInNewContext} from 'node:vm';
import {EventEmitter} from 'node:events';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {createRuntimeFixtureSite,protocolHostPage,watchFixtureResponses} from './runtime-fixture-site.mjs';

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

test('browser byte receipt rejects injected HTML and preserves its exact raw response',async()=>{
  const output=await mkdtemp(resolve(tmpdir(),'eagler-response-receipt-'));
  const page=new EventEmitter(),base='https://127.0.0.1:8199';
  const original=Buffer.from('<!doctype html><p>日語</p>');
  const injected=Buffer.concat([original,Buffer.from('<script src="https://injected.invalid/"></script>')]);
  const identity=bytes=>({bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')});
  const expectedResponses=new Map([['/host',identity(original)],['/runtime.js',identity(Buffer.from('run()'))]]);
  const probe=watchFixtureResponses(page,{base,expectedResponses,output,label:'native'});
  const response=(path,bytes)=>({url:()=>base+path,status:()=>200,body:async()=>bytes,
    allHeaders:async()=>({'content-length':String(bytes.length)}),fromServiceWorker:()=>false});
  try{
    page.emit('response',response('/runtime.js',Buffer.from('run()')));await probe.flush();
    assert.equal(probe.responses.length,1);
    page.emit('response',response('/host',injected));
    await assert.rejects(()=>probe.flush(),/altered or unverified/);
    const mismatch=probe.responses.find(item=>item.url.endsWith('/host'));
    assert.deepEqual(mismatch.expected,identity(original));assert.deepEqual(mismatch.actual,identity(injected));
    assert.deepEqual(await readFile(resolve(output,mismatch.actualBody)),injected);
    assert.match(probe.errors[0],/identity mismatch/);
    probe.stop();assert.equal(page.listenerCount('response'),0);
  }finally{probe.stop();assert.equal(resolve(output,'..'),resolve(tmpdir()));await rm(output,{recursive:true,force:true});}
});

test('complete matching bytes establish content identity while an aborted transfer stays visible; truncated bytes fail',async()=>{
  const page=new EventEmitter(),base='https://127.0.0.1:8199',bytes=Buffer.from([1,2,3,255]);
  const expected={bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};
  const probe=watchFixtureResponses(page,{base,expectedResponses:new Map([['/font',expected]])});
  const response=body=>({url:()=>base+'/font',status:()=>200,body:async()=>body,
    allHeaders:async()=>({'content-length':String(bytes.length)}),fromServiceWorker:()=>false,
    captureDetails:()=>({method:'Network.streamResourceContent',terminal:{event:'loadingFailed',error:'net::ERR_ABORTED',canceled:true}})});
  try{
    page.emit('response',response(bytes));await probe.flush();
    assert.equal(probe.responses[0].contentIdentity,'passed');
    assert.equal(probe.responses[0].transport.error,'net::ERR_ABORTED');
    page.emit('response',response(bytes.subarray(0,-1)));
    await assert.rejects(()=>probe.flush(),/altered or unverified/);
    assert.equal(probe.responses[1].contentIdentity,'failed');
    assert.equal(probe.responses[1].actual.bytes,3);
  }finally{probe.stop();}
});
