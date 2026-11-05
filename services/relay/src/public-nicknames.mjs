import {DatabaseSync} from 'node:sqlite';
import {createHmac, randomBytes} from 'node:crypto';
import {mkdirSync, chmodSync} from 'node:fs';
import {dirname} from 'node:path';
import {normalizePublicNickname} from '../../../common/contracts/public-nickname.mjs';
import {isReservedPublicNickname, PUBLIC_NICKNAME_POLICY as POLICY} from './nickname-policy.mjs';
import {nicknameAddressScope} from './nickname-request-guard.mjs';

const DAY = 86400000;

/** A member ID is an opaque anonymous browser credential, not a person/account.
 * IP buckets limit review abuse independently, so NAT does not impose one shared
 * 24-hour nickname on everyone. All durable mutations precede publication. */
export function createPublicNicknames({file, review, now = Date.now, hourlyLimit = 5, dailyLimit = 20,
  reviewBudget = 1000, maxConcurrent = 1, maxQueued = 1000, maxNames = 50000,
  maxProfiles = 500000, providerIpBudget = 30, onUpdate = () => {}}) {
  if (file !== ':memory:') mkdirSync(dirname(file), {recursive: true, mode: 0o700});
  const db = new DatabaseSync(file);
  if (file !== ':memory:') chmodSync(file,0o600);
  db.exec(`PRAGMA journal_mode=WAL; PRAGMA synchronous=FULL;
    CREATE TABLE IF NOT EXISTS meta (key TEXT PRIMARY KEY, value TEXT NOT NULL);
    CREATE TABLE IF NOT EXISTS names (name TEXT NOT NULL, policy TEXT NOT NULL, approved INTEGER NOT NULL, PRIMARY KEY(name,policy));
    CREATE TABLE IF NOT EXISTS profiles (id TEXT PRIMARY KEY, name TEXT NOT NULL, changed INTEGER NOT NULL);
    CREATE TABLE IF NOT EXISTS buckets (id TEXT PRIMARY KEY, count INTEGER NOT NULL, expires INTEGER NOT NULL);
    CREATE TABLE IF NOT EXISTS review_queue (member TEXT PRIMARY KEY, name TEXT NOT NULL, ip TEXT NOT NULL, created INTEGER NOT NULL, attempts INTEGER NOT NULL DEFAULT 0, next_attempt INTEGER NOT NULL);
    CREATE INDEX IF NOT EXISTS review_queue_due ON review_queue(next_attempt,created);
    CREATE INDEX IF NOT EXISTS review_queue_ip ON review_queue(ip);
    CREATE INDEX IF NOT EXISTS buckets_expiry ON buckets(expires);`);
  const profileColumns = new Set(db.prepare('PRAGMA table_info(profiles)').all().map(column => column.name));
  for (const column of ['cooldown_until','failures','failure_until'])
    if (!profileColumns.has(column)) db.exec(`ALTER TABLE profiles ADD COLUMN ${column} INTEGER NOT NULL DEFAULT 0`);
  if (!profileColumns.has('approval_policy')) {
    db.exec("ALTER TABLE profiles ADD COLUMN approval_policy TEXT NOT NULL DEFAULT ''");
    db.prepare(`UPDATE profiles SET approval_policy=? WHERE EXISTS
      (SELECT 1 FROM names WHERE names.name=profiles.name AND names.policy=? AND approved=1)`).run(POLICY,POLICY);
  }
  db.exec("CREATE INDEX IF NOT EXISTS profiles_empty_expiry ON profiles(failure_until,cooldown_until) WHERE name=''");
  if (!db.prepare('PRAGMA table_info(review_queue)').all().some(column=>column.name==='lease'))
    db.exec("ALTER TABLE review_queue ADD COLUMN lease TEXT NOT NULL DEFAULT ''");
  db.prepare('INSERT OR IGNORE INTO meta VALUES (?,?)').run('salt', randomBytes(32).toString('hex'));
  const salt = db.prepare('SELECT value FROM meta WHERE key=?').get('salt').value;
  const hash = value => createHmac('sha256', salt).update(value).digest('hex');
  const pendingMembers = new Map(), pendingNames = new Map(), pendingIps = new Map(), activeJobs = new Set();
  let active = 0, closed = false, maintenanceAt = 0;
  function maintain() {
    if (now() < maintenanceAt) return;
    maintenanceAt = now() + 60000;
    db.prepare('DELETE FROM buckets WHERE id IN (SELECT id FROM buckets WHERE expires<=? LIMIT 1000)').run(now());
    db.prepare(`DELETE FROM profiles WHERE id IN (SELECT id FROM profiles WHERE name='' AND failure_until<=?
      AND cooldown_until<=? AND NOT EXISTS (SELECT 1 FROM review_queue WHERE member=profiles.id) LIMIT 1000)`).run(now(),now());
  }
  function hasProfileSpace(member, hashed = false) {
    return !!db.prepare('SELECT 1 FROM profiles WHERE id=?').get(hashed ? member : hash(member)) ||
      db.prepare('SELECT COUNT(*) AS n FROM profiles').get().n < maxProfiles;
  }
  function providerState() {
    const row = db.prepare('SELECT value FROM meta WHERE key=?').get('provider-state');
    return row ? JSON.parse(row.value) : {failures:0,until:0};
  }
  function saveProviderState(value) {
    db.prepare('INSERT OR REPLACE INTO meta VALUES (?,?)').run('provider-state',JSON.stringify(value));
  }
  function reserveReview(ip) {
    // A shared SQLite reservation also keeps budgets exact during a handover.
    db.exec('BEGIN IMMEDIATE');
    try {
      const pause = providerState().until;
      let retryAt = pause > now() ? pause : 0;
      for (const [id,limit] of [['review-budget',reviewBudget],['provider:'+ip,providerIpBudget]]) {
        const row = db.prepare('SELECT count,expires FROM buckets WHERE id=?').get(id);
        if (row?.expires > now() && row.count >= limit) retryAt = Math.max(retryAt,row.expires);
      }
      if (!retryAt) {bucket('review-budget',DAY,reviewBudget);bucket('provider:'+ip,DAY,providerIpBudget);}
      db.exec('COMMIT');return retryAt;
    } catch (error) {db.exec('ROLLBACK');throw error;}
  }
  function profile(member, hashed = false) {
    const value = db.prepare('SELECT name,changed,cooldown_until,failures,failure_until,approval_policy FROM profiles WHERE id=?').get(hashed ? member : hash(member));
    if (!value?.name) return value;
    const approved = !isReservedPublicNickname(value.name) && value.approval_policy === POLICY;
    // A policy upgrade quarantines old approvals until the name is reviewed again.
    // Preserve the stored record, but permit correction without its old cooldown.
    return approved ? value : {...value,name:'',cooldown_until:0,failures:0,failure_until:0};
  }
  function current(member, hashed = false, includeQueue = true) {
    const value = profile(member, hashed), failed = value?.failure_until > now() ? value.failures : 0;
    const state = {name: value?.name || '', status: failed >= 2 && value.cooldown_until > now() ? 'cooldown' : failed === 1 ? 'rejected' : value?.name ? 'approved' : 'idle',
      nextChangeAt: value?.cooldown_until || 0, retryAt: 0, attemptsLeft: failed ? Math.max(0, 2 - failed) : null};
    const job = includeQueue && db.prepare('SELECT name FROM review_queue WHERE member=?').get(hashed ? member : hash(member));
    return job ? {...state,status:'queued',pendingName:job.name} : state;
  }
  function saveProfile(member, patch, hashed = false) {
    const previous = profile(member, hashed);
    if (!previous && !hasProfileSpace(member,hashed)) return false;
    const value = {name:'',changed:0,cooldown_until:0,failures:0,failure_until:0,approval_policy:'',...previous,...patch};
    db.prepare(`INSERT INTO profiles (id,name,changed,cooldown_until,failures,failure_until,approval_policy) VALUES (?,?,?,?,?,?,?)
      ON CONFLICT(id) DO UPDATE SET name=excluded.name,changed=excluded.changed,cooldown_until=excluded.cooldown_until,failures=excluded.failures,failure_until=excluded.failure_until,approval_policy=excluded.approval_policy`)
      .run(hashed ? member : hash(member),value.name,value.changed,value.cooldown_until,value.failures,value.failure_until,value.approval_policy);
    return true;
  }
  function rejectName(member, hashed = false) {
    const value = profile(member, hashed), time = now(), exhausted = value?.failure_until > time && value.failures >= 1;
    if (!saveProfile(member,{failures:exhausted ? 2 : 1,failure_until:time + DAY,...(exhausted ? {cooldown_until:time + DAY} : {})},hashed)) return null;
    return current(member,hashed,false);
  }
  function bucket(id, duration, limit) {
    const time = now(), row = db.prepare('SELECT count,expires FROM buckets WHERE id=?').get(id);
    if (row && row.expires > time && row.count >= limit) return row.expires;
    db.prepare('INSERT INTO buckets VALUES (?,?,?) ON CONFLICT(id) DO UPDATE SET count=excluded.count,expires=excluded.expires')
      .run(id, row && row.expires > time ? row.count + 1 : 1, row && row.expires > time ? row.expires : time + duration);
    return 0;
  }
  function submit(member, address, proposed, {queued = false, lease = ''} = {}) {
    if (closed || !/^[A-Za-z0-9_-]{8,64}$/.test(member) || typeof address !== 'string' || !address)
      return Promise.resolve({name: '', status: 'unavailable', nextChangeAt: 0, retryAt: 0, attemptsLeft:null});
    maintain();
    const name = normalizePublicNickname(proposed), old = current(member,queued,false);
    const result = (status, retryAt = 0) => ({...old, status, retryAt});
    function finish(change) {
      if (!queued) return change();
      // Commit the verdict and dequeue together; a crash cannot spend a retry twice.
      db.exec('BEGIN IMMEDIATE');
      try {
        if (!db.prepare('SELECT 1 FROM review_queue WHERE member=? AND lease=?').get(member,lease)) {
          db.exec('COMMIT');return result('unavailable');
        }
        const value = change();
        if (value.status !== 'unavailable') db.prepare('DELETE FROM review_queue WHERE member=?').run(member);
        db.exec('COMMIT');return value;
      } catch (error) {db.exec('ROLLBACK');throw error;}
    }
    if (!name || !/[\p{L}\p{N}\p{Extended_Pictographic}]/u.test(name)) return Promise.resolve(result('rejected'));
    if (name === old.name) return Promise.resolve(finish(()=>old));
    if (old.nextChangeAt > now()) return Promise.resolve(finish(()=>result('cooldown', old.nextChangeAt)));
    if (!hasProfileSpace(member,queued)) return Promise.resolve(result('unavailable',now()+60000));
    const previous = pendingMembers.get(member);
    if (previous) return previous.name === name ? previous.promise : Promise.resolve(result('rate-limited', now() + 10000));
    const ip = queued ? address : hash(nicknameAddressScope(address));
    const retryAt = queued ? 0 : bucket('hour:' + ip, 3600000, hourlyLimit) || bucket('day:' + ip, DAY, dailyLimit);
    if (retryAt || (pendingIps.get(ip) || 0) >= 2) return Promise.resolve(result('rate-limited', retryAt || now() + 10000));
    const promise = (async () => {
      try {
        let approved = isReservedPublicNickname(name) ? 0 : db.prepare('SELECT approved FROM names WHERE name=? AND policy=?').get(name, POLICY)?.approved;
        if (approved === undefined) {
          let task = pendingNames.get(name);
          if (!task) {
            if (active >= maxConcurrent) return result('unavailable', now() + 30000);
            const budgetAt = reserveReview(ip);
            if (budgetAt) return result('unavailable', budgetAt);
            ++active;
            task = Promise.resolve().then(async () => {
              let value;
              try {
                value = await review(name);
                if (typeof value !== 'boolean') throw new Error('Invalid review result');
              } catch (error) {
                if (!closed) {
                  const failures = Math.min(providerState().failures+1,8);
                  saveProviderState({failures,until:now()+Math.min(3600000,30000*2**(failures-1))});
                }
                throw error;
              }
              if (!closed) {
                saveProviderState({failures:0,until:0});
                if (db.prepare('SELECT COUNT(*) AS n FROM names').get().n >= maxNames)
                  db.prepare('DELETE FROM names WHERE rowid IN (SELECT rowid FROM names ORDER BY rowid LIMIT ?)').run(Math.max(1,Math.ceil(maxNames/50)));
                db.prepare('INSERT OR REPLACE INTO names VALUES (?,?,?)').run(name, POLICY, +value);
              }
              return +value;
            }).finally(() => {--active; pendingNames.delete(name);});
            pendingNames.set(name, task);
          }
          approved = await task;
        }
        if (closed) return result('unavailable');
        if (!approved) return finish(()=>rejectName(member,queued) || result('unavailable'));
        // An inactive browser keeps its approved name across server restarts.
        const previous = profile(member,queued), time = now();
        const lastChance = previous?.failure_until > time && previous.failures >= 1;
        // The first accepted name is free unless it used the final retry.
        return finish(()=>{
          if (!saveProfile(member,{name,approval_policy:POLICY,changed:time,cooldown_until:previous?.name || lastChance ? time + DAY : 0,failures:0,failure_until:0},queued)) return result('unavailable');
          return current(member,queued,false);
        });
      } catch { return result('unavailable', Math.max(now()+10000,closed ? 0 : providerState().until)); }
    })().finally(() => {
      pendingMembers.delete(member);
      const count = (pendingIps.get(ip) || 1) - 1;
      if (count) pendingIps.set(ip, count); else pendingIps.delete(ip);
    });
    pendingMembers.set(member, {name, promise}); pendingIps.set(ip, (pendingIps.get(ip) || 0) + 1);
    return promise;
  }
  function enqueue(member, address, proposed) {
    if (closed || !/^[A-Za-z0-9_-]{8,64}$/.test(member) || typeof address !== 'string' || !address)
      return {name:'',status:'unavailable',nextChangeAt:0,retryAt:0,attemptsLeft:null};
    maintain();
    const old = current(member), name = normalizePublicNickname(proposed);
    if (old.status === 'queued') return old; // Reconnects and duplicate clicks are idempotent.
    if (!name || !/[\p{L}\p{N}\p{Extended_Pictographic}]/u.test(name)) return {...old,status:'rejected'};
    if (name === old.name) return old;
    if (old.nextChangeAt > now()) return {...old,status:'cooldown',retryAt:old.nextChangeAt};
    if (!hasProfileSpace(member)) return {...old,status:'unavailable',retryAt:now()+60000};
    // Cached verdicts need no upstream wait or durable job.
    if (isReservedPublicNickname(name) || db.prepare('SELECT approved FROM names WHERE name=? AND policy=?').get(name,POLICY))
      return submit(member,address,name);
    if (db.prepare('SELECT COUNT(*) AS n FROM review_queue').get().n >= maxQueued) return {...old,status:'unavailable'};
    const ip = hash(nicknameAddressScope(address));
    if (db.prepare('SELECT COUNT(*) AS n FROM review_queue WHERE ip=?').get(ip).n >= dailyLimit)
      return {...old,status:'rate-limited',retryAt:now()+3600000};
    const retryAt = bucket('hour:'+ip,3600000,hourlyLimit) || bucket('day:'+ip,DAY,dailyLimit);
    if (retryAt) return {...old,status:'rate-limited',retryAt};
    db.prepare('INSERT INTO review_queue(member,name,ip,created,next_attempt) VALUES (?,?,?,?,?)').run(hash(member),name,ip,now(),now());
    return current(member);
  }
  async function processQueue() {
    if (closed || activeJobs.size >= maxConcurrent) return;
    maintain();
    if (providerState().until > now()) return;
    // Claim durably before awaiting the provider. A crashed worker releases its
    // job after a minute; an overlapping worker cannot spend a second retry.
    const lease = randomBytes(16).toString('hex');
    const jobs = db.prepare(`UPDATE review_queue SET lease=?,next_attempt=? WHERE member IN
      (SELECT member FROM review_queue WHERE next_attempt<=? ORDER BY next_attempt,created LIMIT ?) RETURNING *`)
      .all(lease,now()+60000,now(),maxConcurrent-activeJobs.size);
    await Promise.all(jobs.map(async job=>{
      activeJobs.add(job.member);
      try {
        const value = await submit(job.member,job.ip,job.name,{queued:true,lease});
        if (closed) return;
        if (['unavailable','rate-limited'].includes(value.status)) {
          const delay = Math.min(3600000,30000 * 2 ** Math.min(job.attempts,7));
          db.prepare('UPDATE review_queue SET attempts=attempts+1,next_attempt=? WHERE member=? AND lease=?').run(Math.max(now()+delay,value.retryAt||0),job.member,lease);
        } else onUpdate(job.member,current(job.member,true));
      } finally {activeJobs.delete(job.member);}
    }));
  }
  return {current, submit, enqueue, processQueue, memberKey:hash, close() {if (closed) return;closed = true; db.close();}};
}
