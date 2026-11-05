import assert from 'node:assert/strict';

export function assertCliArguments(argv, {values = [], flags = []}) {
  const names = new Set([...values, ...flags]), seen = new Set();
  assert.equal(names.size, values.length + flags.length, 'Duplicate CLI option declaration');
  for (const argument of argv) {
    const parsed = /^--([a-z][a-z0-9-]*)(?:=([\s\S]*))?$/.exec(argument);
    assert.ok(parsed, `Invalid argument: ${argument}`);
    const [, name, value] = parsed;
    assert.ok(names.has(name), `Unknown option: --${name}`);
    assert.ok(!seen.has(name), `Duplicate option: --${name}`);
    seen.add(name);
    if (values.includes(name)) assert.ok(value?.length, `Option --${name} requires =VALUE`);
    else assert.equal(value, undefined, `Flag --${name} does not accept a value`);
  }
}
