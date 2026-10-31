import {test} from 'vitest';
import assert from 'node:assert/strict';
import {updatePeerRuntime} from '../scripts/update-peer-runtime.mjs';

test("test-peer-runtime-update", async () => {
const fixture=`const unchanged=42; export default async function runtime(Module){
function eagler_peer_connect(a,b,c,d,e){return 'old';}
function eagler_peer_send(a,b,c){return 1;}
function eagler_peer_send_to(a,b,c){return 1;}
function eagler_peer_send_repair_to(a,b,c){return 1;}
return Module;}`;
const bindings=[['eagler_peer_connect',5],['eagler_peer_send',3],
  ['eagler_peer_send_to',3],['eagler_peer_send_repair_to',3]];
const source=bindings.map(([name,count])=>{
  const params=Array.from({length:count},(_,i)=>'int '+String.fromCharCode(97+i));
  return `EM_JS(int, ${name}, (${params.join(',')}), { return 'updated-${name}'; });`;
}).join('\n');
const update=input=>updatePeerRuntime(input,source);
const result=update(fixture);
for(const [name] of bindings)assert.ok(result.output.includes('updated-'+name));
assert.match(result.output,/const unchanged=42/);
assert.match(result.output,/return Module;/);
assert.equal(result.receipt.bindings.length,4);
assert.equal(update(result.output).output,result.output);
assert.throws(()=>update(fixture.replace('a,b,c,d,e','a,b,c,d')),/ABI mismatch/);
assert.throws(()=>update(fixture.replace('eagler_peer_send_to','absent')),/binding missing/);
assert.throws(()=>update(fixture+'function eagler_peer_send_to(a,b,c){}'),/Ambiguous binding/);
console.log('Shared browser bridge update | native signatures | unrelated content | repeatability');
});
