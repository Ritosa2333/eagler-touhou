"""Extract TH02 PF archive; format reference ReC98 th02/formats/pfopen.asm.
Decoder independently implemented from documented PFGETC1 state machine.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def decode(packed, compressed):
    if not compressed:
        return packed
    output = bytearray()
    previous = None
    pos = 0
    while pos < len(packed):
        value = packed[pos]
        pos += 1
        output.append(value)
        if value == previous:
            if pos == len(packed):
                raise ValueError('Missing RLE length')
            count = packed[pos]
            pos += 1
            output.extend(bytes([value]) * count)
        previous = value
    return bytes(output)


def extract(path, destination, allow_size_mismatch=False):
    raw = path.read_bytes()
    entries = []
    first_payload = len(raw)
    for at in range(0, min(len(raw), 255 * 32), 32):
        if at >= first_payload:
            break
        kind, aux, filename, packed_size, original_size, offset, reserved = struct.unpack_from('<HB13sIII4s', raw, at)
        if kind == 0:
            break
        if kind not in (0xf388, 0x9595):
            raise ValueError(f'Unknown PF kind {kind:x} at {at}')
        filename = filename.split(b'\0', 1)[0]
        name = bytes(v ^ 255 for v in filename).decode('cp932')
        if not name or any(c in name for c in '/\\:') or name in ('.', '..'):
            raise ValueError('Unsafe archive filename')
        first_payload = min(first_payload, offset)
        if offset < at + 32 or offset + packed_size > len(raw):
            raise ValueError('Invalid PF payload range')
        packed = bytes(v ^ 0x12 for v in raw[offset:offset + packed_size])
        payload = decode(packed, kind == 0x9595)
        if len(payload) != original_size and not allow_size_mismatch:
            raise ValueError(f'{name}: size {len(payload)} != {original_size}')
        target = destination / name
        destination.mkdir(parents=True, exist_ok=True)
        if target.exists():
            if target.read_bytes() != payload:
                raise ValueError('Existing extraction differs')
        else:
            with target.open('xb') as stream:
                stream.write(payload)
        entries.append(dict(name=name, kind=hex(kind), aux=aux, offset=offset, packed_size=packed_size, size=original_size, decoded_size=len(payload), size_matches=len(payload) == original_size, sha256=hashlib.sha256(payload).hexdigest()))
    return entries


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('archive', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--allow-size-mismatch', action='store_true', help='Preserve full decoded data and flag mismatches in index')
    args = parser.parse_args()
    entries = extract(args.archive, args.output, args.allow_size_mismatch)
    (args.output / 'pf-index.json').write_text(json.dumps(entries, indent=2, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(entries, indent=2, ensure_ascii=False))


if __name__ == '__main__':
    main()
