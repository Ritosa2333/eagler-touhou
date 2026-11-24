import assert from 'node:assert/strict';
import {test} from 'node:test';
import {openTh128SpectatorRelay} from '../runtime/spectator-link.mjs';

class Connection extends EventTarget {
  readyState=0;
  constructor(){super();Connection.latest=this;}
  open(){this.readyState=1;this.dispatchEvent(new Event('open'));}
  send(){assert.fail('viewer must not send');}
  input(data){this.dispatchEvent(new MessageEvent('message',{data}));}
  close(){this.readyState=3;this.dispatchEvent(new Event('close'));}
}
const url='ws://127.0.0.1:53128/?room=th128mp-53128&run=1&spectator=viewer-entry&member=member-viewer&players=2';
test('viewer serializes asynchronous checkpoint setup before passing later confirmed frames',async()=>{
  const seen=[],decoder={announce(value){seen.push(value.session);},async accept(bytes){seen.push(bytes[0]);return 'frame';}};
  const viewer=openTh128SpectatorRelay({url,decoder,WebSocketClass:Connection,onPacket:type=>seen.push(type)});
  const client=Connection.latest;client.open();await viewer.ready;
  client.input(JSON.stringify({type:'spectator-session',session:'1234567890abcdef',seed:34772,generation:0}));
  client.input(new Uint8Array(32).fill(17).buffer);await viewer.drained();
  assert.deepEqual(seen,['1234567890abcdef','announcement',17,'frame']);assert.equal(viewer.send,undefined);
  viewer.close();assert.equal(client.readyState,3);
});
test('viewer route requires a bound observer and refuses a player role',()=>{
  const options={decoder:{announce(){},accept(){}},WebSocketClass:Connection};
  for(const path of [url+'&player=0',url.replace('member-viewer',''),url.replace('spectator=','lobby=')])
    assert.throws(()=>openTh128SpectatorRelay({url:path,...options}),/spectator endpoint/);
});
