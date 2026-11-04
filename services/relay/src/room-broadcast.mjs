// State updates keep an immediate leading edge and one trailing snapshot per
// window. Events bypass coalescing; callers own message schema and membership.
export function createRoomBroadcaster({
  snapshot, send, changed, coalesceMs = 100,
  schedule = setTimeout, cancel = clearTimeout,
}) {
  const pending = new WeakMap();

  function publish(room, event = null) {
    changed();
    if (event) {
      send(room, JSON.stringify(event));
      return;
    }
    const current = pending.get(room);
    if (current) {
      current.dirty = true;
      return;
    }
    send(room, JSON.stringify(snapshot(room)));
    const state = { dirty: false, timer: null };
    pending.set(room, state);
    state.timer = schedule(() => {
      if (pending.get(room) !== state) return;
      pending.delete(room);
      if (state.dirty) publish(room);
    }, coalesceMs);
  }

  function clear(room) {
    const state = pending.get(room);
    if (!state) return;
    pending.delete(room);
    cancel(state.timer);
  }

  return { publish, clear };
}
