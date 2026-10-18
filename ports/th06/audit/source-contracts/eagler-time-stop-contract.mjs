import fs from 'node:fs';

const player = fs.readFileSync(new URL('../../src/Player.cpp', import.meta.url), 'utf8');
const bullets = fs.readFileSync(new URL('../../src/BulletManager.cpp', import.meta.url), 'utf8');
const items = fs.readFileSync(new URL('../../src/ItemManager.cpp', import.meta.url), 'utf8');

function requireOrdered(text, first, second, label) {
  const a = text.indexOf(first);
  const b = text.indexOf(second);
  if (a < 0 || b < 0 || a >= b) {
    throw new Error(`${label}: expected ${JSON.stringify(first)} before ${JSON.stringify(second)}`);
  }
}

requireOrdered(player, 'SyncRenderState(p);', 'if (g_GameManager.isTimeStopped)',
               'player frozen render endpoints');
requireOrdered(bullets, 'SyncRenderState(mgr);', 'if (g_GameManager.isTimeStopped)',
               'enemy bullet frozen render endpoints');

const playerSyncStart = player.indexOf('void Player::SyncRenderState(Player *p)');
const playerUpdateStart = player.indexOf('ChainCallbackResult Player::OnUpdate(Player *p)');
if (playerSyncStart < 0 || playerUpdateStart <= playerSyncStart) {
  throw new Error('player render-state sync helper boundary missing');
}
const playerSyncBody = player.slice(playerSyncStart, playerUpdateStart);
if (playerSyncBody.includes('SyncRenderState(p);')) {
  throw new Error('Player::SyncRenderState must not recursively call itself');
}
for (const needle of [
  'p->prevPositionCenter = p->positionCenter;',
  'p->playerSprite.UpdatePrev();',
  'bullet.prevPosition = bullet.position;',
  'bullet.sprite.UpdatePrev();',
]) {
  if (!playerSyncBody.includes(needle)) {
    throw new Error(`Player::SyncRenderState missing ${needle}`);
  }
}

if (!bullets.includes('bullet.prevAngle = bullet.angle;') ||
    !bullets.includes('laser.prevAngle = laser.angle;') ||
    !bullets.includes('g_ItemManager.SyncRenderState();')) {
  throw new Error('bullet/laser/item time-stop render-state sync contract missing');
}
if (!items.includes('item.prevPosition = item.currentPosition;') ||
    !items.includes('item.sprite.UpdatePrev();')) {
  throw new Error('item frozen render-state sync contract missing');
}

console.log('TH06 Eagler time-stop interpolation contract PASS: frozen player/bullet/laser/item endpoints sync before early return');
