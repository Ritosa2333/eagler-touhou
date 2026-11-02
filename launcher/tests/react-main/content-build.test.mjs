import {test, beforeAll, afterAll} from 'vitest';
import assert from 'node:assert/strict';
import {cp, mkdir, mkdtemp, readFile, rm, stat, symlink, writeFile, readdir, access} from 'node:fs/promises';
import {resolve, dirname} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {execFile} from 'node:child_process';
import {promisify} from 'node:util';

const project = fileURLToPath(new URL('../../', import.meta.url)), execute = promisify(execFile);
let work, source;
const build = (...args) => execute(process.execPath, ['scripts/build-content-pages.mjs', '--quiet', ...args], {cwd: source});
beforeAll(async () => {
  work = await mkdtemp(resolve(project, '.cache/content-build-')); source = resolve(work, 'source');
  for (const path of ['scripts/build-content-pages.mjs', 'lib/atomic-file.mjs', 'lib/build-lock.mjs', 'lib/localized-compatibility.mjs', 'lib/site-brand.mjs',
    'messages/compatibility.json', 'messages/keys.json', 'messages/en.json', 'messages/ja.json', 'content/FIRST_USE_NOTICE.md', 'content/MULTIPLAYER.md', 'docs/FAQ.md',
    'public/compatibility.html', 'public/migrate.html', 'public/assets/brand/face-F02B.svg', 'app/ui/tokens.css']) {
    await mkdir(dirname(resolve(source, path)), {recursive: true}); await cp(resolve(project, path), resolve(source, path));
  }
  await symlink(resolve(project, 'node_modules'), resolve(source, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
});
afterAll(async () => {if (work) await rm(work, {recursive: true, force: true});});

test('cold content generation coordinates concurrent callers and leaves unchanged outputs untouched', async () => {
  await build();
  const paths = ['faq.html', 'content/FIRST_USE_NOTICE.html', 'content/MULTIPLAYER.html', 'compatibility.en.html', 'compatibility.ja.html', 'migrate.html'];
  const before = await Promise.all(paths.map(async path => {const target = resolve(source, 'public', path); return [await readFile(target, 'utf8'), (await stat(target)).mtimeMs];}));
  await Promise.all([build(), build(), build('--check'), build()]);
  for (let index = 0; index < paths.length; index++) {
    const target = resolve(source, 'public', paths[index]);
    assert.equal(await readFile(target, 'utf8'), before[index][0]); assert.equal((await stat(target)).mtimeMs, before[index][1]);
  }
  await assert.rejects(access(resolve(source, '.cache/content-pages.lock')), {code: 'ENOENT'});
});
test('failed content checks preserve the actual file and release ownership for a retry', async () => {
  const path = resolve(source, 'public/faq.html'), stale = await readFile(path, 'utf8') + '\n<!-- stale fixture -->';
  await writeFile(path, stale); await assert.rejects(build('--check'), /faq\.html is stale/);
  assert.equal(await readFile(path, 'utf8'), stale);
  await assert.rejects(access(resolve(source, '.cache/content-pages.lock')), {code: 'ENOENT'});
  await build(); await build('--check');
});
test('atomic output retries transient reader locks and removes its temporary file', async () => {
  const target = resolve(work, 'atomic.txt'), preload = resolve(work, 'transient-lock.mjs'), run = resolve(work, 'atomic-run.mjs');
  await writeFile(target, 'old');
  await writeFile(preload, `import fs from 'node:fs'; import {syncBuiltinESMExports} from 'node:module'; let failures=2; const rename=fs.promises.rename; fs.promises.rename=async (...args)=>{if(failures-->0)throw Object.assign(new Error('temporary reader lock'),{code:'EPERM'});return rename(...args);}; syncBuiltinESMExports();`);
  await writeFile(run, `import {writeFileAtomic} from ${JSON.stringify(pathToFileURL(resolve(project, 'lib/atomic-file.mjs')).href)}; await writeFileAtomic(${JSON.stringify(target)},'new','utf8');`);
  await execute(process.execPath, ['--import', process.versions.bun ? preload : pathToFileURL(preload).href, run]);
  assert.equal(await readFile(target, 'utf8'), 'new');
  assert.equal((await readdir(work)).some(name => name.startsWith('.atomic.txt.')), false);
});
