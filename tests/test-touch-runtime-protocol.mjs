import assert from "node:assert/strict";
import {
  postDirectTouch,
  postHostedKey,
  postThpracMouse,
  postTouchCancel,
  postTouchControls,
} from "../.cache/build/browser/assets/launcher/touch-runtime-protocol.mjs";

const sent = [];
const target = {
  postMessage(message, targetOrigin) {
    sent.push({ message, targetOrigin });
  },
};

const ready = {
  target,
  targetOrigin: "https://launcher.invalid",
  protocol: "eagler-touhou/1",
  game: "th07",
  epoch: 17,
  launched: true,
  ready: true,
  spectator: false,
};
const controls = {
  fireEnabled: true,
  focusEnabled: false,
  bombSerial: 4,
  escapeSerial: 2,
  joystickX: -1234,
  joystickY: 5678,
};

assert.equal(postHostedKey({ ...ready, launched: false }, { code: "KeyR", key: "r", keyCode: 82 }, true), false);
assert.equal(postHostedKey(ready, { code: "KeyR", key: "r", keyCode: 82 }, true), true);
assert.deepEqual(sent.pop().message, {
  protocol: "eagler-touhou/1",
  game: "th07",
  epoch: 17,
  command: "keyboard",
  down: true,
  code: "KeyR",
  key: "r",
  keyCode: 82,
  location: 0,
});

assert.equal(postTouchControls({ ...ready, launched: false }, controls, 150), false);
assert.equal(postTouchControls({ ...ready, ready: false }, controls, 150), false);
assert.equal(postTouchControls({ ...ready, target: null }, controls, 150), false);
assert.equal(postTouchControls({ ...ready, spectator: true }, controls, 150), false);
assert.equal(sent.length, 0, "unavailable/spectator Runtime must not receive live controls");

assert.equal(postTouchControls(ready, controls, 150), true);
assert.deepEqual(sent.pop(), {
  targetOrigin: "https://launcher.invalid",
  message: {
    protocol: "eagler-touhou/1",
    game: "th07",
    epoch: 17,
    command: "touch-controls",
    ...controls,
    touchSensitivity: 150,
  },
});

const touch = { id: -3, x: 0.25, y: 0.75 };
assert.equal(postDirectTouch({ ...ready, launched: false }, "down", touch), false);
assert.equal(postDirectTouch({ ...ready, ready: false }, "down", touch), false);
assert.equal(postDirectTouch(ready, "down", null), false);
assert.equal(postDirectTouch({ ...ready, spectator: true }, "move", touch), true,
  "direct-touch protocol itself stays transport-only; spectator suppression belongs to the UI surface");
assert.equal(sent.pop().message.command, "direct-touch");
assert.equal(postDirectTouch(ready, "up", touch), true);
assert.deepEqual(sent.pop().message, {
  protocol: "eagler-touhou/1",
  game: "th07",
  epoch: 17,
  command: "direct-touch",
  type: "up",
  id: -3,
  x: 0.25,
  y: 0.75,
});

assert.equal(postTouchCancel({ ...ready, ready: false }), false);
assert.equal(postTouchCancel(ready), true);
assert.equal(sent.pop().message.command, "touch-cancel");

assert.equal(postThpracMouse({ ...ready, target: null }, "down", 10, 20), false);
assert.equal(postThpracMouse({ ...ready, epoch: 0 }, "down", 10, 20), false);
assert.equal(postThpracMouse(ready, "move", 123.5, 234.5), true);
assert.deepEqual(sent.pop().message, {
  protocol: "eagler-touhou/1",
  game: "th07",
  epoch: 17,
  command: "thprac-mouse",
  type: "move",
  x: 123.5,
  y: 234.5,
});

assert.equal(sent.length, 0);
const direct = [];
target.__eaglerDirectInputBridge = {
  schema:'eagler-touhou/direct-input/1',protocol:ready.protocol,game:ready.game,epoch:ready.epoch,origin:ready.targetOrigin,
  submit(message) { direct.push(message); return true; },
};
postDirectTouch(ready,'move',touch);
postTouchControls(ready,controls,100);
postHostedKey(ready,{code:'KeyX',key:'x',keyCode:88},true);
postTouchCancel(ready);
assert.deepEqual(direct.map(message=>message.command),['direct-touch','touch-controls','keyboard','touch-cancel']);
assert.equal(sent.length,0,'accepted movement/actions must never also be posted');
target.__eaglerDirectInputBridge.epoch = ready.epoch - 1;
postDirectTouch(ready,'move',touch);
assert.equal(sent.pop().message.epoch,ready.epoch,'stale direct-input bridge epoch must fall back to current postMessage transport');
target.__eaglerDirectInputBridge.epoch = ready.epoch;
postDirectTouch({...ready,targetOrigin:'https://wrong.invalid'},'move',touch);
assert.equal(sent.pop().targetOrigin,'https://wrong.invalid','fast path respects targetOrigin');
target.__eaglerDirectInputBridge.submit = () => false;
postDirectTouch(ready,'up',touch);
assert.equal(sent.pop().message.type,'up','explicit refusal falls back before consumption');
target.__eaglerDirectInputBridge.submit = () => { throw new Error('unknown outcome'); };
assert.throws(()=>postDirectTouch(ready,'move',touch),/unknown outcome/);
assert.equal(sent.length,0,'never retry an input after a throw');
delete target.__eaglerDirectInputBridge;
Object.defineProperty(target,'__eaglerDirectInputBridge',{get(){throw new Error('SecurityError');}});
postTouchCancel(ready);
assert.equal(sent.pop().message.command,'touch-cancel','cross-origin lookup preserves postMessage');
console.log(JSON.stringify({
  touchRuntimeProtocol: "PASS",
  liveControls: "ready-non-spectator",
  directTouch: "transport",
  touchCancel: "ready-runtime",
  thpracMouse: "child-runtime",
}));
