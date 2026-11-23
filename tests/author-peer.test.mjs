import assert from 'node:assert/strict';
import {test} from 'node:test';
import {openAuthorPeerLink} from '../runtime/author-peer.mjs';

const encoder=new TextEncoder(),decoder=new TextDecoder();
const room={role:'player',seat:0,relayUrl:'ws://127.0.0.1:40123/?room=th128mp-34772&run=1&member=member-test&player=0&players=2'};
function harness(selected='rtc'){
  const heap=new Uint8Array(65536),out=[],pending=[],allocations=new Set(),starts=[];
  let pointer=8,live=0,closed=0,error='';
  const module={HEAPU8:heap,_malloc(bytes){const result=pointer;pointer+=bytes;allocations.add(result);return result;},
    _free(value){assert.ok(allocations.delete(value));},UTF8ToString(pointer){return decoder.decode(heap.subarray(pointer,heap.indexOf(0,pointer)));},
    _th128_mp_open(at,seat){starts.push([module.UTF8ToString(at),seat]);return 1;},_th128_mp_close(){closed++;},
    _th128_mp_state(){return error?-1:live;},_th128_mp_mode(){const at=65000;heap.set(encoder.encode(selected+'\0'),at);return at;},
    _th128_mp_error(){const at=65020;heap.set(encoder.encode(error+'\0'),at);return at;},
    _th128_mp_send(at,size){out.push(heap.slice(at,at+size));return 1;},
    _th128_mp_poll(at,size){const packet=pending.shift();if(!packet)return 0;if(packet.length>size)return -1;heap.set(packet,at);return packet.length;},
    _th128_mp_buffered(){return 0;},_th128_mp_spectator_begin(){return 1;},_th128_mp_spectator_send(){return 1;},
  };
  return {module,out,pending,starts,allocations,open:()=>{live=1;},fail:text=>{error=text;},closed:()=>closed};
}

test('native author route owns ordered control messages; its RTC peer is the other bound seat',async()=>{
  const h=harness(),rows=[];
  const link=openAuthorPeerLink({module:h.module,room,onMessage:(value,seat)=>rows.push([value,seat])});
  try{
    assert.deepEqual(h.starts,[[room.relayUrl,0]]);
    assert.equal((await link.pump()).route,'connecting');assert.throws(()=>link.send({frame:0}),/route/);
    h.open();assert.equal((await link.pump()).route,'rtc');
    link.send({frame:0,held:64});assert.equal(decoder.decode(h.out[0]),'{"frame":0,"held":64}');
    h.pending.push(encoder.encode('{"frame":0,"held":128}'),encoder.encode('{"frame":1,"held":0}'));
    assert.equal((await link.pump()).received,2);assert.deepEqual(rows,[[{frame:0,held:128},1],[{frame:1,held:0},1]]);
    assert.throws(()=>openAuthorPeerLink({module:h.module,room,onMessage(){}}),/already owned/);
  }finally{link.close();}
  assert.equal(h.allocations.size,0);assert.equal(h.closed(),1);
});

test('the shared Relay lane requires the author sender envelope and owns cleanup on rejection',async()=>{
  const h=harness('relay'),rows=[];
  const link=openAuthorPeerLink({module:h.module,room,onMessage:(value,seat)=>rows.push([value,seat])});
  h.open();await link.pump();
  h.pending.push(new Uint8Array([0xe6,1,...encoder.encode('{"frame":0}')]));
  await link.pump();assert.deepEqual(rows,[[{frame:0},1]]);
  h.pending.push(new Uint8Array([0xe6,0,...encoder.encode('{"frame":1}')]));
  await assert.rejects(link.pump(),/sender/);assert.equal(h.allocations.size,0);assert.equal(h.closed(),1);
  const next=openAuthorPeerLink({module:h.module,room,onMessage(){}});h.fail('author link stopped');
  await assert.rejects(next.pump(),/author link stopped/);assert.equal(h.closed(),2);
});

for(const route of ['rtc','relay'])test(`3P author ${route} sender is the native channel or Relay seat, never packet contents`,async()=>{
  const h=harness(route),rows=[],calls=[],r={...room,seat:2,rules:{mode:'same-stage-coop',players:3},relayUrl:room.relayUrl.replace('player=0&players=2','player=2&players=3')};
  h.module._th128_mp_open_coop=(ptr,seat,count)=>{calls.push([h.module.UTF8ToString(ptr),seat,count]);return 1;};
  const link=openAuthorPeerLink({module:h.module,room:r,onMessage:(msg,seat)=>rows.push([msg.seat,seat])});
  try{assert.deepEqual(calls,[[r.relayUrl,2,3]]);assert.deepEqual(h.starts,[]);h.open();const tag=route==='rtc'?0xec:0xe6;
    for(const src of [0,1])h.pending.push(Uint8Array.of(tag,src,...encoder.encode(JSON.stringify({seat:1-src}))));
    assert.equal((await link.pump()).route,route);assert.deepEqual(rows,[[1,0],[0,1]]);
    h.pending.push(Uint8Array.of(tag,2,...encoder.encode('{}')));await assert.rejects(link.pump(),/sender/);
  }finally{link.close();}assert.equal(h.allocations.size,0);
});
test('both 2P and 3P cooperating builds require the sender-aware native adapter; naked or cross-route envelopes are refused',async()=>{
  for(const players of [2,3]){
    const h=harness(),r={...room,rules:{mode:'same-stage-coop',players},relayUrl:room.relayUrl.replace('players=2',`players=${players}`)};
    assert.throws(()=>openAuthorPeerLink({module:h.module,room:r,onMessage(){}}),/cooperative transport/i);assert.equal(h.allocations.size,0);
    h.module._th128_mp_open_coop=()=>1;h.open();for(const bytes of [encoder.encode('{"seat":1}'),Uint8Array.of(0xe6,1,...encoder.encode('{}')),Uint8Array.of(0xec,players,...encoder.encode('{}'))]){
      const link=openAuthorPeerLink({module:h.module,room:r,onMessage(){throw Error('must not deliver');}});h.pending.push(bytes);await assert.rejects(link.pump(),/sender/);assert.equal(h.allocations.size,0);
    }
  }
});