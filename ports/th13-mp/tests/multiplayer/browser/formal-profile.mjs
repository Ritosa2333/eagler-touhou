// Optional, read-only CDP sampling on the already running room pages.
// Keep the regular audit drain active while the returned capture runs.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';

const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');

export async function startCpuProfiles({pages, directory,
  seconds = Number(process.env.TH13_CPU_PROFILE_SECONDS || 0)}) {
  if (!seconds) return null;
  assert(Number.isFinite(seconds) && seconds > 0 && seconds <= 60, 'CPU profile duration in (0,60] seconds');
  const helper = await readFile(new URL(import.meta.url));
  const report = {scope: 'Read-only CDP CPU samples; diagnostic interval may affect wall-time performance',
    requestedSeconds: seconds, samplingIntervalUs: 1000, started: new Date().toISOString(),
    helperSha256: sha256(helper), seats: []};
  const captures = [];
  try {
  for (const [seat, page] of pages.entries()) {
    const session = await page.context().newCDPSession(page);
    captures.push({seat, session});
    const before = await page.evaluate(() => {
      const m = document.querySelector('#gameFrame')?.contentWindow?.Module;
      return {url: location.href, runtime: m?._th13_mp_diagnostic_report
        ? JSON.parse(m.UTF8ToString(m._th13_mp_diagnostic_report())) : null};
    });
    await session.send('Profiler.enable');
    await session.send('Profiler.setSamplingInterval', {interval: report.samplingIntervalUs});
    await session.send('Profiler.start');
    captures.at(-1).before = before;
  }
  } catch (error) {
    await Promise.allSettled(captures.map(async ({session}) => {
      try { await session.send('Profiler.disable'); } finally { await session.detach(); }
    }));
    throw error;
  }
  let timer, completion;
  const finish = () => completion ||= (async () => {
    clearTimeout(timer);
    const results = await Promise.allSettled(captures.map(async ({seat, session, before}) => {
      try {
        const {profile} = await session.send('Profiler.stop');
        const file = resolve(directory, `cpu-seat-${seat}.cpuprofile`);
        const bytes = Buffer.from(JSON.stringify(profile) + '\n');
        await writeFile(file, bytes, {flag: 'wx'});
        return {seat, before, file, bytes: bytes.length, sha256: sha256(bytes),
          startedUs: profile.startTime, finishedUs: profile.endTime,
          samples: profile.samples?.length || 0, nodes: profile.nodes.length};
      } finally { await session.detach(); }
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
