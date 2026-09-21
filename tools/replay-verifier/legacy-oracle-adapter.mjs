import {readFile} from 'node:fs/promises';
import {join} from 'node:path';

const observableNumbers = [
  ...Array.from({length: 72}, (_, index) => index).filter(index => ![25, 26, 27, 42, 43].includes(index)),
  ...[0x64, 0x68, 0x74, 0x80, 0x84, 0x88, 0x98].flatMap(offset => [offset, offset + 1, offset + 2, offset + 3]),
];

async function json(path) { return JSON.parse(await readFile(path, 'utf8')); }

function filterOriginal(rows, bounds) {
  return rows.filter(row => {
    const limit = bounds.find(value => value.stage === row.stage);
    return (row.flags & 4) !== 0 && limit && row.frame <= Math.min(limit.inputFrames, limit.timingFrames);
  });
}

function normalizedNumbers(row) { return observableNumbers.map(index => row.numbers[index]); }

function comparableFlags(row) {
  // 0x800 records the Stage 4B route selected by the Stage 3B dialogue.  The
  // retail player carries it into later stages, while imported Replay stages
  // are selected directly from the Replay stage table.  Once the observed
  // stage is part of the trace, this bit is route provenance rather than
  // simulation state.  Keep every other game flag authoritative.
  return row.flags & ~0x800;
}

function ticksFromRows(rows, digestCanonicalJson) {
  return rows.map(row => { const flags = comparableFlags(row); return ({
    replaySampleIndex: row.frame, clockDisposition: 'advanced', appliedInput: row.keys,
    clocks: {replayFrame: row.frame, gameFrames: row.gameFrames},
    scalars: {stage: row.stage, flags, rngSeed: row.rng, rngCalls: row.rngCalls, playerState: row.playerState},
    categories: {
      rng: digestCanonicalJson([row.rng, row.rngCalls]),
      player: digestCanonicalJson([row.player, row.playerState]),
      world: digestCanonicalJson([flags, normalizedNumbers(row)]),
    },
  }); });
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

function gameplayRows(rows) {
  let stage = null, gameFrames = null;
  return rows.filter(row => {
    const advanced = row.stage !== stage || row.gameFrames !== gameFrames;
    stage = row.stage; gameFrames = row.gameFrames;
    return advanced;
  });
}

function traceIdentity(fixture) {
  return {
      game: 'th08', profile: 'jp-1.00d/historical-oracle-v1', replaySha256: fixture.replaySha256,
      executableSha256: '330fbdbf58a710829d65277b4f312cfbb38d5448b3df523e79350b879213d924',
      resourceSha256: '9d7edf43b8ddd347cbb641836f6b5050745dd936f688daebbf9382ca557043bb',
      stateSchema: 'th08/legacy-replay-oracle-row/v1', traceCodec: 'in-memory/v1',
      digestAlgorithm: 'sha256-truncated-128/legacy-json-v1',
  };
}

export function recordsFromRows(fixture, rows, provider, {digestCanonicalJson, recordsFromSegments}) {
  const stage = rows[0]?.stage, ticks = ticksFromRows(rows, digestCanonicalJson);
  return recordsFromSegments({
    comparisonIdentity: traceIdentity(fixture),
    coverage: {requiredCategories: ['rng', 'player', 'world'], optionalCategories: []},
    provenance: {provider}, runReason: 'demo-complete',
    segments: [{segmentId: `stage-${stage}#0`, route: `stage-${stage}`, ticks, reason: 'demo-returned'}],
  });
}

export function recordsFromLongRows({fixture, rows, provider, identity, complete}, {digestCanonicalJson, recordsFromSegments}) {
  // Present cadence is intentionally not part of the logic oracle.  Retail
  // can Present several times while the gameplay clock is frozen (loading,
  // dialogue/phase transitions and the return to title), whereas the browser
  // driver advances those application frames one at a time.  RNG consumed in
  // a frozen interval is still checked at the next advancing gameplay tick.
  const groups = contiguousStages(gameplayRows(rows));
  return recordsFromSegments({
    comparisonIdentity: {
      game: 'th08', profile: 'jp-1.00d/ordinary-replay-v1', replaySha256: fixture.replaySha256,
      executableSha256: identity.executableSha256, resourceSha256: identity.resourceSha256,
      stateSchema: 'th08/ordinary-replay-state/v1', traceCodec: 'jsonl/v1',
      digestAlgorithm: 'sha256-truncated-128/legacy-json-v1',
    },
    coverage: {requiredCategories: ['rng', 'player', 'world'], optionalCategories: []},
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
      game: 'th08', profile: 'jp-1.00d/ordinary-replay-v1', replaySha256: fixture.replaySha256,
      executableSha256: identity.executableSha256, resourceSha256: identity.resourceSha256,
      stateSchema: 'th08/ordinary-replay-state/v1', traceCodec: 'jsonl/v1',
      digestAlgorithm: 'sha256-truncated-128/legacy-json-v1',
    },
    coverage: {requiredCategories: ['rng', 'player', 'world'], optionalCategories: []}, provenance: {provider},
  };
  let sequence = 0, logicalTick = 0, tickCount = 0, segmentCount = 0, stage = null, segmentId = null;
  let previousRawStage = null, previousGameFrames = null;
  for await (const row of rows) {
    const advanced = row.stage !== previousRawStage || row.gameFrames !== previousGameFrames;
    previousRawStage = row.stage; previousGameFrames = row.gameFrames;
    if (!advanced) continue;
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

export async function loadLegacyOraclePair(oracleRoot, fixture, commonAdapter) {
  const source = await loadLegacyOriginal(oracleRoot, fixture);
  const directory = join(oracleRoot, 'artifacts', 'cpp', 'oracle');
  const candidateArtifact = await json(join(directory, `cpp-${fixture.id}.json`));
  if (candidateArtifact.error)
    throw new Error(`${fixture.id}: historical artifact contains an execution error`);
  const candidateRows = candidateArtifact.rows;
  if (candidateRows.length !== fixture.expectedTicks)
    throw new Error(`${fixture.id}: expected ${fixture.expectedTicks} candidate ticks, got ${candidateRows.length}`);
  return {
    expected: recordsFromRows(fixture, source.rows, 'th08-original-jit/historical-artifact', commonAdapter),
    actual: recordsFromRows(fixture, candidateRows, 'th08-cpp-reconstruction/historical-artifact', commonAdapter),
  };
}

export async function loadLegacyOriginal(oracleRoot, fixture) {
  const directory = join(oracleRoot, 'artifacts', 'cpp', 'oracle');
  const comparison = await json(join(directory, `comparison-${fixture.id}.json`));
  if (!comparison.passed || comparison.differences?.length)
    throw new Error(`${fixture.id}: historical comparison is not a passing source artifact`);
  const artifact = await json(join(directory, `${fixture.id}.json`));
  if (artifact.error) throw new Error(`${fixture.id}: historical original artifact contains an execution error`);
  const rows = filterOriginal(artifact.rows, comparison.recordingBounds);
  if (rows.length !== fixture.expectedTicks)
    throw new Error(`${fixture.id}: expected ${fixture.expectedTicks} original ticks, got ${rows.length}`);
  return {rows, recordingBounds: comparison.recordingBounds};
}
