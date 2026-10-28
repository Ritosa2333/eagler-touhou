import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve, relative} from 'node:path';
import {createRequire} from 'node:module';
import {runInNewContext} from 'node:vm';
import {execFileSync} from 'node:child_process';
import {RUNTIME_PROTOCOL_COMMANDS, RUNTIME_PROTOCOL_COMMAND_BEHAVIOR, RUNTIME_PROTOCOL_EVENTS,
  TOUCH_SENSITIVITY_MIN, TOUCH_SENSITIVITY_MAX} from '../../launcher/lib/contracts/runtime-protocol.mjs';

const require = createRequire(new URL('../../launcher/package.json', import.meta.url));
const {parse} = require('acorn');

export function runtimeSourceFiles(game, product) {
  if (game === 'th12') return ['src/managed-shell.mjs', 'src/managed-input.mjs', 'site/game.mjs'];
  if (product.runtimeFileLayout !== 'directory') return ['resources/shell.html'];
  if (game === 'th09') return ['web/sdl-runtime/managed.mjs'];
  // TH13 owns its Host protocol and shared keyboard/gamepad bridge in these two
  // modules; it does not ship the older eagler-host.mjs split.
  if (game === 'th13') return ['web/sdl-runtime/shell.mjs', 'web/sdl-runtime/directory-keyboard.mjs'];
  if (['th075', 'th15'].includes(game)) return ['web/sdl-runtime/shell.mjs'];
  return ['web/sdl-runtime/shell.mjs', 'web/sdl-runtime/eagler-host.mjs'];
}

function result(id, checks, files, operation, {complete = true} = {}) {
  try {
    operation();
    return {id, status: 'passed', level: 'source-contract', checks, files, complete};
  } catch (error) {
    return {id, status: 'failed', level: 'source-contract', checks, files, complete, error: error.message};
  }
}

/** Each source owner is checked separately, including separate MP owners. */
export function checkRuntimeSource({game, variant, product, directory, nativeFiles, root}) {
  const files = runtimeSourceFiles(game, product);
  const read = name => readFileSync(resolve(root, directory, name), 'utf8').replaceAll('\r\n', '\n');
  const source = files.map(read).join('\n');
  const nativeBodies = nativeFiles.map(path => ({path, text:readFileSync(path, 'utf8')}));
  const native = nativeBodies.map(item => item.text).join('\n');
  const records = [];
  if (game === 'th12') {
    const testFile='tests/managed-adapter.test.mjs';
    return [
      ['runtime-protocol',['RUNTIME-01','RUNTIME-02','RUNTIME-03'],'managed protocol:'],
      ['touch-sensitivity',['TOUCH-14'],'managed touch:'],
      ['native-gamepad',['INPUT-04'],'managed gamepad:'],
    ].map(([id,checks,pattern])=>result(id,checks,[...files,testFile],()=>{
      execFileSync(process.execPath,['--test','--test-name-pattern='+pattern,resolve(root,directory,testFile)],
        {cwd:resolve(root,directory),encoding:'utf8',windowsHide:true});
    }));
  }
  const th13ManagedShellTest = 'tests/managed-shell.test.mjs';
  const protocolFiles = game === 'th13' ? [...files, th13ManagedShellTest] : files;
  records.push(result('runtime-protocol', ['RUNTIME-01', 'RUNTIME-02', 'RUNTIME-03'], protocolFiles, () => {
    assert.match(source, /runtimeEpoch/, 'Runtime must bind messages to its navigation epoch');
    assert.match(source, /\.epoch\s*!==\s*epoch|message\.epoch\s*!==\s*epoch/, 'Runtime must reject a retired epoch');
    for (const command of RUNTIME_PROTOCOL_COMMANDS) {
      const games = RUNTIME_PROTOCOL_COMMAND_BEHAVIOR[command].games;
      if (games && !games.includes(game)) continue;
      assert.match(source, new RegExp(`case\\s+["']${command}["']`), `Missing command: ${command}`);
    }
    for (const event of RUNTIME_PROTOCOL_EVENTS) {
      assert.ok(new RegExp(`emit\\(["']${event}["']`).test(source) ||
        (event === 'ready' && /event:\s*["']ready["']/.test(source)), `Missing event: ${event}`);
    }
    if (game === 'th13') {
      // Execute the real shell audio callback, not only the emit() token.
      execFileSync(process.execPath,
        ['--test', '--test-name-pattern=managed frame callback reports the actual SDL audio queue',
          resolve(root, directory, th13ManagedShellTest)],
        {cwd: resolve(root, directory), encoding: 'utf8', windowsHide: true});
    }
  }));
  records.push(result('touch-sensitivity', ['TOUCH-14'], protocolFiles, () => {
    if (product.runtimeFileLayout === 'directory') {
      assert.ok(source.includes(`Math.max(${TOUCH_SENSITIVITY_MIN},Math.min(${TOUCH_SENSITIVITY_MAX},`));
      assert.match(source, new RegExp(`(?:sensitivity|touchSensitivity)>=${TOUCH_SENSITIVITY_MIN}&&[\\w.]+<=${TOUCH_SENSITIVITY_MAX}`));
    } else {
      assert.ok(source.includes(`touchSensitivity < ${TOUCH_SENSITIVITY_MIN}`));
      assert.ok(source.includes(`touchSensitivity > ${TOUCH_SENSITIVITY_MAX}`));
    }
    if (game === 'th13') {
      // Execute the real touch adapters and native endpoint arguments. This
      // covers TOUCH-14 only; TOUCH-10 (unlimited movement cheat-rate) remains
      // unverified and must not be inferred from this gate.
      execFileSync(process.execPath,
        ['--test', '--test-name-pattern=managed (touch settings|direct touch)',
          resolve(root, directory, th13ManagedShellTest)],
        {cwd: resolve(root, directory), encoding: 'utf8', windowsHide: true});
    }
  }));
  const th13NativePadFiles = game === 'th13'
    ? nativeBodies.filter(item => /BrowserGamepadState\(|ApplyGamepadVirtualKeys\(/.test(item.text))
      .map(item => relative(resolve(root,directory),item.path).replaceAll('\\','/'))
    : [];
  const padFiles = game === 'th13'
    ? [...th13NativePadFiles, 'web/sdl-runtime/directory-keyboard.mjs', 'tests/gamepad-bridge.test.mjs']
    : nativeBodies.filter(item => /BrowserGamepadState\(|ApplyGamepadVirtualKeys\(|SDL_GetGamepadButton|SDL_GetGamepadAxis/.test(item.text))
      .map(item => relative(resolve(root,directory),item.path).replaceAll('\\','/'));
  records.push(result('native-gamepad', ['INPUT-04'], padFiles, () => {
    if (game === 'th13') {
      // The published browser bridge is necessary but not sufficient: the
      // compiled Runtime must consume the Launcher-owned gamepad state through
      // the same native contract as TH11/TH15/TH20. A shell-only bridge with a
      // native SDL joystick sampler would leave the browser mapping unread.
      assert.match(native, /BrowserGamepadState\(/, 'Compiled native source must sample the Launcher gamepad owner');
      assert.match(native, /ApplyGamepadVirtualKeys\(/, 'Compiled native source must deliver normalized gamepad actions');
      // TH13 consumes the Launcher's canonical gamepad mapping through the
      // shared browser bridge instead of reading SDL gamepad state natively.
      // Keep the source-shape assertions and execute the real bridge so a
      // renamed schema, stale epoch, or permissive sample() cannot pass.
      assert.match(source, /__eaglerGamepadInputBridge\s*=\s*Object\.freeze\(\s*\{\s*schema:\s*'eagler-touhou\/gamepad-input\/1'/);
      assert.match(source, /value\.schema\s*===\s*'eagler-touhou\/gamepad-input\/1'/);
      assert.match(source, /value\.protocol\s*===\s*protocol/);
      assert.match(source, /value\.game\s*===\s*game/);
      assert.match(source, /value\.epoch\s*===\s*epoch/);
      assert.match(source, /value\.origin\s*===\s*origin/);
      assert.match(source, /typeof value\.sample\s*===\s*'function'/);
      assert.match(source, /typeof value\.clear\s*===\s*'function'/);
      assert.match(source, /typeof value\.enabled\s*===\s*'function'/);
      execFileSync(process.execPath, ['--test', resolve(root, directory, 'tests/gamepad-bridge.test.mjs')],
        {cwd: resolve(root, directory), encoding: 'utf8', windowsHide: true});
    } else if (['th11', 'th15', 'th20'].includes(game)) {
      assert.match(native, /BrowserGamepadState\(/, 'Compiled native source must sample the Launcher gamepad owner');
      assert.match(native, /ApplyGamepadVirtualKeys\(/, 'Compiled native source must deliver normalized gamepad actions');
    } else {
      assert.match(native, /SDL_GetGamepadButton/, 'Compiled native source must read normalized gamepad buttons');
      assert.match(native, /SDL_GetGamepadAxis/, 'Compiled native source must read normalized gamepad axes');
    }
  }));
  if (product.runtimeFileLayout === 'directory' && product.features.focusHitbox !== false) {
    const file = `web/sdl-runtime/${game === 'th09' ? 'managed.mjs' : 'shell.mjs'}`;
    records.push(result('always-hitbox-abi', ['RUNTIME-06'], [file], () => {
      const text = read(file);
      const ast = parse(text, {ecmaVersion: 'latest', sourceType: 'module'});
      const name = ['th08', 'th10', 'th20'].includes(game) ? 'applyOptions' : 'apply';
      const declaration = ast.body.find(node => node.type === 'FunctionDeclaration' && node.id?.name === name);
      assert.ok(declaration, `Missing option application owner: ${name}`);
      const symbol = {th08:'sdl_touch_display', th09:'_th09_always_hitbox', th10:'application_touch_display',
        th11:'th11_always_hitbox', th15:'_th15_always_hitbox', th20:'sdl_always_hitbox'}[game];
      assert.match(native, new RegExp(`\\b${symbol.replace(/^_/, '')}\\s*\\(`), `Missing native option endpoint: ${symbol}`);
      for (const enabled of [false, true, false]) {
        const calls = [];
        const core = new Proxy({}, {get: (_, key) => (...args) => {if (key === symbol) calls.push(args.at(-1)); return 0;}});
        const context = {core, Module:{}, options:{alwaysHitbox:enabled}, app:1, music:false, launched:false,
          multiplayer:null, multiplayerRuntime:variant === 'multiplayer', runtimeBuild:{multiplayer:variant === 'multiplayer'}, applyTouchOptions(){}, practice:null,
          touchPolicy(){return 0;}, fileExists(){return false;}, root(){return '/save';}};
        runInNewContext(text.slice(declaration.start, declaration.end) + `;${name}()`, context);
        assert.deepEqual(calls, [enabled ? 1 : 0], 'The real option adapter must forward the exact value');
      }
    }, {complete:false}));
  }
  return records;
}
