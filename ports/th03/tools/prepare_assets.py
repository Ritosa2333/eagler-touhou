"""Package only TH03 assets from the existing local NP21 installation."""
import gzip
import hashlib
import json
from pathlib import Path
import shutil
import struct

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT.parent / 'touhou-np2'


def files(data):
    u16 = lambda at: struct.unpack_from('<H', data, at)[0]
    u32 = lambda at: struct.unpack_from('<I', data, at)[0]
    header, sector, spt, heads = (u32(at) for at in (8, 16, 20, 24))
    part = header + sector
    base = header + ((u16(part + 10) * heads + data[part + 9]) * spt + data[part + 8]) * sector
    bps = u16(base + 11)
    cluster_size = bps * data[base + 13]
    fat = base + u16(base + 14) * bps
    root = fat + data[base + 16] * u16(base + 22) * bps
    clusters = root + ((u16(base + 17) * 32 + bps - 1) // bps) * bps

    def chain(cluster):
        seen = set()
        while 2 <= cluster < 0xff8:
            if cluster in seen or cluster >= 0xff0:
                raise ValueError('Invalid FAT12 cluster chain')
            seen.add(cluster)
            offset = clusters + (cluster - 2) * cluster_size
            if offset + cluster_size > len(data):
                raise ValueError('Cluster outside disk')
            yield offset
            value = u16(fat + cluster * 3 // 2)
            cluster = value >> 4 if cluster & 1 else value & 0xfff

    def walk(offsets, parent=''):
        for offset, size in offsets:
            for at in range(offset, offset + size, 32):
                entry = data[at:at + 32]
                if not entry[0]:
                    return
                if entry[0] == 0xe5 or entry[11] == 15 or entry[11] & 8:
                    continue
                stem = entry[:8].decode('cp932').strip()
                ext = entry[8:11].decode('cp932').strip()
                if stem in ('.', '..'):
                    continue
                path = parent + stem + ('.' + ext if ext else '')
                offsets = list(chain(u16(at + 26)))
                if entry[11] & 16:
                    yield from walk([(p, cluster_size) for p in offsets], path + '/')
                else:
                    yield path, offsets, u32(at + 28)

    yield from walk([(root, u16(base + 17) * 32)])


def main():
    web = ROOT / 'web'
    vendor = web / 'vendor/np2'
    disks = web / 'disks'
    vendor.mkdir(parents=True, exist_ok=True)
    disks.mkdir(parents=True, exist_ok=True)
    for name in ('np2-wasm.js', 'np21.js', 'np21.wasm', 'font.bmp', 'font_cn.bmp',
                 'LICENSE', 'NOTICE.md', 'SHA256SUMS.json'):
        shutil.copy2(SOURCE / 'vendor/np2' / name, vendor / name)
    wrapper_path = vendor / 'np2-wasm.js'
    wrapper = wrapper_path.read_text(encoding='utf-8')
    edits = [
        ("                document.addEventListener('visibilitychange', this.onVisibilityChange.bind(this));\n", ''),
        ("    static create(config) {\n        return new Promise(async (resolve, reject) => {\n            const factory = (await import('./np21.js')).default;",
         "    static async create(config) {\n        const factory = (await import('./np21.js')).default;\n        return new Promise((resolve, reject) => {"),
    ]
    for old, new in edits:
        if wrapper.count(old) != 1:
            raise ValueError('NP21 wrapper source changed')
        wrapper = wrapper.replace(old, new, 1)
    wrapper_path.write_text(wrapper, encoding='utf-8', newline='\n')
    (vendor / 'np2-original.js').write_text(wrapper.replace("import('./np21.js')", "import('./np21-solo.js')"), encoding='utf-8', newline='\n')
    from build_solo_runtime import build as build_solo_runtime
    build_solo_runtime()
    hashes = {name: hashlib.sha256((vendor / name).read_bytes()).hexdigest()
              for name in ('np2-wasm.js', 'np21.js', 'np21.wasm', 'font.bmp', 'font_cn.bmp')}
    (vendor / 'SHA256SUMS.json').write_text(json.dumps(hashes, indent=2) + '\n', encoding='utf-8')
    (vendor / 'NOTICE.md').write_text(
        '# NP21 runtime\n\nSource: https://github.com/irori/np2-wasm\n'
        'License: BSD-3-Clause, see LICENSE.\n'
        'Copied from the local touhou-np2 installation. Native JS/WASM and font.bmp are unchanged.\n'
        'font_cn.bmp is from the user-provided game bundle.\n'
        'The local wrapper delegates visibility handling to app.js and propagates import errors.\n'
        'SHA256SUMS.json records the packaged files.\n', encoding='utf-8')
    source_manifest = json.loads((SOURCE / 'disks/manifest.json').read_text(encoding='utf-8-sig'))
    manifest = {'version': 1, 'games': {}}
    inventory = {}
    for lang in ('jp', 'cn'):
        key = '3-' + lang
        meta = dict(source_manifest['games'][key])
        source = SOURCE / meta['url']
        data = gzip.decompress(source.read_bytes())
        if len(data) != meta['size'] or hashlib.sha256(data).hexdigest() != meta['sha256']:
            raise ValueError('TH03 source disk hash mismatch: ' + lang)
        entries = list(files(data))
        cfg = next(item for item in entries if item[0] == 'YUMEZIKU/YUME.CFG')
        if len(cfg[1]) != 1 or cfg[2] != 8:
            raise ValueError('Unexpected TH03 configuration layout')
        meta['keyboardConfig'] = {'offset': cfg[1][0], 'size': cfg[2],
                                  'original': data[cfg[1][0]:cfg[1][0] + cfg[2]].hex()}
        shutil.copy2(source, disks / source.name)
        manifest['games'][key] = meta
        inventory[lang] = [{'path': path, 'size': size} for path, _, size in entries if path.startswith('YUMEZIKU/')]
        print(f'{key}: verified original disk; YUME.CFG at {cfg[1][0]:#x}')
    (disks / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    reports = ROOT / 'reports'
    reports.mkdir(exist_ok=True)
    (reports / 'asset-inventory.json').write_text(json.dumps(inventory, indent=2) + '\n', encoding='utf-8')
    from build_pause import build
    from build_solo_assist import build as build_solo_assist
    build_solo_assist()
    build()


if __name__ == '__main__':
    main()
