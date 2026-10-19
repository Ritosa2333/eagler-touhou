import {spawn, execFileSync} from 'node:child_process';
import {randomUUID} from 'node:crypto';
import {mkdir, readFile, writeFile, rename} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createWriteStream} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {acquireOutputLock} from './output-lock.mjs';
import {isFullWindowsImagePath} from './windows-image-path.mjs';

function stop(child) {
  if (child.exitCode !== null || child.signalCode !== null || !child.pid) return;
  if (process.platform === 'win32') {
    spawn('taskkill.exe', ['/pid', String(child.pid), '/T', '/F'], {windowsHide:true, stdio:'ignore'});
  } else {
    // Nested test drivers may create another process group. PPID ancestry,
    // rather than the outer process group alone, owns their lifetime.
    try {
      process.kill(child.pid, 'SIGSTOP');
      const descendants = new Set([child.pid]);
      for (let changed=true; changed;) {
        changed=false;
        const rows = execFileSync('ps', ['-axo','pid=,ppid='], {encoding:'utf8'})
          .trim().split('\n').map(row=>row.trim().split(/\s+/).map(Number));
        for (const [pid, parent] of rows) if (descendants.has(parent) && !descendants.has(pid)) {
          try {process.kill(pid, 'SIGSTOP'); descendants.add(pid); changed=true;}
          catch (error) {if (error.code !== 'ESRCH') throw error;}
        }
      }
      for (const pid of [...descendants].reverse()) {
        try {process.kill(pid, 'SIGKILL');} catch (error) {if (error.code !== 'ESRCH') throw error;}
      }
    } catch (error) {
      try {process.kill(child.pid, 'SIGKILL');} catch (killError) {if (killError.code !== 'ESRCH') throw killError;}
      if (error.code !== 'ESRCH') throw error;
    }
  }
}

async function execute(task, {output, environment, unsetEnvironment, signal, cleanupImages}) {
  const startedAt = new Date().toISOString(), start = performance.now();
  const log = resolve(output, `${task.id}.log`), stream = createWriteStream(log);
  const hasCleanupDeclaration = cleanupImages?.length > 0;
  const cleanupReportPath = hasCleanupDeclaration ? resolve(output, `${task.id}.cleanup-${randomUUID()}.json`) : undefined;
  let child, timer, timedOut = false, cancelled = false, logError;
  stream.on('error', error => {logError=error.message; if (child) stop(child);});
  const result = await new Promise(accept => {
    const finish = (code, exitSignal, error) => accept({code, signal:exitSignal, ...(error ? {error} : {})});
    const syntaxOnly=task.command === process.execPath && task.args?.[0] === '--check';
    const supervised= !syntaxOnly || hasCleanupDeclaration;
    const command=supervised ? process.env.TH_PYTHON ?? process.env.PYTHON ?? (process.platform === 'win32' ? 'python' : 'python3') : task.command;
    const args=supervised ? [fileURLToPath(new URL('./supervise.py', import.meta.url)),
      ...(hasCleanupDeclaration ? ['--cleanup-report', cleanupReportPath,
        ...cleanupImages.flatMap(image=>['--cleanup-image', image.path, image.sha256])] : []),
      '--', task.command, ...(task.args ?? [])] : task.args;
    const childEnvironment={...process.env};
    for (const key of unsetEnvironment) delete childEnvironment[key];
    Object.assign(childEnvironment,environment);
    for (const key of unsetEnvironment) delete childEnvironment[key];
    Object.assign(childEnvironment,task.environment,{PYTHONDONTWRITEBYTECODE:'1'});
    delete childEnvironment.NODE_OPTIONS;
    child = spawn(command, args, {cwd:task.cwd, shell:false, windowsHide:true,
      detached:process.platform !== 'win32', stdio:['ignore','pipe','pipe'],
      env:childEnvironment});
    child.stdout.pipe(stream, {end:false});
    child.stderr.pipe(stream, {end:false});
    child.once('error', error => finish(null, null, error.message));
    child.once('close', (code, exitSignal) => finish(code, exitSignal));
    timer = setTimeout(() => {timedOut = true; stop(child);}, task.timeoutMs);
    timer.unref();
    const cancel = () => {cancelled = true; stop(child);};
    if (signal?.aborted) cancel();
    else signal?.addEventListener('abort', cancel, {once:true});
    child.once('close', () => signal?.removeEventListener('abort', cancel));
  });
  clearTimeout(timer);
  if (!stream.destroyed) await new Promise(accept => {stream.once('error', accept); stream.end(accept);});
  let cleanupEvidence;
  if (hasCleanupDeclaration) {
    try {
      cleanupEvidence = {...JSON.parse(await readFile(cleanupReportPath, 'utf8')), reportFile:cleanupReportPath};
    } catch (error) {
      cleanupEvidence = {schema:'eagler-touhou/process-cleanup/1', declared:cleanupImages, observed:[], remaining:[],
        status:'not-produced', error:`Could not read cleanup evidence: ${error.message}`, reportFile:cleanupReportPath};
    }
  }
  const cleanupStatusOk = !cleanupEvidence || ['not-requested','passed'].includes(cleanupEvidence.status) ||
    (cleanupEvidence.status === 'command-failed' && result.code !== 0);
  return {id:task.id, command:[task.command, ...(task.args ?? [])], cwd:task.cwd,
    startedAt, durationMs:Math.round(performance.now() - start), log,
    status:timedOut ? 'timeout' : cancelled ? 'cancelled' : result.code === 0 && !result.error && !logError && cleanupStatusOk ? 'passed' : 'failed',
    ...result, ...(cleanupEvidence ? {cleanupImages:cleanupEvidence} : {}), ...(logError ? {error:`Test log failed: ${logError}`} : {})};
}

/** A selected task must finish successfully. Missing inputs and timeouts fail. */
export async function runTasks(tasks, {output, concurrency=4, environment={}, signal, failFast=false,
  onResult=()=>{}, metadata={},unsetEnvironment=[]} = {}) {
  if (!Array.isArray(tasks) || !tasks.length) throw new Error('No tests selected');
  if (!Number.isSafeInteger(concurrency) || concurrency < 1 || concurrency > 32)
    throw new Error('Test concurrency must be an integer from 1 to 32');
  if (!Array.isArray(unsetEnvironment)||unsetEnvironment.some(key=>typeof key!=='string'||!/^[-A-Z0-9_]+$/i.test(key)))
    throw new Error('unsetEnvironment must contain valid environment variable names');
  const ids = new Set();
  const cleanupDeclarations = [];
  const cleanupByTask = new Map();
  for (const task of tasks) {
    if (!/^[a-zA-Z0-9][a-zA-Z0-9._-]*$/.test(task.id) || ids.has(task.id))
      throw new Error(`Invalid or duplicate task id: ${task.id}`);
    ids.add(task.id);
    if (!task.command || !task.cwd || !Number.isSafeInteger(task.timeoutMs) || task.timeoutMs < 1)
      throw new Error(`Task requires a command, cwd and timeout: ${task.id}`);
    if (task.cleanupImages !== undefined) {
      if (!Array.isArray(task.cleanupImages)) throw new Error(`cleanupImages must be an array: ${task.id}`);
      const normalized = task.cleanupImages.map((image, index) => {
        if (!image || typeof image !== 'object' || Array.isArray(image) ||
            Object.keys(image).some(key=>!['path','sha256'].includes(key)) ||
            typeof image.path !== 'string' || !image.path.trim() ||
            !isFullWindowsImagePath(image.path) ||
            typeof image.sha256 !== 'string' || !/^[0-9a-f]{64}$/i.test(image.sha256))
          throw new Error(`cleanupImages[${index}] requires a drive-qualified or UNC-share image path and SHA-256: ${task.id}`);
        return {path:image.path, sha256:image.sha256.toLowerCase()};
      });
      const identities = normalized.map(image=>`${image.path.replaceAll('\\','/').toLowerCase()}|${image.sha256}`);
      if (new Set(identities).size !== identities.length) throw new Error(`cleanupImages contains duplicates: ${task.id}`);
      cleanupByTask.set(task.id, normalized);
      if (normalized.length) cleanupDeclarations.push({id:task.id, images:normalized});
    }
  }
  output = resolve(output);
  const started = performance.now();
  await mkdir(output, {recursive:true});
  const release=await acquireOutputLock(output);
  try {
  const report = {schema:'eagler-touhou/test-run/1', ...metadata,
    cleanupDeclarations,
    startedAt:new Date().toISOString(), selected:tasks.map(task=>task.id), results:[], status:'running'};
  const save = async () => {
    const pending = resolve(output, 'report.pending.json');
    await writeFile(pending, JSON.stringify(report, null, 2) + '\n');
    await rename(pending, resolve(output, 'report.json'));
  };
  await save();
  const pending = [...tasks], active = new Map(), held = new Set();
  const controller = new AbortController();
  const taskSignal = signal ? AbortSignal.any([signal, controller.signal]) : controller.signal;
  let stopping = false;
  try {
  while (pending.length || active.size) {
    if (!signal?.aborted && !stopping) for (let i=0; i<pending.length && active.size<concurrency;) {
      const task = pending[i], resources = task.resources ?? [];
      if (resources.some(resource=>held.has(resource))) {i++; continue;}
      pending.splice(i, 1);
      resources.forEach(resource=>held.add(resource));
      active.set(task.id, execute(task, {output, environment, unsetEnvironment, signal:taskSignal,
        cleanupImages:cleanupByTask.get(task.id)}).then(result=>({task, result})));
    }
    if (!active.size) {
      for (const task of pending.splice(0)) report.results.push({id:task.id, status:'cancelled', durationMs:0});
      break;
    }
    const {task, result} = await Promise.race(active.values());
    active.delete(task.id);
    (task.resources ?? []).forEach(resource=>held.delete(resource));
    report.results.push(result);
    if (failFast && result.status !== 'passed') stopping = true;
    await save();
    await onResult(result);
  }
  } catch (error) {
    controller.abort();
    const unfinished = [...active.entries()];
    const settled = await Promise.allSettled(unfinished.map(([, promise])=>promise));
    settled.forEach((outcome, index)=>report.results.push(outcome.status === 'fulfilled' ? outcome.value.result :
      {id:unfinished[index][0], status:'failed', durationMs:0, error:String(outcome.reason)}));
    for (const task of pending) report.results.push({id:task.id, status:'cancelled', durationMs:0});
    report.status='failed'; report.error=String(error.message ?? error);
    report.finishedAt=new Date().toISOString(); report.durationMs=Math.round(performance.now()-started);
    await save().catch(()=>{});
    throw error;
  }
  report.status = report.results.length === tasks.length && report.results.every(result=>result.status === 'passed') ? 'passed' : 'failed';
  report.finishedAt = new Date().toISOString();
  report.durationMs = Math.round(performance.now() - started);
  report.taskDurationMs = report.results.reduce((sum, result)=>sum + result.durationMs, 0);
  await save();
  return report;
  } finally {await release();}
}
