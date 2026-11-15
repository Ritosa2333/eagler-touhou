import {isIPv4} from 'node:net';

export function readNat4PortSteps(raw) {
  const steps = new Map();
  for (const entry of raw.split(',').map(value => value.trim()).filter(Boolean)) {
    const [address, step, extra] = entry.split('=');
    if (!isIPv4(address) || !/^(?:0|-?(?:[1-9]|1[0-6]))$/.test(step) || extra !== undefined)
      throw new Error('EAGLER_NETPLAY_NAT4_PORT_STEPS requires IPv4=step entries');
    steps.set(address, Number(step));
  }
  return steps;
}

function observedCandidate(candidate, address) {
  const fields = candidate.candidate.trim().split(/\s+/);
  if (!/^candidate:[A-Za-z0-9+/]{1,28}$/.test(fields[0]) || fields[1] !== '1' ||
      fields[2]?.toLowerCase() !== 'udp' || fields[4] !== address ||
      fields[6] !== 'typ' || fields[7] !== 'srflx' || !/^\d{1,5}$/.test(fields[5])) return;
  const port = Number(fields[5]);
  if (port >= 1024 && port <= 65535) return {candidate, fields, port};
}

function predictedCandidate(observed, step) {
  const port = observed.port + step;
  if (port < 1024 || port > 65535) return;
  if (!step) return observed.candidate;
  const fields = [...observed.fields];
  fields[0] += 'next';
  fields[5] = String(port);
  return {...observed.candidate, candidate: fields.join(' ')};
}

export function createNat4CandidateExchange(addresses, steps, send, observers = []) {
  const candidates = new Map();
  const observed = new Map();
  const descriptions = new Set();
  const pending = [];
  const knownObservers = new Set(observers);
  const maximumBases = 8;
  let closed = false, releasedPrediction = false;
  let timer = setTimeout(() => {
    // Some ICE agents deduplicate identical public mappings from different
    // STUN servers. Offer the other seat's independently observed prediction
    // before opening the normal candidate path.
    if (!release(true)) flush();
  }, knownObservers.size && addresses.some(address => !steps.has(address)) ? 4500 : 3000);
  timer.unref();

  function flush() {
    closed = true;
    clearTimeout(timer);
    candidates.clear();
    observed.clear();
    for (const [player, signal] of pending) send(player, signal);
    pending.length = 0;
  }

  function release(partial = false) {
    if (releasedPrediction || descriptions.size !== 2 || !candidates.size || (!partial && candidates.size !== 2)) return false;
    releasedPrediction = true;
    for (const [source, predicted] of candidates) {
      if (observed.has(source)) console.log(`NAT4 prediction seat=${source} source=stun port=${predicted.candidate.split(' ')[5]}`);
      send(source, {description: null, candidate: predicted});
    }
    candidates.clear();
    observed.clear();
    clearTimeout(timer);
    timer = setTimeout(flush, 1200);
    timer.unref();
    return true;
  }

  return {
    send(player, {description, candidate, observer}) {
      if (closed) {
        if (!observer) send(player, {description, candidate});
        return;
      }
      if (description) {
        const sdp = description.sdp.replace(/^a=(?:candidate:[^\r\n]*|end-of-candidates)(?:\r?\n|$)/gm, '');
        send(player, {description: {...description, sdp}, candidate: null});
        descriptions.add(player);
      }
      if (candidate) {
        if (!observer && pending.length < 64) pending.push([player, {description: null, candidate}]);
        const address = addresses[player];
        const mapping = !releasedPrediction && !candidates.has(player) && observedCandidate(candidate, address);
        if (mapping) {
          let prediction;
          if (steps.has(address)) {
            prediction = predictedCandidate(mapping, steps.get(address));
          } else if (knownObservers.has(observer)) {
            if (!observed.has(player)) observed.set(player, new Map());
            const bases = observed.get(player);
            // Combine only observations with the same ICE foundation. A
            // multi-homed peer can report each observer from several bases.
            const foundation = mapping.fields[0];
            if (!bases.has(foundation) && bases.size < maximumBases) bases.set(foundation, new Map());
            const byObserver = bases.get(foundation);
            if (!byObserver) return;
            if (!byObserver.has(observer)) byObserver.set(observer, mapping);
            if (byObserver.size === 3) {
              const [first, second, third] = [...byObserver.values()].sort((a, b) => a.port - b.port);
              const step = second.port - first.port;
              if (step >= 0 && step <= 16 && third.port - second.port === step)
                prediction = predictedCandidate(third, step);
            }
          }
          if (prediction) candidates.set(player, prediction);
        }
      }
      release();
    },
    close() {
      closed = true;
      clearTimeout(timer);
      candidates.clear();
      observed.clear();
      pending.length = 0;
    },
  };
}
