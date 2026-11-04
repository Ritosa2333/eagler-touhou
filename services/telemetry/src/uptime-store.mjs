import {publicIdentity} from './status-model.mjs';

const MINUTE = 60000, HOUR = 60 * MINUTE, DAY = 24 * HOUR;
const idPattern = /^[a-z0-9-]{1,40}$/;
export const UPTIME_RANGES = Object.freeze({'24h': {duration: DAY, count: 48}, '7d': {duration: 7 * DAY, count: 56}, '90d': {duration: 90 * DAY, count: 90}});
export const uptimeGroup = target => target.group ?? ({resource:'download', turn:'turn', site:'site'}[target.kind] ?? 'netplay');

/** Five-minute detail for one day, hourly aggregates for ninety days.
 * Empty intervals stay unknown; monitoring downtime is never painted green. */
export function createUptimeStore(initial = {}, clock = Date.now) {
  const entries = new Map();
  let revision = 0, saved = 0;
  for (const [id, value] of Object.entries(initial.entries ?? {})) {
    if (!idPattern.test(id) || !value || typeof value !== 'object') continue;
    const clean = values => new Map(Object.entries(values ?? {}).filter(([key, b]) =>
      Number.isSafeInteger(Number(key)) && b && Number.isSafeInteger(b.total) && b.total > 0 &&
      Number.isSafeInteger(b.up) && b.up >= 0 && b.up <= b.total && Number.isFinite(b.latency) && b.latency >= 0));
    entries.set(id, {fine: clean(value.fine), hours: clean(value.hours), last: Number(value.last) || 0});
  }
  function prune() {
    const now = clock();
    for (const value of entries.values()) {
      for (const [key] of value.fine) if (Number(key) < now - DAY - 300000) {value.fine.delete(key); revision++;}
      for (const [key] of value.hours) if (Number(key) < now - 90 * DAY - HOUR) {value.hours.delete(key); revision++;}
    }
  }
  function record(services) {
    for (const s of services) {
      const at = Date.parse(s.checkedAt);
      if (!idPattern.test(s.id) || !Number.isFinite(at) || at > clock() + MINUTE || !['operational', 'outage'].includes(s.status)) continue;
      const value = entries.get(s.id) ?? {fine: new Map(), hours: new Map(), last: 0};
      if (at <= value.last) continue;
      entries.set(s.id, value); value.last = at;
      for (const [table, width] of [[value.fine, 300000], [value.hours, HOUR]]) {
        const key = String(Math.floor(at / width) * width);
        const bucket = table.get(key) ?? {total:0, up:0, latency:0, responses:0};
        bucket.total++; if (s.status === 'operational') {bucket.up++; bucket.responses++; bucket.latency += Math.max(0, Number(s.latencyMs) || 0);}
        table.set(key, bucket);
      }
      revision++;
    }
    prune();
  }
  function snapshot(targets, services, range = '24h', {showDownloads = true} = {}) {
    prune();
    const spec = UPTIME_RANGES[range]; if (!spec) throw Error('Invalid uptime range');
    const now = clock(), width = spec.duration / spec.count;
    const end = (Math.floor(now / width) + 1) * width, start = end - spec.duration;
    const latest = new Map(services.map(s => [s.id, s]));
    return {schema:'eagler-uptime/1', range, updatedAt:new Date(now).toISOString(),
      services:targets.filter(t => showDownloads || uptimeGroup(t) !== 'download').map(t => {
        const points = Array.from({length:spec.count}, (_, i) => ({at:start + i * width, total:0, up:0, latency:0, responses:0}));
        const entry = entries.get(t.id), table = range === '24h' ? entry?.fine : entry?.hours;
        if (table) for (const [key, value] of table) {
          const at = Number(key), index = Math.floor((at - start) / width);
          if (index < 0 || index >= points.length || at > now) continue;
          for (const field of ['total','up','latency','responses']) points[index][field] += value[field] || 0;
        }
        const current = latest.get(t.id);
        const total = points.reduce((sum, p) => sum + p.total, 0), up = points.reduce((sum, p) => sum + p.up, 0);
        return {...publicIdentity(t), group:uptimeGroup(t), status:current?.status ?? 'unknown',
          checkedAt:current?.checkedAt ?? null, latencyMs:current?.status === 'operational' ? current.latencyMs : null,
          availability:total ? up / total : null,
          points:points.map(p => ({at:new Date(p.at).toISOString(), samples:p.total, availability:p.total ? p.up / p.total : null,
            latencyMs:p.responses ? p.latency / p.responses : null}))};
      })};
  }
  return {record, snapshot, revision:()=>revision, isDirty:()=>revision !== saved, markSaved:r=>{saved=r;},
    export:()=>{prune(); return {entries:Object.fromEntries([...entries].map(([id, v]) => [id, {fine:Object.fromEntries(v.fine),hours:Object.fromEntries(v.hours),last:v.last}]))};}};
}
