import {Fat12} from './disk.js';

const folder = 'YUMEZIKU';
const mount = '/home/web_user/game/';

// Run the files from the existing private HDI under DOSBox-X internal DOS.
// The original boot sectors, DOS installation and VEM486 are not executed.
export function extractGame(disk) {
  const fat = new Fat12(disk), files = [], entries = new Map();
  // PC-98 FAT names are CP932. MEMFS uses Unicode paths; treating the bytes
  // as Latin-1 makes the game's Japanese-named data archives inaccessible.
  const decode = new TextDecoder('shift_jis');
  function walk(ranges, parent = '') {
    for (const [start, size] of ranges) for (let at = start; at < start + size; at += 32) {
      const first = disk[at], attr = disk[at + 11];
      if (!first) break;
      if (first === 0xe5 || (attr & 8) || attr === 15) continue;
      const namePart = decode.decode(disk.subarray(at, at + 8)).trim();
      const ext = decode.decode(disk.subarray(at + 8, at + 11)).trim();
      if (namePart === '.' || namePart === '..') continue;
      const name = parent + namePart + (ext ? '.' + ext : '');
      if (attr & 16) {
        if (name === folder || name.startsWith(folder + '/')) {
          walk(fat.chain(fat.view.getUint16(at + 26, true)).map(c => [fat.offset(c), fat.clusterSize]), name + '/');
        }
      } else if (name.startsWith(folder + '/')) {
        files.push({path: 'game/' + name, contents: fat.view.getUint32(at + 28, true) ? fat.read(at) : new Uint8Array()});
        entries.set(name, at);
      }
    }
  }
  walk([[fat.root, fat.rootSize]]);
  for (const name of ['MAIN.EXE', 'OP.EXE', 'GAME.BAT', 'ZUN.COM', '夢時空1.DAT', '夢時空2.DAT']) {
    if (!entries.has(folder + '/' + name)) throw Error('梦时空磁盘缺少 ' + name);
  }
  return {
    files, entries,
    owns(path) { return path.startsWith(mount + folder + '/') && entries.has(path.slice(mount.length)); },
    export(fs) {
      const copy = new Uint8Array(disk), saved = new Fat12(copy);
      for (const [name, at] of entries) {
        const bytes = fs.readFile(mount + name);
        const old = saved.view.getUint32(at + 28, true) ? saved.read(at) : new Uint8Array();
        if (bytes.length !== old.length || bytes.some((b, i) => b !== old[i])) saved.replace(at, bytes);
      }
      return copy;
    },
  };
}

// TH03 keeps its original refresh at this deterministic instruction budget.
// Larger budgets mainly execute additional guest idle loops between refreshes.
export const NETWORK_CYCLES=16000;
export function prepareFastBoot(extracted, {network = false} = {}) {
  const bat = extracted.files.find(f => f.path === 'game/' + folder + '/GAME.BAT');
  if (!bat) throw Error('梦时空启动脚本缺失');
  const text = Array.from(bat.contents, c => String.fromCharCode(c)).join('');
  const budget = `z:\\system\\config.com -set "cpu cycles=${network ? NETWORK_CYCLES : 'max 100%'}"\r\n`;
  // Unlike TH04, TH03 plays the logo in a separate ZUN -3 command before OP.
  // Restore the game budget there too, leaving only driver setup at 1000.
  const withLogoBudget = text.replace(/^([ \t]*zun[ \t]+-3[ \t]*)\r*$/gmi,
    (_, command) => `${budget}${command}\r`);
  let entries = 0;
  const replacement = withLogoBudget.replace(/^([ \t]*)(op|main)([ \t]*)\r*$/gmi, (_, indent, command, tail) => {
    entries++;
    return `${budget}${indent}${command}${tail}\r`;
  });
  if (entries !== 6) throw Error('梦时空启动入口与当前版本不匹配');
  return {path: 'game/' + folder + '/FASTBOOT.BAT', contents: Uint8Array.from(replacement, c => c.charCodeAt(0))};
}

export function dosboxConfig({network = false, fastBoot = false} = {}) {
  return `[dosbox]\nmachine=pc98\nmemsize=16\n[cpu]\ncore=simple\ncputype=386\ncycles=${fastBoot ? 1000 : network ? NETWORK_CYCLES : 'max 100%'}\n[render]\nframeskip=0\nscaler=none\naspect=false\n[sblaster]\nsbtype=none\n[pc98]\npc-98 fm board=board86\n[mixer]\nrate=44100\nblocksize=512\nprebuffer=20\n[autoexec]\n@echo off\nmount a /home/web_user/game\na:\npath a:\\${folder};a:\\\ncd ${folder}\n${fastBoot ? 'FASTBOOT.BAT' : 'GAME.BAT'}\n`;
}
