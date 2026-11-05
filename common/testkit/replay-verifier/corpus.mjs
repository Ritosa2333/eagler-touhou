const statuses = new Set(['ready', 'missing-replay', 'missing-original-provider', 'missing-candidate-provider', 'coverage-incomplete']);

export function validateCorpus(corpus) {
  if (!corpus || corpus.schema !== 'eagler/replay-corpus/v1') throw new Error('Unsupported Replay corpus schema');
  if (!/^th\d{2}$/.test(corpus.game ?? '')) throw new Error('Invalid corpus game id');
  if (corpus.policy?.gameplayMutation !== 'forbidden') throw new Error('Corpus must forbid gameplay mutation');
  const demo = corpus.demo;
  if (!demo || !Array.isArray(demo.rotationOrder) || !demo.rotationOrder.length) throw new Error('Demo rotation order is required');
  if (new Set(demo.rotationOrder).size !== demo.rotationOrder.length) throw new Error('Duplicate Demo in rotation order');
  if (!Array.isArray(corpus.cases) || !corpus.cases.length) throw new Error('Replay cases are required');
  const ids = new Set();
  for (const entry of corpus.cases) {
    if (!entry?.id || ids.has(entry.id)) throw new Error('Replay case ids must be unique');
    ids.add(entry.id);
    if (!statuses.has(entry.status)) throw new Error(`Invalid Replay case status for ${entry.id}`);
  }
  const demos = corpus.cases.filter(entry => entry.kind === 'demo').map(entry => entry.source);
  if (JSON.stringify(demos) !== JSON.stringify(demo.rotationOrder))
    throw new Error('Demo cases must preserve the game-owned rotation order exactly');
  return corpus;
}

export function corpusReadiness(corpus) {
  validateCorpus(corpus);
  const missing = corpus.cases.filter(entry => entry.status !== 'ready');
  return {game: corpus.game, ready: missing.length === 0, total: corpus.cases.length, available: corpus.cases.length - missing.length, missing};
}
