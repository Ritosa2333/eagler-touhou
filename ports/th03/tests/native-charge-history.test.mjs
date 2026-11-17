import test from 'node:test';
import assert from 'node:assert/strict';
import {NativeChargeHistory, validChargeSample} from '../web/native-charge-history.mjs';

test('out of order samples correct predictions and survive rewind', () => {
  const history = new NativeChargeHistory();
  const held = [48, 0, -1, 4], release = [0, 0, 1024, 5];
  history.markUsed(1, held);
  assert.equal(history.accept(1, release, 1), true);
  assert.equal(history.accept(0, held, 1), false);
  history.rewind(1);
  assert.deepEqual(history.get(1), release);
  history.markUsed(1, history.get(1));
  assert.equal(history.accept(1, release, 1), false);
  history.confirm(0); history.confirm(1);
  assert.equal(history.confirmed, 2);
});

test('samples are copied, bounded and immutable', () => {
  const history = new NativeChargeHistory(2), value = [0, 0, -1, 4];
  history.accept(0, value, 0); value[0] = 2;
  assert.deepEqual(history.get(0), [0, 0, -1, 4]);
  assert.throws(() => history.accept(0, value, 0), /Conflicting/);
  assert.throws(() => history.accept(3, value, 0), /Invalid/);
  assert.throws(() => history.confirm(1), /confirmed/);
  assert.equal(validChargeSample([0, 0, 1024, 0]), false);
  assert.equal(validChargeSample([0, 0, -1, 1]), false);
  assert.equal(validChargeSample([4081, 0, -1, 0]), false);
});
