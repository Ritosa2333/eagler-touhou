export function attribute(result, expectedFrames = [], actualFrames = []) {
  const hint = hintFor(result, expectedFrames, actualFrames);
  const tick = result.comparedTicks | 0;
  const fieldDiff = result.status === 'DIVERGED' ? firstFieldDiff(expectedFrames[tick], actualFrames[tick], result.firstDifference?.category) : null;
  return {...result, attributionHint: hint, fieldDiff};
}

function hintFor(result, expectedFrames, actualFrames) {
  if (result.status === 'PASS') return 'match';
  if (result.reason === 'category-digest' && result.firstDifference?.category === 'rng') {
    const tick = result.comparedTicks | 0;
    const left = expectedFrames[tick]?.categories?.rng;
    const right = actualFrames[tick]?.categories?.rng;
    if (left && right && left.shape && right.shape && left.shape !== right.shape) return 'rng-runtime';
    return 'web';
  }
  if (result.status === 'DIVERGED' && result.firstDifference?.category && result.firstDifference.category !== 'rng') return 'web';
  if (result.status === 'DIVERGED') return 'web';
  if (result.reason === 'comparison-identity') return 'capture';
  if (['sequence-discontinuity', 'early-eof', 'logical-tick-discontinuity', 'missing-run-end', 'run-not-complete', 'segment-not-complete', 'invalid-run-end-lifecycle'].includes(result.reason)) return 'capture';
  if (result.status === 'INCOMPLETE' || result.status === 'ERROR' || result.status === 'INCOMPATIBLE') return 'capture';
  return 'capture';
}

function firstFieldDiff(expected, actual, category) {
  if (!expected || !actual) return null;
  const left = category ? expected.categories?.[category] : expected.categories || expected;
  const right = category ? actual.categories?.[category] : actual.categories || actual;
  return diff(left, right, category || '');
}

function diff(left, right, path) {
  if (Object.is(left, right)) return null;
  if (Array.isArray(left) && Array.isArray(right)) {
    if (left.length !== right.length) return {path: `${path}.length`, expected: left.length, actual: right.length};
    for (let i = 0; i < left.length; i++) {
      const found = diff(left[i], right[i], `${path}[${i}]`);
      if (found) return found;
    }
    return null;
  }
  if (left && right && typeof left === 'object' && typeof right === 'object') {
    for (const key of [...new Set([...Object.keys(left), ...Object.keys(right)])].sort()) {
      if (!Object.hasOwn(left, key) || !Object.hasOwn(right, key)) return {path: path ? `${path}.${key}` : key, expected: left[key], actual: right[key]};
      const found = diff(left[key], right[key], path ? `${path}.${key}` : key);
      if (found) return found;
    }
    return null;
  }
  return {path, expected: left, actual: right};
}
