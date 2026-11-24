// Read back submitted sprites, not presentation strings or private layout metadata.
import assert from 'node:assert/strict';
import {quadVertices} from '../../th128/site/runtime/draw.mjs';

export const foreground = r => r.bank === 11 && (r.color & 0xffffff) !== 0;
export const style0 = r => foreground(r) && r.sprite >= 0 && r.sprite < 98;
export const style4 = r => foreground(r) && r.sprite >= 243 && r.sprite <= 255;
export function runs(group, select) {
  const lines = new Map();
  for (const r of group.filter(select)) {
    if (!lines.has(r.y)) lines.set(r.y, []);
    lines.get(r.y).push(r);
  }
  return [...lines].sort(([y], [z]) => y-z).map(([,line]) => line.sort((a,b) => a.x-b.x).map(r => {
    if (style0(r)) return String.fromCharCode(r.sprite+32);
    if (r.sprite === 255) return '%';
    assert.ok(r.sprite <= 252, 'use the style-4 digit or percent cell');
    return String(r.sprite-243);
  }).join(''));
}
export function seatGroups(records, art, count) {
  const originals = new Set(records), added = art.filter(r => r.id>=0x3f000000 && !originals.has(r));
  const tops = added.filter(r => style0(r) && r.sprite === 'P'.charCodeAt(0)-32).map(r => r.y).sort((a,b) => a-b);
  assert.equal(tops.length, count, 'one actual ascii.anm P header per seat');
  assert.ok(added.every(r => r.y >= tops[0]), 'no unowned HUD line before P1');
  return tops.map((y,i) => added.filter(r => r.y >= y && r.y < (tops[i+1] ?? Infinity)));
}
export function bounds(r) {
  const [[left,top],,[right,bottom]] = quadVertices(r);
  return {left,top,right,bottom};
}
