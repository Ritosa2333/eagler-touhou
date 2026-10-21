import test from 'node:test';
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import {stopDiagnosticChild,terminateDiagnosticProcessTree,trackDiagnosticChild} from './diagnostic-lifecycle.mjs';

class FakeChild extends EventEmitter{
  constructor({pid=321,onKill=()=>{}}={}){super();this.pid=pid;this.exitCode=null;this.signalCode=null;this.signals=[];this.onKill=onKill;}
  kill(signal){this.signals.push(signal);return this.onKill(signal,this);}
}

test('a close observed as null code and SIGTERM is a normal owned-server shutdown',async()=>{
  const child=new FakeChild({onKill(signal,self){
    if(signal==='SIGTERM')queueMicrotask(()=>self.emit('close',null,'SIGTERM'));
    return true;
  }});
  const result=await stopDiagnosticChild(child,{tracker:trackDiagnosticChild(child),platform:'linux',graceMs:20,finalMs:20});
  assert.deepEqual({exited:result.exited,normal:result.normal,code:result.code,signal:result.signal},
    {exited:true,normal:true,code:null,signal:'SIGTERM'});
  assert.deepEqual(child.signals,['SIGTERM']);
});

test('a child with no observed close after both bounded waits is an explicit failure',async()=>{
  const child=new FakeChild();
  const result=await stopDiagnosticChild(child,{tracker:trackDiagnosticChild(child),platform:'linux',graceMs:5,finalMs:5});
  assert.equal(result.exited,false);assert.equal(result.normal,false);assert.equal(result.escalated,true);
  assert.deepEqual(child.signals,['SIGTERM','SIGKILL']);
});

test('a force-killed child is observed but is not reported as a normal graceful shutdown',async()=>{
  const child=new FakeChild({onKill(signal,self){
    if(signal==='SIGKILL')queueMicrotask(()=>self.emit('close',null,'SIGKILL'));
    return true;
  }});
  const result=await stopDiagnosticChild(child,{tracker:trackDiagnosticChild(child),platform:'linux',graceMs:5,finalMs:20});
  assert.equal(result.exited,true);assert.equal(result.normal,false);assert.equal(result.signal,'SIGKILL');
});

test('a Windows tree-kill helper spawn error is captured and settles promptly',async()=>{
  const result=await terminateDiagnosticProcessTree(321,{timeoutMs:20,spawnProcess(){
    const helper=new FakeChild();helper.pid=undefined;
    queueMicrotask(()=>helper.emit('error',new Error('taskkill unavailable')));
    return helper;
  }});
  assert.equal(result.started,false);
  assert.match(result.error,/taskkill unavailable/);
});

test('a failed Windows tree-kill attempt still reports an unclosed server as alive',async()=>{
  const child=new FakeChild();
  const result=await stopDiagnosticChild(child,{tracker:trackDiagnosticChild(child),platform:'win32',graceMs:5,finalMs:5,
    terminateTree:async()=>({started:false,error:'taskkill spawn error'})});
  assert.equal(result.exited,false);assert.equal(result.normal,false);
  assert.deepEqual(result.killHelper,{started:false,error:'taskkill spawn error'});
});
