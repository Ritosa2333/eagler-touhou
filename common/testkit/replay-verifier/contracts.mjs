export const TRACE_SCHEMA = 'eagler/replay-trace/v1';
export const RESULT_SCHEMA = 'eagler/replay-result/v1';

export const RESULT_STATUS = Object.freeze({
  PASS: 'PASS',
  DIVERGED: 'DIVERGED',
  INCOMPLETE: 'INCOMPLETE',
  INCOMPATIBLE: 'INCOMPATIBLE',
  ERROR: 'ERROR',
});

export const RECORD_TYPE = Object.freeze({
  RUN_START: 'run-start',
  SEGMENT_START: 'segment-start',
  TICK: 'tick',
  SEGMENT_END: 'segment-end',
  RUN_END: 'run-end',
});

const recordTypes = new Set(Object.values(RECORD_TYPE));
const digestPattern = /^[0-9a-f]{32}$/;

function object(value, label) {
  if (!value || typeof value !== 'object' || Array.isArray(value))
    throw new TypeError(`${label} must be an object`);
  return value;
}

function nonEmptyString(value, label) {
  if (typeof value !== 'string' || !value)
    throw new TypeError(`${label} must be a non-empty string`);
  return value;
}

function nonNegativeInteger(value, label) {
  if (!Number.isSafeInteger(value) || value < 0)
    throw new TypeError(`${label} must be a non-negative safe integer`);
  return value;
}

function stringArray(value, label) {
  if (!Array.isArray(value) || value.some(item => typeof item !== 'string' || !item))
    throw new TypeError(`${label} must be an array of non-empty strings`);
  if (new Set(value).size !== value.length)
    throw new TypeError(`${label} must not contain duplicates`);
  return value;
}

function sha256(value, label) {
  nonEmptyString(value, label);
  if (!/^[0-9a-f]{64}$/.test(value)) throw new TypeError(`${label} must be lowercase SHA-256`);
}

function canonicalJson(value, label) {
  if (value === null || typeof value === 'boolean' || typeof value === 'string') return;
  if (typeof value === 'number') {
    if (!Number.isSafeInteger(value))
      throw new TypeError(`${label} numbers must be safe integers; encode floats and wider integers as canonical strings`);
    return;
  }
  if (Array.isArray(value)) {
    value.forEach((item, index) => canonicalJson(item, `${label}[${index}]`));
    return;
  }
  object(value, label);
  for (const [key, item] of Object.entries(value)) {
    nonEmptyString(key, `${label} key`);
    canonicalJson(item, `${label}.${key}`);
  }
}

export function validateRunStart(record) {
  object(record, 'run-start record');
  if (record.type !== RECORD_TYPE.RUN_START) throw new TypeError('First record must be run-start');
  if (record.schema !== TRACE_SCHEMA) throw new TypeError(`Unsupported Replay trace schema ${record.schema}`);
  object(record.comparisonIdentity, 'run-start comparisonIdentity');
  for (const key of ['game', 'profile', 'stateSchema', 'traceCodec', 'digestAlgorithm'])
    nonEmptyString(record.comparisonIdentity[key], `comparisonIdentity.${key}`);
  for (const key of ['replaySha256', 'executableSha256', 'resourceSha256'])
    sha256(record.comparisonIdentity[key], `comparisonIdentity.${key}`);
  object(record.coverage, 'run-start coverage');
  stringArray(record.coverage.requiredCategories, 'coverage.requiredCategories');
  stringArray(record.coverage.optionalCategories ?? [], 'coverage.optionalCategories');
  object(record.provenance, 'run-start provenance');
  nonEmptyString(record.provenance.provider, 'provenance.provider');
  return record;
}

export function validateRecord(record) {
  object(record, 'Replay trace record');
  if (!recordTypes.has(record.type)) throw new TypeError(`Unknown Replay trace record type ${record.type}`);
  if (record.type === RECORD_TYPE.RUN_START) return validateRunStart(record);
  nonNegativeInteger(record.sequence, `${record.type}.sequence`);
  if (record.type === RECORD_TYPE.RUN_END) {
    nonEmptyString(record.reason, 'run-end.reason');
    if (typeof record.complete !== 'boolean') throw new TypeError('run-end.complete must be boolean');
    return record;
  }
  nonEmptyString(record.segmentId, `${record.type}.segmentId`);
  if (record.type === RECORD_TYPE.SEGMENT_START) return record;
  nonNegativeInteger(record.logicalTick, `${record.type}.logicalTick`);
  if (record.type === RECORD_TYPE.SEGMENT_END) {
    nonEmptyString(record.reason, 'segment-end.reason');
    if (typeof record.complete !== 'boolean') throw new TypeError('segment-end.complete must be boolean');
    return record;
  }
  nonNegativeInteger(record.replaySampleIndex, 'tick.replaySampleIndex');
  nonEmptyString(record.clockDisposition, 'tick.clockDisposition');
  object(record.clocks, 'tick.clocks');
  object(record.scalars, 'tick.scalars');
  canonicalJson(record.appliedInput, 'tick.appliedInput');
  canonicalJson(record.clocks, 'tick.clocks');
  canonicalJson(record.scalars, 'tick.scalars');
  object(record.categories, 'tick.categories');
  for (const [category, digest] of Object.entries(record.categories)) {
    nonEmptyString(category, 'tick category');
    if (typeof digest !== 'string' || !digestPattern.test(digest))
      throw new TypeError(`tick category ${category} must be a lowercase 128-bit hex digest`);
  }
  if (!Object.hasOwn(record, 'appliedInput')) throw new TypeError('tick.appliedInput is required');
  return record;
}

export function validateAdapter(adapter) {
  object(adapter, 'Replay verifier adapter');
  for (const method of ['describe', 'inspectReplay', 'openCandidate'])
    if (typeof adapter[method] !== 'function') throw new TypeError(`Replay verifier adapter is missing ${method}()`);
  const description = object(adapter.describe(), 'Replay verifier adapter description');
  if (description.adapterApiVersion !== 1) throw new TypeError(`Unsupported adapter API ${description.adapterApiVersion}`);
  nonEmptyString(description.game, 'adapter game');
  nonEmptyString(description.adapterVersion, 'adapter version');
  object(description.features, 'adapter features');
  if (description.features.original && typeof adapter.openOriginal !== 'function')
    throw new TypeError('Adapter declares original support but is missing openOriginal()');
  return description;
}
