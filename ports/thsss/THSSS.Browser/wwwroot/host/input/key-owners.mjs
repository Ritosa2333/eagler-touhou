// One physical key can be held by several owners. The game sees a change
// only when the last owner releases it or the first owner presses it.
export function createKeyOwners(send, { sampleTaps = false } = {}) {
  const owners = new Map();
  const down = new Set();
  const unsampled = new Set();
  const taps = new Set();

  function setKey(name, pressed, source) {
    if (!name || !source) return false;
    // The game treats Enter as Z. Releasing either input must preserve the other.
    if (name === "Return") {
      name = "Z";
      source += ":confirm";
    }
    let group = owners.get(name);
    if (!group) {
      group = new Set();
      owners.set(name, group);
    }
    if (pressed) group.add(source);
    else group.delete(source);
    const want = group.size > 0;
    if (!want && unsampled.has(name) && down.has(name)) {
      taps.add(name);
      return false;
    }
    taps.delete(name);
    if (want === down.has(name)) return false;
    if (want) {
      down.add(name);
      if (sampleTaps) unsampled.add(name);
    }
    else down.delete(name);
    send(name, want);
    return true;
  }

  function releaseSource(source) {
    for (const [name, group] of owners) {
      group.delete(source);
      group.delete(source + ":confirm");
      if (group.size === 0 && down.delete(name)) {
        unsampled.delete(name);
        taps.delete(name);
        send(name, false);
      }
    }
  }

  function releaseAll() {
    for (const name of down) send(name, false);
    owners.clear();
    down.clear();
    unsampled.clear();
    taps.clear();
  }

  function finishStep() {
    unsampled.clear();
    for (const name of taps) {
      if (down.delete(name)) send(name, false);
    }
    taps.clear();
  }

  return { setKey, releaseSource, releaseAll, finishStep, isDown: (name) => down.has(name === "Return" ? "Z" : name) };
}
