import {RESULT_STATUS} from './contracts.mjs';

export function exitCodeForResult(result) { return result.status === RESULT_STATUS.PASS ? 0 : 1; }

export function formatResult(result) {
  const lines = [`Replay verifier: ${result.status}`, `reason: ${result.reason}`];
  if (Number.isSafeInteger(result.comparedTicks)) lines.push(`compared ticks: ${result.comparedTicks}`);
  if (result.firstDifference) {
    const at = [result.firstDifference.segmentId, result.firstDifference.logicalTick].filter(value => value !== undefined).join(':');
    if (at) lines.push(`first difference: ${at}`);
    if (result.firstDifference.category) lines.push(`category: ${result.firstDifference.category}`);
    const detail = result.firstDifference.difference;
    if (detail?.path) lines.push(`field: ${detail.path}`, `expected: ${JSON.stringify(detail.expected)}`, `actual: ${JSON.stringify(detail.actual)}`);
  }
  if (result.message) lines.push(`error: ${result.message}`);
  return `${lines.join('\n')}\n`;
}
