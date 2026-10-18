import fs from 'node:fs';

const gameWindow = fs.readFileSync(new URL('../../src/GameWindow.cpp', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
const bulletManager = fs.readFileSync(new URL('../../src/BulletManager.cpp', import.meta.url), 'utf8').replaceAll('\r\n', '\n');
const anmManager = fs.readFileSync(new URL('../../src/AnmManager.cpp', import.meta.url), 'utf8').replaceAll('\r\n', '\n');

function requireText(text, needle, label) {
    if (!text.includes(needle)) {
        throw new Error(`missing ${label}: ${needle}`);
    }
}

function sliceFunction(text, signature, nextSignature) {
    const start = text.indexOf(signature);
    if (start < 0) throw new Error(`cannot locate ${signature}`);
    const end = nextSignature ? text.indexOf(nextSignature, start + signature.length) : -1;
    return text.slice(start, end >= 0 ? end : undefined);
}

const init = sliceFunction(gameWindow, 'ZunResult GameWindow::InitD3dRendering()', 'ZunResult GameWindow::InitD3dDevice');
const drawBullet = sliceFunction(bulletManager, 'void BulletManager::DrawBullet(Bullet *bullet)',
                                 'void BulletManager::DrawBulletNoHwVertex(Bullet *bullet)');
const drawBulletFallback = sliceFunction(bulletManager, 'void BulletManager::DrawBulletNoHwVertex(Bullet *bullet)',
                                         'ZunResult BulletManager::AddedCallback');
const draw = sliceFunction(anmManager, 'ZunResult AnmManager::Draw(const AnmVm *vm)',
                           'ZunResult AnmManager::DrawFacingCamera');
const draw2 = sliceFunction(anmManager, 'ZunResult AnmManager::Draw2(const AnmVm *vm)', '#define AnmF32Arg');
const draw3 = sliceFunction(anmManager, 'ZunResult AnmManager::Draw3(const AnmVm *vm)',
                            'ZunResult AnmManager::Draw2(const AnmVm *vm)');

// The portable GLES renderer provides the transform path that the original
// D3D capability bit represented. It must not silently remain zero-initialized.
requireText(init, 'g_Supervisor.hasD3dHardwareVertexProcessing = 1;',
            'GLES hardware-vertex-equivalent capability');

// Preserve the original branch ownership: the normal-capability bullet path
// uses Draw2, while the fallback uses screen-space Draw().
requireText(drawBullet, 'g_AnmManager->Draw2(anmVm);', 'normal bullet Draw2 path');
requireText(drawBulletFallback, 'g_AnmManager->Draw(anmVm);', 'fallback bullet Draw path');

// The visible reason this branch matters: rotated fallback sprites quantize
// their center, whereas Draw2 delegates rotation to Draw3 and Draw3 keeps the
// floating-point position in the world transform.
requireText(draw, 'xOffset = rintf(vm->pos.x);', 'fallback X center quantization');
requireText(draw, 'yOffset = rintf(vm->pos.y);', 'fallback Y center quantization');
requireText(draw2, 'return this->Draw3(vm);', 'rotated Draw2 to Draw3 delegation');
requireText(draw3, 'worldTransformMatrix.m[3][0] = vm->pos.x;', 'Draw3 sub-pixel X');
requireText(draw3, 'worldTransformMatrix.m[3][1] = -vm->pos.y;', 'Draw3 sub-pixel Y');

console.log('TH06 bullet render-path contract: PASS (GLES uses original hardware-equivalent path; rotated bullets retain sub-pixel position)');
