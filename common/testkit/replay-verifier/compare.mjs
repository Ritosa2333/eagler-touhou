import {RESULT_SCHEMA, RESULT_STATUS, RECORD_TYPE, validateRecord, validateRunStart} from './contracts.mjs';

function stable(value) {
  if (Array.isArray(value)) return value.map(stable);
  if (value && typeof value === 'object')
    return Object.fromEntries(Object.keys(value).sort().map(key => [key, stable(value[key])]));
  return value;
}

function equal(left, right) { return JSON.stringify(stable(left)) === JSON.stringify(stable(right)); }

function firstDifference(expected, actual, path = '') {
  if (Object.is(expected, actual)) return null;
  if (Array.isArray(expected) && Array.isArray(actual)) {
    if (expected.length !== actual.length)
      return {path: `${path}.length`, expected: expected.length, actual: actual.length};
    for (let i = 0; i < expected.length; i++) {
      const difference = firstDifference(expected[i], actual[i], `${path}[${i}]`);
      if (difference) return difference;
    }
    return null;
  }
  if (expected && actual && typeof expected === 'object' && typeof actual === 'object') {
    const keys = [...new Set([...Object.keys(expected), ...Object.keys(actual)])].sort();
    for (const key of keys) {
      if (!Object.hasOwn(expected, key)) return {path: `${path}.${key}`, expected: undefined, actual: actual[key]};
      if (!Object.hasOwn(actual, key)) return {path: `${path}.${key}`, expected: expected[key], actual: undefined};
      const difference = firstDifference(expected[key], actual[key], path ? `${path}.${key}` : key);
      if (difference) return difference;
    }
    return null;
  }
  return {path, expected, actual};
}

function result(status, fields = {}) { return {schema: RESULT_SCHEMA, status, ...fields}; }
function location(record) {
  return record && record.type === RECORD_TYPE.TICK
    ? {sequence: record.sequence, segmentId: record.segmentId, logicalTick: record.logicalTick}
    : {sequence: record?.sequence, recordType: record?.type};
}

function validateSequence(record, expectedSequence, side) {
  if (record.type === RECORD_TYPE.RUN_START) return null;
  if (record.sequence !== expectedSequence)
    return result(RESULT_STATUS.INCOMPLETE, {
      reason: 'sequence-discontinuity', side, expectedSequence, actualSequence: record.sequence,
      firstDifference: location(record),
    });
  return null;
}

function traceState() { return {activeSegment: null, nextTick: 0, segments: 0}; }

function validateProgress(record, state, side) {
  if (record.type === RECORD_TYPE.RUN_START)
    return result(RESULT_STATUS.INCOMPLETE, {reason: 'duplicate-run-start', side});
  if (record.type === RECORD_TYPE.SEGMENT_START) {
    if (state.activeSegment !== null)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'nested-segment', side, firstDifference: location(record)});
    state.activeSegment = record.segmentId; state.nextTick = 0; state.segments++;
  } else if (record.type === RECORD_TYPE.TICK) {
    if (state.activeSegment !== record.segmentId)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'tick-outside-segment', side, firstDifference: location(record)});
    if (record.logicalTick !== state.nextTick)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'logical-tick-discontinuity', side, expectedLogicalTick: state.nextTick, actualLogicalTick: record.logicalTick, firstDifference: location(record)});
    state.nextTick++;
  } else if (record.type === RECORD_TYPE.SEGMENT_END) {
    if (state.activeSegment !== record.segmentId)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'segment-end-without-start', side, firstDifference: location(record)});
    if (record.logicalTick !== state.nextTick)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'segment-end-tick', side, expectedLogicalTick: state.nextTick, actualLogicalTick: record.logicalTick, firstDifference: location(record)});
    state.activeSegment = null;
  } else if (record.type === RECORD_TYPE.RUN_END) {
    if (state.activeSegment !== null || state.segments === 0)
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'invalid-run-end-lifecycle', side, activeSegment: state.activeSegment, segments: state.segments, firstDifference: location(record)});
  }
  return null;
}

function compareTick(expected, actual, requiredCategories) {
  for (const key of ['segmentId', 'logicalTick', 'replaySampleIndex', 'clockDisposition', 'appliedInput', 'clocks', 'scalars']) {
    const difference = firstDifference(expected[key], actual[key], key);
    if (difference) return {reason: 'tick-field', difference};
  }
  for (const category of requiredCategories) {
    if (!Object.hasOwn(expected.categories, category) || !Object.hasOwn(actual.categories, category))
      return {reason: 'missing-category', category, expected: expected.categories[category], actual: actual.categories[category]};
    if (expected.categories[category] !== actual.categories[category])
      return {reason: 'category-digest', category, expected: expected.categories[category], actual: actual.categories[category]};
  }
  return null;
}

async function next(iterator) {
  try { return await iterator.next(); }
  catch (error) { return {error}; }
}

export async function compareTraces(expectedRecords, actualRecords, {requiredCategories} = {}) {
  let expectedIterator, actualIterator;
  try {
    expectedIterator = expectedRecords[Symbol.asyncIterator]?.() ?? expectedRecords[Symbol.iterator]?.();
    actualIterator = actualRecords[Symbol.asyncIterator]?.() ?? actualRecords[Symbol.iterator]?.();
    if (!expectedIterator || !actualIterator) throw new TypeError('Replay traces must be iterable');
  } catch (error) { return result(RESULT_STATUS.ERROR, {reason: 'trace-open-error', message: error.message}); }
  try {
  let expectedCount = 0, actualCount = 0, expectedSequence = 0, actualSequence = 0;
  const expectedState = traceState(), actualState = traceState();
  const optionalDifferences = [];
  const expectedStartResult = await next(expectedIterator), actualStartResult = await next(actualIterator);
  if (expectedStartResult.error || actualStartResult.error)
    return result(RESULT_STATUS.ERROR, {reason: 'trace-read-error', message: String((expectedStartResult.error ?? actualStartResult.error)?.message)});
  if (expectedStartResult.done || actualStartResult.done)
    return result(RESULT_STATUS.INCOMPLETE, {reason: 'missing-run-start', side: expectedStartResult.done ? 'expected' : 'actual'});
  let expectedStart, actualStart;
  try {
    expectedStart = validateRunStart(expectedStartResult.value);
    actualStart = validateRunStart(actualStartResult.value);
  } catch (error) { return result(RESULT_STATUS.ERROR, {reason: 'invalid-run-start', message: error.message}); }
  if (!equal(expectedStart.comparisonIdentity, actualStart.comparisonIdentity))
    return result(RESULT_STATUS.INCOMPATIBLE, {reason: 'comparison-identity', difference: firstDifference(expectedStart.comparisonIdentity, actualStart.comparisonIdentity)});
  const categories = requiredCategories ?? expectedStart.coverage.requiredCategories;
  for (const category of categories) {
    if (!expectedStart.coverage.requiredCategories.includes(category) || !actualStart.coverage.requiredCategories.includes(category))
      return result(RESULT_STATUS.INCOMPATIBLE, {reason: 'coverage', category});
  }

  while (true) {
    const expectedResult = await next(expectedIterator), actualResult = await next(actualIterator);
    if (expectedResult.error || actualResult.error)
      return result(RESULT_STATUS.ERROR, {reason: 'trace-read-error', message: String((expectedResult.error ?? actualResult.error)?.message), comparedTicks: expectedCount});
    if (expectedResult.done || actualResult.done) {
      if (expectedResult.done && actualResult.done)
        return result(RESULT_STATUS.INCOMPLETE, {reason: 'missing-run-end', comparedTicks: expectedCount});
      return result(RESULT_STATUS.INCOMPLETE, {reason: 'early-eof', side: expectedResult.done ? 'expected' : 'actual', comparedTicks: expectedCount});
    }
    let expected, actual;
    try { expected = validateRecord(expectedResult.value); actual = validateRecord(actualResult.value); }
    catch (error) { return result(RESULT_STATUS.ERROR, {reason: 'invalid-record', message: error.message, comparedTicks: expectedCount}); }
    const expectedGap = validateSequence(expected, expectedSequence, 'expected');
    if (expectedGap) return {...expectedGap, comparedTicks: expectedCount};
    const actualGap = validateSequence(actual, actualSequence, 'actual');
    if (actualGap) return {...actualGap, comparedTicks: expectedCount};
    expectedSequence++; actualSequence++;
    const expectedProgress = validateProgress(expected, expectedState, 'expected');
    if (expectedProgress) return {...expectedProgress, comparedTicks: expectedCount};
    const actualProgress = validateProgress(actual, actualState, 'actual');
    if (actualProgress) return {...actualProgress, comparedTicks: expectedCount};
    if (expected.type !== actual.type)
      return result(RESULT_STATUS.DIVERGED, {reason: 'record-type', firstDifference: {...location(expected), expected: expected.type, actual: actual.type}, comparedTicks: expectedCount});
    if (expected.type === RECORD_TYPE.TICK) {
      const difference = compareTick(expected, actual, categories);
      if (difference)
        return result(RESULT_STATUS.DIVERGED, {reason: difference.reason, firstDifference: {...location(expected), ...difference}, comparedTicks: expectedCount});
      for (const category of expectedStart.coverage.optionalCategories) {
        if (expected.categories[category] !== actual.categories[category] && optionalDifferences.length < 32)
          optionalDifferences.push({...location(expected), category, expected: expected.categories[category], actual: actual.categories[category]});
      }
      expectedCount++; actualCount++;
      continue;
    }
    if (expected.type === RECORD_TYPE.RUN_END) {
      if (!expected.complete || !actual.complete)
        return result(RESULT_STATUS.INCOMPLETE, {reason: 'run-not-complete', expected: {complete: expected.complete, reason: expected.reason}, actual: {complete: actual.complete, reason: actual.reason}, comparedTicks: expectedCount});
      const difference = firstDifference({reason: expected.reason, summary: expected.summary}, {reason: actual.reason, summary: actual.summary});
      if (difference)
        return result(RESULT_STATUS.DIVERGED, {reason: 'run-end', firstDifference: {...location(expected), difference}, comparedTicks: expectedCount});
      const expectedTail = await next(expectedIterator), actualTail = await next(actualIterator);
      if (expectedTail.error || actualTail.error)
        return result(RESULT_STATUS.ERROR, {reason: 'trace-read-error', message: String((expectedTail.error ?? actualTail.error)?.message), comparedTicks: expectedCount});
      if (!expectedTail.done || !actualTail.done)
        return result(RESULT_STATUS.INCOMPLETE, {reason: 'records-after-run-end', side: !expectedTail.done ? 'expected' : 'actual', comparedTicks: expectedCount});
      return result(RESULT_STATUS.PASS, {reason: 'traces-equal', comparedTicks: expectedCount, requiredCategories: categories, optionalDifferences});
    }
    if (expected.type === RECORD_TYPE.SEGMENT_END && (!expected.complete || !actual.complete))
      return result(RESULT_STATUS.INCOMPLETE, {
        reason: 'segment-not-complete',
        firstDifference: location(expected),
        expected: {complete: expected.complete, reason: expected.reason},
        actual: {complete: actual.complete, reason: actual.reason},
        comparedTicks: expectedCount,
      });
    const comparableExpected = {...expected}; delete comparableExpected.sequence;
    const comparableActual = {...actual}; delete comparableActual.sequence;
    const difference = firstDifference(comparableExpected, comparableActual);
    if (difference)
      return result(RESULT_STATUS.DIVERGED, {reason: 'lifecycle-record', firstDifference: {...location(expected), difference}, comparedTicks: expectedCount});
  }
  } finally {
    await Promise.allSettled([expectedIterator.return?.(), actualIterator.return?.()]);
  }
}
