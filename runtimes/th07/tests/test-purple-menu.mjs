import { execFileSync } from 'node:child_process';
import { readFileSync, mkdirSync } from 'node:fs';
import { resolve } from 'node:path';
import assert from 'node:assert/strict';
const root = resolve(import.meta.dirname, '..');
const sdk = process.env.EMSDK;
const imgui = process.env.THPRAC_IMGUI_ROOT;
assert(sdk && imgui, 'Set EMSDK and THPRAC_IMGUI_ROOT explicitly');
const out = resolve(root, 'build/purple-check');
mkdirSync(out, { recursive: true });
const env = { ...process.env, EM_CONFIG: process.env.EM_CONFIG ?? resolve(sdk, '.emscripten') };
execFileSync('python', [resolve(sdk, 'install/emscripten/emcc.py'),
  resolve(root, 'tests/th07-purple-menu.cpp'),
  ...['imgui.cpp', 'imgui_draw.cpp', 'imgui_widgets.cpp', 'imgui_tables.cpp', 'imgui_freetype.cpp'].map(file => resolve(imgui, file)),
  '-I' + resolve(root, 'src'), '-I' + resolve(process.env.EAGLER_COMMON_ROOT ?? resolve(root, '../../eagler-common'), 'include'), '-I' + imgui, '-DIMGUI_DISABLE_WIN32_FUNCTIONS', '-DTH_ENABLE_THPRAC',
  '--use-port=sdl3_ttf', '-include', 'cstdint', '-Wno-nontrivial-memcall', '-O1', '-std=c++17', '-sDEFAULT_TO_CXX=1',
  '-sALLOW_MEMORY_GROWTH=1', '-sENVIRONMENT=node', '-o', resolve(out, 'menu.js')],
  { cwd: root, env, windowsHide: true, stdio: 'inherit' });
execFileSync(process.execPath, [resolve(out, 'menu.js')], { cwd: root, env, windowsHide: true, stdio: 'inherit' });

const shell = readFileSync(resolve(root, 'resources/shell.html'), 'utf8');
const expression = shell.slice(shell.indexOf('const thpracKeyboardBitForEvent = event => {') + 'const thpracKeyboardBitForEvent = '.length,
  shell.indexOf('const keyboardBitForEvent =')).trim().replace(/;$/, '');
const map = Function('return (' + expression + ')')();
for (const [code, key, keyCode, bit] of [['F12','f12',123,9], ['KeyU','u',85,10], ['F8','f8',119,11], ['Tab','tab',9,8]]) {
  for (const event of [{code}, {key}, {keyCode}]) assert.equal(map(event), 1 << bit, JSON.stringify(event));
}
const owner = readFileSync(resolve(root, 'src/PracticeRuntime.cpp'), 'utf8');
for (const [scan, bit] of [['F12',9], ['U',10], ['F8',11]]) assert(owner.includes(`case SDL_SCANCODE_${scan}: bit = ${bit}; break;`));
const enemy = readFileSync(resolve(root, 'src/EnemyManager.cpp'), 'utf8');
assert(/if \(!PracticeRuntime::OverlayEnemyInvincible\(\)\)\s+enemy->life -= damage;\s+enemy->lastDamage = damage;/.test(enemy));
console.log('TH07 F12/U/F8/Tab bridge and native enemy subtraction source boundary PASS');
execFileSync('python', [resolve(sdk, 'install/emscripten/emcc.py'), resolve(root, 'tests/th07-purple-input.cpp'),
  '-I' + resolve(root, 'src'), '-I' + resolve(process.env.EAGLER_COMMON_ROOT ?? resolve(root, '../../eagler-common'), 'include'), '-O1', '-std=c++17', '-sDEFAULT_TO_CXX=1', '-sENVIRONMENT=node',
  '-o', resolve(out, 'input.js')], { cwd: root, env, windowsHide: true, stdio: 'inherit' });
execFileSync(process.execPath, [resolve(out, 'input.js')], { cwd: root, env, windowsHide: true, stdio: 'inherit' });
const controller = readFileSync(resolve(root, 'src/Controller.cpp'), 'utf8');
const guiInput = readFileSync(resolve(root, 'src/ThpracImGui.cpp'), 'utf8').replaceAll('\r\n', '\n');
assert(guiInput.includes('if (sampled)\n        g_GameHeldButtons = buttons;'));
for (const [key, button] of [[37,'LEFT'],[38,'UP'],[39,'RIGHT'],[40,'DOWN'],[90,'SHOOT'],[16,'FOCUS']])
  assert(guiInput.includes(`io.KeysDown[${key}] = (g_GameHeldButtons & TH_BUTTON_${button}) != 0;`));
assert(guiInput.includes('io.NavInputs[ImGuiNavInput_Activate] = 0.0f;'));
assert(controller.indexOf('buttons = PracticeRuntime::FilterGameplayInput(buttons);') > controller.indexOf('buttons |= Touch::GetButtonBits();'));
assert(owner.includes('g_GameManager.globals->livesRemaining < 1.0f'));
const player = readFileSync(resolve(root, 'src/Player.cpp'), 'utf8');
assert(/globals->livesRemaining <= 0 &&\s+!PracticeRuntime::OverlayInfiniteLives\(\)/.test(player));
console.log('TH07 merged input placement and purple no-continue life owner source boundary PASS');
