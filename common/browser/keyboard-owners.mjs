// Generic browser ownership only. The adapter maps DOM events to logical bits.
export function keyboardLocation(event) {
  const code = event.code || "";
  if (/^(Shift|Control)(Left|Right)$/.test(code)) return code.endsWith("Left") ? 1 : 2;
  if (code.startsWith("Numpad")) return 3;
  return Number.isInteger(event.location) ? event.location : 0;
}

export function updateKeyboardOwners(owners, event, bit, down) {
  if (!bit) return false;
  const location = keyboardLocation(event);
  const code = event.code && event.code !== "Unidentified" ? event.code : "";
  const id = code ? `code:${code}` : `${bit}:${location}`;
  const candidates = [...owners.entries()].filter(([, owner]) => owner.bit === bit);
  const sameSide = candidates.filter(([, owner]) => owner.location === location);
  if (down) {
    const existing = owners.get(id) || (!code && sameSide.length === 1 ? sameSide[0][1]
      : !code && !location && candidates.length === 1 ? candidates[0][1] : null);
    // Lifecycle cancellation must not be undone by an old auto-repeat event.
    if (event.repeat && !existing) return false;
    if (!existing) owners.set(id, {bit, location, code});
    return true;
  }
  let released = owners.has(id) ? [[id, owners.get(id)]] : sameSide;
  if (!released.length) released = candidates.filter(([, owner]) => !owner.code && !owner.location);
  // With no physical evidence, release all candidates rather than arbitrarily
  // removing one and permanently stranding the other. Known sides stay distinct.
  if (!released.length && !code && !location) released = candidates;
  for (const [owner] of released) owners.delete(owner);
  return released.length > 0;
}
