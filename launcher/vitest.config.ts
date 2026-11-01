import { fileURLToPath } from 'node:url';
import { paraglideVitePlugin } from '@inlang/paraglide-js';
import { defineConfig } from 'vitest/config';
import { resolveAuthoredSource } from './lib/authored-source-resolution.mjs';
import { REPOSITORY_NODE_TESTS } from './tests/test-plan.mjs';

// Separate from the React Router build so unit tests never publish frontend assets.
export default defineConfig({
  plugins: [
    {
      name: 'vitest-authored-sources',
      enforce: 'pre',
      resolveId(source, importer) {
        return resolveAuthoredSource(fileURLToPath(new URL('.', import.meta.url)), source, importer);
      },
    },
    paraglideVitePlugin({
      project: fileURLToPath(new URL('./project.inlang', import.meta.url)),
      outdir: fileURLToPath(new URL('./src/paraglide', import.meta.url)),
      strategy: ['baseLocale'],
      outputStructure: 'message-modules',
      emitTsDeclarations: true,
      disableAsyncLocalStorage: true,
    }),
  ],
  resolve: { alias: { '~': fileURLToPath(new URL('./app', import.meta.url)) } },
  test: {
    include: [
      'tests/vitest/**/*.test.ts',
      'tests/react-main/*.test.mjs',
      'tests/support/*.test.mjs',
      ...REPOSITORY_NODE_TESTS,
    ],
    environment: 'node',
    pool: 'forks',
    maxWorkers: 1,
    fileParallelism: false,
    isolate: true,
    testTimeout: 240_000,
    hookTimeout: 240_000,
    sequence: { hooks: 'list' },
    clearMocks: true,
    restoreMocks: true,
  },
});
