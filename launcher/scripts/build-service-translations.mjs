import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';

const project = fileURLToPath(new URL('..', import.meta.url));
// Node acceptance and static publication consume the same compiled messages.
await build({entryPoints: ['i18n', 'localized-error-text'].map(name => resolve(project, `src/launcher/${name}.mts`)),
  outdir: resolve(project, '.cache/build/browser/assets/launcher'), outExtension: {'.js': '.mjs'},
  bundle: true, format: 'esm', platform: 'browser', logLevel: 'silent'});
