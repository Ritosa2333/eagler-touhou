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
  const knownObservers = new Set(observers);
  const maximumBases = 8;
  let closed = false;
  const timer = setTimeout(() => {
    // Some ICE agents deduplicate identical public mappings from different
    // STUN servers. An independently observed prediction may still help.
    release(true);
    close();
  }, knownObservers.size && addresses.some(address => !steps.has(address)) ? 4500 : 3000);
  timer.unref();

  function close() {
    closed = true;
    clearTimeout(timer);
    candidates.clear();
    observed.clear();
  }

  function release(partial = false) {
    if (closed || descriptions.size !== 2 || !candidates.size || (!partial && candidates.size !== 2)) return;
    for (const [source, predicted] of candidates) {
      if (observed.has(source)) console.log(`NAT4 prediction seat=${source} source=stun port=${predicted.candidate.split(' ')[5]}`);
      if (observed.has(source) || steps.get(addresses[source]) !== 0)
        send(source, {description: null, candidate: predicted});
    }
    close();
  }

  return {
    send(player, {description, candidate, observer}) {
      // Keep ICE gathering and NAT4 observation independent. Leave trickle ICE
      // open until the predictor has had a chance to send its candidate.
      if (!observer) {
        const outgoing = !closed && description
          ? {...description, sdp:description.sdp.replace(/^a=end-of-candidates(?:\r?\n|$)/gm, '')}
          : description;
        send(player, {description:outgoing, candidate});
      }
      if (closed) return;
      if (description) {
        descriptions.add(player);
      }
      if (candidate) {
        const address = addresses[player];
        const mapping = !candidates.has(player) && observedCandidate(candidate, address);
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
    close,
  };
}
