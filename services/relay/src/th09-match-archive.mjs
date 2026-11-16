import { gzip } from 'node:zlib';
import { decodeTh09MatchArchive, encodeTh09MatchArchive, TH09_MATCH_ARCHIVE_COMPRESSED_LIMIT } from '../../../common/contracts/th09-match-archive.mjs';

export function createTh09MatchArchiveQueue(outbox) {
  // Compression is outside the packet handler. Bound in-flight memory and CPU
  // across rooms, including when the account service is momentarily unavailable.
  let pending = 0;
  return (run, report) => {
    if (!outbox || !run.spectatorTimeline) return;
    if (report) {
      if (!report.players.some(Boolean)) return;
      const start = Number(report.roundId.split(':')[1]);
      const queue = run.th09MatchesToArchive ??= new Map();
      if (queue.size >= 4) queue.delete(queue.keys().next().value);
      queue.set(start, report);
    }
    flushTh09MatchArchives(run, enqueue);
  };

  function enqueue(history, round, start) {
    if (pending >= 4) return;
    ++pending;
    setImmediate(() => {
      let bytes;
      try {
        bytes = encodeTh09MatchArchive(history, {...round, start});
        if (!bytes) { --pending; return; }
        decodeTh09MatchArchive(bytes);
      } catch {
        --pending;
        return;
      }
      gzip(bytes, (error, compressed) => {
        --pending;
        if (error || compressed.length > TH09_MATCH_ARCHIVE_COMPRESSED_LIMIT) return;
        outbox.enqueue({...round, replay: compressed.toString('base64')});
      });
    });
  }
}

function flushTh09MatchArchives(run, enqueue) {
  if (!run.th09MatchesToArchive?.size) return;
  for (const [start, report] of run.th09MatchesToArchive) {
    if (start + report.frames > (run.spectatorNextFrame ?? 0)) continue;
    run.th09MatchesToArchive.delete(start);
    if (!run.spectatorHistoryUnavailable) enqueue(run.spectatorHistory.slice(), report, start);
  }
}
