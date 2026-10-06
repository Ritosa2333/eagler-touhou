import test from 'node:test';
import assert from 'node:assert/strict';
import {parseMakeDependencies} from '../portable/make-dependencies.mjs';

test('WASI Clang Windows depfiles retain drive and directory separators', () => {
  const depfile = String.raw`source: D\:\workspace\eagler\tests\test.cpp \
  D\:\workspace\eagler\tests\..\th10_web\cpp\game\Arithmetic.hpp`;
  const expected = [
    String.raw`D:\workspace\eagler\tests\test.cpp`,
    String.raw`D:\workspace\eagler\tests\..\th10_web\cpp\game\Arithmetic.hpp`,
  ];
  assert.deepEqual(parseMakeDependencies(depfile), expected);
  assert.deepEqual(parseMakeDependencies(depfile.replaceAll(String.raw`D\:`, 'D:')), expected);
});

test('Make escapes for spaces, hash, dollar and backslash remain supported', () => {
  const depfile = String.raw`source: /src/space\ name.hpp /src/hash\#tag.hpp /src/money$$.hpp /src/back\\slash.hpp`;
  assert.deepEqual(parseMakeDependencies(depfile), [
    '/src/space name.hpp', '/src/hash#tag.hpp', '/src/money$.hpp', '/src/back\\slash.hpp',
  ]);
  assert.deepEqual(parseMakeDependencies('source:\r\n'), []);
  assert.throws(() => parseMakeDependencies('other: missing.hpp'), /target/);
});
