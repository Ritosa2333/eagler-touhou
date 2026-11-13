// Optional, read-only CDP sampling on the already running room pages.
// Keep the regular audit drain active while the returned capture runs.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {boundedOperation} from './bounded-operation.mjs';

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

export async function startCpuProfiles({pages, sessions, directory,
  seconds = Number(process.env.TH13_CPU_PROFILE_SECONDS || 0), deadlineMs = 15000}) {
  if (!seconds) return null;
  assert(Number.isFinite(seconds) && seconds > 0 && seconds <= 60, 'CPU profile duration in (0,60] seconds');
  assert(Array.isArray(sessions) && sessions.length === pages.length && sessions.every(Boolean),
    'Profiling requires the existing per-page CPU throttle sessions');
  const helper = await readFile(new URL(import.meta.url));
  const report = {scope: 'Read-only CDP CPU samples; diagnostic interval may affect wall-time performance',
    requestedSeconds: seconds, samplingIntervalUs: 1000, started: new Date().toISOString(),
    helperSha256: sha256(helper), seats: []};
  const captures = [];
  try {
  for (const [seat, page] of pages.entries()) {
    const session = sessions[seat];
    captures.push({seat, session});
    const before = await boundedOperation(`profile seat ${seat} state`,()=>page.evaluate(() => {
      const m = document.querySelector('#gameFrame')?.contentWindow?.Module;
      return {url: location.href, runtime: m?._th13_mp_diagnostic_report
        ? JSON.parse(m.UTF8ToString(m._th13_mp_diagnostic_report())) : null};
    }),deadlineMs);
    await boundedOperation(`profile seat ${seat} enable`,()=>session.send('Profiler.enable'),deadlineMs);
    await boundedOperation(`profile seat ${seat} interval`,()=>session.send('Profiler.setSamplingInterval', {interval: report.samplingIntervalUs}),deadlineMs);
    await boundedOperation(`profile seat ${seat} start`,()=>session.send('Profiler.start'),deadlineMs);
    captures.at(-1).before = before;
  }
  } catch (error) {
    await Promise.allSettled(captures.map(({session}) =>
      boundedOperation('profile disable',()=>session.send('Profiler.disable'),deadlineMs)));
    throw error;
  }
  let timer, completion;
  const finish = () => completion ||= (async () => {
    clearTimeout(timer);
    const results = await Promise.allSettled(captures.map(async ({seat, session, before}) => {
      const {profile} = await boundedOperation(`profile seat ${seat} stop`,()=>session.send('Profiler.stop'),deadlineMs);
      const file = resolve(directory, `cpu-seat-${seat}.cpuprofile`);
      const bytes = Buffer.from(JSON.stringify(profile) + '\n');
      await writeFile(file, bytes, {flag: 'wx'});
      return {seat, before, file, bytes: bytes.length, sha256: sha256(bytes),
        startedUs: profile.startTime, finishedUs: profile.endTime,
        samples: profile.samples?.length || 0, nodes: profile.nodes.length};
    }));
    report.seats = results.map((result, seat) => result.status === 'fulfilled'
      ? result.value : {seat, error: String(result.reason)});
    report.finished = new Date().toISOString();
    report.passed = report.seats.every(row => !row.error && row.samples > 0);
    await writeFile(resolve(directory, 'cpu-profiles.json'), JSON.stringify(report, null, 2) + '\n');
    return report;
  })();
  timer = setTimeout(() => { finish().catch(() => {}); }, seconds * 1000);
  return {finish};
}
