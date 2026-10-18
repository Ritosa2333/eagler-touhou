// Schemas 1/2/3/4 from th10_web/cpp/multiplayer/SessionExports.cpp.
// Diagnostic capture only: call with a freshly copied 44-word canonical in
// the same JS turn, outside measured loops. This is not a portable graph hash.
const SEED = 2166136261;
const PRIME = 16777619;
const HEADER_WORDS = 10;
const POOL_WORDS = 10;
const POOL_TYPES = [
  {id: 1, name: 'rollback_hints', type: 'StageHint', capacity: 8192, blockSize: 0x88},
  {id: 2, name: 'rollback_entries', type: 'UpdateChainEntry', capacity: 4096},
  {id: 3, name: 'rollback_effects', type: 'ScreenEffect', capacity: 128, blockSize: 0x44},
  {id: 4, name: 'rollback_hint_owners', type: 'StageHints', capacity: 1, blockSize: 0x1a8},
  {id: 5, name: 'rollback_laser_owners', type: 'LaserManager', capacity: 1, blockSize: 0x45c},
  {id: 6, name: 'rollback_bomb_owners', type: 'Bomb', capacity: 3, blockSize: 0x48},
  {id: 7, name: 'rollback_session_owners', type: 'GameSession', capacity: 1, blockSize: 0x60},
  {id: 8, name: 'rollback_game_effect_owners', type: 'GameEffects', capacity: 1, blockSize: 0x18},
];

function requireThat(condition, message) {
  if (!condition) throw new Error('Graph storage: ' + message);
}

function uint32(value) {
  return Number.isInteger(value) && value >= 0 && value <= 0xffffffff;
}

function wasmAddress(value, name) {
  // An i32 export can expose a valid address above 2 GiB as a signed Number.
  requireThat(Number.isInteger(value) && value >= -0x80000000 &&
    value <= 0xffffffff, name + ' is not a wasm32 address');
  const address = value >>> 0;
  requireThat(address !== 0, name + ' is null');
  return address;
}

function hex(bytes) {
  let result = '';
  for (const byte of bytes) result += byte.toString(16).padStart(2, '0');
  return result;
}

function hashBytes(bytes, seed) {
  let value = seed;
  for (const byte of bytes) value = Math.imul(value ^ byte, PRIME) >>> 0;
  return value;
}

function hashIndex(index, seed) {
  let value = seed;
  for (let shift = 0; shift < 32; shift += 8)
    value = Math.imul(value ^ ((index >>> shift) & 0xff), PRIME) >>> 0;
  return value;
}

/**
 * Synchronously copy the complete native word 11 byte coverage and raw hints.
 * No runtime step, asynchronous work, pointer normalization, or owner writes.
 * Throws on malformed descriptors, changing memory, or a raw word 11 mismatch.
 * A hash-mismatch error carries the unaccepted raw capture as error.capture.
 */
export function captureGraphStorage(core, app, rawCanonical) {
  requireThat(typeof core?.multiplayer_graph_storage === 'function',
    'multiplayer_graph_storage export is missing');
  requireThat((Array.isArray(rawCanonical) || ArrayBuffer.isView(rawCanonical)) &&
    rawCanonical.length === 44, 'expected exactly 44 canonical words');
  const canonical = Array.from(rawCanonical);
  requireThat(canonical.every(uint32) && canonical[0] === 2,
    'expected raw canonical schema 2 with uint32 words');
  const applicationAddress = wasmAddress(app, 'application');
  const descriptorAddress = wasmAddress(
    core.multiplayer_graph_storage(applicationAddress), 'descriptor');
  const buffer = core.memory?.buffer;
  // A synchronous JS turn cannot exclude mutation by other WASM threads.
  requireThat(Object.prototype.toString.call(buffer) === '[object ArrayBuffer]',
    'expected unshared WASM memory');
  const memoryBytes = buffer.byteLength;

  function range(address, byteLength, name, alignment = 1) {
    requireThat(uint32(address) && address > 0 &&
      Number.isSafeInteger(byteLength) && byteLength > 0 &&
      address % alignment === 0 && address + byteLength <= memoryBytes &&
      address + byteLength <= 0x100000000, name + ' is outside WASM memory');
  }
  function copy(address, byteLength, name, alignment = 1) {
    range(address, byteLength, name, alignment);
    return new Uint8Array(buffer, address, byteLength).slice();
  }

  const headerBytes = copy(descriptorAddress, HEADER_WORDS * 4, 'header', 4);
  const headerView = new DataView(headerBytes.buffer);
  const header = Array.from({length: HEADER_WORDS},
    (_, index) => headerView.getUint32(index * 4, true));
  const [version, headerWords, poolWords, poolCount, pointerBytes,
    chainAddress, chainBytes, entryBytes, updateOffset, drawOffset] = header;
  const poolTypes=version===1?POOL_TYPES.slice(0,3):
    version===2?POOL_TYPES.slice(0,4):version===3?POOL_TYPES.slice(0,6):version===4?POOL_TYPES:null;
  requireThat(poolTypes && headerWords === HEADER_WORDS &&
    poolWords === POOL_WORDS && poolCount === poolTypes.length &&
    pointerBytes === 4, 'unsupported or inactive descriptor schema');
  requireThat(entryBytes > 0 && entryBytes % 4 === 0 &&
    chainBytes === entryBytes * 2 && updateOffset === 0 &&
    drawOffset === entryBytes, 'invalid UpdateChain layout');

  const descriptorBytes = copy(descriptorAddress,
    (headerWords + poolCount * poolWords) * 4, 'descriptor table', 4);
  const descriptorView = new DataView(descriptorBytes.buffer);
  const descriptorWords = Array.from({length: descriptorBytes.length / 4},
    (_, index) => descriptorView.getUint32(index * 4, true));
  requireThat(header.every((value, index) => value === descriptorWords[index]),
    'descriptor changed during capture');
  const chain = copy(chainAddress, chainBytes, 'chain_value', 4);
  const pools = [];
  let computed11 = hashBytes(chain, SEED);

  for (let poolIndex = 0; poolIndex < poolCount; ++poolIndex) {
    const offset = headerWords + poolIndex * poolWords;
    const [id, blocksAddress, blockSize, stride, capacity, occupiedAddress,
      occupiedBytes, cursorAddress, cursor, activeCount] =
      descriptorWords.slice(offset, offset + poolWords);
    const expected = poolTypes[poolIndex];
    requireThat(id === expected.id, 'unexpected, duplicate, or reordered pool ID');
    requireThat(capacity === expected.capacity &&
      blockSize === (expected.blockSize ?? entryBytes) &&
      blockSize > 0 && blockSize <= stride && stride % 4 === 0,
      expected.name + ' has invalid slot layout');
    requireThat(occupiedBytes === capacity && cursor < capacity &&
      activeCount <= capacity, expected.name + ' has invalid allocator metadata');
    range(blocksAddress, stride * capacity, expected.name + ' blocks', 4);
    const occupied = copy(occupiedAddress, occupiedBytes, expected.name + ' occupied');
    const cursorBytes = copy(cursorAddress, 4, expected.name + ' cursor', 4);
    requireThat(new DataView(cursorBytes.buffer).getUint32(0, true) === cursor,
      expected.name + ' cursor disagrees with descriptor');

    // Match native active(i): any nonzero occupied byte is active. Keep the
    // original occupied byte; coercing it to 1 would weaken the raw oracle.
    const slots = [];
    let poolHash = computed11;
    const inWord11=id===2||id===3||id===5||id===6||id===7||id===8;
    if (inWord11) {
      poolHash = hashBytes(occupied, poolHash);
      poolHash = hashBytes(cursorBytes, poolHash);
    }
    for (let index = 0; index < capacity; ++index) {
      if (occupied[index] === 0) continue;
      const address = blocksAddress + index * stride;
      const bytes = copy(address, blockSize, expected.name + ' slot ' + index);
      slots.push({index, address, bytesHex: hex(bytes)});
      if (inWord11) poolHash = hashBytes(bytes, hashIndex(index, poolHash));
    }
    requireThat(slots.length === activeCount,
      expected.name + ' active count disagrees with occupied bytes');
    if (inWord11) computed11 = poolHash;
    pools.push({
      id, name: expected.name, type: expected.type,
      blocksAddress, blockSize, stride, capacity,
      occupiedAddress, occupiedBytes, occupiedHex: hex(occupied),
      cursorAddress, cursor, cursorHex: hex(cursorBytes), activeCount, slots,
    });
  }

  requireThat(core.memory.buffer === buffer && buffer.byteLength === memoryBytes,
    'WASM memory changed during capture');
  const raw11 = canonical[11];
  const capture = {
    schema: {version, headerWords, poolWords, poolCount, pointerBytes, endian: 'little'},
    applicationAddress, memoryBytes,
    descriptor: {address: descriptorAddress, byteLength: descriptorBytes.length,
      words: descriptorWords, bytesHex: hex(descriptorBytes)},
    chain: {type: 'UpdateChain', address: chainAddress, byteLength: chainBytes,
      entryType: 'UpdateChainEntry', entryBytes, updateOffset, drawOffset,
      bytesHex: hex(chain)},
    pools, raw11, computed11,
    hash: {algorithm: 'FNV-1a-32', seed: SEED, prime: PRIME,
      order: ['chain_value', 'rollback_entries', 'rollback_effects',
        ...(version>=3?['rollback_laser_owners','rollback_bomb_owners']:[]),
        ...(version>=4?['rollback_session_owners','rollback_game_effect_owners']:[])],
      poolOrder: ['occupied bytes', 'cursor LE32', 'ascending active index LE32 + BlockSize bytes']},
    coverage: {
      word11: {completeNativeByteCoverage: true, matchesRawCanonical: computed11 === raw11},
      word10: {complete: false, rawHintsPoolCaptured: true,
        rawHintsOwnerPoolCaptured: version>=2,
        missing: 'Scene owners and stage components are outside this helper'},
      pointers: 'raw, unchanged',
      inactiveSlotBytes: false,
      storageAlignmentTailBytes: false,
    },
  };
  if (computed11 !== raw11) {
    const error = new Error('Graph storage: word 11 mismatch: computed ' +
      computed11 + ', raw canonical ' + raw11);
    error.capture = capture;
    throw error;
  }
  return capture;
}
