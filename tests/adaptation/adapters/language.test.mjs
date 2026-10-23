import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {languageObservationRequest,readLanguageObservation} from './language.mjs';

function nativeFixture(){
  const trace={schema:'eagler-touhou/language-trace/1',enabled:true,eventCapacity:256,uniqueEvents:0,
    droppedEvents:0,stringTruncated:0,totalObservations:0,events:[]};
  const bytes=new TextEncoder().encode(JSON.stringify(trace)),calls=[];
  const core={memory:{buffer:new ArrayBuffer(4096)},observe_on(value){calls.push(['enable',value]);},
    observed_size(){return bytes.length+1;},alloc(size){calls.push(['alloc',size]);return 16;},
    observed_copy(pointer,capacity){assert.equal(capacity,bytes.length+1);
      new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);return bytes.length;},
    free(pointer){calls.push(['free',pointer]);}};
  const context=vm.createContext({TextDecoder,Uint8Array,ArrayBuffer,__nextRuntime:{api:core}});
  const request={game:'next',variant:'normal',contract:{runtime:{global:'__nextRuntime',core:'api',module:'Module'},
    enableExport:'observe_on',sizeExport:'observed_size',copyExport:'observed_copy',allocateExport:'alloc',freeExport:'free'}};
  const reader=vm.runInContext(`(${readLanguageObservation.toString()})`,context);
  return{trace,calls,core,reader,request};
}

test('the shared reader enables and copies a new title observation without title-specific export names',()=>{
  const f=nativeFixture();
  assert.equal(f.reader({...f.request,action:'enable'}).enabled,true);
  const observed=f.reader(f.request);
  assert.equal(observed.available,true);assert.equal(observed.trace.schema,f.trace.schema);
  assert.deepEqual(f.calls.filter(call=>call[0]==='enable'),[['enable',1]]);
  assert.deepEqual(f.calls.at(-1),['free',16]);
  assert.equal(languageObservationRequest({game:'th08',variant:'multiplayer'}).contract,null);
  const titleRequest=languageObservationRequest({game:'th08',variant:'normal'});
  assert.deepEqual(structuredClone(titleRequest.contract).browserDiagnostics,
    {runtimeAsset:'th08-sdl.mjs',source:'ports/th08/web/cpp/game/Localization.cpp | TH08_LOCALIZATION_LOG | ports/th08/web/cpp/sdl/FontHost.cpp | SDL_Log'});
  assert.equal(Object.hasOwn(titleRequest.contract,'classifyLanguageDiagnostic'),false,
    'The title classifier remains in Node and is not serialized to the Runtime frame');
});

test('invalid native copies fail and release their allocation instead of producing an empty observation',()=>{
  for(const mutate of [
    f=>{f.core.observed_copy=()=>0;},
    f=>{f.core.observed_copy=(_pointer,capacity)=>capacity;},
    f=>{f.core.observed_copy=pointer=>{new Uint8Array(f.core.memory.buffer).fill(65);return 10;};},
    f=>{f.core.observed_copy=pointer=>{new Uint8Array(f.core.memory.buffer,pointer,3).set([255,255,0]);return 2;};},
  ]){
    const f=nativeFixture();mutate(f);assert.throws(()=>f.reader(f.request));
    assert.deepEqual(f.calls.at(-1),['free',16]);
  }
  const f=nativeFixture();f.core.observed_size=()=>32*1024*1024;
  assert.throws(()=>f.reader(f.request),/Invalid native.*size/);assert.equal(f.calls.length,0);
});
