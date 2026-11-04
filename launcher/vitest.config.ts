import {fileURLToPath} from 'node:url';
import {defineConfig} from 'vitest/config';
import {authoredSources} from './scripts/ui-rewrite/authored-sources.ts';
import {uiTranslations} from './scripts/ui-rewrite/i18n.ts';
import {FRONTEND_BUILD_TESTS, FRONTEND_TEST_PATTERNS, REPOSITORY_NODE_TESTS} from './tests/test-plan.mjs';
import {assertFrontendRuntime} from './scripts/ui-rewrite/check-runtime.mjs';

const bunVersion = assertFrontendRuntime();

/** UI tests share source resolution and messages, without publishing an app. */
export default defineConfig({
  plugins: [authoredSources(), uiTranslations(fileURLToPath(new URL('.', import.meta.url)))],
  test: {
    environment: 'node', pool: 'forks', maxWorkers: 1, fileParallelism: false, isolate: true,
    setupFiles: ['./tests/vitest-runtime.mjs'], env: {EAGLER_FRONTEND_BUN_VERSION: bunVersion},
    testTimeout: 240_000, hookTimeout: 240_000,
    sequence: {hooks: 'list'},
    projects: [
      {test: {name: 'unit', include: [...FRONTEND_TEST_PATTERNS.unit], exclude: [...FRONTEND_BUILD_TESTS]}},
      {test: {name: 'build', include: [...FRONTEND_BUILD_TESTS]}},
      {test: {name: 'support', include: [...FRONTEND_TEST_PATTERNS.support]}},
      {test: {name: 'repository', include: REPOSITORY_NODE_TESTS.filter(path => path !== 'tests/test-node-environment.mjs')}},
    ],
  },
});
