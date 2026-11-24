const MAX_COMPRESSED_SIZE = 8 * 1024 * 1024;
const MAX_UNCOMPRESSED_SIZE = 32 * 1024 * 1024;
const MAX_FILE_SIZE = 16 * 1024 * 1024;
const MAX_USER_DATA_SIZE = 4 * 1024 * 1024;
const MAX_FRAMES = 1000000;
const MAX_STAGES = 3;
const EMPTY = new Uint8Array();
const ROUTES = ['A1', 'A2', 'B1', 'B2', 'C1', 'C2', 'Extra'];
const RANKS = ['Easy', 'Normal', 'Hard', 'Lunatic', 'Extra'];
const STAGES = [null, 'A1-1', 'A1-2', 'A1-3', 'A2-2', 'A2-3',
  'B1-1', 'B1-2', 'B1-3', 'B2-2', 'B2-3', 'C1-1', 'C1-2', 'C1-3',
  'C2-2', 'C2-3', 'Extra', 'A1 All', 'A2 All', 'B1 All', 'B2 All', 'C1 All', 'C2 All', 'Ex All'];
const typedArrayPrototype = Object.getPrototypeOf(Uint8Array.prototype);
const bufferGetter = Object.getOwnPropertyDescriptor(typedArrayPrototype, 'buffer').get;
const offsetGetter = Object.getOwnPropertyDescriptor(typedArrayPrototype, 'byteOffset').get;
const lengthGetter = Object.getOwnPropertyDescriptor(typedArrayPrototype, 'byteLength').get;
const resizableGetter = Object.getOwnPropertyDescriptor(ArrayBuffer.prototype, 'resizable')?.get;
let textDecoder = null;
try {
  textDecoder = new TextDecoder('shift_jis');
} catch {
  textDecoder = null;
}

class ReplayFormatError extends Error {
  constructor(message) {
    super(message);
    this.name = 'ReplayFormatError';
    this.code = 'TH128_REPLAY_FORMAT';
  }
}

function require(condition, message) {
  if (!condition) throw new ReplayFormatError(message);
}

function record(value, label) {
  require(value !== null && typeof value === 'object'
    && [Object.prototype, null].includes(Object.getPrototypeOf(value)), `${label} must be a record`);
  return value;
}

function bytes(value, label, cap = MAX_FILE_SIZE, size = null) {
  require(ArrayBuffer.isView(value) && value instanceof Uint8Array, `${label} must be Uint8Array`);
  let buffer;
  let offset;
  let length;
  try {
    buffer = bufferGetter.call(value);
    offset = offsetGetter.call(value);
    length = lengthGetter.call(value);
    require(buffer instanceof ArrayBuffer, `${label} cannot use shared storage`);
    require(!resizableGetter || !resizableGetter.call(buffer), `${label} cannot use resizable storage`);
    new DataView(buffer, offset, length);
  } catch (error) {
    if (error instanceof ReplayFormatError) throw error;
    throw new ReplayFormatError(`${label} has detached or invalid storage`);
  }
  require(Number.isSafeInteger(length) && length <= cap, `${label} exceeds size budget`);
  require(size === null || length === size, `invalid ${label} length`);
  return new Uint8Array(buffer, offset, length);
}

function dataView(data) {
  return new DataView(data.buffer, data.byteOffset, data.byteLength);
}

function span(data, offset, size, label) {
  require(Number.isSafeInteger(offset) && Number.isSafeInteger(size)
    && offset >= 0 && offset <= data.length && size >= 0 && size <= data.length - offset,
  `${label} is out of bounds`);
  return data.subarray(offset, offset + size);
}

function combine(parts, cap, label) {
  let size = 0;
  for (const part of parts) {
    require(part.length <= cap - size, `${label} exceeds size budget`);
    size += part.length;
  }
  const output = new Uint8Array(size);
  let cursor = 0;
  for (const part of parts) {
    output.set(part, cursor);
    cursor += part.length;
  }
  return output;
}

function putUint32(target, offset, value) {
  require(Number.isSafeInteger(value) && value >= 0 && value <= 0xffffffff,
    'invalid uint32 field value');
  dataView(target).setUint32(offset, value, true);
}

function crypt(data, block, initialMask, increment, encrypt = false) {
  require(data.length <= MAX_COMPRESSED_SIZE, 'cipher data exceeds size budget');
  const result = data.slice();
  const tail = data.length % block;
  const count = Math.max(0, data.length - (tail < block / 4 ? tail : 0) - data.length % 2);
  let cursor = 0;
  let mask = initialMask;
  while (cursor < count) {
    const base = cursor;
    const size = Math.min(block, count - cursor);
    for (const first of [base + size - 1, base + size - 2]) {
      for (let target = first; target >= base; target -= 2) {
        if (encrypt) result[cursor] = data[target] ^ mask;
        else result[target] = data[cursor] ^ mask;
        cursor += 1;
        mask = (mask + increment) % 256;
      }
    }
  }
  return result;
}

class BitReader {
  constructor(data) {
    this.data = data;
    this.position = 0;
  }

  get remaining() {
    return this.data.length * 8 - this.position;
  }

  read(width) {
    require(width <= this.remaining, 'truncated LZSS token');
    let value = 0;
    while (width > 0) {
      const available = 8 - this.position % 8;
      const take = Math.min(width, available);
      const current = this.data[Math.floor(this.position / 8)];
      value = value * 2 ** take + Math.floor(current / 2 ** (available - take)) % 2 ** take;
      this.position += take;
      width -= take;
    }
    return value;
  }
}

function decompress(data, expectedSize) {
  require(Number.isSafeInteger(expectedSize) && expectedSize >= 0
    && expectedSize <= MAX_UNCOMPRESSED_SIZE, 'uncompressed size exceeds budget');
  require(data.length > 0 && data.length <= MAX_COMPRESSED_SIZE
    && expectedSize <= data.length * 8, 'impossible LZSS expansion size');
  const reader = new BitReader(data);
  const output = new Uint8Array(expectedSize);
  let written = 0;
  while (written < expectedSize) {
    if (reader.read(1) === 1) {
      output[written++] = reader.read(8);
    } else {
      const index = reader.read(13);
      require(index !== 0, 'LZSS terminated before declared size');
      const length = reader.read(4) + 3;
      require(length <= expectedSize - written, 'LZSS expansion exceeds declared size');
      const distance = (written - index + 8192) % 8192 + 1;
      require(distance <= written, 'LZSS reference to uninitialized history');
      for (let copied = 0; copied < length; copied += 1) {
        output[written] = output[written - distance];
        written += 1;
      }
    }
  }
  require(reader.remaining > 0 && reader.remaining <= 21, 'missing or excessive LZSS terminator data');
  require(reader.read(reader.remaining) === 0, 'nonzero LZSS terminator or padding');
  return output;
}

function compressLiterals(data) {
  const size = Math.ceil((data.length * 9 + 2) / 8);
  require(size <= MAX_COMPRESSED_SIZE, 'literal encoding exceeds compressed size budget');
  const result = new Uint8Array(size);
  let cursor = 0;
  let accumulator = 0;
  let available = 0;
  for (const value of data) {
    accumulator = accumulator * 512 + 256 + value;
    available += 9;
    while (available >= 8) {
      available -= 8;
      result[cursor++] = Math.floor(accumulator / 2 ** available) % 256;
    }
    accumulator %= 2 ** available;
  }
  accumulator *= 4;
  available += 2;
  while (available >= 8) {
    available -= 8;
    result[cursor++] = Math.floor(accumulator / 2 ** available) % 256;
  }
  if (available > 0) result[cursor] = accumulator * 2 ** (8 - available) % 256;
  return result;
}

class ReplayInputs {
  #raw;
  #view;
  #length;

  constructor(raw) {
    require(raw.length % 6 === 0 && raw.length <= MAX_FRAMES * 6, 'invalid input records');
    this.#raw = raw;
    this.#view = dataView(raw);
    this.#length = raw.length / 6;
    Object.freeze(this);
  }

  get length() {
    return this.#length;
  }

  get raw() {
    return this.#raw;
  }

  at(index) {
    require(Number.isSafeInteger(index), 'frame index must be a safe integer');
    const position = index < 0 ? this.#length + index : index;
    require(position >= 0 && position < this.#length, 'frame index out of range');
    require(this.#raw.byteLength === this.#length * 6, 'input records have detached storage');
    const offset = position * 6;
    return [this.#view.getUint16(offset, true), this.#view.getUint16(offset + 2, true),
      this.#view.getUint16(offset + 4, true)];
  }

  *[Symbol.iterator]() {
    for (let index = 0; index < this.#length; index += 1) yield this.at(index);
  }
}

function label(labels, value) {
  return value >= 0 && value < labels.length ? labels[value] : null;
}

function text(data) {
  return textDecoder ? textDecoder.decode(data) : null;
}

function parsePayload(payload) {
  const raw = span(payload, 0, 112, 'metadata').slice();
  const metadataView = dataView(raw);
  const count = metadataView.getUint32(88, true);
  require(count >= 1 && count <= MAX_STAGES, 'unsupported replay stage count');
  const name = raw.subarray(0, 8);
  const ending = name.indexOf(0);
  const score = metadataView.getUint32(20, true);
  const route = metadataView.getUint32(92, true);
  const rank = metadataView.getUint32(100, true);
  const metadata = {
    raw, nameRaw: raw.slice(0, 12), name: text(ending < 0 ? name : name.subarray(0, ending)),
    timestamp: metadataView.getBigUint64(12, true), scoreRaw: score, score: score * 10,
    gameConfigRaw: raw.slice(24, 84), slowdown: metadataView.getFloat32(84, true), stageCount: count,
    route, routeName: label(ROUTES, route), rank, rankName: label(RANKS, rank),
    lastStage: metadataView.getUint32(104, true),
  };
  const stages = [];
  let cursor = 112;
  let frames = 0;
  for (let stageIndex = 0; stageIndex < count; stageIndex += 1) {
    const rawHeader = span(payload, cursor, 144, 'stage header').slice();
    const stageView = dataView(rawHeader);
    const numFrames = stageView.getUint32(4, true);
    const size = stageView.getUint32(8, true);
    require(numFrames <= MAX_FRAMES - frames, 'replay frame budget exceeded');
    require(numFrames * 6 <= size, 'stage payload is shorter than its frame records');
    frames += numFrames;
    const body = span(payload, cursor + 144, size, 'stage body');
    const inputRaw = body.slice(0, numFrames * 6);
    const stage = stageView.getUint16(0, true);
    const positionRaw = [stageView.getInt32(24, true), stageView.getInt32(28, true)];
    const scoreRaw = stageView.getUint32(12, true);
    const shotPowerRaw = stageView.getUint32(16, true);
    const maxPivRaw = stageView.getUint32(20, true);
    const motivationRaw = stageView.getUint32(128, true);
    const perfectFreezeRaw = stageView.getUint32(132, true);
    stages.push({ rawHeader, stage, stageName: label(STAGES, stage), seed: stageView.getUint16(2, true),
      numFrames, size, scoreRaw, score: scoreRaw * 10, shotPowerRaw, shotPower: shotPowerRaw + 1,
      maxPivRaw, maxPiv: maxPivRaw / 100, positionRaw, position: positionRaw.map(value => value / 128),
      continues: stageView.getUint32(32, true), graze: stageView.getUint32(40, true),
      motivationRaw, motivation: motivationRaw / 100, perfectFreezeRaw, perfectFreeze: perfectFreezeRaw / 100,
      freezeArea: stageView.getFloat32(136, true), inputRaw, inputs: new ReplayInputs(inputRaw),
      fpsRaw: body.slice(numFrames * 6) });
    cursor += 144 + size;
  }
  return { metadata, stages, payloadTailRaw: payload.slice(cursor) };
}

function decodePayload(encoded, size) {
  return decompress(crypt(crypt(encoded, 0x800, 0x5e, 0xe7), 0x80, 0x7d, 0x36), size);
}

function validateIdentity(header) {
  const headerView = dataView(header);
  require(headerView.getUint32(0, true) === 0x72383231, 'invalid TH128 replay magic');
  require(headerView.getUint32(4, true) === 2, 'unsupported TH128 replay version');
}

function userSequence(blocks){
  if(!Array.isArray(blocks)||blocks.length>4)return false;
  const kind=block=>block?.kind??(block?.headerRaw?.length===12?dataView(block.headerRaw).getUint32(8,true):-1);
  let count=blocks.length;
  if(count&&kind(blocks[count-1])===0x43415250)--count;
  if(count&&kind(blocks[count-1])===0x31484354)--count;
  return [0,2].includes(count)&&blocks.slice(0,count).every(b=>![0x43415250,0x31484354].includes(kind(b)));
}
export function decodeReplay(input) {
  const data = bytes(input, 'replay file').slice();
  const raw = span(data, 0, 36, 'replay header').slice();
  validateIdentity(raw);
  const headerView = dataView(raw);
  const compressedSize = headerView.getUint32(28, true);
  const uncompressedSize = headerView.getUint32(32, true);
  const userDataOffset = headerView.getUint32(12, true);
  require(compressedSize > 0 && compressedSize <= MAX_COMPRESSED_SIZE, 'compressed size exceeds budget');
  require(uncompressedSize >= 112 && uncompressedSize <= MAX_UNCOMPRESSED_SIZE,
    'uncompressed size exceeds budget');
  const encodedPayload = span(data, 36, compressedSize, 'encoded payload').slice();
  const end = 36 + compressedSize;
  const userBlocks = [];
  let dataGapRaw = EMPTY.slice();
  let fileTailRaw;
  if (userDataOffset !== 0) {
    require(userDataOffset >= end && userDataOffset <= data.length - 12, 'invalid or overlapping USER offset');
    dataGapRaw = data.slice(end, userDataOffset);
    let cursor = userDataOffset;
    let userSize = 0;
    for (let index = 0; index < 4; index += 1) {
      const suffix=index>=2||userBlocks[0]?.kind===0x31484354;
      if(suffix&&(cursor+12>data.length||dataView(data).getUint32(cursor,true)!==0x52455355
        ||![0x43415250,0x31484354].includes(dataView(data).getUint32(cursor+8,true))))break;
      const headerRaw = span(data, cursor, 12, 'USER header').slice();
      const userView = dataView(headerRaw);
      require(userView.getUint32(0, true) === 0x52455355, 'invalid USER block magic');
      const size = userView.getUint32(4, true),kind=userView.getUint32(8,true);
      require(size >= 12 && size <= MAX_USER_DATA_SIZE - userSize, 'invalid USER block size');
      const block = span(data, cursor, size, 'USER block').slice();
      const content = block.slice(12);
      userBlocks.push({ headerRaw, kind, data: content, raw: block, text: text(content) });
      cursor += size;userSize += size;
      if(kind===0x43415250)break;
    }
    require(userSequence(userBlocks),'invalid ordinary USER pair or owned touch/PRAC suffix order');
    fileTailRaw = data.slice(cursor);
  } else {
    fileTailRaw = data.slice(end);
  }
  const rawPayload = decodePayload(encodedPayload, uncompressedSize);
  return { format: 'th128-replay/1', header: { raw, replayVersion: 2,
    gameVersion: headerView.getUint32(16, true), compressedSize, uncompressedSize, userDataOffset },
  ...parsePayload(rawPayload), encodedPayload, rawPayload, dataGapRaw, userBlocks, fileTailRaw };
}

function equalBytes(first, second) {
  if (first.length !== second.length) return false;
  for (let index = 0; index < first.length; index += 1) {
    if (first[index] !== second[index]) return false;
  }
  return true;
}

export function encodeReplay(decoded, options = {}) {
  record(options, 'encode options');
  const recompress = options.recompress === undefined ? false : options.recompress;
  require(typeof recompress === 'boolean', 'recompress must be boolean');
  record(decoded, 'decoded replay');
  const header = bytes(record(decoded.header, 'header').raw, 'replay header', MAX_FILE_SIZE, 36).slice();
  validateIdentity(header);
  const metadata = bytes(record(decoded.metadata, 'metadata').raw, 'replay metadata', MAX_FILE_SIZE, 112).slice();
  require(Array.isArray(decoded.stages) && decoded.stages.length >= 1 && decoded.stages.length <= MAX_STAGES,
    'unsupported replay stage count');
  putUint32(metadata, 88, decoded.stages.length);
  const parts = [metadata];
  let size = 112;
  let frames = 0;
  for (const stage of decoded.stages) {
    record(stage, 'stage');
    const rawHeader = bytes(stage.rawHeader, 'stage header', MAX_FILE_SIZE, 144).slice();
    const inputRaw = bytes(stage.inputRaw, 'input records', MAX_FRAMES * 6);
    const fpsRaw = bytes(stage.fpsRaw, 'FPS data', MAX_UNCOMPRESSED_SIZE);
    require(inputRaw.length % 6 === 0, 'partial replay input record');
    const numFrames = inputRaw.length / 6;
    require(numFrames <= MAX_FRAMES - frames, 'replay frame budget exceeded');
    frames += numFrames;
    const bodySize = inputRaw.length + fpsRaw.length;
    size += 144 + bodySize;
    require(size <= MAX_UNCOMPRESSED_SIZE, 'uncompressed size exceeds budget');
    putUint32(rawHeader, 4, numFrames);
    putUint32(rawHeader, 8, bodySize);
    parts.push(rawHeader, inputRaw, fpsRaw);
  }
  parts.push(bytes(decoded.payloadTailRaw, 'payload tail', MAX_UNCOMPRESSED_SIZE));
  const payload = combine(parts, MAX_UNCOMPRESSED_SIZE, 'uncompressed size');
  let encoded = bytes(decoded.encodedPayload === undefined ? EMPTY : decoded.encodedPayload,
    'cached ciphertext', MAX_COMPRESSED_SIZE);
  let reuse = false;
  if (!recompress && encoded.length > 0) {
    try {
      reuse = equalBytes(decodePayload(encoded, payload.length), payload);
    } catch (error) {
      if (!(error instanceof ReplayFormatError)) throw error;
    }
  }
  if (!reuse) encoded = crypt(crypt(compressLiterals(payload), 0x80, 0x7d, 0x36, true),
    0x800, 0x5e, 0xe7, true);
  const gap = bytes(decoded.dataGapRaw, 'container gap');
  const tail = bytes(decoded.fileTailRaw, 'file tail');
  require(userSequence(decoded.userBlocks),'expected ordinary USER pair with optional owned touch then PRAC USER suffix');
  require(decoded.userBlocks.length > 0 || gap.length === 0, 'container gap needs a USER section');
  const userParts = [];
  let userSize = 0;
  for (const block of decoded.userBlocks) {
    record(block, 'USER block');
    const raw = bytes(block.headerRaw, 'USER header', MAX_FILE_SIZE, 12).slice();
    require(dataView(raw).getUint32(0, true) === 0x52455355, 'invalid USER block magic');
    const content = bytes(block.data, 'USER data', MAX_USER_DATA_SIZE);
    userSize += 12 + content.length;
    require(userSize <= MAX_USER_DATA_SIZE, 'USER data exceeds size budget');
    putUint32(raw, 4, content.length + 12);
    userParts.push(raw, content);
  }
  const userOffset = decoded.userBlocks.length > 0 ? 36 + encoded.length + gap.length : 0;
  require(36 + encoded.length + gap.length + userSize + tail.length <= MAX_FILE_SIZE,
    'replay file exceeds size budget');
  putUint32(header, 12, userOffset);
  putUint32(header, 28, encoded.length);
  putUint32(header, 32, payload.length);
  const output = combine([header, encoded, gap, ...userParts, tail], MAX_FILE_SIZE, 'replay file');
  decodeReplay(output);
  return output;
}
