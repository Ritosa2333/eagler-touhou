import { readFileSync } from 'node:fs';
import { defineConfig } from 'vitest/config';
import config from './vitest.config';

const plan = JSON.parse(readFileSync(new URL('../tools/ports/shared-checks.json', import.meta.url), 'utf8'));
const include: string[] = [
  ...new Set<string>(
    plan.gates.flatMap((gate: { files: string[] }) =>
      gate.files.filter((file) => file.startsWith('launcher/tests/')).map((file) => file.slice('launcher/'.length)),
    ),
  ),
];

// The adaptation registry owns this lane, including its cross-repository checks.
export default defineConfig({ ...config, test: { ...config.test, include } });
