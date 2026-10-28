// TH13 consumes the Launcher's canonical gamepad mapping through the shared
// browser bridge. This test executes the real bridge, not a source-shape mock:
// identity is bound to protocol/game/epoch/origin, invalid bits fail closed,
// and not-ready clears the Launcher owner.
import test from 'node:test';
import assert from 'node:assert/strict';
import {createKeyboardInputBridge} from '../web/sdl-runtime/directory-keyboard.mjs';

const protocol = 'eagler-touhou/1';
const game = 'th13';
const epoch = 7;
const origin = 'https://example.test';

function fixture({ready = true, enabled = true, mappingEpoch = epoch, mappingOrigin = origin,
  sample = seat => [5, 9][seat] ?? -1} = {}) {
  const calls = {clear: 0};
  const seats = [];
  const target = {};
  target.__eaglerGamepadMapping = {
    schema: 'eagler-touhou/gamepad-input/1',
    protocol,
    game,
    epoch: mappingEpoch,
    origin: mappingOrigin,
    enabled: () => enabled,
    sample: seat => {seats.push(seat); return sample(seat);},
    clear: () => {calls.clear++;},
  };
  const keyboard = createKeyboardInputBridge({
    target, protocol, game, epoch, origin,
    ready: () => ready,
    dispatch: () => {},
    reset: () => {},
    profile: () => 'standard',
  });
  const bridge = target.__eaglerGamepadInputBridge;
  return {target, bridge, keyboard, calls, seats,
    setReady: value => {ready = value;},
    setEnabled: value => {enabled = value;}};
}

test('TH13 gamepad bridge publishes and forwards the canonical mapping identity', () => {
  const {target, bridge, seats} = fixture();
  assert.equal(target.__eaglerGamepadInputBridge, bridge);
  assert.equal(Object.isFrozen(bridge), true);
  assert.equal(bridge.schema, 'eagler-touhou/gamepad-input/1');
  assert.equal(bridge.protocol, protocol);
  assert.equal(bridge.game, game);
  assert.equal(bridge.epoch, epoch);
  assert.equal(bridge.origin, origin);
  assert.equal(bridge.owned(), true);
  assert.equal(bridge.sample(0), 5);
  assert.equal(bridge.sample(1), 9);
  assert.equal(bridge.sample(2), -1);
  assert.deepEqual(seats, [0, 1, 2]);
});

test('TH13 gamepad bridge rejects retired epoch/origin and clears when not ready', () => {
  const wrongEpoch = fixture({mappingEpoch: epoch + 1});
  assert.equal(wrongEpoch.bridge.owned(), false);
  assert.equal(wrongEpoch.bridge.sample(0), -1);
  assert.deepEqual(wrongEpoch.seats, []);

  const wrongOrigin = fixture({mappingOrigin: 'https://other.test'});
  assert.equal(wrongOrigin.bridge.owned(), false);
  assert.equal(wrongOrigin.bridge.sample(0), -1);
  assert.deepEqual(wrongOrigin.seats, []);

  const notReady = fixture({ready: false});
  assert.equal(notReady.bridge.sample(0), 0);
  assert.equal(notReady.calls.clear, 1);
  assert.deepEqual(notReady.seats, []);
});

test('TH13 gamepad bridge rejects malformed bits and follows enable/clear ownership', () => {
  const invalid = fixture({sample: () => 4096});
  assert.equal(invalid.bridge.sample(0), 0);
  const negative = fixture({sample: () => -2});
  assert.equal(negative.bridge.sample(0), 0);

  const disabled = fixture({enabled: false});
  assert.equal(disabled.bridge.owned(), false);
  disabled.setEnabled(true);
  assert.equal(disabled.bridge.owned(), true);
  disabled.keyboard.clear();
  assert.equal(disabled.calls.clear, 1);
});
