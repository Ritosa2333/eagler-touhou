import {test, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {mkdir, mkdtemp, readFile, readdir, rm, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {spawn} from 'node:child_process';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {acquireBuildLock} from '../../lib/build-lock.mjs';
import {ensureI18nBuild} from '../../lib/i18n-build.mjs';
import {nativeNodeExecutable} from '../support/node-runtime.mjs';
import {resolveBunExecutable} from '../../host/lib/bun-environment.mjs';
import {setTimeout as delay} from 'node:timers/promises';

const root = fileURLToPath(new URL('../../', import.meta.url));
await mkdir(resolve(root, '.cache'), {recursive: true});
const work = await mkdtemp(resolve(root, '.cache/i18n-tests-'));
after(() => rm(work, {recursive: true, force: true}));
let serial = 0;
async function fixture() {
  const project = resolve(work, 'project-' + ++serial);
  for (const directory of ['messages', 'project.inlang', 'node_modules/@inlang/paraglide-js', 'node_modules/@inlang/plugin-message-format']) await mkdir(resolve(project, directory), {recursive: true});
  for (const path of ['project.inlang/settings.json', 'node_modules/@inlang/paraglide-js/package.json', 'node_modules/@inlang/plugin-message-format/package.json']) await writeFile(resolve(project, path), '{}');
  await writeFile(resolve(project, 'messages/en.json'), 'first');
  return project;
}
async function compile({project, outdir}) {
  const text = await readFile(resolve(project, '../messages/en.json'), 'utf8');
  await writeFile(resolve(outdir, 'messages.js'), 'export const text = ' + JSON.stringify(text) + ';');
  await writeFile(resolve(outdir, 'messages.d.ts'), 'export declare const text: string;');
  await writeFile(resolve(outdir, 'runtime.js'), 'export const baseLocale = "zh-CN";');
}
const cache = project => resolve(project, '.cache/i18n');
async function current(project) {return JSON.parse(await readFile(resolve(cache(project), 'current.json'), 'utf8'));}
async function message(project) {return import(pathToFileURL(resolve(project, 'src/paraglide/messages.js')).href + '?v=' + ++serial);}

test('a retired lock release cannot remove a later owner; contention is bounded', async () => {
  const path = resolve(work, 'owner.lock'), before = process.listenerCount('exit');
  const first = await acquireBuildLock(path); await first();
  const second = await acquireBuildLock(path); await first(); assert.equal(existsSync(path), true);
  await assert.rejects(acquireBuildLock(path, {timeout: 5, interval: 1}), /Timed out waiting/);
  await second(); await second(); const third = await acquireBuildLock(path); await third();
  assert.equal(process.listenerCount('exit'), before);
});

test('Bun lock host uses the validated Node override and caches it', async () => {
  if (!process.versions.bun) return;
  const path = resolve(work, 'explicit-node.lock');
  const script = `
    import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};
    const first=await acquireBuildLock(${JSON.stringify(path)}); await first();
    process.env.EAGLER_BUILD_LOCK_NODE=process.execPath;
    const second=await acquireBuildLock(${JSON.stringify(path)}); await second();
    console.log('cached-node-ok');
  `;
  const child = spawn(resolveBunExecutable(), ['--eval', script], {
    cwd: root,
    env: {...process.env, EAGLER_BUILD_LOCK_NODE: nativeNodeExecutable()},
    windowsHide: true,
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let output = '', error = '';
  child.stdout.setEncoding('utf8'); child.stdout.on('data', chunk => {output += chunk;});
  child.stderr.setEncoding('utf8'); child.stderr.on('data', chunk => {error += chunk;});
  const code = await new Promise((yes, no) => {
    child.once('error', no);
    child.once('exit', (status, signal) => status === 0 ? yes(status) : no(new Error(`Bun Node-host probe failed (${signal ?? status}): ${error}\n${output}`)));
  });
  assert.equal(code, 0);
  assert.match(output, /cached-node-ok/);
});

test('Bun process exit without release closes the helper pipe and releases its lock', async () => {
  if (!process.versions.bun) return;
  const path = resolve(work, 'forgotten-release.lock');
  const script = `
    import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};
    import {readFileSync} from 'node:fs';
    await acquireBuildLock(${JSON.stringify(path)});
    const child=process.platform==='linux'?readFileSync('/proc/self/task/'+process.pid+'/children','utf8').trim():'';
    console.log('owner-exiting:'+child);
  `;
  const child = spawn(resolveBunExecutable(), ['--eval', script], {
    cwd: root,
    env: {...process.env, EAGLER_BUILD_LOCK_NODE: nativeNodeExecutable()},
    windowsHide: true,
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let output = '', error = '';
  child.stdout.setEncoding('utf8'); child.stdout.on('data', chunk => {output += chunk;});
  child.stderr.setEncoding('utf8'); child.stderr.on('data', chunk => {error += chunk;});
  const code = await new Promise((yes, no) => {
    child.once('error', no);
    child.once('exit', (status, signal) => status === 0 ? yes(status) : no(new Error(`Bun forgot-release probe failed (${signal ?? status}): ${error}\n${output}`)));
  });
  assert.equal(code, 0);
  assert.match(output, /owner-exiting/);
  if (process.platform === 'linux') {
    const helperPid = Number(output.match(/owner-exiting:(\d+)/)?.[1]);
    assert.ok(Number.isInteger(helperPid) && helperPid > 0, `Expected the Node lock helper pid: ${output}`);
    const started = Date.now();
    let helperAlive = true;
    while (Date.now() - started < 1500) {
      try { process.kill(helperPid, 0); await delay(25); }
      catch (error) { if (error?.code === 'ESRCH') {helperAlive = false; break;} throw error; }
    }
    assert.equal(helperAlive, false, 'Node lock helper must exit after its Bun owner closes stdin');
  }
  const release = await acquireBuildLock(path, {timeout: 1000});
  await release();
});

test('unexpected Bun lock host exit terminates the protected build', async () => {
  if (!process.versions.bun || process.platform !== 'linux') return;
  const path = resolve(work, 'lost-helper.lock');
  const script = `
    import {readFileSync} from 'node:fs';
    import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};
    await acquireBuildLock(${JSON.stringify(path)});
    const children = readFileSync('/proc/self/task/' + process.pid + '/children', 'utf8').trim().split(/\\s+/).filter(Boolean);
    if (children.length !== 1) throw new Error('Expected exactly one Node lock helper child, got ' + children.join(','));
    process.kill(Number(children[0]), 'SIGKILL');
    await new Promise(resolve => setTimeout(resolve, 250));
    console.log('UNEXPECTED_CONTINUE');
  `;
  const child = spawn(resolveBunExecutable(), ['--eval', script], {
    cwd: root,
    env: {...process.env, EAGLER_BUILD_LOCK_NODE: nativeNodeExecutable()},
    windowsHide: true,
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let output = '', error = '';
  child.stdout.setEncoding('utf8'); child.stdout.on('data', chunk => {output += chunk;});
  child.stderr.setEncoding('utf8'); child.stderr.on('data', chunk => {error += chunk;});
  const result = await new Promise((yes, no) => {
    child.once('error', no);
    child.once('exit', (status, signal) => yes({status, signal}));
  });
  assert.notEqual(result.status, 0, `Bun should fail after losing its lock helper: ${error}\n${output}`);
  assert.doesNotMatch(output, /UNEXPECTED_CONTINUE/);
});

test('Bun lock host rejects an explicit Bun node shim', async () => {
  if (!process.versions.bun) return;
  const path = resolve(work, 'reject-bun.lock');
  const script = `
    import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};
    try { await acquireBuildLock(${JSON.stringify(path)}); throw new Error('Bun shim was accepted'); }
    catch (error) { if (!/real Node.js >=22/.test(error.message)) throw error; console.log('bun-shim-rejected'); }
  `;
  const child = spawn(resolveBunExecutable(), ['--eval', script], {
    cwd: root,
    env: {...process.env, EAGLER_BUILD_LOCK_NODE: resolveBunExecutable()},
    windowsHide: true,
    stdio: ['ignore', 'pipe', 'pipe'],
  });
  let output = '', error = '';
  child.stdout.setEncoding('utf8'); child.stdout.on('data', chunk => {output += chunk;});
  child.stderr.setEncoding('utf8'); child.stderr.on('data', chunk => {error += chunk;});
  const code = await new Promise((yes, no) => {
    child.once('error', no);
    child.once('exit', (status, signal) => status === 0 ? yes(status) : no(new Error(`Bun shim validation probe failed (${signal ?? status}): ${error}\n${output}`)));
  });
  assert.equal(code, 0);
  assert.match(output, /bun-shim-rejected/);
});

test('ordinary error exit releases only its own acquired build lock', async () => {
  const path = resolve(work, 'exit.lock');
  const script = `import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};await acquireBuildLock(${JSON.stringify(path)});process.exit(17);`;
  const code = await new Promise((yes, no) => {
    const child = spawn(process.execPath, ['--input-type=module', '-e', script], {stdio: 'pipe'});
    let error = ''; child.stderr.on('data', bytes => {error += bytes;}); child.once('error', no);
    child.once('exit', code => error ? no(new Error(error)) : yes(code));
  });
  assert.equal(code, 17); const release = await acquireBuildLock(path, {timeout: 100}); await release();
});

for (const ownerKind of ['Node', 'Bun']) for (const waiterKind of ['Node', 'Bun']) {
  test(`killed ${ownerKind} lock owner releases three mutually exclusive ${waiterKind} waiters`, async () => {
    const path = resolve(work, `killed-${ownerKind}-${waiterKind}.lock`), script = path + '.mjs';
    await writeFile(script, `
      import {acquireBuildLock} from ${JSON.stringify(new URL('../../lib/build-lock.mjs', import.meta.url).href)};
      import {openSync,closeSync,unlinkSync} from 'node:fs';
      import {setTimeout as delay} from 'node:timers/promises';
      console.log('waiting');
      const release=await acquireBuildLock(${JSON.stringify(path)},{timeout:5000});
      console.log('acquired');
      if(process.argv[2]==='hold')await new Promise(()=>{setInterval(()=>{},1000);});
      else {const fd=openSync(${JSON.stringify(path + '.active')},'wx');closeSync(fd);await delay(75);unlinkSync(${JSON.stringify(path + '.active')});await release();}
    `);
    const executables = {Node:nativeNodeExecutable(), Bun:resolveBunExecutable()}, children = [];
    function run(kind, mode) {
      const child = spawn(executables[kind], [script,mode], {windowsHide:true,stdio:['ignore','pipe','pipe']});
      children.push(child);let output='',error='';child.stdout.on('data',data=>output+=data);child.stderr.on('data',data=>error+=data);
      const done = new Promise((yes,no)=>{child.once('error',no);child.once('exit',(code,signal)=>code===0?yes():no(Error(`${kind}: ${code}/${signal} ${error}`)));});
      void done.catch(()=>{});
      return {child,done,output:()=>output};
    }
    async function waitFor(value,text) {
      const started=Date.now();
      while(!value.output().includes(text)){assert.ok(Date.now()-started<5000,`${ownerKind}/${waiterKind}: ${text}`);await delay(10);}
    }
    try {
      const owner = run(ownerKind,'hold');await waitFor(owner,'acquired');
      const waiters=Array.from({length:3},()=>run(waiterKind,'once'));
      await Promise.all(waiters.map(waiter=>waitFor(waiter,'waiting')));await delay(100);
      assert.ok(waiters.every(waiter=>!waiter.output().includes('acquired')),'a live owner excludes every waiter');
      owner.child.kill('SIGKILL');await Promise.all(waiters.map(waiter=>waiter.done));
      const release=await acquireBuildLock(path,{timeout:100});await release();
    } finally {
      for(const child of children)if(child.exitCode===null&&child.signalCode===null)child.kill('SIGKILL');
      await Promise.allSettled(children.map(child=>child.exitCode!==null||child.signalCode!==null?Promise.resolve():new Promise(yes=>child.once('exit',yes))));
    }
  });
}

test('legacy lock directories fail closed and are left untouched', async () => {
  const path=resolve(work,'legacy.lock');await mkdir(path);await writeFile(resolve(path,'owner'),'legacy');
  await assert.rejects(acquireBuildLock(path), /legacy lock directory/);
  assert.equal(await readFile(resolve(path,'owner'),'utf8'),'legacy');
});

test('a surviving client compiler cannot publish after its lock-owning parent is killed', async () => {
  const status=resolve(work,'orphan-status.json'), finish=resolve(work,'orphan-finish'), done=resolve(work,'orphan-done');
  const writer=resolve(work,'orphan-writer.mjs'), preload=resolve(work,'orphan-preload.mjs'), parent=resolve(work,'orphan-parent.mjs');
  const supervisor=resolve(work,'orphan-supervisor.mjs'), killed=resolve(work,'orphan-parent-killed');
  await writeFile(writer, `
    import {mkdir,rename,writeFile} from 'node:fs/promises';import {existsSync} from 'node:fs';
    import {resolve} from 'node:path';import {setTimeout as delay} from 'node:timers/promises';
    const outdir=process.argv[process.argv.indexOf('--outDir')+1];
    await writeFile(${JSON.stringify(status + '.tmp')},JSON.stringify({outdir,pid:process.pid}));
    await rename(${JSON.stringify(status + '.tmp')},${JSON.stringify(status)});
    const started=Date.now();while(!existsSync(${JSON.stringify(finish)})){if(Date.now()-started>20000)throw Error('orphan test timed out');await delay(10);}
    await mkdir(outdir,{recursive:true});await writeFile(resolve(outdir,'orphan-probe.mjs'),'orphan');
    await writeFile(${JSON.stringify(done)},'done');
  `);
  await writeFile(preload, `
    import cp from 'node:child_process';import {syncBuiltinESMExports} from 'node:module';
    const spawn=cp.spawn;cp.spawn=(exe,args,options)=>args[0].replaceAll('\\\\','/').endsWith('/typescript/bin/tsc')
      ?spawn(exe,[${JSON.stringify(writer)},...args],{...options,detached:true,stdio:'ignore'}):spawn(exe,args,options);syncBuiltinESMExports();
  `);
  await writeFile(parent, `import {ensureClientServicesBuild} from ${JSON.stringify(new URL('../../lib/client-services-build.mjs',import.meta.url).href)};await ensureClientServicesBuild({force:true});`);
  // A detached test compiler deliberately survives Windows process-tree
  // cleanup, exercising the worst case of a live writer after parent death.
  // The supervisor observes its completion before the test runner cleans up.
  await writeFile(supervisor, `
    import {spawn} from 'node:child_process';import {existsSync} from 'node:fs';import {writeFile} from 'node:fs/promises';
    import {setTimeout as delay} from 'node:timers/promises';
    const owner=spawn(process.execPath,['--import',${JSON.stringify(pathToFileURL(preload).href)},${JSON.stringify(parent)}],{windowsHide:true,stdio:'inherit'});
    const exited=new Promise(yes=>owner.once('exit',yes));
    async function wait(path){const start=Date.now();while(!existsSync(path)){if(Date.now()-start>15000)throw Error('supervisor timeout');await delay(10);}}
    try{await wait(${JSON.stringify(status)});owner.kill('SIGKILL');await exited;await writeFile(${JSON.stringify(killed)},'killed');await wait(${JSON.stringify(done)});}
    finally{if(owner.exitCode===null&&owner.signalCode===null)owner.kill('SIGKILL');}
  `);
  const owner=spawn(nativeNodeExecutable(),[supervisor],{windowsHide:true,stdio:['ignore','pipe','pipe']});
  let error='',orphan,release;owner.stderr.on('data',data=>error+=data);
  const exited=new Promise(yes=>owner.once('exit',yes));
  async function waitFile(path){const started=Date.now();while(!existsSync(path)){assert.ok(Date.now()-started<15000,`writer signal ${path}: ${error}`);await delay(10);}}
  try {
    await waitFile(status);orphan=JSON.parse(await readFile(status,'utf8'));
    assert.ok(orphan.outdir.startsWith(resolve(root,'.cache/client-services-staging-')),'the actual spawn redirects compiler output to private staging');
    await waitFile(killed);
    release=await acquireBuildLock(resolve(root,'.cache/client-services-build.lock'),{timeout:1000});
    await writeFile(finish,'finish');await waitFile(done);
    assert.equal(await readFile(resolve(orphan.outdir,'orphan-probe.mjs'),'utf8'),'orphan');
    assert.equal(existsSync(resolve(root,'.cache/build/browser/assets/orphan-probe.mjs')),false,'the orphan cannot touch publication output');
    await exited;
  } finally {
    release?.();if(owner.exitCode===null&&owner.signalCode===null){owner.kill('SIGKILL');await exited;}
    if(orphan)await writeFile(finish,'finish');
  }
});
test('concurrent translation consumers share compilation, reuse verified output and repair entries', async () => {
  const project = await fixture(); let calls = 0;
  const compiler = async input => {calls++; await compile(input);};
  const first = ensureI18nBuild({project, compiler}), second = ensureI18nBuild({project, compiler});
  assert.equal(first, second); assert.equal((await first).built, true); assert.equal(calls, 1);
  assert.equal((await ensureI18nBuild({project, compiler})).built, false); assert.equal(calls, 1);
  await rm(resolve(project, 'src/paraglide/messages.js'));
  assert.equal((await ensureI18nBuild({project, compiler})).built, false);
  assert.equal((await message(project)).text, 'first'); assert.equal(calls, 1);
});
test('input changes during compilation retry before publishing a generation', async () => {
  const project = await fixture(); let calls = 0;
  const result = await ensureI18nBuild({project, compiler: async input => {
    await compile(input);
    if (++calls === 1) await writeFile(resolve(project, 'messages/en.json'), 'second');
  }});
  assert.equal(result.built, true); assert.equal(calls, 2); assert.equal((await message(project)).text, 'second');
  assert.equal((await readdir(cache(project))).some(name => name.startsWith('staging-')), false);
});
test('a partial failed compile preserves the current entry and can be retried', async () => {
  const project = await fixture(); await ensureI18nBuild({project, compiler: compile});
  const previous = await readFile(resolve(project, 'src/paraglide/messages.js'), 'utf8'), generation = await current(project);
  await writeFile(resolve(project, 'messages/en.json'), 'next');
  await assert.rejects(ensureI18nBuild({project, compiler: async ({outdir}) => {
    await writeFile(resolve(outdir, 'messages.js'), 'partial'); throw new Error('compiler failed');
  }}), /compiler failed/);
  assert.equal(await readFile(resolve(project, 'src/paraglide/messages.js'), 'utf8'), previous);
  assert.deepEqual(await current(project), generation);
  const release = await acquireBuildLock(resolve(cache(project), 'build.lock'), {timeout:100}); await release();
  assert.equal((await readdir(cache(project))).some(name => name.startsWith('staging-')), false);
  await ensureI18nBuild({project, compiler: compile}); assert.equal((await message(project)).text, 'next');
});
test('corrupt generated output is rebuilt into a new immutable generation', async () => {
  const project = await fixture(); await ensureI18nBuild({project, compiler: compile});
  const previous = await current(project);
  await writeFile(resolve(cache(project), previous.generation, 'messages.js'), 'broken');
  assert.equal((await ensureI18nBuild({project, compiler: compile})).built, true);
  assert.notEqual((await current(project)).generation, previous.generation); assert.equal((await message(project)).text, 'first');
});
test('prebuilt packages do not import the compiler or create source output', async () => {
  const project = resolve(work, 'prebuilt'); await mkdir(project);
  assert.deepEqual(await ensureI18nBuild({project, compiler: () => {throw new Error('unexpected');}}), {built: false, mode: 'prebuilt'});
  assert.equal(existsSync(resolve(project, '.cache')), false);
});
