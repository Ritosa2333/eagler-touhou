// Pure ES module (no Node built-ins, browser-safe).
// Observed owned native title record (52232B) + independent saved 9B name field
// -> original six-route EX qualification.
//
// Every validation below reuses a constraint that already exists in the candidate's own
// site/runtime modules; nothing here invents a field default, mutates a counter, or hands
// out a literal 63:
//   site/runtime/sound.mjs
//     nativeProfileView()               size/block-tag checks
//     nativeRouteScorePacket()          score/continue scalar checks + clear counter offset
//     validatedOwnedNativeTitleRecord() record schema/revision/provenance/tainted/whole SHA-256
//   site/runtime/last-name-field.mjs
//     nameBytes()                       9 bytes, NUL terminator, printable style-0 glyphs
//   site/runtime/native-normal-postrun.mjs
//     nameOffset 13164, name[8]===0, name bytes equal profile[13164..13172]
//
// The unlock rule itself comes from the shipped original docs (omake.txt), not from the
// incomplete port's rank 1..3 fact policy:
//   "通过全部的6条路线（任意难度）后，就可以在路线选择中进入EX关。"
// Six clears on ANY difficulty, ranks 0..3; rank 4 (EX) is never counted; the scoped docs
// state no continue condition (see EX-ORIGINAL-QUALIFICATION-SOURCE.md for exact scope).

export const PROFILE_BYTES = 52232;
export const ROUTE_BYTES = 1644;
export const STATS_OFFSET = 13152;
export const CLEAR_OFFSET = 14220;
export const ROUTE_COUNT = 8;
export const NAME_OFFSET = STATS_OFFSET + 12; // 13164: stats block +12, product-verified
export const NAME_BYTES = 9;
export const EX_QUALIFICATION_SCHEMA = 'th128-ex-six-route-qualification/1';
export const OWNED_RECORD_SCHEMA = 'th128-owned-native-title-record/1';
export const OWNED_RECORD_PROVENANCE = 'ordinary-native-postrun';
export const QUALIFY_RANKS = Object.freeze([0, 1, 2, 3]);

export const ORIGINAL_ROUTES = Object.freeze([
  Object.freeze({ route: 'A1', index: 0, bit: 1, stages: Object.freeze([1, 2, 3]) }),
  Object.freeze({ route: 'A2', index: 1, bit: 2, stages: Object.freeze([1, 4, 5]) }),
  Object.freeze({ route: 'B1', index: 2, bit: 4, stages: Object.freeze([6, 7, 8]) }),
  Object.freeze({ route: 'B2', index: 3, bit: 8, stages: Object.freeze([6, 9, 10]) }),
  Object.freeze({ route: 'C1', index: 4, bit: 16, stages: Object.freeze([11, 12, 13]) }),
  Object.freeze({ route: 'C2', index: 5, bit: 32, stages: Object.freeze([11, 14, 15]) }),
]);
export const ORIGINAL_ROUTE_ORDER = Object.freeze(ORIGINAL_ROUTES.map(row => row.route));
export const ALL_ROUTE_BITS = ORIGINAL_ROUTES.reduce((mask, row) => mask | row.bit, 0);

export const ORIGINAL_UNLOCK_EVIDENCE = Object.freeze({
  sourceFile: 'omake.txt',
  installRoot: 'E:\\gee\\妖精大战争\\',
  encoding: 'gb18030',
  quote: '　　喔，对了。EX关也有喔。通过全部的6条路线（任意难度）后，就可以在路线选择中进入EX关。',
  rule: 'six-routes-any-difficulty',
  difficultyGate: 'any (ranks 0..3 all count; rank 4 EX never counts)',
  continueGate: 'none in the scoped shipped docs',
  scope: 'readme.txt/omake.txt only; the detailed manual is in-game only; no EXE analysis',
});

const CLEAR_COUNTER_OFFSET = 0x658;
const RANK_STRIDE = 4;
const SCORE_ROW_OFFSET = 16;
const RANK_ROW_BYTES = 320;
const CLEAR_COUNT_CAP = 99999;

function fail(message, code) {
  const error = new Error(message);
  if (code) error.code = code;
  return error;
}

export async function sha256Hex(bytes) {
  if (!(bytes instanceof Uint8Array)) throw fail('Owned native profile bytes are not a Uint8Array', 'profile-bytes');
  if (!globalThis.crypto?.subtle) throw fail('Owned native profile digest is unavailable', 'digest-unavailable');
  const copy = bytes.slice();
  const digest = await globalThis.crypto.subtle.digest('SHA-256', copy);
  return Array.from(new Uint8Array(digest), value => value.toString(16).padStart(2, '0')).join('');
}

// Mirrors sound.mjs nativeProfileView().
export function nativeProfileView(profile) {
  if (!(profile instanceof Uint8Array) || profile.byteLength !== PROFILE_BYTES) {
    throw fail('Original native title profile size differs', 'profile-size');
  }
  const view = new DataView(profile.buffer, profile.byteOffset, profile.byteLength);
  const block = (offset, tag, bytes) => {
    if (view.getUint32(offset, true) !== tag || view.getUint32(offset + 8, true) !== bytes) {
      throw fail('Original native title profile block differs', 'profile-block');
    }
  };
  for (let route = 0; route < ROUTE_COUNT; route += 1) block(route * ROUTE_BYTES, 0x00035243, ROUTE_BYTES);
  block(STATS_OFFSET, 0x00025453, 1068);
  block(CLEAR_OFFSET, 0x00014443, 38012);
  return view;
}

// Mirrors the scalar checks in sound.mjs nativeRouteScorePacket().
export function routeRow(view, route, rank) {
  if (!Number.isInteger(route) || route < 0 || route >= 7) throw fail('Original native route index differs', 'route-index');
  if (!Number.isInteger(rank) || rank < 0 || rank >= 5) throw fail('Original native rank index differs', 'rank-index');
  const row = route * ROUTE_BYTES + SCORE_ROW_OFFSET + rank * RANK_ROW_BYTES;
  const score = view.getUint32(row, true);
  const continues = view.getUint8(row + 5);
  const stage = view.getUint8(row + 4);
  if (score > 999999999 || continues > 9) throw fail('Original native route score or continue byte differs', 'route-scalar');
  return { score, continues, stage };
}

// Clear counters live at the tail of each route block: route*1644 + 0x658 + rank*4.
export function routeClearCount(view, route, rank) {
  if (!Number.isInteger(route) || route < 0 || route >= ROUTE_COUNT) throw fail('Original native route index differs', 'route-index');
  if (!Number.isInteger(rank) || rank < 0 || rank >= 5) throw fail('Original native rank index differs', 'rank-index');
  const value = view.getUint32(route * ROUTE_BYTES + CLEAR_COUNTER_OFFSET + rank * RANK_STRIDE, true);
  if (value > CLEAR_COUNT_CAP) throw fail('Original native clear count exceeds the native cap', 'clear-count-cap');
  return value;
}

export function routeClearTotals(profile) {
  const view = nativeProfileView(profile);
  return Object.freeze(ORIGINAL_ROUTES.map(row => {
    const counts = QUALIFY_RANKS.map(rank => routeClearCount(view, row.index, rank));
    return Object.freeze({
      route: row.route,
      index: row.index,
      bit: row.bit,
      counts: Object.freeze(counts),
      qualifiedTotal: counts.reduce((sum, value) => sum + value, 0),
      rank4Ignored: routeClearCount(view, row.index, 4),
    });
  }));
}

// Mirrors last-name-field.mjs nameBytes().
export function independentNameBytes(value) {
  if (!(value instanceof Uint8Array) || value.length !== NAME_BYTES || value[8] !== 0
      || value.slice(0, 8).some(code => code < 32 || code > 126)) {
    throw fail('Original last-name field must be eight native glyphs and NUL', 'name-field-bytes');
  }
  return value.slice();
}

// Mirrors native-normal-postrun.mjs nameOffset/independent9B identity checks.
export function independentNameIdentity({ profile, name }) {
  if (!name || name.strictRead !== true) throw fail('Independent9B observation requires the actual owner strict read receipt', 'name-observation');
  if (name.absent === true || !Number.isSafeInteger(name.revision) || name.revision < 1) {
    throw fail('Independent9B observation is absent; the owned qualification needs the actual saved name field', 'name-absent');
  }
  const bytes = independentNameBytes(name.bytes);
  const embedded = independentNameBytes(profile.slice(NAME_OFFSET, NAME_OFFSET + NAME_BYTES));
  if (!bytes.every((value, index) => value === embedded[index])) {
    throw fail('Independent9B differs from the observed owned profile name field', 'name-mismatch');
  }
  return Object.freeze({
    revision: name.revision,
    bytes,
    hex: Array.from(bytes, value => value.toString(16).padStart(2, '0')).join(''),
    digest: typeof name.digest === 'string' ? name.digest : null,
    offset: NAME_OFFSET,
  });
}

// Mirrors sound.mjs validatedOwnedNativeTitleRecord() without touching the record.
export async function validateObservedRecord(record) {
  if (record === null || record === undefined) throw fail('Owned native title record is absent', 'record-absent');
  if (record.schema !== OWNED_RECORD_SCHEMA || !Number.isSafeInteger(record.revision) || record.revision < 1
      || !((record.provenance === OWNED_RECORD_PROVENANCE && record.tainted === false) || (record.provenance === 'extra-native-postrun' && record.tainted === true && record.native?.route === 6 && record.native?.rank === 4))
      || typeof record.sha256 !== 'string' || !(/^[a-f0-9]{64}$/).test(record.sha256)) {
    throw fail('Owned native title record provenance differs', 'record-provenance');
  }
  if (!(record.profile instanceof Uint8Array)) throw fail('Owned native title record has no observed profile bytes', 'record-profile');
  const profile = record.profile.slice(), revision = record.revision, expected = record.sha256;
  const native = record.native ? Object.freeze({ ...record.native }) : null, runId = record.runId, provenance=record.provenance, tainted=record.tainted;
  const view = nativeProfileView(profile);
  for(let route=0;route<7;route++)for(let rank=0;rank<5;rank++)routeRow(view,route,rank);
  const digest = await sha256Hex(profile);
  if (digest !== expected || record.revision !== revision || record.sha256 !== expected) throw fail('Owned native title record whole bytes differ', 'record-digest');
  return Object.freeze({
    schema: OWNED_RECORD_SCHEMA,
    revision,
    sha256: digest,
    provenance,
    tainted,
    profile,
    native,
    runId: typeof runId === 'string' ? runId : null,
  });
}

// Staleness / cancellation owner, shaped like the product's isCurrent() guards.
export class QualificationOwner {
  constructor() {
    this.generation = 0;
    this.cancelled = false;
  }

  begin({ signal = null } = {}) {
    if (signal !== null && typeof signal?.aborted !== 'boolean') throw fail('Qualification owner signal is not an AbortSignal', 'owner-signal');
    this.generation += 1;
    this.cancelled = false;
    this.token=Object.freeze({ generation: this.generation, signal });
    return this.token;
  }

  isCurrent(token) {
    return !this.cancelled && token === this.token && token !== null && typeof token === 'object'
      && token.generation === this.generation && token.signal?.aborted !== true;
  }

  assertCurrent(token) {
    if (!this.isCurrent(token)) throw new DOMException('Owned qualification owner cancelled', 'AbortError');
  }

  cancel() {
    this.cancelled = true;
    this.generation += 1;
  }
}

// Main entry: both strict observations -> qualification snapshot.
export async function evaluateOwnedQualification({ record, name, owner = null, token = null } = {}) {
  if (owner !== null) owner.assertCurrent(token);
  const observed = await validateObservedRecord(record);
  if (owner !== null) owner.assertCurrent(token);
  const totals = routeClearTotals(observed.profile);
  const identity = independentNameIdentity({ profile: observed.profile, name });
  if (owner !== null) owner.assertCurrent(token);

  const qualified = totals.filter(row => row.qualifiedTotal > 0);
  const missingRoutes = totals.filter(row => row.qualifiedTotal === 0).map(row => row.route);
  const mask = qualified.reduce((sum, row) => sum | row.bit, 0);
  const complete = missingRoutes.length === 0;
  if (complete && mask !== ALL_ROUTE_BITS) throw fail('Derived EX mask escaped the six original route bits', 'mask-inconsistent');
  if (!complete && mask === ALL_ROUTE_BITS) throw fail('Six-route mask cannot be complete while routes are missing', 'mask-inconsistent');

  const rankTotals = QUALIFY_RANKS.map(rank => totals.reduce((sum, row) => sum + row.counts[rank], 0));
  return Object.freeze({
    schema: EX_QUALIFICATION_SCHEMA,
    proofAuthenticity: 'observed-owned-native-record',
    originalEvidence: ORIGINAL_UNLOCK_EVIDENCE,
    record: Object.freeze({ revision: observed.revision, sha256: observed.sha256, runId: observed.runId }),
    name: identity,
    routeTotals: totals,
    rankTotals: Object.freeze(rankTotals),
    qualifiedRoutes: Object.freeze(qualified.map(row => row.route)),
    missingRoutes: Object.freeze(missingRoutes),
    mask,
    complete,
    eligible: complete,
    continuePolicy: Object.freeze({ gated: false, note: 'the scoped original docs state no continue condition; continues are recorded, never gated' }),
    ranksAccepted: Object.freeze({ min: 0, max: 3, rank4: 'ignored (EX itself is never an ordinary clear)' }),
    cancellation: Object.freeze({ generation: owner === null ? null : token?.generation ?? null, signalAborted: token?.signal?.aborted === true }),
  });
}

export function assertQualificationEligible(qualification) {
  if (qualification?.schema !== EX_QUALIFICATION_SCHEMA || qualification.eligible !== true) {
    throw fail('EX six-route qualification is incomplete', 'qualification-incomplete');
  }
  return qualification;
}

// The only mask value this module will ever hand out; the six bits are the source of truth.
export function derivedExtraSaveMask(qualification) {
  assertQualificationEligible(qualification);
  return qualification.mask;
}

// Proposed integration with module._th128_title_extra_save(mask) after observe.
export function proposeOriginalExtraSave(qualification) {
  if (!qualification || qualification.schema !== EX_QUALIFICATION_SCHEMA) return Object.freeze({ eligible: false, refusal: 'qualification-missing' });
  if (qualification.eligible !== true) return Object.freeze({ eligible: false, refusal: 'six-native-route-clear-counters-incomplete', mask: qualification.mask, missingRoutes: qualification.missingRoutes });
  return Object.freeze({
    eligible: true,
    wouldCall: '_th128_title_extra_save',
    callArguments: Object.freeze([qualification.mask]),
    source: 'owned-native-six-route-profile',
    debugSemaphore: false,
    note: 'mask only from the six observed route counters; never a literal and never setDebugExtraSaveEligibility()',
  });
}

// readCurrentExtraAccess()-shaped access object for the route-select / EX panel.
export function accessFromQualification(qualification) {
  const base = { schema: 'th128-extra-access/1', provider: 'owned-native-six-route-profile', runtime: 'unavailable' };
  if (!qualification || qualification.schema !== EX_QUALIFICATION_SCHEMA) {
    return Object.freeze({ ...base, eligibility: 'locked', canStart: false, mask: 0, missingRoutes: ORIGINAL_ROUTE_ORDER, blockers: ['owned-native-record-not-observed', 'six-normal-routes-required', 'extra-runtime-not-accepted'], debug: Object.freeze({ enabled: false, tainted: true, affectsNormalClear: false, reason: 'debug-extra-runtime-not-accepted' }) });
  }
  if (qualification.eligible !== true) {
    return Object.freeze({ ...base, eligibility: 'locked', canStart: false, mask: qualification.mask, missingRoutes: qualification.missingRoutes, blockers: ['six-native-route-clear-counters-incomplete', 'extra-runtime-not-accepted'], debug: Object.freeze({ enabled: false, tainted: true, affectsNormalClear: false, reason: 'debug-extra-runtime-not-accepted' }) });
  }
  return Object.freeze({
    ...base,
    eligibility: 'owned-unlocked',
    canStart: false,
    mask: qualification.mask,
    missingRoutes: Object.freeze([]),
    blockers: ['extra-runtime-not-accepted'],
    debug: Object.freeze({ enabled: false, tainted: true, affectsNormalClear: false, reason: 'debug-extra-runtime-not-accepted' }),
  });
}

export function describeQualification(qualification) {
  if (!qualification || qualification.schema !== EX_QUALIFICATION_SCHEMA) return '未观察到 owned 原版存档；六线资格无法判定。';
  const totals = qualification.routeTotals.map(row => `${row.route}${row.counts.join('/')}`).join(' ');
  if (qualification.eligible !== true) return `原生六线未完成：${qualification.qualifiedRoutes.length}/6；缺 ${qualification.missingRoutes.join('、')}。${totals}`;
  return `原生六线已完成：6/6（任意难度，含 rank 0）；mask=0x${qualification.mask.toString(16)}。${totals}`;
}
