import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createConfirmedHistory} from '../runtime/confirmed-history.mjs';
import {createTh128SpectatorEncoder,createTh128SpectatorDecoder} from '../runtime/spectator-wire.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)};
const rules=epoch=>createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'spectator-a13',epoch,
  mode:'parallel-stage',route:'A1-3',seed:34772,inputDelay:0,rollbackWindow:2,...build});
const row=(frame,pair,last=[0,0])=>({frame,inputs:pair.map((held,player)=>({player,held,pressed:held&~last[player],released:last[player]&~held,predicted:false}))});
const payload=message=>message.subarray(1);
async function room(epoch){
  const history=await createConfirmedHistory({rules:rules(epoch),allowedMask:763});
  return createTh128SpectatorEncoder(history.header,build);
}
function bootstrap(checkpoint,frame,serial=1){
  const bytes=payload(checkpoint).slice(0,32),fields=new DataView(bytes.buffer);
  bytes[5]=4;fields.setUint32(20,frame,true);fields.setUint32(28,serial,true);return bytes;
}

test('author Relay stage and frame envelopes rebuild the same two confirmed tracks in a receive-only viewer',async()=>{
  const owner=await room(1),spectator=createTh128SpectatorDecoder({build,matchId:'spectator-a13'});
  assert.deepEqual(Object.keys(owner.announce()).sort(),['generation','seed','session','type']);
  spectator.announce(owner.announce());
  const head=owner.open();
  assert.deepEqual([...head.subarray(0,9)],[0xe8,0x45,0x48,0x53,0x56,2,1,2,0]);
  await spectator.accept(payload(head));
  assert.equal(spectator.read(0),null);assert.equal(spectator.status().bootstrapped,false);
  const first=owner.frame(row(0,[64,128])),second=owner.frame(row(1,[0,128],[64,128]));
  assert.deepEqual([...first.subarray(0,9)],[0xe8,0x45,0x48,0x4e,0x50,4,3,2,0]);
  await spectator.accept(payload(first));await spectator.accept(payload(second));
  await spectator.accept(bootstrap(head,2));
  assert.equal(spectator.status().bootstrapped,true);
  assert.deepEqual(spectator.read(0),row(0,[64,128]));assert.deepEqual(spectator.read(1),row(1,[0,128],[64,128]));
  const end=owner.finish('restart');await spectator.accept(payload(end));
  assert.equal(spectator.status().end,'restart');assert.equal(spectator.status().nextFrame,2);
  assert.equal(spectator.send,undefined);
});

test('speculation, input gaps and frames from another epoch stay outside spectator history',async()=>{
  const owner=await room(1),next=await room(2),spectator=createTh128SpectatorDecoder({build,matchId:'spectator-a13'});
  const head=owner.open(),moved=row(0,[64,128]);
  await assert.rejects(spectator.accept(payload(head)),/announcement/);
  spectator.announce(owner.announce());await spectator.accept(payload(head));
  assert.throws(()=>owner.frame(row(1,[64,128])),/contiguous/);
  moved.inputs[0].predicted=true;assert.throws(()=>owner.frame(moved),/confirmed/);
  const first=owner.frame(row(0,[64,128])),gap=payload(first).slice();new DataView(gap.buffer).setUint32(16,1,true);
  await assert.rejects(spectator.accept(gap),/contiguous/);
  assert.equal(spectator.read(0),null);
  await spectator.accept(payload(first));await spectator.accept(payload(owner.finish('left')));
  spectator.announce(next.announce());
  await spectator.accept(payload(next.open()));
  await assert.rejects(spectator.accept(payload(first)),/identity/);
  assert.equal(spectator.status().epoch,2);assert.equal(spectator.status().nextFrame,0);
  assert.throws(()=>spectator.announce(owner.announce()),/generation/);
});
