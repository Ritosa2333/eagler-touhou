import test from 'node:test';
import assert from 'node:assert/strict';
import {installFrameInspectorBridge} from '../src/frame-inspector-bridge.mjs';

test('inspector accepts only the same-origin parent and serializes seek before state',async()=>{
  let listener;const replies=[],calls=[],parent={postMessage:r=>replies.push(r)};
  const env={parent,location:{origin:'http://127.0.0.1:8113'},addEventListener:(_,f)=>listener=f};
  installFrameInspectorBridge({async seek(frame){calls.push(frame);await Promise.resolve();},state:()=>({cursor:calls.at(-1)})},env);
  const send=(id,source=parent,origin=env.location.origin)=>listener({source,origin,data:{id,protocol:'th12-frame-inspector/1',command:'seek',frame:42}});
  send('foreign',{},env.location.origin);send('origin',parent,'https://example.com');
  send('seek');listener({source:parent,origin:env.location.origin,data:{id:'state',protocol:'th12-frame-inspector/1',command:'state'}});
  await new Promise(resolve=>setImmediate(resolve));
  assert.deepEqual(calls,[42]);assert.deepEqual(replies.map(r=>[r.id,r.state.cursor]),[['seek',42],['state',42]]);
});

test('rejected command returns an error and does not poison later requests',async()=>{
  let listener;const replies=[],parent={postMessage:r=>replies.push(r)};
  const env={parent,location:{origin:'http://127.0.0.1:8113'},addEventListener:(_,f)=>listener=f};
  installFrameInspectorBridge({state:()=>({cursor:0})},env);
  for(const [id,command] of [['bad','write'],['ok','state']])listener({source:parent,origin:env.location.origin,data:{id,command,protocol:'th12-frame-inspector/1'}});
  await new Promise(resolve=>setImmediate(resolve));
  assert.match(replies[0].error,/不支持/);assert.equal(replies[1].state.cursor,0);
});
