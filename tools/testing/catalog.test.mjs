import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
import {isAbsolute, resolve} from 'node:path';
import {profiles, suites, trackedFiles, testFiles, repository} from '../../tests/suites.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {isNodeTest} from './discovery.mjs';

test('tooling discovery includes every executable tool regression exactly once and excludes fixture programs',()=>{
  const files = trackedFiles();
  const expected = files.filter(file=>file.startsWith('tools/') && isNodeTest(file));
  const catalog = suites(), [task] = catalog.tooling.tasks();
  assert.ok(expected.length > 0);
  assert.deepEqual(task.args.filter(arg=>isNodeTest(arg)), expected);
  for (const file of ['tools/workspace.test.mjs','tools/ports/package-plan.test.mjs','tools/ports/acceptance-errors.test.mjs'])
    assert.ok(task.args.includes(file), `required regression is not executed: ${file}`);
  assert.equal(new Set(task.args).size, task.args.length);
  assert.deepEqual(testFiles(['tools/nested/check.test.mjs','tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), ['tools/nested/check.test.mjs']);
  assert.throws(()=>testFiles(['tools/fixtures/do-not-run.test.mjs'], 'tools', /\.test\.mjs$/), /No test files/);
});

test('profiles resolve to real bounded commands and never silently select zero tests',()=>{
  const catalog = suites();
  for (const [name, selected] of Object.entries(profiles)) {
    assert.ok(selected.length, name);
    assert.equal(new Set(selected).size, selected.length, name);
    for (const id of selected) assert.ok(catalog[id], `${name}: missing ${id}`);
  }
  for (const [id, suite] of Object.entries(catalog)) {
    if (suite.tier === 'acceptance') continue;
    const tasks = suite.tasks();
    assert.ok(tasks.length, id);
    for (const task of tasks) {
      assert.ok(Number.isSafeInteger(task.timeoutMs) && task.timeoutMs > 0, id);
      assert.ok(isAbsolute(task.cwd) && existsSync(task.cwd), id);
      if (task.command === process.execPath) for (const file of task.args.filter(arg=>/\.(?:mjs|py)$/.test(arg)))
        assert.ok(existsSync(resolve(task.cwd, file)), `${id}: missing entrypoint ${file}`);
    }
  }
});

test('acceptance requires explicit private inputs and selects the complete product/player matrix',()=>{
  const empty = suites({environment:{}});
  assert.throws(()=>empty['runtime-browser'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  assert.throws(()=>empty['runtime-rooms'].tasks(), /EAGLER_TEST_RUNTIME_MANIFEST/);
  assert.throws(()=>suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:'private-inputs.json'}})['runtime-rooms'].tasks(), /EAGLER_TEST_LAUNCHER_URL/);
  const catalog = suites({environment:{EAGLER_TEST_RUNTIME_MANIFEST:resolve(repository, 'private-inputs.json'), EAGLER_TEST_LAUNCHER_URL:'http://127.0.0.1:8130/'}});
  const [ordinary] = catalog['runtime-browser'].tasks();
  assert.deepEqual(ordinary.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','), Object.keys(PRODUCT_GAMES));
  assert.ok(ordinary.args.includes('--variant=normal'));
  const rooms = catalog['runtime-rooms'].tasks();
  assert.equal(rooms.length, 4);
  for (const task of rooms) {
    const players = Number(task.args.find(arg=>arg.startsWith('--players=')).slice(10));
    const expected = Object.entries(PRODUCT_GAMES).filter(([, product])=>product.multiplayer?.playerCounts.includes(players)).map(([game])=>game);
    assert.deepEqual(task.args.find(arg=>arg.startsWith('--game=')).slice(7).split(','), expected);
    assert.ok(task.args.includes('--disconnect') && task.args.includes('--gamepad'));
  }
});
