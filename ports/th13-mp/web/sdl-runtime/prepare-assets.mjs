// 从 raw 解包目录与原版 thbgmogg.dat 生成网页运行时资源。
// 产物为原版权材料，git 不入库（.gitignore 已忽略 assets/）。
// 用法（容器内，仓库根）：node eagler-touhou/ports/th13/web/sdl-runtime/prepare-assets.mjs
import {copyFileSync, existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {
  audioAssets,
  musicFinalShardSize,
  musicShardAssets,
  musicShardSize,
  runtimeAssets,
} from './runtime-assets.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const workspace = resolve(here, '../../../../..');  // 工作区根（容器内 /work）
const raw = resolve(workspace, 'artifacts/reference/raw');
const music = resolve(workspace, 'games/th13/thbgmogg.dat');
const out = resolve(here, 'assets');
if (!existsSync(raw)) throw Error('raw 解包目录不存在：' + raw + '（先做资源提取）');
if (!existsSync(music)) throw Error('原版音频档案不存在：' + music);
mkdirSync(out, {recursive: true});

for (const name of [...runtimeAssets, ...audioAssets, 'musiccmt.txt']) {
  copyFileSync(resolve(raw, name), resolve(out, name));
  console.log('assets/' + name);
}

const musicBytes = readFileSync(music);
const expectedBytes = (musicShardAssets.length - 1) * musicShardSize + musicFinalShardSize;
if (musicBytes.length !== expectedBytes)
  throw Error(`thbgmogg.dat 长度 ${musicBytes.length} != ${expectedBytes}`);
for (const [index, name] of musicShardAssets.entries()) {
  const start = index * musicShardSize;
  const end = start + (index === musicShardAssets.length - 1 ? musicFinalShardSize : musicShardSize);
  writeFileSync(resolve(out, name), musicBytes.subarray(start, end));
  console.log('assets/' + name);
}
console.log('完成：' + out);
