// 把折叠后的逐帧操作流折成整段摘要流：原版 game-oracle.py --draw-ops 或移植侧 harness game-draw 的原始 JSON（每帧一行 {"frame","ops"}）。
// 口径见 draw-structure.mjs 的 frameDigests（与 tools/oracle/draw-compare.py、tests/harness/DrawDigest.hpp 逐字一致）。
// 用法：node tools/draw-digest.mjs <in.jsonl|-> <out.jsonl|->
//   "-" 表示标准输入 / 标准输出。原版长程用管道：game-oracle.py … --draw --draw-ops - | node tools/draw-digest.mjs - oracle.digest.jsonl
import {createReadStream, createWriteStream} from 'node:fs';
import {digestStream} from './draw-structure.mjs';

const [input, output] = process.argv.slice(2);
if (!input || !output) {
  console.error('usage: node tools/draw-digest.mjs <in.jsonl|-> <out.jsonl|->');
  process.exit(2);
}
const out = createWriteStream(output === '-' ? 1 : output);
for await (const row of digestStream(input === '-' ? process.stdin : createReadStream(input))) {
  if (!out.write(JSON.stringify(row) + '\n')) await new Promise(done => out.once('drain', done));
}
out.end();
