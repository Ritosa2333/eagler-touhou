/** Account identity and chat storage belong to the private API. */
export function createAccountChatSource({
  onRetract,
  fetch: request = globalThis.fetch,
  base = process.env.ACCOUNT_API_INTERNAL_URL || 'http://127.0.0.1:18410',
  secret = process.env.CHAT_BRIDGE_SECRET || process.env.ACCOUNT_RELAY_SECRET,
  roomNamespace = process.env.ACCOUNT_CHAT_ROOM_NAMESPACE || '',
} = {}) {
  if (!secret || secret.length < 32) throw Error('Set CHAT_BRIDGE_SECRET');
  if (roomNamespace && !/^[a-z][a-z0-9-]{0,15}$/.test(roomNamespace)) throw Error('Invalid chat room namespace');
  const storageRoom = room => roomNamespace ? roomNamespace + ':' + room : room;
  const scopedInput = input => roomNamespace ? {...input, room: storageRoom(input.room)} : input;
  const roomMessage = (message, room) => roomNamespace ? {...message, room} : message;
  const stop = new AbortController(),
    removed = new Map();
  let cursor = 0,
    running = false;
  async function call(path, body, service = 'chat') {
    const response = await request(new URL(`/api/internal/${service}/${path}`, base), {
      method: body ? 'POST' : 'GET',
      redirect: 'error',
      signal: AbortSignal.any([stop.signal, AbortSignal.timeout(body ? (path === 'reports' ? 20000 : 10000) : 35000)]),
      headers: { authorization: 'Bearer ' + secret, 'content-type': 'application/json' },
      ...(body ? { body: JSON.stringify(body) } : {}),
    });
    const bytes = await response.arrayBuffer();
    if (bytes.byteLength > 200000) throw Error('Account API response too large');
    const data = JSON.parse(new TextDecoder().decode(bytes));
    if (!response.ok)
      throw Object.assign(Error('Account API request failed'), {
        status: response.status,
        code: data.code || 'service-unavailable',
        retryAt: data.retryAt || 0,
      });
    return data;
  }
  async function poll() {
    while (!stop.signal.aborted) {
      try {
        const events = await call('events?after=' + cursor);
        for (const event of events) {
          if (!Number.isSafeInteger(event.id) || event.id <= cursor) continue;
          cursor = event.id;
          removed.set(event.userId, Math.max(removed.get(event.userId) || 0, event.throughId));
          if (removed.size > 10000) removed.delete(removed.keys().next().value);
          onRetract?.({
            type: 'chat-retract',
            authorId: event.userId,
            throughId: event.throughId,
            bannedUntil: event.bannedUntil,
          });
        }
      } catch {
        if (!stop.signal.aborted)
          await new Promise((resolve) => {
            const timer = setTimeout(done, 1000);
            function done() {
              clearTimeout(timer);
              stop.signal.removeEventListener('abort', done);
              resolve();
            }
            stop.signal.addEventListener('abort', done, { once: true });
          });
      }
    }
  }
  return {
    start() {
      if (!running) {
        running = true;
        void poll();
      }
    },
    async history(room) {
      const data = await call('history?room=' + encodeURIComponent(storageRoom(room)));
      return {
        ...data,
        entries: data.entries.filter((message) => message.messageId > (removed.get(message.authorId) || 0))
          .map(message => roomMessage(message, room)),
      };
    },
    status: (account) =>
      call(
        'status?userId=' + encodeURIComponent(account.userId) + '&sessionId=' + encodeURIComponent(account.sessionId),
      ),
    async send(input) {
      const message = await call('messages', scopedInput(input));
      return message.messageId > (removed.get(message.authorId) || 0) ? roomMessage(message, input.room) : null;
    },
    report: (input) => call('reports', scopedInput(input)),
    recordTh09Round: (input) => call('rounds', input.room ? scopedInput(input) : input, 'th09'),
    recordTh09Replay: ({duelId, roundId, replay}) => call('replays', {duelId, roundId, replay}, 'th09'),
    close() {
      stop.abort();
    },
  };
}
