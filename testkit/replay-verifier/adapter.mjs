import {createHash} from 'node:crypto';

export function digestCanonicalJson(value) {
  return createHash('sha256').update(JSON.stringify(value)).digest('hex').slice(0, 32);
}

export function* recordsFromSegments({comparisonIdentity, coverage, provenance, segments, runReason = 'replay-complete'}) {
  yield {type: 'run-start', schema: 'eagler/replay-trace/v1', comparisonIdentity, coverage, provenance};
  let sequence = 0, tickCount = 0;
  for (const [segmentIndex, segment] of segments.entries()) {
    const segmentId = segment.segmentId ?? `${segment.route}#${segmentIndex}`;
    yield {type: 'segment-start', sequence: sequence++, segmentId, route: segment.route};
    for (const [logicalTick, tick] of segment.ticks.entries()) {
      yield {type: 'tick', sequence: sequence++, segmentId, logicalTick, ...tick};
      tickCount++;
    }
    yield {
      type: 'segment-end', sequence: sequence++, segmentId, logicalTick: segment.ticks.length,
      reason: segment.reason ?? 'segment-complete', complete: segment.complete !== false,
    };
  }
  const complete = segments.every(segment => segment.complete !== false);
  yield {
    type: 'run-end', sequence, reason: runReason, complete,
    summary: {segments: segments.length, ticks: tickCount},
  };
}
