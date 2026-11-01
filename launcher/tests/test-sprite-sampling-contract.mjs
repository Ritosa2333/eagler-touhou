import {test} from 'vitest';
import fs from "node:fs";
import { workspacePath } from "../lib/workspace-layout.mjs";

test("test-sprite-sampling-contract", async () => {
const read = rel => { const [repository, ...segments] = rel.split("/"); return fs.readFileSync(workspacePath(repository.replace(/-eagler$/, ""), ...segments), "utf8").replaceAll("\r\n", "\n"); };

const th06Bullet = read("th06-eagler/src/BulletManager.cpp");
const th06Anm = read("th06-eagler/src/AnmManager.cpp");
if (!th06Bullet.includes("g_AnmManager->Draw2(anmVm);"))
  throw new Error("th06: bullets must use the common Draw2 path");
if (th06Anm.includes("rintf(vm->pos.x)") || th06Anm.includes("rintf(vm->pos.y)") ||
    th06Anm.includes("floorf(g_PrimitivesToDrawVertexBuf"))
  throw new Error("th06: common sprite draw paths must not quantize interpolated positions");
for (const [needle, label] of [
  ["SPRITE_EXTRUSION_GUTTER = 1", "one-texel sprite extrusion gutter"],
  ["BuildSpriteExtrusionAtlas(this, anm->textureIdx, loadedSpriteIndices);", "static ANM atlas build"],
  ["ResolveSpriteDrawSampling(this, vm->sprite", "draw-time atlas/source sampling selection"],
  ["InvalidateSpriteExtrusionAtlas(this, static_cast<i32>(textureDstIdx));", "text-write atlas invalidation"],
  ["InvalidateSpriteExtrusionAtlas(this, textureId);", "screenshot-write atlas invalidation"],
]) {
  if (!th06Anm.includes(needle)) throw new Error(`th06: missing ${label}`);
}

const th07Bullet = read("th07-eagler/src/BulletManager.cpp");
const th07Anm = read("th07-eagler/src/AnmManager.cpp");
if (!th07Bullet.includes("g_AnmManager->Draw(vm);"))
  throw new Error("th07: bullets must use the common Draw path");
if (th07Anm.includes("floorf(g_QuadVertices") || th07Anm.includes("roundToPixel") ||
    th07Anm.includes("roundUnrotatedToPixel"))
  throw new Error("th07: common sprite draw paths must not quantize interpolated positions");
for (const [needle, label] of [
  ["SPRITE_EXTRUSION_GUTTER = 1", "one-texel sprite extrusion gutter"],
  ["BuildSpriteExtrusionAtlas(this, data->textureIdx, loadedSpriteIndices);", "static ANM atlas build"],
  ["ResolveSpriteDrawSampling(this, vm->sprite", "draw-time atlas/source sampling selection"],
  ["InvalidateSpriteExtrusionAtlas(this, static_cast<i32>(spriteDstIdx));", "text-write atlas invalidation"],
  ["InvalidateSpriteExtrusionAtlas(this, textureId);", "screenshot-write atlas invalidation"],
  ["InvalidateSpriteExtrusionAtlas(this, dstIdx);", "texture-copy atlas invalidation"],
]) {
  if (!th07Anm.includes(needle)) throw new Error(`th07: missing ${label}`);
}

console.log(JSON.stringify({
  spriteSampling: "PASS",
  games: ["th06", "th07"],
  interpolation: "unquantized",
  extrusionGutter: 1,
  mutableTextureInvalidation: true,
}));
});
