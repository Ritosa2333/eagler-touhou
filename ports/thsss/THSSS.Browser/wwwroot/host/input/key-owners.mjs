// One physical key can be held by several owners. The game sees a change
// only when the last owner releases it or the first owner presses it.
export function createKeyOwners(send) {
  const owners = new Map();
  const down = new Set();

  function setKey(name, pressed, source) {
    if (!name || !source) return false;
    let group = owners.get(name);
    if (!group) {
      group = new Set();
      owners.set(name, group);
    }
    if (pressed) group.add(source);
    else group.delete(source);
    const want = group.size > 0;
    if (want === down.has(name)) return false;
    if (want) down.add(name);
    else down.delete(name);
    send(name, want);
    return true;
  }

  function releaseSource(source) {
    for (const name of owners.keys()) {
      if (owners.get(name).has(source)) setKey(name, false, source);
    }
  }

  function releaseAll() {
    for (const [name, group] of owners) {
      for (const source of group) setKey(name, false, source);
    }
  }

  return { setKey, releaseSource, releaseAll, isDown: (name) => down.has(name) };
}
