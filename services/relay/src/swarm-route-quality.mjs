const MaximumAgeMs = 240_000;
const MaximumStableRtt = 150;
const DegradedEstimatedRtt = 200;

export function readRouteProbeQuality(value, now) {
  if (!value || !Number.isInteger(value.sent) || !Number.isInteger(value.received) ||
      value.sent < 5 || value.sent > 40 || value.received < 0 || value.received > value.sent) return null;
  if (value.received === 0 && value.p95 === null) return {sent:value.sent, received:0, p95:null, at:now};
  if (value.received < 1 || !Number.isFinite(value.p95) || value.p95 < 0 || value.p95 > 10_000) return null;
  return {sent:value.sent, received:value.received, p95:Math.round(value.p95), at:now};
}

function pathQuality(pair) {
  const rtt = Math.max(...pair.map(item => item.p95 ?? Infinity));
  const loss = Math.max(...pair.map(item => (item.sent - item.received) / item.sent));
  const usable = Number.isFinite(rtt) && loss <= 0.05;
  return {score:rtt + loss * 1000, usable, suitable:usable && rtt <= MaximumStableRtt};
}

function pairedQuality(route, left, right, now) {
  const pair = [left?.get(route),right?.get(route)];
  return pair.some(item => !item || now - item.at > MaximumAgeMs) ? null : pathQuality(pair);
}

export function rankSwarmCandidates(workers, left, right, now, endpointReports = []) {
  return workers.map(({id, ip}) => {
    const endpoints = endpointReports.flatMap(report => {
      const sample = report?.samples.get(id);
      return sample && sample.ip === ip && now - report.at <= MaximumAgeMs ? [sample.rtt] : [];
    });
    return {
      worker:id,
      quality:pairedQuality('swarm-' + id,left,right,now),
      measuredEndpoints:endpoints.length,
      estimatedRtt:endpoints.length ? endpoints.reduce((sum, rtt) => sum + rtt, 0) : Infinity,
    };
  }).sort((a,b) => {
    const tier = item => item.quality?.suitable ? 0 : item.quality == null ? 1 : 2;
    return tier(a) - tier(b) ||
      (a.quality?.score ?? Infinity) - (b.quality?.score ?? Infinity) ||
      b.measuredEndpoints - a.measuredEndpoints || a.estimatedRtt - b.estimatedRtt || a.worker - b.worker;
  });
}

export function selectSwarmRoute(candidates, ordinaryTurnAvailable) {
  const best = candidates[0];
  if (best?.quality?.suitable) return {mode:'swarm', worker:best.worker};
  if (ordinaryTurnAvailable && candidates.length && candidates.every(candidate =>
    candidate.quality || candidate.measuredEndpoints === 2 && candidate.estimatedRtt > DegradedEstimatedRtt))
    return {mode:'turn'};
  return null;
}

export function createSwarmStandbyPlanner({workers, ordinaryTurn, dispatch, initialSerial = 0, clock = Date.now, wait = setTimeout, cancel = clearTimeout}) {
  const swarm = workers.map(worker => 'swarm-' + worker);
  let survey, timer, selected, serial = initialSerial, nextSurvey = 0;
  let nextWorker = 0;

  function finish(route) {
    const finishing = survey;
    survey = undefined;
    nextSurvey = clock() + 30_000;
    ++serial;
    selected = route ? {serial, expires:clock() + 15_000, active:finishing.active} : undefined;
    dispatch({serial, phase: route ? 'select' : 'idle', retired:finishing.serial, ...(route && {route})});
  }

  function waitForReports(milliseconds) {
    if (timer !== undefined) cancel(timer);
    timer = wait(() => {
      timer = undefined;
      advance();
    }, milliseconds);
    timer?.unref?.();
  }

  function advance() {
    const retired = survey.serial;
    const route = survey.queue.shift();
    if (!route) {
      finish(survey.best?.route);
      return;
    }
    survey.route = route;
    survey.serial = ++serial;
    survey.reports = new Map();
    dispatch({serial, phase:'measure', retired, route});
    waitForReports(survey.outage ? 3500 : route === 'direct' ? 4000 : 8000);
  }

  return {
    consider(active, measurement, initial = false) {
      if ((active !== 'direct' && active !== 'turn' && active !== 'swarm' && !swarm.includes(active)) ||
          !measurement) return false;
      const outage = measurement.received === 0;
      if (survey?.outage || (!outage && (survey || clock() < nextSurvey))) return false;
      if (outage && selected?.active === active && clock() < selected.expires - 10_000) return false;
      const current = pathQuality([measurement]);
      if (current.suitable && (!initial || active === 'direct' || active === 'turn')) return false;
      if (outage && survey?.active === active && survey.route !== 'direct') {
        survey.outage = true;
        survey.currentScore = current.score;
        waitForReports(3500);
        return true;
      }
      const candidates = swarm.filter(route => route !== active);
      const offset = nextWorker % Math.max(1, candidates.length);
      const shortlist = [...candidates.slice(offset), ...candidates.slice(0, offset)].slice(0, 2);
      const queue = [
        ...(active === 'direct' || outage ? [] : ['direct']),
        ...(ordinaryTurn ? ['turn'] : []),
        ...shortlist,
      ];
      if (!queue.length) return false;
      if (timer !== undefined) cancel(timer);
      nextWorker = offset + shortlist.length;
      survey = {queue, serial:survey?.serial ?? 0, best:null, currentScore:current.score, active, outage};
      advance();
      return true;
    },
    report(player, id, measurement) {
      if (!survey || survey.serial !== id || survey.reports.has(player) || (player !== 0 && player !== 1)) return;
      if (!measurement || !pathQuality([measurement]).usable) {
        cancel(timer);
        timer = undefined;
        advance();
        return;
      }
      survey.reports.set(player, measurement);
      if (survey.reports.size !== 2) return;
      cancel(timer);
      timer = undefined;
      const score = pathQuality([...survey.reports.values()]);
      if (score.score * 1.1 + 5 < survey.currentScore) {
        if (score.suitable && (survey.route === 'direct' || survey.route === 'turn' || survey.outage)) {
          finish(survey.route);
          return;
        }
        if (!survey.best || score.score < survey.best.score) survey.best = {...score, route:survey.route};
      }
      advance();
    },
    acceptsSignal(id) {
      return survey?.serial === id || !!selected && selected.serial === id && clock() < selected.expires;
    },
    close() {
      if (timer !== undefined) cancel(timer);
      timer = survey = selected = undefined;
    },
  };
}
