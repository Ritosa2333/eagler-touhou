function ticksFromRows(rows, digestCanonicalJson) {
  return rows.map(row => ({
    replaySampleIndex: row.frame,
    clockDisposition: 'advanced',
    appliedInput: row.keys,
    clocks: {replayFrame: row.frame, stageFrames: row.stageFrames, sectionFrames: row.sectionFrames},
    scalars: {stage: row.stage, flags: row.flags, score: row.score, power: row.power, itemValue: row.itemValue, lives: row.lives, rank: row.rank, rngSeed: row.rng, rngCalls: row.rngCalls, playerState: row.playerState, sessionFlags: row.sessionFlags},
    categories: {
      rng: digestCanonicalJson([row.rng, row.rngCalls]),
      player: digestCanonicalJson([row.player, row.playerState]),
      economy: digestCanonicalJson([row.score, row.power, row.itemValue, row.lives, row.rank, row.flags]),
    },
  }));
}

function contiguousStages(rows) {
  const groups = [];
  for (const row of rows) {
    const previous = groups.at(-1);
    if (!previous || previous.stage !== row.stage) groups.push({stage: row.stage, rows: [row]});
    else previous.rows.push(row);
  }
  return groups;
}

export function recordsFromRows(fixture, rows, provider, {digestCanonicalJson, recordsFromSegments}) {
  const ticks = ticksFromRows(rows, digestCanonicalJson);
  return recordsFromSegments({
    comparisonIdentity: {
      game: 'th10', profile: 'jp-1.00a/title-demo-present-v1', replaySha256: fixture.replaySha256,
      executableSha256: '2f14760b6fbbf57549541583283badb9a19a4222b90f0a146d5aa17f01dc9040',
      resourceSha256: '1fb1d0ffe34115f563f5feb43755c0feee2315b0ac2b32e2f9e84c81e9433bea',
      stateSchema: 'th10/title-demo-state/v1', traceCodec: 'in-memory/v1', digestAlgorithm: 'sha256-truncated-128/canonical-json-v1',
    },
    coverage: {requiredCategories: ['rng', 'player', 'economy'], optionalCategories: []},
    provenance: {provider}, runReason: 'demo-complete',
    segments: [{segmentId: `stage-${rows[0]?.stage}#0`, route: `stage-${rows[0]?.stage}`, ticks, reason: 'demo-returned'}],
  });
}

export function recordsFromLongRows({fixture, rows, provider, identity, complete}, {digestCanonicalJson, recordsFromSegments}) {
  // The retail executable keeps presenting while a stage session is being
  // torn down/created. Those rows remain in the raw capture, but are not
  // completed gameplay ticks. Both providers expose the same nonzero session
  // lifecycle flag, so exclude by semantics rather than by count or offset.
  const groups = contiguousStages(rows.filter(row => row.sessionFlags === 0 && row.stageFrames > 0));
  return recordsFromSegments({
    comparisonIdentity: {
      game: 'th10', profile: 'jp-1.00a/ordinary-replay-v1', replaySha256: fixture.replaySha256,
      executableSha256: identity.executableSha256, resourceSha256: identity.resourceSha256,
      stateSchema: 'th10/ordinary-replay-state/v1', traceCodec: 'jsonl/v1',
      digestAlgorithm: 'sha256-truncated-128/canonical-json-v1',
    },
    coverage: {requiredCategories: ['rng', 'player', 'economy'], optionalCategories: []},
    provenance: {provider}, runReason: complete ? 'replay-complete' : 'capture-incomplete',
    segments: groups.map((group, index) => ({
      segmentId: `stage-${group.stage}#${index}`, route: `stage-${group.stage}`,
      ticks: ticksFromRows(group.rows, digestCanonicalJson),
      reason: index < groups.length - 1 || complete ? 'stage-observed' : 'capture-incomplete',
      complete: index < groups.length - 1 || complete,
    })),
  });
}

export async function* recordsFromLongJsonLines({fixture, rows, provider, identity, complete, limit = null}, {digestCanonicalJson}) {
  yield {
    type: 'run-start', schema: 'eagler/replay-trace/v1',
    comparisonIdentity: {
      game: 'th10', profile: 'jp-1.00a/ordinary-replay-v1', replaySha256: fixture.replaySha256,
      executableSha256: identity.executableSha256, resourceSha256: identity.resourceSha256,
      stateSchema: 'th10/ordinary-replay-state/v1', traceCodec: 'jsonl/v1',
      digestAlgorithm: 'sha256-truncated-128/canonical-json-v1',
    },
    coverage: {requiredCategories: ['rng', 'player', 'economy'], optionalCategories: []}, provenance: {provider},
  };
  let sequence = 0, logicalTick = 0, tickCount = 0, segmentCount = 0, stage = null, segmentId = null;
  for await (const row of rows) {
    if (row.sessionFlags !== 0 || row.stageFrames <= 0) continue;
    if (limit !== null && tickCount >= limit) break;
    if (stage !== row.stage) {
      if (segmentId !== null) yield {type: 'segment-end', sequence: sequence++, segmentId, logicalTick, reason: 'stage-observed', complete: true};
      stage = row.stage; logicalTick = 0; segmentId = `stage-${stage}#${segmentCount++}`;
      yield {type: 'segment-start', sequence: sequence++, segmentId, route: `stage-${stage}`};
    }
    yield {type: 'tick', sequence: sequence++, segmentId, logicalTick: logicalTick++, ...ticksFromRows([row], digestCanonicalJson)[0]};
    tickCount++;
  }
  if (segmentId === null) throw Error('Long Replay trace contains no ticks');
  yield {type: 'segment-end', sequence: sequence++, segmentId, logicalTick, reason: complete ? 'stage-observed' : 'capture-incomplete', complete};
  yield {type: 'run-end', sequence, reason: complete ? 'replay-complete' : 'capture-incomplete', complete, summary: {segments: segmentCount, ticks: tickCount}};
}
