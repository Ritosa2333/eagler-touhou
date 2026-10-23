/** Paired lobby measurement | reliable coordination, unretransmitted probes. */
export interface PacketLossDirection { sent: number; received: number; lost: number; percent: number; reordered: number }
export interface PacketLossSnapshot {
  state: 'idle' | 'running' | 'complete' | 'unavailable' | 'interrupted';
  sent: number; outgoing?: PacketLossDirection; incoming?: PacketLossDirection;
}
const count = 300, interval = 16, tail = 1000, prefix = 'loss1:';
const blank = (): PacketLossSnapshot => ({state: 'idle', sent: 0});
export function createPacketLossTest(options: {
  control(value: string): boolean; probe(value: string): boolean; ready(): boolean; changed(): void;
  visible?(): boolean; now?(): number;
  later?(fn: () => void, ms: number): ReturnType<typeof setTimeout>;
  cancel?(timer: ReturnType<typeof setTimeout>): void;
}) {
  const now = options.now ?? (() => performance.now()), later = options.later ?? setTimeout, cancel = options.cancel ?? clearTimeout;
  let snapshot = blank(), disposed = false;
  type Run = {id: string; sent: number; seen: Set<number>; highest: number; reordered: number; sending: boolean;
    ended: boolean; drained: boolean; reported: boolean; remote?: {sent: number; received: number; reordered: number};
    previous: number; notice: number; timers: Set<ReturnType<typeof setTimeout>>};
  let run: Run | null = null;
  const control = (kind: string, id: string, values = {}) => options.control(prefix + JSON.stringify({kind, id, ...values}));
  const stopTimers = (r: Run) => {for (const timer of r.timers) cancel(timer); r.timers.clear();};
  const schedule = (r: Run, fn: () => void, ms: number) => {
    const timer = later(() => {r.timers.delete(timer); if (run === r && !disposed) fn();}, ms); r.timers.add(timer);
  };
  const interrupt = (notify = true) => {
    const r = run; if (!r) return;
    stopTimers(r); run = null; snapshot = {state: 'interrupted', sent: r.sent};
    if (notify) control('cancel', r.id);
    options.changed();
  };
  const healthy = () => options.ready() && (options.visible?.() ?? true);
  const direction = (sent: number, received: number, reordered: number): PacketLossDirection =>
    ({sent, received, lost: sent - received, percent: (sent - received) / sent * 100, reordered});
  const finish = (r: Run) => {
    if (r.sent !== count || !r.drained) return;
    if (!healthy()) {interrupt(); return;}
    if (!r.reported) {
      r.reported = true;
      if (!control('report', r.id, {sent: r.sent, received: r.seen.size, reordered: r.reordered})) {interrupt(false); return;}
    }
    if (!r.remote) return;
    stopTimers(r); run = null;
    snapshot = {state: 'complete', sent: r.sent,
      outgoing: direction(r.sent, r.remote.received, r.remote.reordered),
      incoming: direction(r.remote.sent, r.seen.size, r.reordered)};
    options.changed();
  };
  const create = (id: string) => {
    const r: Run = {id, sent: 0, seen: new Set(), highest: -1, reordered: 0, sending: false,
      ended: false, drained: false, reported: false, previous: now(), notice: now(), timers: new Set()};
    run = r; snapshot = {state: 'running', sent: 0}; options.changed();
    schedule(r, () => interrupt(), 12000);
    return r;
  };
  const begin = (r: Run) => {
    if (r.sending) return; r.sending = true; r.previous = now();
    const step = () => {
      const time = now();
      // Browser suspension or local send pressure is an interrupted test,
      // never a made-up network loss percentage. Do not burst after a pause.
      if (!healthy() || time - r.previous > 250) {interrupt(); return;}
      r.previous = time;
      if (!options.probe(`${prefix}${r.id}:${r.sent}`.padEnd(96, ' '))) {interrupt(); return;}
      ++r.sent; snapshot = {state: 'running', sent: r.sent};
      if (time - r.notice >= 250) {r.notice = time; options.changed();}
      if (r.sent < count) schedule(r, step, interval);
      else if (!control('end', r.id)) interrupt(false);
      else finish(r);
    };
    step();
  };
  return {
    snapshot: () => snapshot,
    start() {
      if (disposed || run) return;
      if (!healthy()) {snapshot = {state: 'unavailable', sent: 0}; options.changed(); return;}
      const r = create(crypto.randomUUID());
      if (!control('start', r.id)) interrupt(false);
    },
    receiveControl(data: unknown): boolean {
      if (typeof data !== 'string' || !data.startsWith(prefix)) return false;
      if (disposed || data.length > 512) return true;
      let message: {kind?: string; id?: string; sent?: number; received?: number; reordered?: number};
      try {message = JSON.parse(data.slice(prefix.length));} catch {return true;}
      if (!message || typeof message.id !== 'string' || !/^[\w-]{1,64}$/.test(message.id)) return true;
      const {id, kind} = message;
      if (kind === 'start') {
        if (!healthy() || run && run.id !== id) {control('cancel', id); return true;}
        if (!run) create(id);
        control('ready', id); return true;
      }
      const r = run; if (!r || r.id !== id) return true;
      if (kind === 'cancel') {interrupt(false); return true;}
      if (!healthy()) {interrupt(); return true;}
      if (kind === 'ready') {if (control('go', id)) begin(r); else interrupt(false);}
      else if (kind === 'go') begin(r);
      else if (kind === 'end' && !r.ended) {
        r.ended = true;
        const started = now();
        schedule(r, () => {
          if (!healthy() || now() - started > tail + 250) {interrupt(); return;}
          r.drained = true; finish(r);
        }, tail);
      } else if (kind === 'report' && !r.remote && message.sent === count &&
        Number.isInteger(message.received) && message.received! >= 0 && message.received! <= count &&
        Number.isInteger(message.reordered) && message.reordered! >= 0 && message.reordered! <= message.received!) {
        r.remote = {sent: count, received: message.received!, reordered: message.reordered!}; finish(r);
      }
      return true;
    },
    receiveProbe(data: unknown) {
      const r = run;
      if (!r || disposed || r.drained || typeof data !== 'string' || data.length > 200) return;
      const match = /^loss1:([\w-]{1,64}):(\d+)\s*$/.exec(data);
      if (!match || match[1] !== r.id) return;
      const sequence = Number(match[2]);
      if (!Number.isInteger(sequence) || sequence < 0 || sequence >= count || r.seen.has(sequence)) return;
      if (sequence < r.highest) ++r.reordered;
      r.highest = Math.max(r.highest, sequence); r.seen.add(sequence);
    },
    interrupt,
    dispose() {if (run) interrupt(); disposed = true;},
  };
}
