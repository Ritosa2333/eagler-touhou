import fs from 'node:fs';

const anm = fs.readFileSync(new URL('../src/AnmManager.cpp', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
const gles = fs.readFileSync(new URL('../src/graphics/Gles.cpp', import.meta.url), 'utf8').replaceAll('\r\n', '\n');

function requireText(text, needle, label) {
    if (!text.includes(needle)) {
        throw new Error(`missing ${label}: ${needle}`);
    }
}

const functionStart = anm.indexOf('void AnmManager::ApplySurfaceToColorBuffer');
const functionEnd = anm.indexOf('\n}', functionStart);
if (functionStart < 0 || functionEnd < 0) {
    throw new Error('cannot locate ApplySurfaceToColorBuffer');
}
const surfaceCopy = anm.slice(functionStart, functionEnd + 2);

requireText(anm, 'd3dDevice->CopyRects', 'original D3D CopyRects evidence');
requireText(gles, 'v_FogFragCoord = length(viewPos.xyz);', 'GLES distance-based fog');
requireText(gles, 'if (u_FogEnabled)', 'GLES fog gate');

requireText(surfaceCopy, 'const bool restoreFog = g_Supervisor.DisableFog() != 0;',
            'surface-copy fog suppression');
requireText(surfaceCopy, 'if (restoreFog)', 'surface-copy fog restore guard');
requireText(surfaceCopy, 'g_Supervisor.EnableFog();', 'surface-copy fog restoration');

const disable = surfaceCopy.indexOf('DisableFog()');
const draw = surfaceCopy.indexOf('this->BackendDrawCall();');
const enable = surfaceCopy.indexOf('g_Supervisor.EnableFog();');
if (!(disable >= 0 && draw > disable && enable > draw)) {
    throw new Error('surface-copy fog ownership order changed');
}

console.log('TH06 surface-copy contract: PASS (CopyRects semantics; fog suppressed and restored)');
