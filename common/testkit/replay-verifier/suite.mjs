import {createHash} from 'node:crypto';
import {compareTraces} from './compare.mjs';
import {RESULT_STATUS} from './contracts.mjs';
import {validateAdapter} from './contracts.mjs';

const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const nonEmpty = value => typeof value === 'string' && value.length > 0;

function session(value, side, caseId) {
  if (!value || typeof value !== 'object' || !value.records ||
      !(value.records[Symbol.asyncIterator] || value.records[Symbol.iterator]) ||
      typeof value.close !== 'function')
    throw new TypeError(`${caseId}: ${side} provider returned an invalid trace session`);
  return value;
}

function normalizeCase(item) {
  if (!item || typeof item !== 'object' || !nonEmpty(item.id))
    throw new TypeError('Replay suite cases need a non-empty id');
  if (item.status !== undefined && item.status !== 'ready')
    return {id: item.id, status: item.status === 'incompatible' ? RESULT_STATUS.INCOMPATIBLE : RESULT_STATUS.INCOMPLETE,
      reason: item.reason ?? `Fixture status is ${item.status}.`};
  if (!/^[a-f0-9]{64}$/.test(item.replaySha256 ?? ''))
    return {id: item.id, status: RESULT_STATUS.INCOMPLETE, reason: 'Fixture has no verified Replay SHA-256.'};
  if (!(item.replayBytes instanceof Uint8Array) || item.replayBytes.byteLength === 0)
    return {id: item.id, status: RESULT_STATUS.INCOMPLETE, reason: item.reason ?? 'Verified Replay bytes are unavailable.'};
  if (digest(item.replayBytes) !== item.replaySha256)
    return {id: item.id, status: RESULT_STATUS.INCOMPATIBLE, reason: 'Replay bytes do not match the fixture SHA-256.'};
  return null;
}

function aggregate(results) {
  if (results.length && results.every(result => result.status === RESULT_STATUS.PASS)) return RESULT_STATUS.PASS;
  if (results.some(result => result.status === RESULT_STATUS.ERROR)) return RESULT_STATUS.ERROR;
  if (results.some(result => result.status === RESULT_STATUS.INCOMPATIBLE)) return RESULT_STATUS.INCOMPATIBLE;
  if (results.some(result => result.status === RESULT_STATUS.DIVERGED)) return RESULT_STATUS.DIVERGED;
  return RESULT_STATUS.INCOMPLETE;
}

/** Run a title-owned Replay lane through shared identity, lifecycle and tick comparison. */
export async function runReplaySuite({adapter, cases, expectedIds, requiredCategories = [], signal} = {}) {
  const description = validateAdapter(adapter);
  if (description.features.original !== true)
    throw new TypeError('Replay comparison adapters must provide an immutable original trace');
  if (!Array.isArray(cases) || !Array.isArray(expectedIds) || !expectedIds.length)
    throw new TypeError('Replay suite needs cases and the lane’s expected fixture ids');
  if (new Set(expectedIds).size !== expectedIds.length || expectedIds.some(id => !nonEmpty(id)))
    throw new TypeError('Expected Replay fixture ids must be unique non-empty strings');
  const byId = new Map();
  for (const item of cases) {
    if (!item || !nonEmpty(item.id) || byId.has(item.id)) throw new TypeError('Replay suite cases must have unique ids');
    byId.set(item.id, item);
  }
  const extra = [...byId.keys()].filter(id => !expectedIds.includes(id));
  if (extra.length) throw new TypeError(`Replay suite has fixtures outside the selected lane: ${extra.join(', ')}`);

  const results = [];
  for (const id of expectedIds) {
    const item = byId.get(id);
    if (!item) {
      results.push({id, status: RESULT_STATUS.INCOMPLETE, reason: 'Selected Replay fixture has no case record.'});
      continue;
    }
    const invalid = normalizeCase(item);
    if (invalid) { results.push(invalid); continue; }
    if (signal?.aborted) {
      results.push({id, status: RESULT_STATUS.INCOMPLETE, reason: 'Replay suite was cancelled before this fixture ran.'});
      continue;
    }

    let original = null, candidate = null;
    try {
      const replayBytes = Uint8Array.from(item.replayBytes);
      const inspection = await adapter.inspectReplay(replayBytes, {signal, fixture: item});
      if (!inspection || typeof inspection !== 'object' || Array.isArray(inspection))
        throw new TypeError('Title Replay inspection must return an object');
      const spec = {fixture: item, replayBytes, inspection};
      if (typeof adapter.validateCandidate === 'function') {
        const validation = await adapter.validateCandidate(spec, {signal});
        if (validation !== undefined && validation !== null) {
          if (!validation || typeof validation !== 'object' || ![
            RESULT_STATUS.DIVERGED, RESULT_STATUS.INCOMPATIBLE, RESULT_STATUS.INCOMPLETE,
          ].includes(validation.status)) throw new TypeError('Candidate validation must return a non-passing result or null');
          results.push({...validation, id, replaySha256: item.replaySha256, inspection});
          continue;
        }
      }
      original = session(await adapter.openOriginal(spec, {signal}), 'original', id);
      candidate = session(await adapter.openCandidate(spec, {signal}), 'candidate', id);
      const comparison = await compareTraces(original.records, candidate.records, {requiredCategories});
      results.push({id, replaySha256: item.replaySha256, status: comparison.status, comparison,
        inspection, originalProvenance: original.provenance ?? null, candidateProvenance: candidate.provenance ?? null});
    } catch (error) {
      results.push({id, replaySha256: item.replaySha256, status: RESULT_STATUS.ERROR,
        reason: 'adapter-error', message: String(error?.stack ?? error)});
    } finally {
      const closed = await Promise.allSettled([original, candidate].filter(Boolean).map(async value => value.close()));
      const closeErrors = closed.filter(result => result.status === 'rejected').map(result => String(result.reason?.stack ?? result.reason));
      if (closeErrors.length) {
        const previous = results.at(-1);
        if (previous?.id === id) Object.assign(previous, {status: RESULT_STATUS.ERROR, closeErrors});
      }
    }
  }
  return {schema: 'eagler/replay-suite-result/v1', game: description.game,
    adapterVersion: description.adapterVersion, expectedIds: [...expectedIds], status: aggregate(results),
    complete: results.length === expectedIds.length && results.every(result => result.status === RESULT_STATUS.PASS), results};
}
