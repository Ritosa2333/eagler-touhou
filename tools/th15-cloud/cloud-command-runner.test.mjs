// Negative-path command orchestration tests, not gameplay or performance evidence.
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,rmSync,readFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {runCloudCommand} from './cloud-command-runner.mjs';
function execute(t,code,timeout=5000){
 const root=mkdtempSync(resolve(tmpdir(),'th15-command-test-'));t.after(()=>rmSync(root,{recursive:true,force:true}));
 const result=runCloudCommand(process.execPath,['--input-type=module','-e',code],{log:resolve(root,'output.log'),cwd:root,timeout});
 return {result,output:readFileSync(resolve(root,'output.log'),'utf8')};
}
test('records successful command and complete output',t=>{
 const {result,output}=execute(t,'console.log("ACTUAL_CHILD")');assert.equal(result.exitCode,0);assert.equal(result.error,null);
 assert.equal(output.trim(),'ACTUAL_CHILD');assert.equal(result.cleanup.verified,true);assert.deepEqual(result.cleanup.liveAfter,[]);
});
test('does not turn a failed command into success',t=>{
 const {result}=execute(t,'process.exit(42)');assert.equal(result.exitCode,42);assert.equal(result.cleanup.verified,true);
});
test('timeout kills an uncooperative leader and its inherited group',t=>{
 const {result}=execute(t,`import {spawn} from 'node:child_process';
  const child=spawn(process.execPath,['-e','process.on("SIGTERM",()=>{});setInterval(()=>{},1000)'],{stdio:'ignore'});
  child.unref();process.on('SIGTERM',()=>{});setInterval(()=>{},1000);`,500);
 assert.equal(result.exitCode,null);assert.equal(result.signal,'SIGKILL');assert.match(result.error,/ETIMEDOUT/);
 assert.ok(result.cleanup.liveBefore.length>=1,'descendant must really exist at timeout');assert.equal(result.cleanup.verified,true);
 assert.deepEqual(result.cleanup.liveAfter,[]);
});
test('normal leader exit does not leak an inherited child',t=>{
 const {result}=execute(t,`import {spawn} from 'node:child_process';
  const child=spawn(process.execPath,['-e','setInterval(()=>{},1000)'],{stdio:'ignore'});child.unref();`);
 assert.equal(result.exitCode,0);assert.ok(result.cleanup.liveBefore.length>=1);assert.equal(result.cleanup.signal,'SIGKILL');
 assert.equal(result.cleanup.verified,true);assert.deepEqual(result.cleanup.liveAfter,[]);
});
test('missing program is failure without an invented process group',t=>{
 const root=mkdtempSync(resolve(tmpdir(),'th15-command-missing-'));t.after(()=>rmSync(root,{recursive:true,force:true}));
 const result=runCloudCommand(resolve(root,'missing'),[],{log:resolve(root,'output.log'),cwd:root,timeout:1000});
 assert.equal(result.exitCode,null);assert.match(result.error,/ENOENT/);assert.equal(result.cleanup.verified,true);
});
