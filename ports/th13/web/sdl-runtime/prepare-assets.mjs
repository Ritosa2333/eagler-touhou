// 从 raw 解包目录（artifacts/reference/raw）拷贝网页运行时所需资源到 ./assets。
// 产物为原版权材料，git 不入库（.gitignore 已忽略 assets/）。
// 用法（容器内，仓库根）：node eagler-touhou/ports/th13/web/sdl-runtime/prepare-assets.mjs
import {copyFileSync, mkdirSync, existsSync} from 'node:fs';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runtimeAssets} from './runtime-assets.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const workspace = resolve(here, '../../../../..');  // 工作区根（容器内 /work）
const raw = resolve(workspace, 'artifacts/reference/raw');
const out = resolve(here, 'assets');
if (!existsSync(raw)) throw Error('raw 解包目录不存在：' + raw + '（先做资源提取）');
mkdirSync(out, {recursive: true});

const files = [...runtimeAssets, 'musiccmt.txt'];
for (const name of files) {
  copyFileSync(resolve(raw, name), resolve(out, name));
  console.log('assets/' + name);
}
console.log('完成：' + out);
