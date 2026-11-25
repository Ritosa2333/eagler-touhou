"""Extract only the ZUN Soft opening from a user-supplied TH05 or TH04 DAT."""
import argparse
import gzip
import struct
from pathlib import Path

from hdi_baseline import inspect
from pf_extract import decode


ART = ('ZUN00.PI', 'ZUN02.BFT', 'ZUN04.BFT', 'ZUN01.BFT', 'ZUN03.BFT')


def extract(data):
    size = struct.unpack_from('<H', data)[0]
    if size % 32 or 16 + size > len(data):
        raise ValueError('Invalid TH04/05 archive table')
    table = bytearray(data[16:16 + size])
    key = data[6]
    for i, value in enumerate(table):
        table[i] = value ^ key
        key = (key - table[i]) & 255
    wanted = set(ART) | {'LOGO.M', 'LOGO.M26', 'LOGO.M86', 'MIKO.EFC'}
    members = {}
    for i in range(0, size, 32):
        mode, member_key, filename, packed, unpacked, offset = struct.unpack_from('<HB13sHHI', table, i)
        if not mode:
            break
        name = filename.split(b'\0', 1)[0].decode('ascii').upper()
        if name not in wanted:
            continue
        if mode not in (0x9595, 0xf388) or offset < 16 + size or offset + packed > len(data):
            raise ValueError(f'Invalid archive member: {name}')
        content = decode(bytes(b ^ member_key for b in data[offset:offset + packed]), mode == 0x9595)
        if len(content) != unpacked:
            raise ValueError(f'Wrong decoded size for {name}: {len(content)} != {unpacked}')
        members[name] = content
    result = {name: members[name] for name in ART}
    result['LOGO.M'] = next(members[name] for name in ('LOGO.M86', 'LOGO.M', 'LOGO.M26') if name in members)
    if 'MIKO.EFC' in members:
        result['MIKO.EFC'] = members['MIKO.EFC']
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path, help='TH05 KAIKI1.DAT, TH04 opening DAT, or original TH04 .hdi[.gz]')
    parser.add_argument('content', type=Path, help='SoEW content directory')
    args = parser.parse_args()
    if args.source.name.lower().endswith(('.hdi', '.hdi.gz')):
        disk = gzip.decompress(args.source.read_bytes()) if args.source.suffix.lower() == '.gz' else args.source.read_bytes()
        member, = (Path(f['path']) for f in inspect(disk)['files'] if Path(f['path']).name == '幻想郷ED.DAT')
        raw = inspect(disk, select={member})[member]
    else:
        raw = args.source.read_bytes()
    members = extract(raw)
    output = args.content / 'zunsoft'
    output.mkdir(parents=True, exist_ok=True)
    for name, data in members.items():
        path = output / name
        if path.exists():
            if path.read_bytes() != data:
                raise ValueError(f'Refusing to overwrite differing asset: {path}')
        else:
            with path.open('xb') as stream:
                stream.write(data)
        print(path)


if __name__ == '__main__':
    main()
