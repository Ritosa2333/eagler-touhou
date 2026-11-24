// 生成 docs/fidelity-map.md 的“逐文件映射”一节：每个 C++ 源文件引用了哪些原版函数。
// 输入是 Ghidra 导出的函数索引（functions.json，见 docs/DECOMP.md）与端口源码。
// 用法（容器内，th13-dev 镜像）：
//   node tools/decomp/fidelity-map.mjs <decomp 目录> [--pe <th13.exe>] [--write]
//     不带 --write：把生成的表打印到标准输出
//     --pe：按 PE 节表给不在函数体内的地址定节；有地址落在映像之外时列出并以 1 退出
//     --write：替换 docs/fidelity-map.md 里 <!-- fidelity-map:begin --> 与
//              <!-- fidelity-map:end --> 之间的内容
import {readFileSync, writeFileSync} from 'node:fs';
import {relative, resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {formatTargets} from '../source-layout.mjs';

const port = resolve(fileURLToPath(new URL('../..', import.meta.url)));
const args = process.argv.slice(2);
const [decomp] = args;
const option = name => args.includes(name) ? args[args.indexOf(name) + 1] : undefined;
const pe = option('--pe');
if (!decomp || decomp.startsWith('--') || !pe) {
  console.error('用法：node tools/decomp/fidelity-map.mjs <decomp 目录> --pe <th13.exe> [--write]');
  process.exit(2);
}

const index = JSON.parse(readFileSync(resolve(decomp, 'functions.json'), 'utf8'));
const functions = index.functions.map(item => ({start: Number(item.address), size: item.bodyBytes}));
const starts = functions.map(item => item.start).sort((a, b) => a - b);
const contains = new Map(functions.map(item => [item.start, item]));
const name = start => '0x' + start.toString(16);

// 地址 → 所在函数入口；不在任何函数体内则返回 null（数据地址或映像基址）。
function entryOf(address) {
  let low = 0, high = starts.length;
  while (low < high) {
    const middle = (low + high) >> 1;
    if (starts[middle] <= address) low = middle + 1;
    else high = middle;
  }
  const start = starts[low - 1];
  return start !== undefined && address < start + contains.get(start).size ? start : null;
}

// PE32 映像范围：头部（映像基址所在）加节表；VirtualSize 为 0 的节退回 SizeOfRawData。
function peSections(bytes) {
  const peHeader = bytes.readUInt32LE(0x3c);
  if (bytes.toString('latin1', 0, 2) !== 'MZ' || bytes.readUInt32LE(peHeader) !== 0x4550 ||
      bytes.readUInt16LE(peHeader + 24) !== 0x10b)
    throw Error('需要 PE32 映像');
  const optional = peHeader + 24;
  const imageBase = bytes.readUInt32LE(optional + 28);
  const table = optional + bytes.readUInt16LE(peHeader + 20);
  const headers = [{name: 'headers', start: imageBase, end: imageBase + bytes.readUInt32LE(optional + 60)}];
  return headers.concat(Array.from({length: bytes.readUInt16LE(peHeader + 6)}, (_, index) => {
    const at = table + index * 40;
    const size = bytes.readUInt32LE(at + 8) || bytes.readUInt32LE(at + 16);
    return {
      name: bytes.toString('latin1', at, at + 8).replace(/\0.*$/, ''),
      start: imageBase + bytes.readUInt32LE(at + 12),
      end: imageBase + bytes.readUInt32LE(at + 12) + size,
    };
  }));
}
const sections = pe ? peSections(readFileSync(resolve(pe))) : [];
const sectionOf = address => sections.find(section => section.start <= address && address < section.end);

const citation = /0x4[0-9a-fA-F]{5}\b/g;
const rows = [];
const summary = {addresses: new Set(), citations: 0, atEntry: 0, inside: 0, outside: 0, data: 0, undefinedCode: 0};
const outsideImage = new Set();
for (const file of formatTargets(port).filter(name => name.startsWith('web/cpp/') || name.startsWith('portable/'))) {
  const text = readFileSync(resolve(port, file), 'utf8');
  const perFunction = new Map();
  for (const match of text.matchAll(citation)) {
    const address = Number.parseInt(match[0], 16);
    summary.addresses.add(address);
    summary.citations++;
    const entry = entryOf(address);
    if (entry === null) {
      summary.outside++;
      const section = sectionOf(address);
      if (!section) outsideImage.add(address);
      else if (section.name === '.text') summary.undefinedCode++;
      else summary.data++;
      continue;
    }
    if (entry === address) summary.atEntry++;
    else summary.inside++;
    perFunction.set(entry, (perFunction.get(entry) ?? 0) + 1);
  }
  if (!perFunction.size) continue;
  const top = [...perFunction].sort((a, b) => b[1] - a[1] || a[0] - b[0]).slice(0, 6);
  rows.push(`| ${file.replace(/^web\/cpp\//, '')} | ${perFunction.size} | ${top.map(([start, count]) => `${name(start)}×${count}`).join(' ')} |`);
}
if (outsideImage.size) {
  console.error(`以下地址不在映像内：${[...outsideImage].sort((a, b) => a - b).map(name).join(' ')}`);
  process.exit(1);
}

const lines = [
  `源码共引用 ${summary.addresses.size} 个不同的原版地址、${summary.citations} 处引用，全部落在映像内：` +
    `${summary.atEntry} 处恰好是函数入口，${summary.inside} 处落在函数体内（函数内的具体指令），` +
    `${summary.data} 处是数据地址，${summary.undefinedCode} 处在代码节内但不在导出的函数体内。` +
    '下面每个文件列出它引用的原版函数（按引用次数，最多六个）。路径省略 `web/cpp/` 前缀。',
  '',
  '| C++ 文件 | 原版函数数 | 主要入口 |',
  '|---|---:|---|',
  ...rows,
];
const table = lines.join('\n');
if (!args.includes('--write')) {
  console.log(table);
  process.exit(0);
}
const document = resolve(port, 'docs/fidelity-map.md');
const text = readFileSync(document, 'utf8');
const begin = text.indexOf('<!-- fidelity-map:begin -->'), end = text.indexOf('<!-- fidelity-map:end -->');
if (begin < 0 || end < begin) throw Error('docs/fidelity-map.md 缺少 fidelity-map 标记');
writeFileSync(document, text.slice(0, begin + '<!-- fidelity-map:begin -->\n'.length) + table + '\n' + text.slice(end));
console.log(`已更新 ${relative(port, document)}：${rows.length} 个文件`);
