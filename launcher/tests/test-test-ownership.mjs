/** L0/repository governance. Every executable test has a discoverable entry;
 * execution plans are unique and package scripts may expose convenient aliases. */
import assert from "node:assert/strict";
import { readFile, readdir } from "node:fs/promises";
import { relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {suites} from '../../tests/suites.mjs';

import {
  REPOSITORY_NODE_TESTS,
  REPOSITORY_PYTHON_TESTS,
  WORKSPACE_NODE_TESTS,
  WORKSPACE_PYTHON_TESTS,
} from "./test-plan.mjs";

const root = resolve(fileURLToPath(new URL("..", import.meta.url)));
const packageJson = JSON.parse(await readFile(resolve(root, "package.json"), "utf8"));
const explicit = new Set(Object.values(packageJson.scripts || {}).flatMap(command =>
  [...String(command).matchAll(/(?:^|\s)(tests\/[A-Za-z0-9_./*-]+\.(?:mjs|py|ps1))(?=\s|$)/g)].map(match => match[1])
));
for (const id of ['launcher-ui','launcher-support']) for (const task of suites()[id].tasks())
  for (const argument of task.args) if (argument.endsWith('.mjs')) {
    const file=relative(root,resolve(task.cwd,argument)).replaceAll('\\','/');
    if (file.startsWith('tests/')) explicit.add(file);
  }
const plannedFiles = [
  ...REPOSITORY_NODE_TESTS, ...REPOSITORY_PYTHON_TESTS,
  ...WORKSPACE_NODE_TESTS, ...WORKSPACE_PYTHON_TESTS,
];
const planned = new Set(plannedFiles);
assert.equal(planned.size, plannedFiles.length, 'each test must appear once in the execution plan');

async function collect(directory) {
  const files = [];
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const path = resolve(directory, entry.name);
    if (entry.isDirectory()) files.push(...await collect(path));
    else if (/^(?:test.*|.*\.test)\.(?:mjs|py|ps1)$/i.test(entry.name)) files.push(relative(root, path).replaceAll("\\", "/"));
  }
  return files;
}

const exempt = new Set(["tests/test-plan.mjs"]);
const executableTests = (await collect(resolve(root, "tests"))).filter(file => !exempt.has(file) && !file.startsWith("tests/fixtures/ui-history/"));
for (const file of planned) assert.ok(executableTests.includes(file), `planned test does not exist: ${file}`);
const explicitPatterns = [...explicit].map(pattern => new RegExp('^' + pattern.split('*')
  .map(part => part.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')).join('[^/]*') + '$'));
const unowned = executableTests.filter(file => !planned.has(file) && !explicitPatterns.some(pattern => pattern.test(file)));
assert.deepEqual(unowned, [], `tests without a declared lane: ${unowned.join(", ")}`);

console.log(JSON.stringify({ testOwnership: "PASS", executableTests: executableTests.length }));
