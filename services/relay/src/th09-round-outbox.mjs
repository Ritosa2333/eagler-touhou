import { mkdirSync } from 'node:fs';
import { dirname } from 'node:path';
import { DatabaseSync } from 'node:sqlite';

/** Preserve confirmed rounds across Relay restarts and retry delivery in order. */
export function createTh09RoundOutbox({ file, send, onStored, retryMs = 1000 }) {
  mkdirSync(dirname(file), { recursive: true, mode: 0o700 });
  const db = new DatabaseSync(file);
  db.exec(`PRAGMA journal_mode=WAL;
    CREATE TABLE IF NOT EXISTS pending_round (
      id INTEGER PRIMARY KEY,
      roundKey TEXT NOT NULL UNIQUE,
      payload TEXT NOT NULL
    )`);
  const head = db.prepare('SELECT id, payload FROM pending_round ORDER BY id LIMIT 1');
  const insert = db.prepare('INSERT INTO pending_round (roundKey, payload) VALUES (?, ?) ON CONFLICT DO NOTHING');
  const remove = db.prepare('DELETE FROM pending_round WHERE id = ?');
  const replayBacklog = db.prepare("SELECT count(*) AS count FROM pending_round WHERE roundKey LIKE '%/replay'");
  let closed = false;
  let draining = false;
  let timer;
  let delay = retryMs;

  function schedule(wait = 0) {
    if (closed || draining || timer) return;
    timer = setTimeout(() => {
      timer = undefined;
      void drain();
    }, wait);
    timer.unref();
  }

  async function drain() {
    draining = true;
    let retry = false;
    try {
      while (!closed) {
        const next = head.get();
        if (!next) break;
        const round = JSON.parse(next.payload);
        let result;
        try {
          result = await send(round);
        } catch (error) {
          if (closed) return;
          if (![400, 409, 413].includes(error?.status) && !(round.replay && error?.status === 404)) {
            console.warn('th09 round delivery delayed:', error?.code || error?.name);
            retry = true;
            break;
          }
          console.error('th09 round rejected:', round.duelId, round.roundId, error.code || error.status);
        }
        if (closed) return;
        remove.run(next.id);
        delay = retryMs;
        if (result) onStored?.(round, result.players, result.settlement);
      }
    } catch (error) {
      console.error('th09 round outbox failed:', error?.code || error?.name);
      retry = true;
    } finally {
      draining = false;
      if (retry) {
        schedule(delay);
        delay = Math.min(delay * 2, 30000);
      }
    }
  }

  schedule();
  return {
    enqueue(round) {
      if (closed || (!round.room && !round.players.some(Boolean)) || (round.players[0] && round.players[0] === round.players[1])) return;
      try {
        if (round.replay && replayBacklog.get().count >= 128) return;
        insert.run(`${round.duelId}/${round.roundId}${round.replay ? '/replay' : ''}`, JSON.stringify(round));
        schedule();
      } catch (error) {
        console.error('th09 round could not be queued:', round.duelId, round.roundId, error?.code || error?.name);
      }
    },
    close() {
      if (closed) return;
      closed = true;
      clearTimeout(timer);
      db.close();
    },
  };
}
