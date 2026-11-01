import { defineConfig } from 'vitest/config';
import { WORKSPACE_NODE_TESTS } from './tests/test-plan.mjs';
import config from './vitest.config';

// Cross-repository contracts retain their explicit opt-in lane.
export default defineConfig({ ...config, test: { ...config.test, include: [...WORKSPACE_NODE_TESTS] } });
