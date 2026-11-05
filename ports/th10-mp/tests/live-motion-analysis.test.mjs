import test from 'node:test';
import assert from 'node:assert/strict';
import {analyzeLiveMotion} from '../portable/multiplayer/live-motion-analysis.mjs';

// These fixtures describe known raster translations, not correlation outputs.
// The raw layout below is copied from the native ABI, independently of parser
// exports. Test pixels are opaque textured sprites on a flat background.
const INVALID = 0xffffffff, HEADER = 32, RECORD = 80;
const bases = [[100, 110], [280, 150], [320, 310]];
const colors = (x, y, slot) => {
  let value = (Math.imul(x + 7, 73856093) ^ Math.imul(y + 13, 19349663) ^ Math.imul(slot + 1, 83492791)) >>> 0;
  value ^= value >>> 13; value = Math.imul(value, 1274126177) >>> 0; value ^= value >>> 16;
  return [32 + (value & 191), 32 + ((value >>> 8) & 191), 32 + ((value >>> 16) & 191)];
};
function roi(slot, boundsCenter, {spriteCenter = boundsCenter, background = [12, 18, 25],
  texture = colors, backgroundTexture = null} = {}) {
  const x = Math.floor(boundsCenter[0] - 22), y = Math.floor(boundsCenter[1] - 22), width = 44, height = 44;
  const rgba = new Uint8Array(width * height * 4);
  for (let row = 0; row < height; ++row) for (let column = 0; column < width; ++column) {
    const px = x + column + 0.5, py = y + row + 0.5;
    const inside = Math.abs(px - spriteCenter[0]) < 10 && Math.abs(py - spriteCenter[1]) < 10;
    const rgb = backgroundTexture ? backgroundTexture(px, py, slot) : inside ?
      texture(Math.floor(px - spriteCenter[0] + 10), Math.floor(py - spriteCenter[1] + 10), slot) : background;
    const at = (row * width + column) * 4;
    rgba.set([...rgb, 255], at);
  }
  return {slot, x, y, width, height, rgba};
}
function presented(sample) {
  sample.presentedWords = sample.words.slice();
  sample.presentedWords[8] = 300 + sample.index;
  sample.presentedWords[9] = 3;
  sample.presentedWords[10] = sample.words[10] | 32;
}
function fixture({endpoints = 2, alphas = [0.25, 0.75], motion = [4, 0]} = {}) {
  const samples = [];
  for (let endpoint = 0; endpoint < endpoints; ++endpoint) for (const alpha of alphas) {
    const index = samples.length, words = new Uint32Array(352), f = new Float32Array(words.buffer);
    words.set([1, 352, 80, 4, 1, 1, 200 + index, 100 + index, 0, 2, 31, 2,
      0x12345678, 1, 0, 0, 2, 100 + endpoint, 98 + endpoint, 0, 0, INVALID, 2, 1000,
      7, 640, 480, 3]);
    f[28] = alpha; f[29] = alpha; words[30] = 3;
    const centers = [];
    for (let slot = 0; slot < 3; ++slot) {
      const at = HEADER + RECORD * slot, base = bases[slot];
      const before = [base[0] + endpoint * motion[0], base[1] + endpoint * motion[1]];
      const current = [before[0] + motion[0], before[1] + motion[1]];
      const center = [before[0] + alpha * motion[0], before[1] + alpha * motion[1]];
      centers.push(center);
      words.set([slot < 2 ? 1 : 2, slot, slot < 2 ? slot : 42, slot < 2 ? slot : 777,
        10 + slot, slot < 2 ? slot : INVALID, 1, 63 | (slot === 0 ? 64 : 0), 0, 1,
        17 + slot, 91, 1, 2, 3, 3, 0xffffffff, 0xffffffff, 100 + endpoint,
        8, 9, 7, 0, 0, 640, 480, 100 + endpoint, 0x1000 + slot * 0x100], at);
      // A constant native (192,16) sprite anchor plus the shader half pixel.
      f.set([before[0] - 192.5, 480 - before[1] - 16.5, 0,
        current[0] - 192.5, 480 - current[1] - 16.5, 0], at + 28);
      f.set([center[0] - 0.5, 480 - center[1] - 0.5, 0], at + 34);
      if (slot === 2) f.set([center[0] - 192.5 + 224, 480 - center[1] - 16.5 + 16, 0], at + 37);
      f[at + 46] = f[at + 47] = 1;
      const corners = [[-10, -10, 0, 0], [10, -10, 1, 0], [-10, 10, 0, 1], [10, 10, 1, 1]];
      corners.forEach(([dx, dy, u, v], vertex) => {
        const q = at + 50 + 7 * vertex;
        f.set([center[0] - 0.5 + dx, 480 - center[1] - 0.5 + dy, 0.4, 1], q);
        words[q + 4] = 0xffffffff; f[q + 5] = u; f[q + 6] = v;
      });
      words[at + 78] = 1;
    }
    const sample = {index, atMs: index * 1000 / 120, words, presentedWords: null,
      blit: [0, 0, 640, 480, 0, 480, 640, 0, 0x4000, 0x2600], bufferSize: [640, 480],
      bindingMatched: true, readbackStatus: 'complete', pixels: centers.map((center, slot) => roi(slot, center)),
      fixtureCenters: centers};
    presented(sample); samples.push(sample);
  }
  const last = samples.length - 1;
  return {samples, observerStatus: [1, 0, 1, 1, 200 + last, 100 + last,
    samples.length, samples.length, 0, 0, 0, 42, 777, 1, 300 + last, 0], errors: [], dropped: 0};
}
function alterOdd(data, callback) {
  data.samples.forEach((sample, index) => { if (index % 2) { callback(sample, data.samples[index - 1]); presented(sample); } });
  return data;
}
function copyQuad(to, from, shift = [0, 0]) {
  const floats = new Float32Array(to.words.buffer);
  for (let slot = 0; slot < 3; ++slot) for (let vertex = 0; vertex < 4; ++vertex) {
    const at = HEADER + RECORD * slot + 50 + vertex * 7;
    to.words.set(from.words.subarray(at, at + 7), at);
    floats[at] += shift[0]; floats[at + 1] -= shift[1];
  }
}

function completedFrontier(data, slot = 1) {
  for (const sample of data.samples) {
    const words = sample.words, object = HEADER + slot * RECORD, aux = HEADER + 3 * RECORD;
    const previous = words.slice(object + 28, object + 31), current = words.slice(object + 31, object + 34);
    words.fill(0, aux, aux + RECORD);
    words.set([0x52465031, 1, 1 << slot, 16, 2, 0, 0, 0], aux);
    const at = aux + 8 + slot * 16, frame = words[17];
    words.set([9, frame - 1, frame, frame * 2, frame * 2 + 1, 1, 0, 1], at);
    words.set(previous, at + 8); words.set(current, at + 11);
    // Logical prediction has already reached and held the last real target.
    words.set(current, object + 28); words[object + 7] |= 256;
    presented(sample);
  }
  return data;
}

test('remote held logical targets use only the explicit complete-frontier display pair', () => {
  const data = completedFrontier(fixture());
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, true);
  assert.equal(result.coverage.players['1'].windows, 2);
  for (const window of result.geometry.windows.filter(w => w.slot === 1)) {
    assert.equal(window.endpointSource, 'completed-display-frontier');
    assert(window.context.every(c => c.endpointSource === 'completed-display-frontier'));
    assert.deepEqual(window.context[0].logicalPrevious, window.context[0].logicalCurrent);
    assert.equal(window.context[0].displayFrontier.epoch, 9);
  }
});

test('removing the explicit frontier contract cannot disguise the old held-target defect', () => {
  const data = completedFrontier(fixture());
  for (const sample of data.samples) {
    sample.words[HEADER + RECORD + 7] &= ~256;
    sample.words.fill(0, HEADER + 3 * RECORD);
    presented(sample);
  }
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert(result.violations.some(v => v.slot === 1 && v.reason === 'stationary-endpoints-but-quad-moved'));
});

test('malformed frontier metadata, wrong target, wrong seat and fake snap are rejected', () => {
  const base = HEADER + 3 * RECORD, pair = base + 8 + 16;
  const mutations = [
    w => { w[base]++; }, w => { w[base + 1]++; }, w => { w[base + 2] = 1; },
    w => { w[base + 3]++; }, w => { w[base + 4]++; }, w => { w[base + 7] = 1; },
    w => { w[pair] = 0; }, w => { w[pair + 2]--; },
    w => { w[pair + 4] = w[pair + 3] - 1; }, w => { w[pair + 5] = 3; },
    w => { w[pair + 7] = 3; }, w => { w[pair + 11] ^= 1; },
    w => { w[pair + 14] = 1; }, w => { w[base + 50] = 1; },
    w => { w[HEADER + 2 * RECORD + 7] |= 256; },
  ];
  for (const mutate of mutations) {
    const data = completedFrontier(fixture());
    for (const sample of data.samples) { mutate(sample.words); presented(sample); }
    const result = analyzeLiveMotion(data);
    assert.equal(result.passed, false);
    assert.equal(result.captureComplete, false);
    assert(result.accounting.integrityRejectedSamples > 0);
  }
  assert.equal(analyzeLiveMotion(completedFrontier(fixture(), 0)).captureComplete, false);
});

test('a display-frontier epoch replacement breaks continuity even with the same player address', () => {
  const data = completedFrontier(fixture());
  alterOdd(data, sample => { sample.words[HEADER + 3 * RECORD + 8 + 16]++; });
  const result = analyzeLiveMotion(data);
  assert.equal(result.coverage.players['1'].windows, 0);
  assert(result.accounting.boundaryReasons['identity-changed'] > 0);
  assert.deepEqual(result.violations, []);
});

test('known localized motion covers each player independently and a natural bullet slot', () => {
  const data = fixture(), before = data.samples.map(s => Array.from(s.words));
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, true);
  assert.deepEqual(result.violations, []);
  assert.equal(result.coverage.players['0'].windows, 2);
  assert.equal(result.coverage.players['1'].windows, 2);
  assert.equal(result.coverage.bullets.windows, 2);
  assert.equal(result.geometry.windows.filter(w => w.status === 'covered').length, 6);
  for (const window of result.pixels.windows) {
    assert.equal(window.status, 'covered');
    assert.deepEqual(window.comparisons[0].expected, [2, -0]);
    assert(window.comparisons[0].expectedPeak.correlation > 0.999999);
    assert.equal(window.comparisons[0].search.sameSupport, true);
  }
  assert.deepEqual(data.samples.map(s => Array.from(s.words)), before);
});

test('GL bottom-left rows and the native vertical flip also validate upward and diagonal motion', () => {
  for (const motion of [[0, 4], [4, 4], [-4, -4]]) {
    const result = analyzeLiveMotion(fixture({motion}));
    assert.equal(result.passed, true, JSON.stringify({motion, reasons: result.accounting.pixelPairReasons}));
    assert.deepEqual(result.pixels.windows[0].comparisons[0].expected, motion.map(x => x / 2));
  }
});

test('ordinary serialized word and byte arrays have the same result', () => {
  const data = fixture();
  for (const sample of data.samples) {
    sample.words = Array.from(sample.words); sample.presentedWords = Array.from(sample.presentedWords);
    for (const image of sample.pixels) image.rgba = Array.from(image.rgba);
  }
  assert.equal(analyzeLiveMotion(data).passed, true);
});

test('bullet script_position carries world translation and is not a fixed local transform', () => {
  const valid = fixture();
  assert.equal(analyzeLiveMotion(valid).coverage.bullets.met, true);
  const held = alterOdd(fixture(), (sample, old) => {
    const at = HEADER + 2 * RECORD + 37;
    sample.words.set(old.words.subarray(at, at + 3), at);
  });
  const result = analyzeLiveMotion(held);
  assert.equal(result.coverage.bullets.met, false);
  assert(result.accounting.geometryPairReasons['bullet-script-position-not-world-translation'] > 0);
  assert.equal(result.passed, false);
});

test('old ROI bytes and coordinates with fresh counters and moving quads cannot pass', () => {
  const data = alterOdd(fixture(), (sample, old) => {
    sample.pixels = old.pixels.map(image => ({...image, rgba: image.rgba.slice()}));
  });
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert(result.violations.some(v => v.reason === 'pixels-held-while-quad-moved'));
  assert.equal(result.coverage.players['0'].windows, 0);
});

test('new alpha and commit serials cannot disguise held submitted geometry', () => {
  const result = analyzeLiveMotion(alterOdd(fixture(), copyQuad));
  assert.equal(result.passed, false);
  assert(result.violations.some(v => v.reason === 'quad-held-with-advancing-alpha'));
});

test('early endpoints and backwards geometry are explicit violations', () => {
  for (const [shift, reason] of [[[3, 0], 'quad-ahead-of-alpha'], [[-2, 0], 'quad-moved-backwards']]) {
    const data = alterOdd(fixture(), (sample, old) => copyQuad(sample, old, shift));
    const result = analyzeLiveMotion(data);
    assert.equal(result.passed, false);
    assert(result.violations.some(v => v.reason === reason), reason);
  }
});

test('background color changes while the textured object stays put are not motion', () => {
  const data = alterOdd(fixture(), (sample, old) => {
    sample.pixels = sample.fixtureCenters.map((center, slot) => roi(slot, center,
      {spriteCenter: old.fixtureCenters[slot], background: [64, 21, 90]}));
  });
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert(result.violations.some(v => v.reason === 'pixels-held-while-quad-moved'));
});

test('moving texture covering the background as well cannot certify a sprite', () => {
  const data = fixture();
  for (const sample of data.samples) sample.pixels = sample.fixtureCenters.map((center, slot) => roi(slot, center,
    {backgroundTexture: (x, y) => colors(Math.floor(x - center[0]), Math.floor(y - center[1]), slot)}));
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert.equal(result.coverage.players['0'].windows, 0);
  assert(Object.keys(result.accounting.pixelPairReasons).some(r =>
    r === 'texture-not-localized-to-quad' || r === 'background-comotion-ambiguous'));
});

test('repeated textures with ambiguous candidate translations are not covered', () => {
  const data = fixture();
  const repeated = (x, y) => ((Math.floor(x / 2) + Math.floor(y / 2)) & 1) ? [230, 40, 90] : [30, 210, 180];
  for (const sample of data.samples) sample.pixels = sample.fixtureCenters.map((center, slot) => roi(slot, center, {texture: repeated}));
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert.equal(result.coverage.bullets.windows, 0);
});

test('owner incarnation replacement is a boundary, not a joined motion window', () => {
  const data = alterOdd(fixture(), sample => {
    for (let slot = 0; slot < 3; ++slot) sample.words[HEADER + RECORD * slot + 4] += 100;
  });
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert.equal(result.coverage.players['0'].windows, 0);
  assert(result.accounting.boundaryReasons['identity-changed'] > 0);
  assert.deepEqual(result.violations, []);
});

test('source pointers are diagnostic and never replace the specified stable identity', () => {
  const data = fixture();
  for (const sample of data.samples) {
    for (let slot = 0; slot < 3; ++slot) sample.words[HEADER + RECORD * slot + 27] += sample.index * 128;
    presented(sample);
  }
  assert.equal(analyzeLiveMotion(data).passed, true);
});

test('each player needs its own two windows; they cannot pool their counts', () => {
  const short = analyzeLiveMotion(fixture({endpoints: 1}));
  assert.equal(short.coverage.players['0'].windows, 1);
  assert.equal(short.coverage.players['1'].windows, 1);
  assert.equal(short.coverage.met, false);
  const missingSeat = fixture();
  for (const sample of missingSeat.samples) sample.pixels = sample.pixels.filter(p => p.slot !== 1);
  const result = analyzeLiveMotion(missingSeat);
  assert.equal(result.coverage.players['0'].windows, 2);
  assert.equal(result.coverage.players['1'].windows, 0);
  assert.equal(result.passed, false);
});

test('metadata-only remains geometry evidence and is neither a drop nor pixel coverage', () => {
  const data = fixture();
  for (const sample of data.samples) { sample.readbackStatus = 'metadata-only'; sample.pixels = []; }
  const result = analyzeLiveMotion(data);
  assert.equal(result.geometry.windows.filter(w => w.status === 'covered').length, 6);
  assert.equal(result.pixels.windows.length, 0);
  assert.equal(result.accounting.readbacks['metadata-only'], 4);
  assert.equal(result.captureComplete, true);
  assert.equal(result.passed, false);
});

test('complete pixels on both sides of metadata-only never form a pixel pair', () => {
  const data = fixture({alphas: [0.25, 0.5, 0.75]});
  data.samples.forEach((sample, index) => {
    if (index % 3 === 1) { sample.readbackStatus = 'metadata-only'; sample.pixels = []; }
  });
  const result = analyzeLiveMotion(data);
  assert.equal(result.geometry.windows.filter(w => w.status === 'covered').length, 6);
  assert.equal(result.pixels.windows.reduce((n, w) => n + w.comparisons.length, 0), 0);
  assert.equal(result.passed, false);
});

test('dropped, failed and pending readbacks cannot be silently removed', () => {
  for (const readbackStatus of ['dropped', 'failed', 'pending']) {
    const data = fixture(); data.samples[1].readbackStatus = readbackStatus; data.samples[1].pixels = [];
    const result = analyzeLiveMotion(data);
    assert.equal(result.captureComplete, false, readbackStatus);
    assert.equal(result.accounting.readbacks[readbackStatus], 1);
    assert.equal(result.coverage.players['0'].windows, 1);
    assert.equal(result.passed, false);
    assert.equal(result.geometry.windows.filter(w => w.status === 'covered').length, 6);
  }
});

test('unsupported ROIs are explicit not-covered observations, not fabricated drops', () => {
  const data = fixture();
  for (const sample of data.samples) { sample.readbackStatus = 'no-supported-roi'; sample.pixels = []; }
  const result = analyzeLiveMotion(data);
  assert.equal(result.captureComplete, true);
  assert.equal(result.dropped, 0);
  assert.equal(result.passed, false);
  assert.equal(result.accounting.pixelSampleReasons['readback-no-supported-roi'], 12);
});

test('a legal stationary scene has neither motion coverage nor a false motion violation', () => {
  const result = analyzeLiveMotion(fixture({motion: [0, 0]}));
  assert.equal(result.passed, false);
  assert.equal(result.coverage.players['0'].windows, 0);
  assert.deepEqual(result.violations, []);
  assert.equal(result.accounting.geometryPairReasons['legal-stationary-object'], 6);
});

test('subpixel translations cannot supply the required resolvable pixel evidence', () => {
  const result = analyzeLiveMotion(fixture({motion: [1, 0]}));
  assert.equal(result.passed, false);
  assert.equal(result.accounting.pixelPairReasons['subpixel-motion'], 6);
  assert.equal(result.geometry.windows.filter(w => w.status === 'covered').length, 6);
});

test('resource, viewport, local transforms, UV and shape changes invalidate translation qualification', () => {
  const mutations = [
    sample => { for (let slot = 0; slot < 3; ++slot) sample.words[HEADER + RECORD * slot + 19]++; },
    sample => { for (let slot = 0; slot < 3; ++slot) { sample.words[HEADER + RECORD * slot + 22] = 1; sample.words[HEADER + RECORD * slot + 24] = 639; } },
    sample => { for (let slot = 0; slot < 3; ++slot) new Float32Array(sample.words.buffer)[HEADER + RECORD * slot + 37] = 1; },
    sample => { for (let slot = 0; slot < 3; ++slot) new Float32Array(sample.words.buffer)[HEADER + RECORD * slot + 40] = 1; },
    sample => { for (let slot = 0; slot < 3; ++slot) new Float32Array(sample.words.buffer)[HEADER + RECORD * slot + 55] = 0.1; },
    sample => { for (let slot = 0; slot < 3; ++slot) new Float32Array(sample.words.buffer)[HEADER + RECORD * slot + 50] += 1; },
  ];
  for (const mutate of mutations) {
    const result = analyzeLiveMotion(alterOdd(fixture(), mutate));
    assert.equal(result.passed, false);
    assert.equal(result.coverage.players['0'].windows, 0);
    assert.deepEqual(result.violations, []);
  }
});

test('pending and presented records must match every content word and the real phase transition', () => {
  const data = fixture(); data.samples[1].presentedWords[HEADER + 27] ^= 4;
  const result = analyzeLiveMotion(data);
  assert.equal(result.passed, false);
  assert.equal(result.accounting.integrityRejectedSamples, 1);
  assert.equal(result.samples[1].reason, 'unmatched-commit-content');
  const wrongPhase = fixture(); wrongPhase.samples[0].words[9] = 1;
  assert.equal(analyzeLiveMotion(wrongPhase).samples[0].reason, 'unmatched-commit-header');
});

test('an unsupported blit, mismatched binding or repeated draw cannot gain coverage', () => {
  for (const mutate of [sample => { sample.blit[9] = 0x2601; },
    sample => { sample.bindingMatched = false; }, sample => { sample.words[7]--; presented(sample); }]) {
    const data = fixture(); data.samples.forEach((sample, index) => { if (index % 2) mutate(sample); });
    const result = analyzeLiveMotion(data);
    assert.equal(result.passed, false);
    assert.equal(result.coverage.players['0'].windows, 0);
  }
});

test('network waiting and frozen draws never supply movement windows', () => {
  for (const mutate of [sample => { sample.words[22] = 12; }, sample => { sample.words[10] |= 2048; }]) {
    const data = fixture(); data.samples.forEach(sample => { mutate(sample); presented(sample); });
    const result = analyzeLiveMotion(data);
    assert.equal(result.passed, false);
    assert.equal(result.coverage.bullets.windows, 0);
    assert.deepEqual(result.violations, []);
  }
});

test('capping a short window keeps the boundary pair and its bad geometry', () => {
  const data = fixture({endpoints: 1, motion: [10, 0], alphas: Array.from({length: 9}, (_, i) => (i + 1) / 10)});
  copyQuad(data.samples[8], data.samples[7]); presented(data.samples[8]);
  const result = analyzeLiveMotion(data);
  assert(result.violations.some(v => v.domain === 'geometry' && v.from === 7 && v.to === 8 &&
    v.reason === 'quad-held-with-advancing-alpha'));
  assert(result.geometry.windows.every(w => w.sampleCount <= 8));
  assert.equal(result.passed, false);
});

test('reported capture errors and external drop totals remain blocking despite good windows', () => {
  for (const update of [data => { data.errors.push('PBO readback failed'); }, data => { data.dropped = 1; }]) {
    const data = fixture(); update(data);
    const result = analyzeLiveMotion(data);
    assert.equal(result.coverage.met, true);
    assert.equal(result.captureComplete, false);
    assert.equal(result.passed, false);
  }
});


// LMT1 fixtures have an independent raster oracle. The asset is an original
// 64x64 top-row-first atlas; the draw uses UV [16,36)/64 and the packet carries
// exactly the one-texel halo crop (15,15,22,22). The framebuffer is generated
// bottom row first from screen coordinates, never from analyzer helpers.
const LMT_SIDE = 64, LMT_CROP = 22, LMT_PITCH = LMT_CROP * 4;
function lmtNoise(x, y, seed) {
  let n = (Math.imul(x + 19, 0x45d9f3b) ^ Math.imul(y + 37, 0x119de1f3) ^
    Math.imul(seed + 5, 0x27d4eb2d)) >>> 0;
  n = Math.imul(n ^ (n >>> 16), 0x7feb352d) >>> 0;
  n = Math.imul(n ^ (n >>> 15), 0x846ca68b) >>> 0;
  return (n ^ (n >>> 16)) >>> 0;
}
function lmtBackground(x, y, slot) {
  const n = lmtNoise(Math.floor(x), Math.floor(y), slot + 71);
  // A stationary, nonuniform screen texture. Its maximum central-difference
  // energy is below 64, so it cannot manufacture intrinsic foreground support.
  return [24 + (n & 15), 28 + ((n >>> 8) & 15), 32 + ((n >>> 16) & 15)];
}
function lmtAtlas(slot, alphaAt = () => 255) {
  const rgba = new Uint8Array(LMT_SIDE * LMT_SIDE * 4);
  for (let topRow = 0; topRow < LMT_SIDE; ++topRow) {
    for (let column = 0; column < LMT_SIDE; ++column) {
      const n = lmtNoise(column, topRow, slot + 3), at = (topRow * LMT_SIDE + column) * 4;
      rgba.set([n & 255, (n >>> 8) & 255, (n >>> 16) & 255, alphaAt(column, topRow)], at);
    }
  }
  return rgba;
}
function lmtAssetSample(atlas, sourceX, sourceY, filter) {
  if (filter === 0) {
    const at = (Math.floor(sourceY) * LMT_SIDE + Math.floor(sourceX)) * 4;
    return Array.from(atlas.subarray(at, at + 4));
  }
  // Pixel centres in a normalized atlas: linear filtering uses the four
  // neighbours around (u*width-.5,v*height-.5), including the real halo.
  const left = Math.floor(sourceX - 0.5), top = Math.floor(sourceY - 0.5);
  const horizontal = sourceX - 0.5 - left, vertical = sourceY - 0.5 - top;
  const result = [0, 0, 0, 0];
  for (let row = 0; row < 2; ++row) for (let column = 0; column < 2; ++column) {
    const weight = (column ? horizontal : 1 - horizontal) * (row ? vertical : 1 - vertical);
    const at = ((top + row) * LMT_SIDE + left + column) * 4;
    for (let channel = 0; channel < 4; ++channel) result[channel] += atlas[at + channel] * weight;
  }
  return result;
}
function lmtFramebuffer(slot, boundsCenter, atlas, {filter, opacityByte, spriteCenter = boundsCenter}) {
  const x = Math.floor(boundsCenter[0] - 22), y = Math.floor(boundsCenter[1] - 22);
  const width = 44, height = 44, rgba = new Uint8Array(width * height * 4);
  for (let bottomRow = 0; bottomRow < height; ++bottomRow) {
    for (let column = 0; column < width; ++column) {
      const screenX = x + column + 0.5, screenY = y + bottomRow + 0.5;
      const background = lmtBackground(screenX, screenY, slot);
      const fromLeft = screenX - (spriteCenter[0] - 10);
      const fromTop = spriteCenter[1] + 10 - screenY; // independent CPU-top / GL-bottom reversal
      const texel = fromLeft >= 0 && fromLeft < 20 && fromTop >= 0 && fromTop < 20 ?
        lmtAssetSample(atlas, 16 + fromLeft, 16 + fromTop, filter) : [0, 0, 0, 0];
      const alpha = texel[3] / 255 * opacityByte / 255, at = (bottomRow * width + column) * 4;
      for (let channel = 0; channel < 3; ++channel)
        rgba[at + channel] = Math.round(texel[channel] * alpha + background[channel] * (1 - alpha));
      rgba[at + 3] = 255;
    }
  }
  return {slot, x, y, width, height, rgba};
}
function lmtPacket(sample, slot, atlas, filter) {
  const at = HEADER + slot * RECORD, header = new Uint32Array(32);
  header.set([0x4c4d5431, 1, 32, 1, ...sample.words.slice(4, 8), slot,
    sample.words[at + 20], sample.words[at + 19], 7, 7, 7,
    21, 64, 64, 256, 15, 15, 22, 22, 88, 1936, 128, 128, filter, filter, 2, 2, 63, 0]);
  const rgba = new Uint8Array(LMT_CROP * LMT_PITCH);
  for (let row = 0; row < LMT_CROP; ++row) {
    const source = ((15 + row) * LMT_SIDE + 15) * 4;
    rgba.set(atlas.subarray(source, source + LMT_PITCH), row * LMT_PITCH);
  }
  return {header, rgba};
}
function lmtFixture({filter = 0, opacityByte = 128, alphaAt, motion = [4, 4],
  endpoints = 2, alphas = [0.3125, 0.8125]} = {}) {
  const data = fixture({endpoints, alphas, motion});
  data.textureEvidenceEnabled = true;
  data.fixtureAtlases = [lmtAtlas(0, alphaAt), lmtAtlas(1, alphaAt)];
  for (const sample of data.samples) {
    const floats = new Float32Array(sample.words.buffer);
    for (let slot = 0; slot < 2; ++slot) {
      const at = HEADER + slot * RECORD;
      // Distinct texture owners really have distinct assets; no same-version
      // alias with different bytes is smuggled across the two native players.
      sample.words[at + 19] = 80 + slot; sample.words[at + 20] = 90 + slot;
      for (let vertex = 0; vertex < 4; ++vertex) {
        const q = at + 50 + vertex * 7;
        sample.words[q + 4] = ((opacityByte << 24) | 0xffffff) >>> 0;
        floats[q + 5] = (vertex & 1 ? 36 : 16) / 64;
        floats[q + 6] = (vertex >= 2 ? 36 : 16) / 64;
      }
      const atlas = data.fixtureAtlases[slot];
      sample.pixels[slot] = lmtFramebuffer(slot, sample.fixtureCenters[slot], atlas, {filter, opacityByte});
      sample.pixels[slot].texture = lmtPacket(sample, slot, atlas, filter);
    }
    presented(sample);
  }
  return data;
}
function lmtEachPacket(data, callback) {
  for (const sample of data.samples) for (const image of sample.pixels)
    if (image.slot < 2) callback(image.texture, image, sample);
}
function lmtComparisons(result, slot = null) {
  return result.pixels.windows.filter(window => window.slot < 2 && (slot === null || window.slot === slot))
    .flatMap(window => window.comparisons);
}
function assertLmtNoPlayerCoverage(result, context) {
  assert.equal(result.passed, false, context);
  for (const slot of ['0', '1']) assert.equal(result.coverage.players[slot].windows, 0, context);
  assert(lmtComparisons(result).every(pair => pair.status !== 'covered'), context);
  assert(lmtComparisons(result).every(pair => pair.supportSource !== 'localized-screen-rgb'), context);
}
function assertLmtIntegrity(data, reason, context) {
  const result = analyzeLiveMotion(data);
  assertLmtNoPlayerCoverage(result, context);
  assert.equal(result.captureComplete, false, context);
  assert.equal(result.accounting.integrityRejectedSamples, 0, context);
  assert.equal(result.accounting.integrityRejectedPixelPackets, 8, context);
  assert.equal(result.accounting.pixelSampleReasons[reason], 8, context);
  assert.equal(result.coverage.bullets.met, true, context);
  assert.deepEqual(result.violations, [], context);
}
function assertLmtCovered(result, context) {
  assert.equal(result.passed, true, context + ': ' + JSON.stringify(result.accounting.pixelPairReasons));
  assert.equal(result.captureComplete, true, context);
  assert.deepEqual(result.violations, [], context);
  assert.equal(result.policy.minimumCorrelation, 0.92);
  assert.equal(result.policy.minimumCorrelationMargin, 0.08);
  assert.equal(result.coverage.players['0'].windows, 2);
  assert.equal(result.coverage.players['1'].windows, 2);
  assert.equal(result.coverage.bullets.windows, 2);
  assert.equal(lmtComparisons(result).length, 4);
  for (const pair of lmtComparisons(result)) {
    assert.equal(pair.status, 'covered', context);
    assert.equal(pair.supportSource, 'submit-stamped-cpu-texture', context);
    assert.equal(pair.foreground.quadOpacity, 128 / 255, context);
    assert(pair.foreground.minimumSelectedAlpha >= 0.4, context);
    assert(pair.expectedPeak.correlation >= 0.92, context);
    assert(pair.expectedPeak.correlation - pair.zero.correlation >= 0.08, context);
    assert(pair.expectedPeak.correlation - pair.alternativePeak.correlation >= 0.08, context);
    assert.equal(pair.search.sameSupport, true, context);
  }
}

for (const [filter, name] of [[0, 'Nearest'], [1, 'Linear']]) {
  test('LMT1 ' + name + ' certifies independently blended 50% sprites over a stationary textured background', () => {
    const data = lmtFixture({filter}), packet = data.samples[0].pixels[0].texture;
    assert.deepEqual(Array.from(packet.header), [0x4c4d5431, 1, 32, 1, 1, 1, 200, 100, 0, 90, 80,
      7, 7, 7, 21, 64, 64, 256, 15, 15, 22, 22, 88, 1936, 128, 128, filter, filter, 2, 2, 63, 0]);
    assert.deepEqual(Array.from(packet.rgba.subarray(0, 4)),
      Array.from(data.fixtureAtlases[0].subarray((15 * 64 + 15) * 4, (15 * 64 + 15) * 4 + 4)));
    assertLmtCovered(analyzeLiveMotion(data), name + ' opacity 128/255');
  });
}

test('the Linear oracle uses four distinct texels and real RGB blending, not a relabelled Nearest image', () => {
  const nearest = lmtFixture({filter: 0}), linear = lmtFixture({filter: 1});
  const image = linear.samples[0].pixels[0], atlas = linear.fixtureAtlases[0], pixel = (22 * image.width + 22) * 4;
  const background = lmtBackground(image.x + 22.5, image.y + 22.5, 0);
  // This particular framebuffer centre maps to source (26.25,25.75).
  // CPU rows are top-down: the four texel weights therefore differ in X/Y.
  for (let channel = 0; channel < 3; ++channel) {
    const texel = atlas[(25 * 64 + 25) * 4 + channel] * 3 / 16 +
      atlas[(25 * 64 + 26) * 4 + channel] * 9 / 16 +
      atlas[(26 * 64 + 25) * 4 + channel] / 16 +
      atlas[(26 * 64 + 26) * 4 + channel] * 3 / 16;
    assert.equal(image.rgba[pixel + channel], Math.round(texel * 128 / 255 + background[channel] * 127 / 255));
  }
  assert.notDeepEqual(image.rgba, nearest.samples[0].pixels[0].rgba);
  assert.equal(image.rgba[pixel + 3], 255);
});

test('LMT1 20% diffuse opacity is explicitly not-covered and cannot fall back to textured RGB', () => {
  const result = analyzeLiveMotion(lmtFixture({opacityByte: 51}));
  assertLmtNoPlayerCoverage(result, '20% opacity');
  assert.equal(result.captureComplete, true);
  assert.equal(result.accounting.pixelPairReasons['texture-foreground-opacity-insufficient'], 4);
  assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
  assert.deepEqual(result.violations, []);
});

test('a transparent intrinsic mask cannot certify background or fall back to good moving RGB', () => {
  for (const transparentActualSprite of [true, false]) {
    const data = lmtFixture({alphaAt: transparentActualSprite ? () => 0 : undefined});
    // The second negative keeps the positive actual RGB motion and falsifies
    // only packet alpha. Even attractive screen matches cannot bypass the mask.
    if (!transparentActualSprite) lmtEachPacket(data, packet => {
      for (let at = 3; at < packet.rgba.length; at += 4) packet.rgba[at] = 0;
    });
    const result = analyzeLiveMotion(data);
    assertLmtNoPlayerCoverage(result, 'transparent intrinsic mask');
    assert.equal(result.captureComplete, true);
    assert.equal(result.accounting.pixelPairReasons['insufficient-common-texture-support'], 4);
    assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
    assert.deepEqual(result.violations, []);
  }
});

test('an asymmetric top-only CPU mask agrees with bottom-row framebuffer motion for both filters', () => {
  for (const filter of [0, 1]) {
    const data = lmtFixture({filter, motion: [0, 4], alphaAt: (_x, y) => y >= 16 && y < 24 ? 255 : 0});
    const sample = data.samples[0], image = sample.pixels[0], packet = image.texture;
    assert.equal(packet.rgba[(1 * 22 + 1) * 4 + 3], 255); // original CPU (16,16)
    assert.equal(packet.rgba[(20 * 22 + 1) * 4 + 3], 0); // original CPU (16,35)
    const center = sample.fixtureCenters[0];
    const lowerX = Math.floor(center[0]), lowerY = Math.floor(center[1] - 5);
    const at = ((lowerY - image.y) * image.width + lowerX - image.x) * 4;
    assert.deepEqual(Array.from(image.rgba.subarray(at, at + 3)), lmtBackground(lowerX + 0.5, lowerY + 0.5, 0));
    assertLmtCovered(analyzeLiveMotion(data), 'top-only mask filter ' + filter);
  }
});

test('vertically reversing the CPU packet cannot pass with the unchanged asymmetric real framebuffer', () => {
  for (const filter of [0, 1]) {
    const data = lmtFixture({filter, motion: [0, 4], alphaAt: (_x, y) => y >= 16 && y < 24 ? 255 : 0});
    lmtEachPacket(data, packet => {
      const original = packet.rgba.slice();
      for (let row = 0; row < LMT_CROP; ++row)
        packet.rgba.set(original.subarray((LMT_CROP - 1 - row) * LMT_PITCH, (LMT_CROP - row) * LMT_PITCH), row * LMT_PITCH);
    });
    const result = analyzeLiveMotion(data);
    assertLmtNoPlayerCoverage(result, 'flipped CPU rows filter ' + filter);
    assert.equal(result.captureComplete, true);
    assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
    assert.equal(result.accounting.pixelPairReasons['insufficient-common-texture-support'], 4);
    assert.deepEqual(result.violations, []);
  }
});

test('fresh LMT1 tuples and fresh ROI locations cannot hide actual pixels held at the preceding sprite position', () => {
  const data = lmtFixture({filter: 1});
  alterOdd(data, (sample, previous) => {
    for (let slot = 0; slot < 2; ++slot) {
      const texture = sample.pixels[slot].texture;
      sample.pixels[slot] = lmtFramebuffer(slot, sample.fixtureCenters[slot], data.fixtureAtlases[slot],
        {filter: 1, opacityByte: 128, spriteCenter: previous.fixtureCenters[slot]});
      sample.pixels[slot].texture = texture;
      assert.deepEqual(Array.from(texture.header.subarray(4, 8)), Array.from(sample.words.subarray(4, 8)));
    }
  });
  const result = analyzeLiveMotion(data);
  assertLmtNoPlayerCoverage(result, 'held actual sprite');
  assert.equal(result.captureComplete, true);
  assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
  assert.equal(result.violations.filter(v => v.slot < 2 && v.reason === 'pixels-held-while-quad-moved').length, 4);
});

test('required missing LMT1 packets are integrity failures even when the actual RGB motion is good', () => {
  const data = lmtFixture();
  lmtEachPacket(data, (_packet, image) => { delete image.texture; });
  assertLmtIntegrity(data, 'required-texture-packet-missing', 'missing required packets');
});

test('every LMT1 draw tuple, slot, owner and surface component is bound to the actual pending draw', () => {
  for (const [field, name] of [[4, 'observerEpoch'], [5, 'identityEpoch'], [6, 'callbackSerial'],
    [7, 'drawSerial'], [8, 'slot'], [9, 'owner'], [10, 'surface']]) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => { packet.header[field]++; });
    assertLmtIntegrity(data, 'texture-packet-draw-or-owner-mismatch', name);
  }
});

test('each LMT1 authority flag is required and unknown extra flags cannot enable RGB fallback', () => {
  for (const flags of [62, 61, 59, 55, 47, 31, 127]) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => { packet.header[30] = flags; });
    assertLmtIntegrity(data, 'texture-packet-version-or-authority-mismatch', 'flags ' + flags);
  }
});

test('invalid captured, current CPU and current GPU versions are LMT1 integrity failures', () => {
  for (const [field, value] of [[11, INVALID], [11, 8], [12, 8], [13, 8]]) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => { packet.header[field] = value; });
    assertLmtIntegrity(data, 'texture-packet-version-or-authority-mismatch', 'version word ' + field);
  }
});

test('LMT1 header shape, ABI version, result code and payload byte types are validated before scoring', () => {
  const cases = [
    ['magic', packet => { packet.header[0]++; }],
    ['schema', packet => { packet.header[1]++; }],
    ['headerWords', packet => { packet.header[2]--; }],
    ['short header', packet => { packet.header = packet.header.slice(0, 31); }],
    ['nonuint header', packet => { packet.header = Array.from(packet.header); packet.header[4] = -1; }],
    ['RGBA offset', packet => { packet.header[24] = 124; }],
    ['maximum side', packet => { packet.header[25] = 127; }],
    ['reserved', packet => { packet.header[31] = 1; }],
    ['zero result', packet => { packet.header[3] = 0; }],
    ['unknown result', packet => { packet.header[3] = 19; }],
    ['nonbyte-array payload', packet => { packet.rgba = {length: packet.rgba.length}; }],
  ];
  for (const [label, mutate] of cases) {
    const data = lmtFixture(); lmtEachPacket(data, mutate);
    assertLmtIntegrity(data, 'invalid-texture-packet-abi', label);
  }
  for (const byte of [-1, 256, 0.5, NaN]) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => { packet.rgba = Array.from(packet.rgba); packet.rgba[0] = byte; });
    assertLmtIntegrity(data, 'invalid-texture-pixel-bytes', 'payload byte ' + byte);
  }
});

test('format, pitches, exact crop and payload extent failures are blocking LMT1 evidence errors', () => {
  const cases = [
    ['unknown source format', 'invalid-texture-packet-storage', p => { p.header[14] = 27; }],
    ['empty source width', 'invalid-texture-packet-storage', p => { p.header[15] = 0; }],
    ['empty source height', 'invalid-texture-packet-storage', p => { p.header[16] = 0; }],
    ['source pitch too short', 'invalid-texture-packet-storage', p => { p.header[17] = 255; }],
    ['source extent overflow', 'invalid-texture-packet-storage', p => { p.header[17] = INVALID; }],
    ['crop beyond source X', 'invalid-texture-packet-storage', p => { p.header[18] = 63; }],
    ['crop beyond source Y', 'invalid-texture-packet-storage', p => { p.header[19] = 63; }],
    ['wrong RGBA pitch', 'invalid-texture-packet-storage', p => { p.header[22] -= 4; }],
    ['wrong declared payload', 'invalid-texture-packet-storage', p => { p.header[23]--; }],
    ['short payload', 'invalid-texture-packet-storage', p => { p.rgba = p.rgba.slice(0, -1); }],
    ['extra payload', 'invalid-texture-packet-storage', p => { p.rgba = Uint8Array.from([...p.rgba, 0]); }],
    ['wrong crop origin X', 'texture-packet-uv-footprint-mismatch', p => { p.header[18]++; }],
    ['wrong crop origin Y', 'texture-packet-uv-footprint-mismatch', p => { p.header[19]++; }],
    ['missing full halo column', 'texture-packet-uv-footprint-mismatch', p => {
      p.header[20] = 21; p.header[22] = 84; p.header[23] = 1848; p.rgba = p.rgba.slice(0, 1848);
    }],
    ['missing full halo row', 'texture-packet-uv-footprint-mismatch', p => {
      p.header[21] = 21; p.header[23] = 1848; p.rgba = p.rgba.slice(0, 1848);
    }],
  ];
  for (const [label, reason, mutate] of cases) {
    const data = lmtFixture(); lmtEachPacket(data, mutate);
    assertLmtIntegrity(data, reason, label);
  }
});

test('invalid LMT1 min/mag filtering and U/V address enums are integrity failures', () => {
  for (const [field, value] of [[26, 2], [27, 2], [28, 3], [29, 3]]) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => { packet.header[field] = value; });
    assertLmtIntegrity(data, 'invalid-texture-sampler', 'sampler word ' + field);
  }
});

test('every declared native non-OK LMT1 status with an empty payload is only not-covered', () => {
  const reasons = ['disabled', 'no-pending', 'request-mismatch', 'unsupported-record', 'resource-missing',
    'resource-binding-mismatch', 'version-mismatch', 'gpu-unavailable', 'gpu-written', 'unsupported-format',
    'unsupported-sampling', 'invalid-uv', 'footprint-out-of-bounds', 'crop-too-large', 'invalid-storage',
    'unsupported-host', 'no-submit-stamp'];
  for (let resultCode = 2; resultCode <= 18; ++resultCode) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => {
      packet.header[3] = resultCode; packet.header[22] = packet.header[23] = 0;
      packet.rgba = new Uint8Array();
    });
    const result = analyzeLiveMotion(data);
    assertLmtNoPlayerCoverage(result, 'native result ' + resultCode);
    assert.equal(result.captureComplete, true);
    assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
    assert.equal(result.accounting.pixelSampleReasons['native-texture-' + reasons[resultCode - 2]], 8);
    assert.deepEqual(result.violations, []);
  }
});

test('a rejected native LMT1 packet cannot retain a pitch, declared payload or stale bytes', () => {
  for (const keep of ['pitch', 'declared payload', 'bytes']) {
    const data = lmtFixture();
    lmtEachPacket(data, packet => {
      packet.header[3] = 10;
      if (keep !== 'pitch') packet.header[22] = 0;
      if (keep !== 'declared payload') packet.header[23] = 0;
      if (keep !== 'bytes') packet.rgba = new Uint8Array();
    });
    assertLmtIntegrity(data, 'rejected-texture-has-payload', keep);
  }
});

test('a legitimate LMT1 source version change ends motion qualification without an integrity accusation', () => {
  const data = alterOdd(lmtFixture(), sample => {
    for (const image of sample.pixels) if (image.slot < 2)
      image.texture.header[11] = image.texture.header[12] = image.texture.header[13] = 8;
  });
  const result = analyzeLiveMotion(data);
  assertLmtNoPlayerCoverage(result, 'source version changed');
  assert.equal(result.captureComplete, true);
  assert.equal(result.accounting.integrityRejectedPixelPackets, 0);
  assert.equal(result.accounting.pixelPairReasons['texture-source-version-or-layout-changed'], 4);
  assert.deepEqual(result.violations, []);
});

test('changing even an unused halo byte under the same LMT1 owner/version is an integrity failure', () => {
  const data = alterOdd(lmtFixture(), sample => {
    for (const image of sample.pixels) if (image.slot < 2) image.texture.rgba[0] ^= 1;
  });
  const result = analyzeLiveMotion(data);
  assertLmtNoPlayerCoverage(result, 'same-version payload change');
  assert.equal(result.captureComplete, false);
  assert.equal(result.accounting.integrityRejectedPixelPackets, 4);
  assert.equal(result.accounting.pixelPairReasons['same-version-texture-bytes-changed'], 4);
  assert.deepEqual(result.violations, []);
});

test('LMT1 headers, actual framebuffer bytes and CPU crop bytes retain their meaning as ordinary serialized arrays', () => {
  for (const filter of [0, 1]) {
    const data = lmtFixture({filter});
    const serialized = JSON.parse(JSON.stringify({
      textureEvidenceEnabled: true, observerStatus: Array.from(data.observerStatus), errors: [], dropped: 0,
      samples: data.samples.map(sample => ({...sample,
        words: Array.from(sample.words), presentedWords: Array.from(sample.presentedWords),
        pixels: sample.pixels.map(image => ({...image, rgba: Array.from(image.rgba),
          ...(image.texture ? {texture: {header: Array.from(image.texture.header), rgba: Array.from(image.texture.rgba)}} : {})})),
      })),
    }));
    assert(Array.isArray(serialized.samples[0].pixels[0].texture.header));
    assert(Array.isArray(serialized.samples[0].pixels[0].texture.rgba));
    assertLmtCovered(analyzeLiveMotion(serialized), 'serialized LMT1 filter ' + filter);
  }
});
