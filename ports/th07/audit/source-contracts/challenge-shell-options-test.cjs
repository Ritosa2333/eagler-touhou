const assert = require('node:assert/strict');
const { readFileSync } = require('node:fs');
const { resolve } = require('node:path');
const { runInNewContext } = require('node:vm');

// Execute the real configure path up to its normalized Module options.
const html = readFileSync(resolve(__dirname, '../../resources/shell.html'), 'utf8');
const start = html.indexOf('const installMusic = async message => {');
const assignment = html.indexOf('Module.eaglerOptions = {', start);
const end = html.indexOf('\n        };', assignment);
assert.ok(start >= 0 && assignment > start && end > assignment);
const helperStart = html.indexOf('// BEGIN NETPLAY PERFORMANCE PROFILE');
const helperEnd = html.indexOf('// END NETPLAY PERFORMANCE PROFILE', helperStart);
assert.ok(helperStart >= 0 && helperEnd > helperStart);
const configure = html.slice(helperStart, helperEnd) + '\n' + html.slice(start, end + '\n        };'.length) + '\nreturn Module.eaglerOptions; }; installMusic(message);';
const run = options => runInNewContext(configure, {
  message: { options }, Module: {}, launched: false, thpracCatalog: [], URL,
});

(async () => {
  for (const enabled of [true, false]) {
    const options = await run({ netplayChallengeMode: enabled });
    assert.equal(options.netplayChallengeMode, enabled);
  }
  assert.equal((await run({})).netplayChallengeMode, false);
  await assert.rejects(run({ netplayChallengeMode: 'true' }), /invalid eagler-touhou option/);
  console.log('Challenge configure -> Module options: PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
