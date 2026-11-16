const TurnDelayMs = 500;
const ComparisonGraceMs = 350;
const DegradedRouteMs = 3000;
const CommitTimeoutMs = 2000;
const FastDirectRttMs = 85;

function quality(reports) {
  if (reports.length !== 2 || reports.some(report => !report || report.received < 4)) return null;
  const rtt = Math.max(...reports.map(report => report.p95));
  const loss = Math.max(...reports.map(report => 1 - report.received / report.sent));
  return {
    direct: reports.every(report => report.direct),
    rtt,
    suitable: rtt <= 150 && loss <= 0.05,
    score: rtt + loss * 1000,
  };
}

// The original WebRTC connection can carry either direct/NAT4 or Swarm.
// The second connection measures first-party TURN independently before frame zero.
export function createRtcStartup({timeout, openTurn, select, complete, fail, now = () => performance.now(), wait = setTimeout, cancel = clearTimeout}) {
  const started = now(), lanes = [new Map(), new Map()], acknowledgements = new Set();
  let chosen, timer, turnOpen = false, firstSuitable, closed = false;
  function stop() {
    closed = true;
    cancel(timer);
  }
  function choose(serial) {
    chosen = serial;
    cancel(timer);
    timer = wait(() => {stop(); fail();}, CommitTimeoutMs);
    timer?.unref?.();
    select(serial);
  }
  function check() {
    if (closed || chosen !== undefined) return;
    cancel(timer);
    const elapsed = now() - started;
    const primary = quality([...lanes[0].values()]);
    const turn = quality([...lanes[1].values()]);
    if (primary?.direct && primary.suitable && primary.rtt <= FastDirectRttMs &&
        (!turn?.suitable || turn.score + 15 >= primary.score))
      return choose(0);
    if (primary?.suitable && turn?.suitable)
      return choose(turn.score + 15 < primary.score ? 1 : 0);
    if ((primary?.suitable || turn?.suitable) && firstSuitable === undefined) firstSuitable = elapsed;
    const firstFailed = [...lanes[0].values()].includes(null);
    if (!turnOpen && (elapsed >= TurnDelayMs || firstFailed)) {
      turnOpen = true;
      openTurn();
    }
    const bothReported = lanes.every(lane => lane.size === 2 || [...lane.values()].includes(null));
    if (firstSuitable !== undefined && (bothReported || elapsed >= firstSuitable + ComparisonGraceMs))
      return choose(primary?.suitable ? 0 : 1);
    if ((primary || turn) && (bothReported || elapsed >= DegradedRouteMs))
      return choose(!primary || turn && turn.score < primary.score ? 1 : 0);
    if (elapsed >= timeout || bothReported) {
      stop();
      fail();
      return;
    }
    const next = Math.min(timeout, !turnOpen ? TurnDelayMs : Infinity,
      firstSuitable !== undefined ? firstSuitable + ComparisonGraceMs : Infinity,
      elapsed < DegradedRouteMs && (primary || turn) ? DegradedRouteMs : Infinity);
    timer = wait(check, Math.max(1, next - elapsed));
    timer?.unref?.();
  }
  return {
    start: check,
    report(player, serial, report) {
      if (closed || chosen !== undefined || (serial !== 0 && (serial !== 1 || !turnOpen)) ||
          lanes[serial].has(player)) return;
      lanes[serial].set(player, report);
      check();
    },
    acceptsTurnSignal: () => turnOpen && !closed,
    acknowledge(player, serial) {
      if (closed || serial !== chosen || chosen === undefined) return;
      acknowledgements.add(player);
      if (acknowledgements.size !== 2) return;
      stop();
      complete(serial);
    },
    close: stop,
  };
}
