// Pure gate protocol; legacy baselines require an explicit audited migration.
export function parseComparison(run, expected) {
  if (run.error || run.signal || ![0, 1].includes(run.status)) throw Error('Comparator process failed or diagnostic result');
  const lines = (run.stdout || '').trim().split(/\r?\n/);
  const legacy = lines.filter(x => x.startsWith('RESULT '));
  const structured = lines.filter(x => x.startsWith('RESULT_JSON '));
  if (legacy.length !== 1 || structured.length !== 1) throw Error('Expected unique RESULT and RESULT_JSON');
  const m = /^RESULT first_divergence_frame=(none|\d+) window=(\d+) ops=(\d+)$/.exec(legacy[0]);
  if (!m || /ignored=/.test(run.stdout)) throw Error('Invalid or filtered RESULT');
  const value = JSON.parse(structured[0].slice('RESULT_JSON '.length));
  const first = m[1] === 'none' ? null : Number(m[1]);
  if (!Number.isSafeInteger(expected) || expected <= 0 || value.schema !== 'th13/compare/v1' || value.kind !== 'ops' ||
      Number(m[2]) !== expected || value.window !== expected || value.expected_frames !== expected ||
      !Number.isSafeInteger(Number(m[3])) || Number(m[3]) < 0 || value.ops !== Number(m[3]) ||
      value.first_divergence_frame !== first || value.exit_code !== run.status ||
      !Array.isArray(value.ignored) || value.ignored.length || value.ulp !== 0 ||
      value.error !== null || value.qualified !== (run.status === 0) ||
      (first === null) !== (run.status === 0) ||
      (first !== null && (!Number.isSafeInteger(first) || first < 0 || first >= expected))) {
    throw Error('Comparator protocol/window mismatch');
  }
  return {first: m[1], window: expected, ops: Number(m[3]), line: legacy[0]};
}

export function checkIdentity(prior, fixture, identity) {
  const hash = value => typeof value === 'string' && /^[a-f0-9]{64}$/.test(value);
  if (!hash(identity.corpusSha256) || !hash(identity.oracleSha256) ||
      (fixture.kind === 'replay' && !hash(fixture.replaySha256)) ||
      (fixture.replaySha256 != null && !hash(fixture.replaySha256))) {
    throw Error('Invalid/missing corpus/oracle/replay identity');
  }
  if (!prior) return false;
  if (!prior.identity || prior.identity.corpusSha256 !== identity.corpusSha256 ||
      prior.identity.oracleSha256 !== identity.oracleSha256 ||
      prior.identity.caseId !== fixture.id ||
      prior.identity.replaySha256 !== (fixture.replaySha256 ?? null)) {
    throw Error('Golden corpus identity missing/mismatched: audited migration required (no automatic input replacement)');
  }
  return true;
}

export function classify(verdict, prior) {
  if (!prior) return 'UNBASELINED';
  if (verdict.first === 'none') return 'EXACT_MATCH';
  if (prior.first_divergence_frame === 'none') return 'REGRESSION';
  if (!/^\d+$/.test(String(prior.first_divergence_frame))) throw Error('Invalid baseline frontier');
  return Number(verdict.first) >= Number(prior.first_divergence_frame) ? 'BASELINE_HELD' : 'REGRESSION';
}
