import {defineConfig} from 'vitest/config';
import config from './vitest.config';
import {WORKSPACE_NODE_TESTS} from './tests/test-plan.mjs';

/** Source/resource contracts require the selected game checkouts explicitly. */
export default defineConfig({
  ...config,
  test: {...config.test, projects: [{test: {name: 'workspace', include: [...WORKSPACE_NODE_TESTS]}}]},
});
