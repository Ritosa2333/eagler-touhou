import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import test from 'node:test';
import {createOptionalPractice} from '../th10_web/sdl-runtime/practice-loader.mjs';

test('ordinary and diagnostic binaries without THPrac never load practice modules', async () => {
  for (const core of [{}, {practice_enable: undefined}, {practice_enable: 0}]) {
    let imports = 0;
    const bridge = await createOptionalPractice({core}, async () => {
      ++imports;
      throw Error('Optional practice files are intentionally absent');
    });
    assert.equal(bridge, null);
    assert.equal(imports, 0);
  }
});

test('THPrac binaries keep their native bridge even before a user enables its UI', async () => {
  const services = {core: {practice_enable() {}}, getApp: () => 0};
  const expected = {configure() {}, tick() {}};
  let imports = 0;
  assert.equal(await createOptionalPractice(services, async () => {
    ++imports;
    return {createPractice(actual) {assert.equal(actual, services);return expected;}};
  }), expected);
  assert.equal(imports, 1);
});

test('a missing required practice module fails rather than silently dropping THPrac', async () => {
  await assert.rejects(createOptionalPractice({core: {practice_enable() {}}}, async () => {
    throw Error('missing practice module');
  }), /missing practice module/);
});

test('the real shell only statically imports the always-packaged loader', () => {
  const shell = readFileSync(new URL('../th10_web/sdl-runtime/shell.mjs', import.meta.url), 'utf8');
  assert.match(shell, /import\s*\{createOptionalPractice\}\s*from\s*['"]\.\/practice-loader\.mjs['"]/);
  assert.doesNotMatch(shell, /from\s*['"]\.\/practice(?:-config|-sections)?\.mjs['"]/);
  assert.match(shell, /practice\?\.tick\(\)/);
});
