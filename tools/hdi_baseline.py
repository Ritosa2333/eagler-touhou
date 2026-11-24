"""Read-only PC-98 HDI FAT12 inventory/extraction with bounded chain checks."""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def sha(data):
    return hashlib.sha256(data).hexdigest()


def inspect(image, destination=None, *, select=None):
    data = image.read_bytes() if isinstance(image, Path) else image
    selected = {} if select is not None else None
    if select is not None and destination is not None:
        raise ValueError('Selective inspection is read-only')
    u16 = lambda p: struct.unpack_from('<H', data, p)[0]
    u32 = lambda p: struct.unpack_from('<I', data, p)[0]
    header, disk_size, sector, sectors, heads, cylinders = struct.unpack_from('<6I', data, 8)
    if header < 32 or header + disk_size != len(data) or not all((sector, sectors, heads, cylinders)):
        raise ValueError('Invalid/unsupported HDI geometry')
    partitions = []
    entries = []
    for index in range(16):
        p = header + sector + index * 32
        if not data[p] & 0x7f:
            continue
        start_sector, start_head, start_cylinder = struct.unpack_from('<BBH', data, p + 8)
        base = header + ((start_cylinder * heads + start_head) * sectors + start_sector) * sector
        bps, spc, reserved, fats, root_count, total, media, spf = struct.unpack_from('<HBHBHHBH', data, base + 11)
        if bps not in (512, 1024, 2048, 4096) or not spc or not spf or not fats:
            raise ValueError('Unsupported BPB')
        root_sectors = (root_count * 32 + bps - 1) // bps
        clusters = (total - reserved - fats * spf - root_sectors) // spc
        if not 0 < clusters < 4085:
            raise ValueError('Only FAT12 supported')
        if base + total * bps > len(data):
            raise ValueError('Partition outside image')
        fat_base = base + reserved * bps
        fat = data[fat_base:fat_base + spf * bps]
        for copy in range(1, fats):
            if fat != data[fat_base + copy * spf * bps:fat_base + (copy + 1) * spf * bps]:
                raise ValueError('FAT copies differ')
        root_base = fat_base + fats * spf * bps
        data_base = root_base + root_sectors * bps
        cluster_bytes = bps * spc
        partitions.append(dict(index=index, offset=base, bps=bps, spc=spc, clusters=clusters, fat='FAT12', fat_copies_match=True))
        visited_dirs = set()

        def chain(first):
            result, seen = bytearray(), set()
            while first < 0xff8:
                if first < 2 or first >= clusters + 2 or first in seen:
                    raise ValueError('Invalid or cyclic FAT chain')
                seen.add(first)
                at = data_base + (first - 2) * cluster_bytes
                result.extend(data[at:at + cluster_bytes])
                at = first + first // 2
                value = int.from_bytes(fat[at:at + 2], 'little')
                first = value >> 4 if first & 1 else value & 0xfff
            return bytes(result)

        def directory(raw, prefix):
            for at in range(0, len(raw), 32):
                e = raw[at:at + 32]
                if e[0] == 0:
                    break
                if e[0] == 0xe5 or e[11] == 0x0f or e[11] & 8:
                    continue
                name = e[:8].rstrip(b' ').decode('cp932')
                ext = e[8:11].rstrip(b' ').decode('cp932')
                if name in ('.', '..'):
                    continue
                name += '.' + ext if ext else ''
                if any(c in name for c in '/\\:') or name in ('', '.', '..'):
                    raise ValueError('Unsafe filename')
                path = prefix / name
                first = int.from_bytes(e[26:28], 'little')
                size = int.from_bytes(e[28:32], 'little')
                if e[11] & 16:
                    if select is not None and not any(path in target.parents for target in select):
                        continue
                    if first in visited_dirs:
                        raise ValueError('Duplicate directory cluster')
                    visited_dirs.add(first)
                    directory(chain(first), path)
                    continue
                if select is not None and path not in select:
                    continue
                payload = chain(first) if first else b''
                if len(payload) < size:
                    raise ValueError('Truncated file chain')
                payload = payload[:size]
                entries.append(dict(path=path.as_posix(), size=size, sha256=sha(payload), attributes=e[11], partition=index))
                if selected is not None:
                    if path in selected:
                        raise ValueError('Duplicate selected FAT member')
                    selected[path] = payload
                if destination is not None:
                    target = destination / path
                    target.parent.mkdir(parents=True, exist_ok=True)
                    if target.exists():
                        if target.read_bytes() != payload:
                            raise ValueError('Refusing to overwrite differing extraction')
                    else:
                        with target.open('xb') as stream:
                            stream.write(payload)
        directory(data[root_base:root_base + root_count * 32], Path('partition_' + str(index)))
    if not partitions:
        raise ValueError('No partitions')
    if selected is not None:
        if selected.keys() != select:
            raise ValueError('Missing selected FAT member')
        return selected
    return dict(source=str(image), size=len(data), sha256=sha(data), geometry=dict(header=header, sector=sector, sectors=sectors, heads=heads, cylinders=cylinders), partitions=partitions, files=entries)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--original', type=Path, required=True)
    parser.add_argument('--patched', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    results = {label: inspect(image, args.output / label) for label, image in [('original', args.original), ('patched', args.patched)]}
    left, right = ({f['path']: f for f in results[label]['files']} for label in ('original', 'patched'))
    results['comparison'] = dict(added=sorted(right.keys() - left.keys()), removed=sorted(left.keys() - right.keys()), changed=sorted(p for p in left.keys() & right.keys() if left[p]['sha256'] != right[p]['sha256']), identical=sorted(p for p in left.keys() & right.keys() if left[p]['sha256'] == right[p]['sha256']))
    for label, image in [('original', args.original), ('patched', args.patched)]:
        with image.open('rb') as stream:
            if hashlib.file_digest(stream, 'sha256').hexdigest() != results[label]['sha256']:
                raise ValueError('Source changed during inspection')
    (args.output / 'baseline.json').write_text(json.dumps(results, ensure_ascii=False, indent=2), encoding='utf-8')
    print(json.dumps({label: dict(size=results[label]['size'], sha256=results[label]['sha256'], files=len(results[label]['files']), partitions=results[label]['partitions']) for label in ('original', 'patched')}, ensure_ascii=False, indent=2))
    print(json.dumps(results['comparison'], ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
