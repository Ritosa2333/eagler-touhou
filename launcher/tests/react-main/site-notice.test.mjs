import {afterAll} from 'vitest';
import {readUiHistoryText} from '../support/ui-history.mjs';
/** Native Vitest registration with an independently bundled pinned oracle. */
import {authoredSourcesPlugin} from './authored-sources.mjs';
import {builtinModules, createRequire} from 'node:module';
import {dirname, isAbsolute, join, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {mkdtemp, writeFile, rm} from 'node:fs/promises';

const directory = dirname(fileURLToPath(import.meta.url));
const repo = resolve(directory, '../..');
const requireFromRepo = createRequire(join(repo, 'package.json'));
const {build} = requireFromRepo('esbuild');
const builtins = new Set(builtinModules.flatMap(name => [name, `node:${name}`]));
// Build only from repository inputs; isolate and remove each run's generated bundle.
const buildDirectory = await mkdtemp(join(directory, 'site-notice-build-'));
const output = join(buildDirectory, 'checks.mjs');
afterAll(async () => { await rm(buildDirectory, {recursive: true, force: true}); });
{
  const result = await build({
    absWorkingDir: repo,
    entryPoints: [join(directory, 'site-notice.checks.tsx')],
    bundle: true, platform: 'node', format: 'esm', write: false, jsx: 'automatic',
    plugins: [{name: 'read-only-repository-inputs', setup(build) {
      build.onResolve({filter: /^@pinned\/site-notice$/}, () => ({path: 'site-notice', namespace: 'pinned-main'}));
      // Keep the history reader's import.meta.url anchored to its source file;
      // bundling it into this temporary directory would redirect fixture lookup.
      build.onResolve({filter: /^\.\.\/support\/ui-history\.mjs$/}, () => ({
        path: pathToFileURL(join(repo, 'tests/support/ui-history.mjs')).href, external: true,
      }));
      // The shared favicon replaced the old title icon. Keep the committed
      // parser/lifecycle oracle independent, adapting only this public asset URL.
      build.onLoad({filter: /.*/, namespace: 'pinned-main'}, () => ({loader: 'ts', contents: readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', 'src/launcher/site-notice.mts').replaceAll('assets/th06.ico', 'assets/favicon.ico')}));
      build.onResolve({filter: /^@source\//}, args => ({path: join(repo, args.path.slice('@source/'.length))}));
      build.onResolve({filter: /^[^./]/}, args => {
        if (args.kind === 'entry-point' || isAbsolute(args.path)) return;
        if (args.path.startsWith('@source/') || args.path.startsWith('@pinned/')) return;
        return {path: builtins.has(args.path) ? args.path : import.meta.resolve(args.path), external: true};
      });
    }}, authoredSourcesPlugin(repo)],
  });
  await writeFile(output, result.outputFiles[0].text);
  const {registerChecks} = await import(`${pathToFileURL(output).href}?run=${Date.now()}`);
  registerChecks(repo);
}
