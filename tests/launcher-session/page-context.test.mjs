import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
const url=new URL('../../site/runtime/page-launch-context.mjs',import.meta.url),module=fs.existsSync(url)?await import(url):{};
function create(options={}){assert.equal(typeof module.createPageLaunchContext,'function','Current page launch context missing');return module.createPageLaunchContext(options);}
test('the page ready promise waits for the actual runtime finish, never module import alone',async()=>{
 const c=create(),runtime={keyboard(){},clearInput(){},exit(){}};c.bindRuntime(runtime);let resolved=false;const pending=c.waitRuntime().then(x=>{resolved=true;return x;});await Promise.resolve();assert.equal(resolved,false);
 c.finish();assert.equal(await pending,runtime);
});
test('startup configuration is detached and applied only by the bound runtime',()=>{
 const config={language:'zh',options:{touchEnabled:false}},c=create({config});config.options.touchEnabled=true;
 assert.equal(c.config.options.touchEnabled,false);
});
test('abort before bind rejects readiness and disposes a late runtime without resurrecting the page',async()=>{
 const c=create();c.abort();let exits=0;c.bindRuntime({keyboard(){},clearInput(){},exit(){exits++;}});c.finish();
 await assert.rejects(c.waitRuntime(),e=>e.name==='AbortError');assert.equal(exits,1);
});
test('runtime failure is preserved and never converted to launch success',async()=>{
 const c=create();c.fail(Error('actual startup failed'));await assert.rejects(c.waitRuntime(),/actual startup failed/);c.finish();await assert.rejects(c.waitRuntime(),/actual startup failed/);
});
test('first-frame callback follows a positive actual presentation after ready and is never fabricated by finish',async()=>{
 const presentations=[],c=create({onPresented:value=>presentations.push(value)});c.bindRuntime({keyboard(){},clearInput(){},exit(){}});
 c.notePresented(true);c.finish();assert.deepEqual(presentations,[]);c.notePresented(false);assert.deepEqual(presentations,[]);c.notePresented(true);assert.deepEqual(presentations,[true]);
 c.abort();c.notePresented(true);assert.deepEqual(presentations,[true]);
});
test('exit clears one runtime only once even when abort and page cleanup overlap',()=>{
 let stops=0;const c=create();c.bindRuntime({keyboard(){},clearInput(){},exit(){stops++;}});c.finish();c.abort();c.abort();assert.equal(stops,1);assert.equal(c.signal.aborted,true);
});
