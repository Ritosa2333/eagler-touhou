import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { build } from 'esbuild';

const project = fileURLToPath(new URL('..', import.meta.url));
// Paraglide and JSON catalogs form self-contained browser modules. The error
// mapper imports the shared translation owner instead of bundling a second copy.
await build({
  entryPoints: ['i18n', 'localized-error-text'].map((name) => resolve(project, `src/launcher/${name}.mts`)),
  outdir: resolve(project, '.cache/build/browser/assets/launcher'),
  outExtension: { '.js': '.mjs' },
  bundle: true,
  format: 'esm',
  platform: 'browser',
  logLevel: 'silent',
  plugins: [
    {
      name: 'shared-translation-owner',
      setup(context) {
        context.onResolve({ filter: /^\.\/i18n\.mts$/ }, () => ({ path: './i18n.mjs', external: true }));
      },
    },
  ],
});
