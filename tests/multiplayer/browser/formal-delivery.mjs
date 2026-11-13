import assert from 'node:assert/strict';

// Compare all collected HTTP response bodies. This function cannot prove that
// pending responses have settled or that a response belongs to the game iframe;
// the caller must establish both before relying on this result.
export function assertBuildDelivery({expectedFiles, delivery, playerCount, frameGuids, documentEpochs}) {
  assert(Number.isSafeInteger(playerCount) && playerCount > 0, 'Delivery player count');
  assert(Array.isArray(expectedFiles), 'Expected executable list');
  assert.equal(expectedFiles.length, 2, 'Expected runtime loader and WASM');
  assert.deepEqual([...expectedFiles.map(file => file?.name)].sort(),
    ['th13-sdl.mjs', 'th13-sdl.wasm']);
  for (const file of expectedFiles) {
    assert(Number.isSafeInteger(file.bytes) && file.bytes > 0, 'Expected executable size');
    assert.equal(typeof file.sha256, 'string', 'Expected executable hash type');
    assert.match(file.sha256, /^[a-f0-9]{64}$/, 'Expected executable hash');
  }
  assert.equal(frameGuids === undefined, documentEpochs === undefined, 'Delivery frame and document identities must be paired');
  if (frameGuids !== undefined) {
    assert(Array.isArray(documentEpochs) && documentEpochs.length === playerCount, 'Delivery document epochs');
    assert(documentEpochs.every(value => Number.isSafeInteger(value) && value >= 0), 'Delivery document epoch');
    assert(Array.isArray(frameGuids) && frameGuids.length === playerCount, 'Delivery frame identities');
    assert(frameGuids.every(value => typeof value === 'string' && value.length > 0 && value.trim() === value),
      'Delivery frame identity');
    assert.equal(new Set(frameGuids).size, playerCount, 'Delivery frame identities must be unique');
  }
  assert(Array.isArray(delivery), 'Delivery response list');
  const rows = delivery.map((row, index) => {
    assert(row && typeof row === 'object', `Delivery ${index}: invalid response`);
    assert(Number.isSafeInteger(row.seat) && row.seat >= 0 && row.seat < playerCount,
      `Delivery ${index}: invalid seat`);
    if (frameGuids !== undefined) {
      assert.equal(row.frameGuid, frameGuids[row.seat], `Seat ${row.seat}: delivery frame identity`);
      assert.equal(row.documentEpoch, documentEpochs[row.seat], `Seat ${row.seat}: delivery document epoch`);
    }
    assert.equal(typeof row.url, 'string', `Delivery ${index}: invalid URL`);
    let url;
    try {url = new URL(row.url);} catch {assert.fail(`Delivery ${index}: invalid URL`);}
    assert(['http:', 'https:'].includes(url.protocol) && url.hostname && !url.username && !url.password,
      `Delivery ${index}: invalid URL origin`);
    const file=expectedFiles.find(file => url.pathname.endsWith('/' + file.name));
    assert(file, `Delivery ${index}: unexpected executable filename`);
    assert(Number.isSafeInteger(row.bytes) && row.bytes > 0, `Seat ${row.seat}: delivered ${file.name} size`);
    assert.equal(typeof row.sha256, 'string', `Seat ${row.seat}: delivered ${file.name} hash`);
    assert.match(row.sha256, /^[a-f0-9]{64}$/, `Seat ${row.seat}: delivered ${file.name} hash`);
    return {row, file};
  });
  for (let seat = 0; seat < playerCount; ++seat) {
    for (const file of expectedFiles) {
      const matching = rows.filter(value => value.row.seat === seat && value.file === file);
      assert(matching.length > 0, `Seat ${seat}: missing delivered ${file.name}`);
      for (const {row} of matching) {
        assert.equal(row.bytes, file.bytes, `Seat ${seat}: delivered ${file.name} size`);
        assert.equal(row.sha256, file.sha256, `Seat ${seat}: delivered ${file.name} hash`);
      }
    }
  }
  return {seats: playerCount, responses: rows.length, passed: true,
    scope: frameGuids === undefined ? 'collected response hashes only; frame identity not checked' :
      'collected response hashes and supplied frame/document identities; response completeness is caller responsibility'};
}
