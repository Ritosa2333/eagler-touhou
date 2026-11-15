// Internal loopback transport to the sole owner of the guest nickname database.
export function createRemoteNicknameService() {
  const secret = process.env.NICKNAME_BRIDGE_SECRET;
  if (!secret || secret.length < 32) throw Error('Set NICKNAME_BRIDGE_SECRET');
  const base = 'http://127.0.0.1:' + (process.env.NICKNAME_BRIDGE_PORT || 18411);
  const pending = new Map(), listeners = new Set();
  let closed = false;
  async function call(path,fields) {
    const res = await fetch(base + path, {method:'POST', headers:{authorization:'Bearer ' + secret,
      'content-type':'application/json'}, body:JSON.stringify(fields),signal:AbortSignal.timeout(13000)});
    if (!res.ok) throw Error('Nickname service unavailable');
    return res.json();
  }
  function track(member,profile) {
    if (!closed && ['queued','reviewing'].includes(profile.status) && !pending.has(member)) {
      if (pending.size >= 1000) throw Error('Nickname polling capacity reached');
      const poll = async () => {
        try {
          const latest = await call('/current',{member});
          if (closed) return;
          if (!['queued','reviewing'].includes(latest.status)) {
            pending.delete(member);
            for (const listener of listeners) listener(member,latest);
            return;
          }
        } catch { /* The API still owns the pending review after a disconnect. */ }
        if (!closed) pending.set(member,setTimeout(poll,3000).unref());
      };
      pending.set(member,setTimeout(poll,1000).unref());
    }
    return profile;
  }
  return {async current(member) {return track(member,await call('/current',{member}));},
    async submit(member,address,name) {return track(member,await call('/submit',{member,address,name}));},
    onUpdate(listener) {listeners.add(listener);return () => listeners.delete(listener);},
    close() {closed=true;for (const timer of pending.values()) clearTimeout(timer);pending.clear();listeners.clear();}};
}
