// Offline evidence from LiveMotionObservation.hpp schema 1. This module never
// reads a live heap/GL context, creates motion, or substitutes a calculated quad.
// Pixel evidence is deliberately limited: submit-stamped CPU textures may
// constrain foreground support, but are not a final-compositing/occlusion oracle.
const INVALID = 0xffffffff;
const HEADER = 32, RECORD = 80, WORDS = 352, SLOTS = 4;
const FRAME_REQUIRED = 1 | 2 | 4 | 8 | 16;
const FRAME_UNSAFE = 64 | 128 | 256 | 512 | 1024;
const RECORD_REQUIRED = 1 | 2 | 4 | 8 | 16 | 32;
const COMPLETED_FRONTIER_ENDPOINTS = 256, FRONTIER_MAGIC = 0x52465031;
const REJECTION_NAMES = ['no-quad', 'unsupported-mode', 'multiple-quads',
  'resources-unavailable', 'different-target', 'non-finite-geometry',
  'unsupported-viewport', 'duplicate-owner', 'historical-owner'];
const READBACK_STATES = ['complete', 'metadata-only', 'no-supported-roi', 'pending', 'dropped', 'failed'];

export const liveMotionAnalysisPolicy = Object.freeze({
  schema: 1, geometryTolerancePx: 0.03, minimumPixelMotionPx: 1,
  pixelAssociationTolerancePx: 0.75, minimumCorrelation: 0.92,
  minimumCorrelationMargin: 0.08, strongContradictionCorrelation: 0.96,
  minimumTextureContrast: 8, minimumFeaturePoints: 12, maximumFeaturePoints: 96,
  maximumSearchComponentPx: 12, minimumBackgroundPoints: 12,
  maximumPairMs: 50, maximumWindowMs: 80, maximumWindowSamples: 8,
  maximumRoiSide: 96, predictionLimit: 12,
  textureEvidenceVersion: 1, minimumIntrinsicAlpha: 224,
  minimumForegroundAlpha: 0.4, minimumIntrinsicGradientEnergy: 64,
});
const P = liveMotionAnalysisPolicy;
const finite = Number.isFinite;
const uint = x => Number.isInteger(x) && x >= 0 && x <= INVALID;
const count = (into, reason) => { into[reason] = (into[reason] || 0) + 1; };
const norm = v => Math.hypot(v[0], v[1]);
const distance = (a, b) => Math.hypot(a[0] - b[0], a[1] - b[1]);
const same = (a, b) => (Array.isArray(a) || ArrayBuffer.isView(a)) &&
  a.length === b.length && a.every((v, i) => v === b[i]);
const wordArray = (value, length) => {
  if ((!Array.isArray(value) && !(value instanceof Uint32Array)) || value.length !== length)
    return null;
  for (const word of value) if (!uint(word)) return null;
  return new Uint32Array(value);
};
const incremented = (previous, current) => current > previous && current - previous === 1;
const outcome = (status, reason, detail = {}) => ({status, reason, ...detail});
const notCovered = (reason, detail) => outcome('not-covered', reason, detail);
const violation = (reason, detail) => outcome('violation', reason, detail);

function decodeFrame(value) {
  const words = wordArray(value, WORDS);
  if (!words || words[0] !== 1 || words[1] !== WORDS || words[2] !== RECORD || words[3] !== SLOTS)
    return null;
  const floats = new Float32Array(words.buffer);
  const header = {
    observerEpoch: words[4], identityEpoch: words[5], callbackSerial: words[6],
    drawSerial: words[7], commitSerial: words[8], phase: words[9], flags: words[10],
    drawKind: words[11], session: [words[12], words[13]], generation: words[14],
    localSeat: words[15], playerCount: words[16], logicalFrame: words[17],
    confirmed: words[18], rollbacks: words[19], worldFlags: words[20],
    correctionEnd: words[21], missingInputs: words[22], intervalScaleMilli: words[23],
    surface: words[24], surfaceWidth: words[25], surfaceHeight: words[26],
    alpha: floats[28], worldAlpha: floats[29],
  };
  const objects = Array.from({length: SLOTS}, (_, slot) => {
    const offset = HEADER + slot * RECORD;
    const w = words.subarray(offset, offset + RECORD);
    const f = floats.subarray(offset, offset + RECORD);
    return {
      words: w, kind: w[0], slot: w[1], objectIndex: w[2], objectId: w[3],
      incarnation: w[4], seat: w[5], state: w[6] | 0, flags: w[7], rejections: w[8],
      quadCount: w[9], viewport: Array.from(w.subarray(22, 26)),
      endpointFrame: w[26], previous: Array.from(f.subarray(28, 31)),
      current: Array.from(f.subarray(31, 34)), previousState: w[78] | 0,
      quad: Array.from({length: 4}, (_, i) => {
        const at = 50 + i * 7;
        return {x: f[at], y: f[at + 1], z: f[at + 2], w: f[at + 3],
          color: w[at + 4], u: f[at + 5], v: f[at + 6]};
      }),
    };
  });
  const frontierError = applyFrontierExtension(words, header, objects);
  return {words, header, objects, frontierError};
}

function applyFrontierExtension(words, header, objects) {
  for (const object of objects) {
    object.logicalPrevious = object.previous.slice();
    object.logicalCurrent = object.current.slice();
    object.frontier = null;
  }
  const aux = words.subarray(HEADER + 3 * RECORD);
  const mask = objects.slice(0, 2).reduce((value, o, slot) =>
    value | ((o.flags & COMPLETED_FRONTIER_ENDPOINTS) ? 1 << slot : 0), 0);
  if (objects[2].flags & COMPLETED_FRONTIER_ENDPOINTS) return 'bullet-cannot-use-player-frontier';
  if (!mask) return aux[0] === FRONTIER_MAGIC ? 'extension-without-owner-flag' : null;
  if (aux[0] !== FRONTIER_MAGIC || aux[1] !== 1 || aux[2] !== mask || aux[3] !== 16 ||
      aux[4] !== 2 || aux[5] || aux[6] || aux[7] || aux.subarray(40).some(Boolean))
    return 'invalid-frontier-extension-header';
  const floats = new Float32Array(aux.buffer, aux.byteOffset, aux.length);
  for (let slot = 0; slot < 2; ++slot) {
    const at = 8 + slot * 16, record = aux.subarray(at, at + 16), object = objects[slot];
    if (!(mask & (1 << slot))) {
      if (record.some(Boolean)) return 'unflagged-frontier-payload';
      continue;
    }
    const [epoch, fromFrame, toFrame, fromRevision, toRevision, state, deaths, flags] = record;
    const previous = Array.from(floats.subarray(at + 8, at + 11));
    const current = Array.from(floats.subarray(at + 11, at + 14));
    if (object.kind !== 1 || object.seat !== slot || slot === header.localSeat || !epoch ||
        fromFrame === INVALID || toFrame !== header.logicalFrame || fromFrame > toFrame ||
        fromRevision > toRevision || (state | 0) !== object.state || ![1, 3, 7].includes(flags) ||
        record[14] || record[15] || !previous.every(finite) || !current.every(finite) ||
        !same(record.subarray(11, 14), object.words.subarray(31, 34)) ||
        ((flags & 2) && !same(record.subarray(8, 11), record.subarray(11, 14))))
      return 'invalid-frontier-endpoint-payload';
    object.frontier = {epoch, fromFrame, toFrame, fromRevision, toRevision,
      state: state | 0, deaths, flags, signature: Array.from(record)};
    // Raw record words 28..33 retain their original logical-tick meaning.
    // Only this explicitly flagged, versioned display pair drives its quad.
    object.previous = previous; object.current = current;
  }
  return null;
}

function frameEligibility(sample, frame, presented) {
  if (!sample || !uint(sample.index) || !finite(sample.atMs))
    return notCovered('invalid-sample-envelope', {integrity: true});
  if (!frame || !presented) return notCovered('invalid-observer-abi', {integrity: true});
  if (frame.frontierError || presented.frontierError)
    return notCovered('invalid-completed-frontier-extension', {integrity: true,
      detail: frame.frontierError || presented.frontierError});
  const h = frame.header, committed = presented.header;
  // Getter phase 1 returns Header::Pending=2; getter phase 0 returns Presented=3.
  if (h.phase !== 2 || h.commitSerial !== 0 || committed.phase !== 3 ||
      !committed.commitSerial || (h.flags & 32) || committed.flags !== (h.flags | 32))
    return notCovered('unmatched-commit-header', {integrity: true});
  for (let word = 0; word < WORDS; ++word) {
    if (word === 8 || word === 9 || word === 10) continue;
    if (frame.words[word] !== presented.words[word])
      return notCovered('unmatched-commit-content', {integrity: true, word});
  }
  if ((h.flags & FRAME_REQUIRED) !== FRAME_REQUIRED || (h.flags & FRAME_UNSAFE))
    return notCovered('unmapped-or-unsafe-draw', {flags: h.flags});
  if (sample.bindingMatched !== true) return notCovered('default-binding-unmatched');
  if (!h.observerEpoch || !h.identityEpoch || !h.callbackSerial || !h.drawSerial ||
      !h.surface || h.logicalFrame === INVALID || ![2, 3].includes(h.playerCount) ||
      h.localSeat >= h.playerCount || !(h.session[0] || h.session[1]))
    return notCovered('invalid-frame-identity', {integrity: true});
  if (!finite(h.alpha) || !finite(h.worldAlpha) || h.alpha < 0 || h.alpha > 1 ||
      h.worldAlpha < 0 || h.worldAlpha > 1)
    return notCovered('invalid-alpha', {integrity: true});
  if (!same(sample.bufferSize || [], [640, 480]) || !same(sample.blit || [],
      [0, 0, h.surfaceWidth, h.surfaceHeight, 0, 480, 640, 0, 0x4000, 0x2600]) ||
      !h.surfaceWidth || !h.surfaceHeight)
    return notCovered('unsupported-blit-or-buffer');
  const owners = frame.objects.filter(o => o.flags & 1);
  if (owners.length !== frame.words[27] || owners.filter(o => o.flags & 2).length !== frame.words[30] ||
      owners.filter(o => o.rejections).length !== frame.words[31] || (frame.objects[3].flags & 1))
    return notCovered('invalid-object-accounting', {integrity: true});
  if ((h.flags & (2048 | 4096 | 8192)) || (h.worldFlags & 0x74))
    return notCovered('frozen-replay-or-open-frame');
  if (h.correctionEnd !== INVALID) return notCovered('recovery-pending');
  if (h.missingInputs >= P.predictionLimit) return notCovered('network-prediction-limit');
  return outcome('eligible', 'eligible-frame');
}

function polygon(quad) {
  const center = quad.reduce((v, p) => [v[0] + p[0] / 4, v[1] + p[1] / 4], [0, 0]);
  const points = quad.slice().sort((a, b) => Math.atan2(a[1] - center[1], a[0] - center[0]) -
    Math.atan2(b[1] - center[1], b[0] - center[0]));
  let area = 0;
  for (let i = 0; i < 4; ++i) {
    const a = points[i], b = points[(i + 1) % 4];
    area += a[0] * b[1] - a[1] * b[0];
  }
  return {points, area: area / 2, bounds: [Math.min(...quad.map(v => v[0])),
    Math.min(...quad.map(v => v[1])), Math.max(...quad.map(v => v[0])), Math.max(...quad.map(v => v[1]))]};
}
function edgeDistance(shape, x, y) {
  let result = Infinity;
  for (let i = 0; i < 4; ++i) {
    const a = shape.points[i], b = shape.points[(i + 1) % 4];
    const dx = b[0] - a[0], dy = b[1] - a[1], length = Math.hypot(dx, dy);
    if (!length) return -Infinity;
    result = Math.min(result, (dx * (y - a[1]) - dy * (x - a[0])) / length);
  }
  return result;
}
function objectEligibility(frame, object, slot) {
  const h = frame.header, o = object;
  if (!(o.flags & 1)) return notCovered('owner-not-seen');
  if (o.rejections) return notCovered('native-rejection', {rejections: REJECTION_NAMES.filter((_, i) => o.rejections & (1 << i)), mask: o.rejections});
  if (o.slot !== slot || (slot < 2 ? o.kind !== 1 || o.seat !== slot || o.objectId !== slot || o.objectIndex !== slot :
      o.kind !== 2 || o.objectIndex >= 2000 || o.state !== 1) || !o.incarnation || o.words[79] !== 0)
    return notCovered('unsupported-object-identity');
  if (o.flags & 128) return notCovered('new-incarnation');
  if (h.drawKind !== 2 || (o.flags & RECORD_REQUIRED) !== RECORD_REQUIRED || o.quadCount !== 1)
    return notCovered('not-interpolated-screen-quad');
  if (o.state !== (o.frontier ? o.frontier.state : o.previousState) || o.endpointFrame !== h.logicalFrame || o.endpointFrame === INVALID ||
      !o.previous.every(finite) || !o.current.every(finite))
    return notCovered('discontinuous-endpoints');
  if (!o.words[19] || o.words[21] !== h.surface) return notCovered('resource-target-unmatched');
  const [x, y, width, height] = o.viewport;
  if (!width || !height || x + width > h.surfaceWidth || y + height > h.surfaceHeight)
    return notCovered('unsupported-viewport');
  for (const v of o.quad) {
    if (![v.x, v.y, v.z, v.w, v.u, v.v].every(finite) || v.w <= 0)
      return notCovered('invalid-submitted-quad');
    // Screen shader viewport subtraction and glViewport cancel. The viewport
    // still clips; clipped sprites cannot supply a whole translation template.
    if (v.x + 0.5 < x || v.x + 0.5 > x + width || v.y + 0.5 < y || v.y + 0.5 > y + height)
      return notCovered('quad-clipped-by-viewport');
  }
  if (!o.quad.some(v => v.color >>> 24)) return notCovered('transparent-quad');
  const mapped = o.quad.map(v => [(v.x + 0.5) * 640 / h.surfaceWidth,
    480 - (v.y + 0.5) * 480 / h.surfaceHeight]);
  const shape = polygon(mapped);
  if (!(shape.area >= 4)) return notCovered('degenerate-or-tiny-quad');
  return outcome('eligible', 'eligible-object', {mapped, shape});
}

function identityOf(h, o) {
  return [h.observerEpoch, h.identityEpoch, ...h.session, h.generation,
    o.kind, o.objectIndex, o.objectId, o.incarnation, o.frontier ? 1 : 0, o.frontier?.epoch || 0];
}
function endpointKey(h, o) {
  return identityOf(h, o).concat(o.endpointFrame, Array.from(o.words.subarray(28, 34)),
    o.frontier?.signature || []).join(':');
}
function sampleContext(observation) {
  const {sample, h, o, mapped} = observation;
  return {index: sample.index, atMs: sample.atMs, callbackSerial: h.callbackSerial,
    drawSerial: h.drawSerial, commitSerial: observation.commitSerial, logicalFrame: h.logicalFrame,
    alpha: h.alpha, worldAlpha: h.worldAlpha, missingInputs: h.missingInputs,
    endpointSource: o.frontier ? 'completed-display-frontier' : 'logical-tick',
    logicalPrevious: o.logicalPrevious, logicalCurrent: o.logicalCurrent,
    displayFrontier: o.frontier ? {...o.frontier, signature: undefined} : null,
    intervalScaleMilli: h.intervalScaleMilli, quad: mapped.map(v => v.slice()),
    readbackStatus: sample.readbackStatus};
}
function continuity(a, b) {
  if (b.offset !== a.offset + 1 || !incremented(a.sample.index, b.sample.index) ||
      !incremented(a.commitSerial, b.commitSerial)) return 'nonconsecutive-commit';
  if (b.sample.atMs <= a.sample.atMs || b.sample.atMs - a.sample.atMs > P.maximumPairMs)
    return 'commit-time-gap';
  if (b.h.drawSerial <= a.h.drawSerial || b.h.callbackSerial < a.h.callbackSerial)
    return 'reused-draw-or-callback';
  if (!same(a.identity, b.identity)) return 'identity-changed';
  if (a.h.rollbacks !== b.h.rollbacks) return 'correction-boundary';
  if (a.key !== b.key) return 'endpoint-pair-changed';
  return null;
}

function geometryPair(a, b) {
  const detail = {from: a.sample.index, to: b.sample.index,
    alpha: [a.h.worldAlpha, b.h.worldAlpha], elapsedMs: b.sample.atMs - a.sample.atMs};
  const appearance = [6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 78, 79];
  // PlayerDraw places world translation in VM position. BulletFrame instead
  // writes the interpolated world point plus its fixed anchor to script_position.
  // The latter is motion evidence, not an invariant local animation transform.
  if (a.o.kind !== 2) appearance.push(37, 38, 39);
  if (appearance.some(i => a.o.words[i] !== b.o.words[i]) || a.h.surface !== b.h.surface ||
      a.h.surfaceWidth !== b.h.surfaceWidth || a.h.surfaceHeight !== b.h.surfaceHeight)
    return notCovered('appearance-resource-or-viewport-changed', detail);
  for (let v = 0; v < 4; ++v) {
    const aw = a.o.words, bw = b.o.words, at = 50 + 7 * v;
    if (aw[at + 4] !== bw[at + 4] || aw[at + 5] !== bw[at + 5] || aw[at + 6] !== bw[at + 6] ||
        Math.abs(a.o.quad[v].z - b.o.quad[v].z) > 1e-6 || Math.abs(a.o.quad[v].w - b.o.quad[v].w) > 1e-6)
      return notCovered('quad-uv-color-or-depth-changed', detail);
  }
  const shifts = a.mapped.map((v, i) => [b.mapped[i][0] - v[0], b.mapped[i][1] - v[1]]);
  const actual = shifts.reduce((v, d) => [v[0] + d[0] / 4, v[1] + d[1] / 4], [0, 0]);
  const shapeResidualPx = Math.max(...shifts.map(v => distance(v, actual)));
  if (shapeResidualPx > P.geometryTolerancePx)
    return notCovered('quad-relative-shape-changed', {...detail, shapeResidualPx});
  const motion = [(a.o.current[0] - a.o.previous[0]) * 640 / a.h.surfaceWidth,
    -(a.o.current[1] - a.o.previous[1]) * 480 / a.h.surfaceHeight];
  const deltaAlpha = b.h.worldAlpha - a.h.worldAlpha;
  const expected = motion.map(v => v * deltaAlpha);
  if (a.o.kind === 2) {
    const af = new Float32Array(a.o.words.buffer, a.o.words.byteOffset, RECORD);
    const bf = new Float32Array(b.o.words.buffer, b.o.words.byteOffset, RECORD);
    const scriptDelta = [bf[37] - af[37], bf[38] - af[38], bf[39] - af[39]];
    const sourceDelta = a.o.current.map((v, i) => (v - a.o.previous[i]) * deltaAlpha);
    if (scriptDelta.some((v, i) => !finite(v) || Math.abs(v - sourceDelta[i]) > P.geometryTolerancePx))
      return notCovered('bullet-script-position-not-world-translation', {...detail, scriptDelta, sourceDelta});
  }
  Object.assign(detail, {expected, actual, shapeResidualPx, residualPx: distance(expected, actual)});
  if (deltaAlpha < -1e-6) return violation('alpha-regressed-within-endpoint-pair', detail);
  if (norm(motion) <= P.geometryTolerancePx) {
    return norm(actual) <= P.geometryTolerancePx ? notCovered('legal-stationary-object', detail) :
      violation('stationary-endpoints-but-quad-moved', detail);
  }
  if (deltaAlpha <= 1e-6) return norm(actual) <= P.geometryTolerancePx ?
    notCovered('held-alpha', detail) : violation('quad-moved-with-held-alpha', detail);
  if (detail.residualPx > P.geometryTolerancePx) {
    const reason = norm(actual) <= P.geometryTolerancePx ? 'quad-held-with-advancing-alpha' :
      actual[0] * expected[0] + actual[1] * expected[1] < 0 ? 'quad-moved-backwards' :
      norm(actual) > norm(expected) + P.geometryTolerancePx ? 'quad-ahead-of-alpha' : 'quad-displacement-mismatch';
    return violation(reason, detail);
  }
  return outcome('covered', 'quad-follows-world-alpha', detail);
}

const TEXTURE_REJECTIONS = ['invalid', 'ok', 'disabled', 'no-pending', 'request-mismatch',
  'unsupported-record', 'resource-missing', 'resource-binding-mismatch', 'version-mismatch',
  'gpu-unavailable', 'gpu-written', 'unsupported-format', 'unsupported-sampling', 'invalid-uv',
  'footprint-out-of-bounds', 'crop-too-large', 'invalid-storage', 'unsupported-host', 'no-submit-stamp'];
const SOURCE_PIXEL_BYTES = Object.freeze({20: 3, 21: 4, 22: 4, 23: 2, 24: 2, 25: 2, 26: 2, 28: 1, 50: 1});
function textureEvidence(observation, image) {
  const {o, h} = observation, packet = image.texture;
  const invalid = (reason, detail = {}) => notCovered(reason, {...detail, integrity: true});
  if (o.kind !== 1) return packet ? invalid('texture-packet-on-non-player') :
    outcome('eligible', 'no-player-texture-required', {texture: null});
  if (!packet) return observation.requireTexture ? invalid('required-texture-packet-missing') :
    outcome('eligible', 'legacy-rgb-only', {texture: null});
  const header = wordArray(packet.header, 32), rgba = packet.rgba;
  if (!header || header[0] !== 0x4c4d5431 || header[1] !== 1 || header[2] !== 32 ||
      header[24] !== 128 || header[25] !== 128 || header[31] ||
      header[3] < 1 || header[3] >= TEXTURE_REJECTIONS.length ||
      (!Array.isArray(rgba) && !(rgba instanceof Uint8Array)))
    return invalid('invalid-texture-packet-abi');
  if (header[3] !== 1) {
    if (header[22] || header[23] || rgba.length) return invalid('rejected-texture-has-payload');
    return notCovered('native-texture-' + TEXTURE_REJECTIONS[header[3]], {nativeTextureStatus: header[3]});
  }
  if (!same(header.subarray(4, 8), [h.observerEpoch, h.identityEpoch, h.callbackSerial, h.drawSerial]) ||
      header[8] !== o.slot || header[9] !== o.words[20] || header[10] !== o.words[19])
    return invalid('texture-packet-draw-or-owner-mismatch');
  if (header[11] === INVALID || header[11] !== header[12] || header[12] !== header[13] || header[30] !== 63)
    return invalid('texture-packet-version-or-authority-mismatch');
  const [format, width, height, pitch, x, y, cropWidth, cropHeight, rgbaPitch, bytes] = header.subarray(14, 24);
  const sourceBytes = SOURCE_PIXEL_BYTES[format];
  const sourceExtent = (height - 1) * pitch + width * sourceBytes;
  if (!sourceBytes || !width || !height || !pitch || pitch < width * sourceBytes ||
      !Number.isSafeInteger(sourceExtent) || sourceExtent > INVALID ||
      !cropWidth || !cropHeight || cropWidth > 128 || cropHeight > 128 ||
      x + cropWidth > width || y + cropHeight > height ||
      rgbaPitch !== cropWidth * 4 || bytes !== rgbaPitch * cropHeight || rgba.length !== bytes)
    return invalid('invalid-texture-packet-storage');
  for (const byte of rgba) if (!Number.isInteger(byte) || byte < 0 || byte > 255)
    return invalid('invalid-texture-pixel-bytes');
  if (header[26] > 1 || header[27] > 1 || header[28] > 2 || header[29] > 2)
    return invalid('invalid-texture-sampler');
  const u = o.quad.map(v => v.u), v = o.quad.map(v => v.v);
  const u0 = Math.min(...u), u1 = Math.max(...u), v0 = Math.min(...v), v1 = Math.max(...v);
  const left = Math.floor(u0 * width) - 1, right = Math.ceil(u1 * width) + 1;
  const top = Math.floor(v0 * height) - 1, bottom = Math.ceil(v1 * height) + 1;
  if (!(u0 >= 0 && u0 < u1 && u1 <= 1 && v0 >= 0 && v0 < v1 && v1 <= 1) ||
      left !== x || top !== y || right - left !== cropWidth || bottom - top !== cropHeight)
    return invalid('texture-packet-uv-footprint-mismatch');
  if (header[26] !== header[27]) return notCovered('mixed-texture-filtering-unsupported');
  if (o.quad.some(q => Math.abs(q.w - o.quad[0].w) > 1e-6))
    return notCovered('perspective-texture-support-unsupported');
  if (o.quad.some(q => q.color !== o.quad[0].color))
    return notCovered('nonuniform-diffuse-texture-support-unsupported');
  const triangles = [];
  for (const indexes of [[0, 1, 2], [1, 2, 3]]) {
    const [a, b, c] = indexes.map(i => observation.mapped[i]);
    const ab = [b[0] - a[0], b[1] - a[1]], ac = [c[0] - a[0], c[1] - a[1]];
    const determinant = ab[0] * ac[1] - ab[1] * ac[0];
    if (!finite(determinant) || Math.abs(determinant) < 1e-8)
      return notCovered('degenerate-texture-triangle');
    triangles.push({indexes, a, ab, ac, determinant});
  }
  const color = o.quad[0].color;
  return outcome('eligible', 'submit-stamped-cpu-texture', {texture: {
    header, image: {x, y, width: cropWidth, height: cropHeight, rgba},
    sourceWidth: width, sourceHeight: height, filter: header[26], triangles,
    quad: o.quad, tint: [((color >>> 16) & 255) / 255, ((color >>> 8) & 255) / 255,
      (color & 255) / 255, (color >>> 24) / 255],
  }});
}
function textureAt(texture, x, y) {
  for (const t of texture.triangles) {
    const dx = x - t.a[0], dy = y - t.a[1];
    const b = (dx * t.ac[1] - dy * t.ac[0]) / t.determinant;
    const c = (t.ab[0] * dy - t.ab[1] * dx) / t.determinant;
    if (b < -1e-7 || c < -1e-7 || b + c > 1 + 1e-7) continue;
    const [qa, qb, qc] = t.indexes.map(i => texture.quad[i]);
    const u = qa.u + b * (qb.u - qa.u) + c * (qc.u - qa.u);
    const v = qa.v + b * (qb.v - qa.v) + c * (qc.v - qa.v);
    // Framebuffer x/y above are actual pixel centres. CPU image rows already
    // correspond to v=0 at the top; the observed quad provides the Y mapping.
    const tx = texture.filter ? u * texture.sourceWidth - 0.5 : Math.floor(u * texture.sourceWidth);
    const ty = texture.filter ? v * texture.sourceHeight - 0.5 : Math.floor(v * texture.sourceHeight);
    if (!contains(texture.image, tx, ty)) return null;
    return [0, 1, 2, 3].map(channelIndex => channel(texture.image, tx, ty, channelIndex));
  }
  return null;
}
function intrinsicFeature(texture, x, y) {
  const samples = [[0, 0], [-1, 0], [1, 0], [0, -1], [0, 1]]
    .map(([dx, dy]) => textureAt(texture, x + 0.5 + dx, y + 0.5 + dy));
  if (samples.some(v => !v || v[3] < P.minimumIntrinsicAlpha)) return null;
  const minimumAlpha = Math.min(...samples.map(v => v[3] / 255 * texture.tint[3]));
  if (minimumAlpha < P.minimumForegroundAlpha) return null;
  const value = (sample, c) => sample[c] * texture.tint[c] * sample[3] / 255 * texture.tint[3];
  let xx = 0, yy = 0, xy = 0;
  for (let c = 0; c < 3; ++c) {
    const dx = (value(samples[2], c) - value(samples[1], c)) / 2;
    const dy = (value(samples[4], c) - value(samples[3], c)) / 2;
    xx += dx * dx; yy += dy * dy; xy += dx * dy;
  }
  return {xx, yy, xy, energy: (xx + yy) / 3, minimumAlpha};
}
function texturePair(a, b) {
  const left = a.pixel.texture, right = b.pixel.texture;
  if (!left && !right) return null;
  if (!left || !right) return notCovered('texture-evidence-mode-changed');
  if (!same(left.header.subarray(9, 31), right.header.subarray(9, 31)))
    return notCovered('texture-source-version-or-layout-changed');
  if (!same(left.image.rgba, right.image.rgba))
    return notCovered('same-version-texture-bytes-changed', {integrity: true});
  if (left.tint[3] < P.minimumForegroundAlpha)
    return notCovered('texture-foreground-opacity-insufficient', {opacity: left.tint[3]});
  return null;
}

function pixelImage(observation) {
  const {sample, o, shape} = observation;
  if (sample.readbackStatus !== 'complete') return notCovered('readback-' + sample.readbackStatus);
  if (!Array.isArray(sample.pixels)) return notCovered('missing-pixel-rois');
  const found = sample.pixels.filter(p => p && p.slot === o.slot);
  if (found.length !== 1) return notCovered('missing-or-duplicate-object-roi');
  const image = found[0], {x, y, width, height, rgba} = image;
  if (![x, y, width, height].every(Number.isInteger) || x < 0 || y < 0 || width <= 0 || height <= 0 ||
      width > P.maximumRoiSide || height > P.maximumRoiSide || x + width > 640 || y + height > 480 ||
      (!Array.isArray(rgba) && !(rgba instanceof Uint8Array)) || rgba.length !== width * height * 4)
    return notCovered('invalid-pixel-roi');
  for (const byte of rgba) if (!Number.isInteger(byte) || byte < 0 || byte > 255)
    return notCovered('invalid-pixel-bytes');
  const [left, bottom, right, top] = shape.bounds;
  if (left < x || bottom < y || right > x + width || top > y + height)
    return notCovered('roi-does-not-cover-quad');
  for (const other of observation.frame.objects) {
    if (other.slot === o.slot || !(other.flags & 2) || !(other.flags & 16) || other.quadCount !== 1 ||
        other.words[21] !== observation.h.surface || !other.quad.every(v => finite(v.x) && finite(v.y))) continue;
    const points = other.quad.map(v => [(v.x + 0.5) * 640 / observation.h.surfaceWidth,
      480 - (v.y + 0.5) * 480 / observation.h.surfaceHeight]);
    const bounds = polygon(points).bounds;
    if (Math.min(right, bounds[2]) > Math.max(left, bounds[0]) &&
        Math.min(top, bounds[3]) > Math.max(bottom, bounds[1]))
      return notCovered('observed-object-overlap');
  }
  const evidence = textureEvidence(observation, image);
  if (evidence.status !== 'eligible') return evidence;
  return outcome('eligible', 'eligible-pixels', {image, texture: evidence.texture});
}
function contains(image, x, y, padding = 0) {
  return x >= image.x + padding && y >= image.y + padding &&
    x <= image.x + image.width - 1 - padding && y <= image.y + image.height - 1 - padding;
}
function channel(image, x, y, color) {
  const ix = Math.floor(x), iy = Math.floor(y), fx = x - ix, fy = y - iy;
  const at = ((iy - image.y) * image.width + ix - image.x) * 4 + color;
  const nextX = fx ? 4 : 0, nextY = fy ? image.width * 4 : 0;
  const a = image.rgba[at] * (1 - fx) + image.rgba[at + nextX] * fx;
  const b = image.rgba[at + nextY] * (1 - fx) + image.rgba[at + nextY + nextX] * fx;
  return a * (1 - fy) + b * fy;
}
function gradient(image, x, y) {
  let xx = 0, yy = 0, xy = 0;
  for (let c = 0; c < 3; ++c) {
    const dx = (channel(image, x + 1, y, c) - channel(image, x - 1, y, c)) / 2;
    const dy = (channel(image, x, y + 1, c) - channel(image, x, y - 1, c)) / 2;
    xx += dx * dx; yy += dy * dy; xy += dx * dy;
  }
  return {xx, yy, xy, energy: (xx + yy) / 3};
}
function template(image, points) {
  const sums = [0, 0, 0], squares = [0, 0, 0];
  const values = points.map(p => [0, 1, 2].map(c => {
    const v = channel(image, p.x, p.y, c); sums[c] += v; squares[c] += v * v; return v;
  }));
  const variance = squares.reduce((total, v, c) => total + v - sums[c] * sums[c] / points.length, 0);
  return {points, values, sums, variance, contrast: Math.sqrt(Math.max(0, variance) / (3 * points.length))};
}
function correlation(source, image, shift) {
  const sums = [0, 0, 0], squares = [0, 0, 0], products = [0, 0, 0], n = source.points.length;
  for (let i = 0; i < n; ++i) {
    const p = source.points[i], x = p.x + shift[0], y = p.y + shift[1];
    if (!contains(image, x, y)) return null;
    for (let c = 0; c < 3; ++c) {
      const v = channel(image, x, y, c);
      sums[c] += v; squares[c] += v * v; products[c] += source.values[i][c] * v;
    }
  }
  let variance = 0, covariance = 0;
  for (let c = 0; c < 3; ++c) {
    variance += squares[c] - sums[c] * sums[c] / n;
    covariance += products[c] - source.sums[c] * sums[c] / n;
  }
  if (!(source.variance > 1) || !(variance > 1)) return null;
  const ratio = Math.sqrt(variance / source.variance);
  if (ratio < 0.5 || ratio > 2) return null;
  return Math.max(-1, Math.min(1, covariance / Math.sqrt(source.variance * variance)));
}
function reduced(points, maximum) {
  if (points.length <= maximum) return points;
  return Array.from({length: maximum}, (_, i) => points[Math.floor(i * points.length / maximum)]);
}
function pixelPair(a, b, geometry) {
  const detail = {from: a.sample.index, to: b.sample.index, expected: geometry.expected};
  if (geometry.status !== 'covered') return notCovered('geometry-not-covered', {...detail, geometryReason: geometry.reason});
  if (norm(geometry.expected) < P.minimumPixelMotionPx) return notCovered('subpixel-motion', detail);
  const textureBoundary = texturePair(a, b);
  if (textureBoundary) return {...detail, ...textureBoundary};
  const texture = a.pixel.texture;
  const expected = geometry.expected;
  const rx = Math.max(3, Math.ceil(Math.abs(expected[0])) + 2), ry = Math.max(3, Math.ceil(Math.abs(expected[1])) + 2);
  if (Math.max(rx, ry) > P.maximumSearchComponentPx) return notCovered('motion-outside-bounded-search', detail);
  const source = a.pixel.image, destination = b.pixel.image, featurePoints = [], ringPoints = [];
  let interiorCandidates = 0, xx = 0, yy = 0, xy = 0, insideEnergy = 0, ringEnergy = 0;
  for (let y = source.y + 1; y < source.y + source.height - 1; ++y) {
    for (let x = source.x + 1; x < source.x + source.width - 1; ++x) {
      const edge = edgeDistance(a.shape, x + 0.5, y + 0.5);
      if (edge >= 1.5) {
        // Support comes from the CPU asset at the actual submitted UVs, before
        // evaluating any candidate displacement or destination correlation.
        const intrinsic = texture ? intrinsicFeature(texture, x, y) : null;
        if (texture && (!intrinsic || intrinsic.energy < P.minimumIntrinsicGradientEnergy)) continue;
        const g = gradient(source, x, y);
        if (g.energy < 64) continue;
        ++interiorCandidates;
        // One support set for EVERY candidate: a favorable displacement cannot
        // win by silently dropping inconvenient pixels at the ROI boundary.
        if (!contains(destination, x - rx, y - ry, 1) || !contains(destination, x + rx, y + ry, 1)) continue;
        featurePoints.push({x, y, g, intrinsic}); xx += g.xx; yy += g.yy; xy += g.xy; insideEnergy += g.energy;
      } else if (edge <= -2 && edge >= -8 && !(x & 1) && !(y & 1) &&
          contains(destination, x, y) && contains(destination, x + expected[0], y + expected[1], 1)) {
        ringPoints.push({x, y}); ringEnergy += gradient(source, x, y).energy;
      }
    }
  }
  if (featurePoints.length < P.minimumFeaturePoints || featurePoints.length < interiorCandidates * 0.75)
    return notCovered('insufficient-common-texture-support', {...detail, featurePoints: featurePoints.length, interiorCandidates});
  const support = texture ? featurePoints.slice().sort((a, b) => b.intrinsic.energy - a.intrinsic.energy ||
    a.y - b.y || a.x - b.x).slice(0, P.maximumFeaturePoints) : reduced(featurePoints, P.maximumFeaturePoints);
  if (texture) {
    xx = yy = xy = insideEnergy = 0;
    let ixx = 0, iyy = 0, ixy = 0;
    for (const p of support) {
      xx += p.g.xx; yy += p.g.yy; xy += p.g.xy; insideEnergy += p.g.energy;
      ixx += p.intrinsic.xx; iyy += p.intrinsic.yy; ixy += p.intrinsic.xy;
    }
    if (ixx * iyy - ixy * ixy < 0.03 * (ixx + iyy) ** 2)
      return notCovered('one-dimensional-intrinsic-texture', detail);
  }
  if (xx * yy - xy * xy < 0.03 * (xx + yy) ** 2)
    return notCovered('one-dimensional-or-ambiguous-texture', detail);
  if (ringPoints.length < P.minimumBackgroundPoints) return notCovered('insufficient-background-context', detail);
  insideEnergy /= texture ? support.length : featurePoints.length; ringEnergy /= ringPoints.length;
  if (insideEnergy < Math.max(64, ringEnergy * 1.25))
    return notCovered('texture-not-localized-to-quad', {...detail, insideEnergy, ringEnergy});
  const features = template(source, support);
  if (features.contrast < P.minimumTextureContrast)
    return notCovered('low-texture-contrast', {...detail, contrast: features.contrast});
  const candidates = [];
  const add = shift => { const score = correlation(features, destination, shift); if (score !== null) candidates.push({shift, correlation: score}); };
  for (let dy = -ry; dy <= ry; ++dy) for (let dx = -rx; dx <= rx; ++dx) add([dx, dy]);
  if (!expected.every(Number.isInteger)) add(expected.slice());
  const byScore = (a, b) => b.correlation - a.correlation || distance(a.shift, expected) - distance(b.shift, expected);
  candidates.sort(byScore);
  const close = (shift, target) => distance(shift, target) <= P.pixelAssociationTolerancePx;
  const best = candidates[0], expectedPeak = candidates.find(v => close(v.shift, expected));
  const other = candidates.find(v => !close(v.shift, expected));
  const zero = candidates.find(v => v.shift[0] === 0 && v.shift[1] === 0);
  const evidence = {...detail, supportPoints: features.points.length, contrast: features.contrast,
    supportSource: texture ? 'submit-stamped-cpu-texture' : 'localized-screen-rgb',
    foreground: texture ? {owner: texture.header[9], surface: texture.header[10], version: texture.header[11],
      quadOpacity: texture.tint[3], minimumSelectedAlpha: Math.min(...support.map(p => p.intrinsic.minimumAlpha)),
      minimumSelectedIntrinsicEnergy: Math.min(...support.map(p => p.intrinsic.energy)),
      selection: 'intrinsic-alpha-and-gradient-ranked-before-displacement-search'} : null,
    search: {x: [-rx, rx], y: [-ry, ry], candidates: candidates.length, sameSupport: true},
    expectedPeak: expectedPeak || null, alternativePeak: other || null, zero: zero || null,
    best: best || null, insideEnergy, ringEnergy};
  const ring = template(source, reduced(ringPoints, 64));
  const ringExpected = correlation(ring, destination, expected), ringZero = correlation(ring, destination, [0, 0]);
  evidence.background = {points: ring.points.length, contrast: ring.contrast,
    expectedCorrelation: ringExpected, zeroCorrelation: ringZero};
  if (ring.contrast >= P.minimumTextureContrast && ringExpected !== null && ringExpected >= P.minimumCorrelation &&
      (ringZero === null || ringExpected - ringZero >= P.minimumCorrelationMargin))
    return notCovered('background-comotion-ambiguous', evidence);
  if (expectedPeak && zero && other && expectedPeak.correlation >= P.minimumCorrelation &&
      expectedPeak.correlation - zero.correlation >= P.minimumCorrelationMargin &&
      expectedPeak.correlation - other.correlation >= P.minimumCorrelationMargin)
    return outcome('covered', 'localized-texture-follows-quad', evidence);
  if (best && expectedPeak && !close(best.shift, expected) && best.correlation >= P.strongContradictionCorrelation &&
      best.correlation - expectedPeak.correlation >= P.minimumCorrelationMargin) {
    const second = candidates.find(v => !close(v.shift, best.shift));
    if (second && best.correlation - second.correlation >= P.minimumCorrelationMargin)
      return violation(norm(best.shift) < 0.5 ? 'pixels-held-while-quad-moved' : 'pixel-motion-disagrees-with-quad', evidence);
  }
  return notCovered('pixel-match-ambiguous-or-occluded', evidence);
}

function collector(domain, violations, reasons) {
  const active = [null, null, null], windows = [];
  function close(slot, boundary) {
    const group = active[slot]; if (!group) return;
    group.boundary = boundary;
    group.status = group.comparisons.some(p => p.status === 'violation') ? 'violation' :
      group.comparisons.length && group.comparisons.every(p => p.status === 'covered') ? 'covered' : 'not-covered';
    group.reason = group.comparisons.find(p => p.status === 'violation')?.reason ||
      group.comparisons.find(p => p.status !== 'covered')?.reason ||
      (group.comparisons.length ? 'all-consecutive-comparisons-covered' : 'insufficient-consecutive-samples');
    count(reasons, group.reason); windows.push(group); active[slot] = null;
  }
  function accept(observation, previous, pair) {
    const slot = observation.o.slot;
    let group = active[slot];
    if (!previous || !group || group.sampleCount >= P.maximumWindowSamples ||
        observation.sample.atMs - group.firstAtMs > P.maximumWindowMs) {
      close(slot, previous ? 'short-window-limit' : 'continuity-boundary');
      const first = previous && pair ? previous : observation;
      group = active[slot] = {id: domain + '-' + windows.length + '-' + slot,
        kind: observation.o.kind === 1 ? 'player' : 'bullet', slot,
        identity: observation.identity.slice(), endpointFrame: observation.o.endpointFrame,
        endpointSource: observation.o.frontier ? 'completed-display-frontier' : 'logical-tick',
        previousPosition: observation.o.previous.slice(), currentPosition: observation.o.current.slice(),
        coverageKey: observation.identity.concat(observation.o.endpointFrame).join(':'),
        firstAtMs: first.sample.atMs, lastAtMs: first.sample.atMs,
        sampleCount: 0, sampleIndexes: [], comparisons: [], context: []};
      // Keep the boundary comparison when capping a short window. The shared
      // previous commit is an actual sample, not an invented endpoint/phase.
      if (first !== observation) {
        group.sampleCount = 1; group.sampleIndexes.push(first.sample.index);
        group.context.push(sampleContext(first));
      }
    }
    group.sampleCount++; group.lastAtMs = observation.sample.atMs;
    group.sampleIndexes.push(observation.sample.index);
    group.context.push(sampleContext(observation));
    if (pair) {
      group.comparisons.push(pair);
      if (pair.status === 'violation') violations.push({domain, windowId: group.id,
        slot, identity: group.identity.slice(), ...pair});
    }
  }
  return {windows, close, accept, finish() { for (let slot = 0; slot < 3; ++slot) close(slot, 'end-of-input'); }};
}

// minimumPlayerWindows applies independently to native seats 0 AND 1. Slot 2
// is the naturally selected bullet; slot 3 is reserved, never extra coverage.
export function analyzeLiveMotion(input, {minimumPlayerWindows = 2, minimumBulletWindows = 1} = {}) {
  if (!Number.isInteger(minimumPlayerWindows) || minimumPlayerWindows < 1 ||
      !Number.isInteger(minimumBulletWindows) || minimumBulletWindows < 1)
    throw new TypeError('Live motion window thresholds must be positive integers');
  const samples = Array.isArray(input?.samples) ? input.samples : [];
  const errors = Array.isArray(input?.errors) ? input.errors.map(value => String(value?.message ?? value)) :
    input?.errors === undefined ? [] : ['Invalid errors collection'];
  if (!Array.isArray(input?.samples)) errors.push('Invalid samples collection');
  if (input?.textureEvidenceEnabled !== undefined && typeof input.textureEvidenceEnabled !== 'boolean')
    errors.push('Invalid texture evidence mode');
  const dropped = input?.dropped === undefined ? 0 : input.dropped;
  if (!uint(dropped)) errors.push('Invalid dropped count');
  const status = wordArray(input?.observerStatus, 16);
  if (!status || status[0] !== 1) errors.push('Invalid final observer status ABI');
  const observerNames = ['schema', 'enabled', 'observerEpoch', 'identityEpoch', 'callbackSerial', 'drawSerial',
    'publishedCount', 'boundCount', 'surfaceInvalidations', 'unmatchedCommits', 'scopesRejected',
    'selectedBulletIndex', 'selectedBulletId', 'stopped', 'lastRendererPresentations', 'callbackOpen'];
  const report = {schema: 1, scope: [
    'Finite observational evidence for naturally submitted player/bullet screen quads and matched default-framebuffer ROIs; not a whole-screen or no-dropped-frame guarantee.',
    'Every supplied metadata sample is accounted. Pixel windows require consecutive complete real commits and never cross metadata-only, dropped, pending, failed, identity or endpoint boundaries.',
    'Both native object paths use observed world_alpha. Submitted vertices are checked for relative phase displacement; no logical tick, alpha, endpoint quad or rendered pixel is invented.',
    'Remote completed-display-frontier pairs require native flag 256 and the reserved-slot RFP1 version-1 payload. Raw logical previous/current words remain present and the display target must exactly equal the logical current target.',
    'The ABI has no independent absolute endpoint-quad anchor: a common constant geometric offset is outside this relative-displacement check.',
    'LMT1 player packets, when requested, must match the real draw/owner/source version and exact full-halo UV crop. Their intrinsic alpha/gradient selects foreground support before candidate scoring; low-opacity, unsupported or malformed packets never fall back to RGB-only acceptance.',
    'Actual localized RGB correlation keeps the same bounded translation competitors, zero-motion and background checks. CPU texture support is not final compositing or complete occlusion truth; ambiguous/background-comoving cases remain not-covered.',
    'Only the observed full-source 640x480 vertically flipped GL_NEAREST color blit is supported. Diagnostic readbacks and this analysis are not pure-performance measurements.',
  ], policy: {...P}, textureEvidenceEnabled: input?.textureEvidenceEnabled === true,
    errors, dropped: uint(dropped) ? dropped : null,
    observerStatus: status ? Object.fromEntries(observerNames.map((name, i) => [name, status[i]])) : null,
    accounting: {samples: samples.length, readbacks: {}, frameReasons: {}, objectReasons: {}, boundaryReasons: {},
      geometryPairReasons: {}, pixelSampleReasons: {}, pixelPairReasons: {}, geometryWindowReasons: {}, pixelWindowReasons: {},
      integrityRejectedSamples: 0, integrityRejectedPixelPackets: 0, maximumObservedCommitGapMs: 0},
    samples: [], violations: []};
  const geometry = collector('geometry', report.violations, report.accounting.geometryWindowReasons);
  const pixels = collector('pixels', report.violations, report.accounting.pixelWindowReasons);
  const previous = [null, null, null], previousPixel = [null, null, null];
  let lastMatched = null;
  for (let offset = 0; offset < samples.length; ++offset) {
    const sample = samples[offset], frame = decodeFrame(sample?.words), presented = decodeFrame(sample?.presentedWords);
    const readbackStatus = READBACK_STATES.includes(sample?.readbackStatus) ? sample.readbackStatus : 'invalid';
    count(report.accounting.readbacks, readbackStatus);
    let eligible = frameEligibility(sample, frame, presented);
    if (readbackStatus === 'invalid') eligible = notCovered('invalid-readback-status', {integrity: true});
    count(report.accounting.frameReasons, eligible.reason);
    if (eligible.integrity) report.accounting.integrityRejectedSamples++;
    const row = {offset, index: sample?.index ?? null, atMs: sample?.atMs ?? null,
      status: eligible.status, reason: eligible.reason, readbackStatus, objects: []};
    report.samples.push(row);
    if (eligible.status !== 'eligible') {
      for (let slot = 0; slot < 3; ++slot) {
        previous[slot] = previousPixel[slot] = null;
        geometry.close(slot, eligible.reason); pixels.close(slot, eligible.reason);
        count(report.accounting.objectReasons, 'frame-not-eligible');
        row.objects.push({slot, status: 'not-covered', reason: 'frame-not-eligible'});
      }
      continue;
    }
    const h = frame.header;
    row.drawSerial = h.drawSerial; row.commitSerial = presented.header.commitSerial;
    if (lastMatched && incremented(lastMatched.commit, row.commitSerial) && sample.atMs >= lastMatched.atMs)
      report.accounting.maximumObservedCommitGapMs = Math.max(report.accounting.maximumObservedCommitGapMs, sample.atMs - lastMatched.atMs);
    lastMatched = {commit: row.commitSerial, atMs: sample.atMs};
    for (let slot = 0; slot < 3; ++slot) {
      const o = frame.objects[slot], check = objectEligibility(frame, o, slot);
      count(report.accounting.objectReasons, check.reason);
      const objectRow = {slot, kind: o.kind, status: check.status, reason: check.reason};
      if (check.rejections) objectRow.rejections = check.rejections;
      row.objects.push(objectRow);
      if (check.status !== 'eligible') {
        previous[slot] = previousPixel[slot] = null;
        geometry.close(slot, check.reason); pixels.close(slot, check.reason); continue;
      }
      const current = {sample, frame, h, o, offset, commitSerial: row.commitSerial,
        requireTexture: input?.textureEvidenceEnabled === true,
        identity: identityOf(h, o), key: endpointKey(h, o), mapped: check.mapped, shape: check.shape};
      let prior = previous[slot], boundary = prior ? continuity(prior, current) : null;
      if (boundary) {
        count(report.accounting.boundaryReasons, boundary); objectRow.boundary = boundary;
        geometry.close(slot, boundary); pixels.close(slot, boundary); previousPixel[slot] = null; prior = null;
      }
      const pair = prior ? geometryPair(prior, current) : null;
      if (pair) { count(report.accounting.geometryPairReasons, pair.reason); objectRow.geometryReason = pair.reason; }
      geometry.accept(current, prior, pair); previous[slot] = current;
      current.pixel = pixelImage(current); objectRow.pixelReason = current.pixel.reason;
      if (current.pixel.integrity) report.accounting.integrityRejectedPixelPackets++;
      count(report.accounting.pixelSampleReasons, current.pixel.reason);
      if (current.pixel.status !== 'eligible') {
        pixels.close(slot, current.pixel.reason); previousPixel[slot] = null; continue;
      }
      const pixelPrior = previousPixel[slot];
      const pixelComparison = pixelPrior && prior === pixelPrior ? pixelPair(pixelPrior, current, pair) : null;
      if (pixelComparison?.integrity) report.accounting.integrityRejectedPixelPackets++;
      if (pixelComparison) count(report.accounting.pixelPairReasons, pixelComparison.reason);
      pixels.accept(current, pixelPrior && prior === pixelPrior ? pixelPrior : null, pixelComparison);
      previousPixel[slot] = current;
    }
  }
  geometry.finish(); pixels.finish();
  report.geometry = {windows: geometry.windows}; report.pixels = {windows: pixels.windows};
  const coverage = {required: {playerWindowsPerSeat: minimumPlayerWindows, bulletWindows: minimumBulletWindows},
    players: {'0': {windows: 0, windowIds: []}, '1': {windows: 0, windowIds: []}}, bullets: {windows: 0, windowIds: []}};
  const seen = new Set();
  for (const window of pixels.windows) {
    window.countedForCoverage = window.status === 'covered' && !seen.has(window.coverageKey);
    if (!window.countedForCoverage) continue;
    seen.add(window.coverageKey);
    const group = window.kind === 'player' ? coverage.players[String(window.slot)] : coverage.bullets;
    group.windows++; group.windowIds.push(window.id);
  }
  for (const group of Object.values(coverage.players)) group.met = group.windows >= minimumPlayerWindows;
  coverage.bullets.met = coverage.bullets.windows >= minimumBulletWindows;
  coverage.met = Object.values(coverage.players).every(g => g.met) && coverage.bullets.met;
  report.coverage = coverage;
  report.captureComplete = !report.errors.length && report.dropped === 0 && !report.accounting.integrityRejectedSamples &&
    !report.accounting.integrityRejectedPixelPackets &&
    !['pending', 'dropped', 'failed', 'invalid'].some(reason => report.accounting.readbacks[reason]);
  report.passed = coverage.met && report.captureComplete && report.violations.length === 0;
  return report;
}
