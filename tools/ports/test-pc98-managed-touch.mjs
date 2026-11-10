import assert from 'node:assert/strict';

globalThis.location = {origin: 'http://localhost'};
globalThis.addEventListener = () => {};
globalThis.document = {addEventListener() {}};

for (const game of ['th02', 'th04']) {
  const {managedInput} = await import(`../../ports/${game}/build-eagler/managed-input.mjs`);
  const input = managedInput({game, epoch: 1, active: () => true, options: () => ({touchEnabled: true, touchSensitivity: 150})});
  const pointer = (type, id, x) => input.pointer({type, id, x, y: .5});
  pointer('down', 1, .5); pointer('move', 1, .4); pointer('up', 1, .4);
  pointer('down', 2, .4); pointer('up', 2, .4);
  const first = input.motion();
  assert(first.active && first.begin && first.x < 0, `${game}: a tap must not erase the preceding unsampled swipe`);
  assert.equal(input.motion().x, 0, `${game}: movement must be consumed once`);
  input.motion(); input.motion();
  assert.equal(input.motion().active, false, `${game}: released touch must expire`);

  pointer('down', 3, .5); pointer('move', 3, .45); pointer('up', 3, .45);
  pointer('down', 4, .45); pointer('move', 4, .40); pointer('up', 4, .40);
  const combined = input.motion();
  assert(combined.active && combined.x === first.x, `${game}: consecutive swipes must accumulate`);
  console.log(`PASS ${game} consecutive touch gestures`);
}
