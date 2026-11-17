// Execute the actual complete shell with a pending module factory. EventTarget
// models browser lifecycle delivery; this is not actual bfcache/browser proof.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {runInNewContext} from 'node:vm';
const [sourcePath,outputPath]=process.argv.slice(2);
assert(sourcePath&&outputPath);
const source=readFileSync(sourcePath,'utf8');
assert(source.includes('const startupAbort=new AbortController();'));
const moduleBody=source.replace(/^import .*;\r?\n/gm,'');
assert(!/^import /m.test(moduleBody));
const rows=[];
for(const [name,events,expected] of [
 ['ordinary-departure',[['pagehide',false]],[true]],
 ['cache-entry',[['pagehide',true]],[false]],
 ['cache-return-and-departure',[['pagehide',true],['pageshow',true],['pagehide',false]],[false,false,true]],
 ['two-cache-returns-and-departure',[['pagehide',true],['pageshow',true],['pagehide',true],['pageshow',true],['pagehide',false]],[false,false,false,false,true]],
 ['ordinary-departure-is-terminal',[['pagehide',false],['pageshow',false]],[true,true]],
 ['unspecified-persisted',[['pagehide',undefined]],[true]],
]){
 const window=new EventTarget(),document=new EventTarget(),canvas=new EventTarget(),controllers=[];
 document.querySelector=selector=>selector==='canvas'?canvas:null;document.hidden=false;
 const context={window,document,location:{search:'?game=th14',origin:'http://localhost',href:'http://localhost/th14.html'},parent:window,URLSearchParams,AbortController:class extends AbortController{constructor(){super();controllers.push(this);}},console,performance,Uint8Array,Int32Array,TextDecoder,URL,
  createBrowserKeyboard:()=>({clear(){},event(){}}),bindOutsideTouches:()=>()=>{},runtimeEpochContract:()=>({fields:{},accepts(){return true;}}),scanCodes:{},createModule:()=>new Promise(()=>{}),setInterval:()=>0,
 };
 runInNewContext(moduleBody,context,{filename:sourcePath,timeout:1000});
 assert.equal(controllers.length,1);
 const observations=[];
 for(const [type,persisted] of events){const event=new Event(type);Object.defineProperty(event,'persisted',{value:persisted});window.dispatchEvent(event);observations.push(controllers[0].signal.aborted);}
 rows.push({name,observations,expected,passed:JSON.stringify(observations)===JSON.stringify(expected)});
}
const q={passed:rows.every(r=>r.passed),rows,sourceSha256:createHash('sha256').update(source).digest('hex'),actualCompleteShellEvaluated:true,moduleFactoryIntentionallyPending:true,eventTargetFixture:true,actualBrowserHiddenAccepted:false,actualBrowserBfcacheAccepted:false,completeGame:false};
writeFileSync(outputPath,JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify(q));process.exitCode=q.passed?0:1;
