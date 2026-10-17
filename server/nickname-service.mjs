import {Worker} from 'node:worker_threads';
import {isAbsolute} from 'node:path';

/** The relay thread never runs SQL, filesystem writes or moderation requests. */
export async function createNicknameService(env = process.env) {
  if (env.EAGLER_NETPLAY_PUBLIC_NICKNAMES !== '1') return null;
  const file = env.EAGLER_NETPLAY_NICKNAME_STATE, apiKey = env.EAGLER_NETPLAY_NICKNAME_API_KEY;
  if (!file || !isAbsolute(file) || !apiKey) throw new Error('Public nicknames require private state and API key configuration');
  const integer = (key, fallback) => {
    const raw = env['EAGLER_NETPLAY_NICKNAME_' + key];
    if (raw === undefined) return fallback;
    if (!/^\d+$/.test(raw) || Number(raw) < 1 || Number(raw) > 100000) throw new Error('Invalid nickname limit');
    return Number(raw);
  };
  const worker = new Worker(new URL('./nickname-worker.mjs', import.meta.url), {
    workerData: {file, apiKey, apiBase: env.EAGLER_NETPLAY_NICKNAME_API_BASE, model: env.EAGLER_NETPLAY_NICKNAME_MODEL || 'deepseek-flash',
      hourlyLimit: integer('IP_HOURLY_LIMIT', 5), dailyLimit: integer('IP_DAILY_LIMIT', 20),
      reviewBudget: integer('DAILY_BUDGET', 1000)},
    resourceLimits: {maxOldGenerationSizeMb: 32, maxYoungGenerationSizeMb: 4},
  });
  const pending = new Map(), listeners = new Set();
  let serial = 0, closed = false;
  const startup = new Promise((resolve, reject) => {
    const timeout = setTimeout(() => reject(new Error('Nickname worker startup failed')), 10000);
    worker.once('message', message => {clearTimeout(timeout); message.ready ? resolve() : reject(new Error('Nickname worker startup failed'));});
    worker.once('error', () => {clearTimeout(timeout); reject(new Error('Nickname worker startup failed'));});
    worker.once('exit', () => {clearTimeout(timeout); reject(new Error('Nickname worker startup failed'));});
  });
  function failPending() {
    closed = true;
    for (const item of pending.values()) {clearTimeout(item.timeout); item.reject(new Error('Nickname service unavailable'));}
    pending.clear();
  }
  worker.on('error', failPending); worker.on('exit', failPending);
  worker.on('message', message => {
    if(message.update){for(const listener of listeners)listener(message.member,message.profile);return;}
    const item = pending.get(message.id);
    if (!item) return;
    pending.delete(message.id); clearTimeout(item.timeout);
    if (message.error) item.reject(new Error('Nickname service unavailable')); else item.resolve(message.value);
  });
  try { await startup; } catch (error) {await worker.terminate(); throw error;}
  function call(operation, member, address, name) {
    if (closed || pending.size >= 64) return Promise.reject(new Error('Nickname service unavailable'));
    return new Promise((resolve, reject) => {
      const id = ++serial, timeout = setTimeout(() => {pending.delete(id); reject(new Error('Nickname service unavailable'));}, 12000);
      pending.set(id, {resolve, reject, timeout}); worker.postMessage({id, operation, member, address, name});
    });
  }
  return {current: member => call('current', member), submit: (member, address, name) => call('submit', member, address, name),
    onUpdate(listener) {listeners.add(listener);return()=>listeners.delete(listener);},
    close() {failPending(); return worker.terminate();}};
}
