"""Read the TH12 1.00b PE sound tables without executing native code.

Sound records are at VA4ae5a0 (60 records,20 bytes each); their filename-index
table is at VA4aea50. The output schema is the runtime's sound-index array.
"""
import hashlib
import json
import os
import pathlib
import re
import shutil
import struct

import pefile

ROOT = pathlib.Path(__file__).resolve().parents[1]
ORIGINAL = pathlib.Path(os.environ.get('TH12_ORIGINAL', ROOT / 'local/original')).expanduser().resolve()
RETAIL = pathlib.Path(os.environ.get('TH12_RETAIL', ROOT / 'local/retail')).expanduser().resolve()
ASSETS = pathlib.Path(os.environ.get('TH12_ASSETS', ROOT / 'site/assets')).expanduser().resolve()
from retail_version import validate_executable


def main():
    executable = ORIGINAL / 'th12.exe'
    validate_executable(executable)
    pe = pefile.PE(str(executable))
    image = pe.get_memory_mapped_image()
    image_base = pe.OPTIONAL_HEADER.ImageBase

    def offset(address, length):
        at = address - image_base
        if at < 0 or at + length > len(image):
            raise ValueError('Native sound table points outside the PE image')
        return at

    sounds = []
    for index in range(60):
        at = offset(0x4ae5a0 + index * 20, 20)
        sound_id, filename_index = struct.unpack_from('<II', image, at)
        pointer = struct.unpack_from('<I', image, offset(0x4aea50 + filename_index * 4, 4))[0]
        start = offset(pointer, 1)
        end = image.find(b'\0', start, min(len(image), start + 200))
        if end < 0:
            raise ValueError('Native sound filename is not terminated')
        filename = image[start:end].decode('ascii')
        if not re.fullmatch(r'se_[A-Za-z0-9_]+\.wav', filename):
            raise ValueError(f'Unsupported native sound filename: {filename!r}')
        sounds.append({'id': sound_id, 'file': filename,
                       'volume': struct.unpack_from('<h', image, at + 8)[0],
                       'loop': bool(struct.unpack_from('<I', image, at + 12)[0])})
    sounds.sort(key=lambda sound: sound['id'])
    if len({sound['id'] for sound in sounds}) != len(sounds):
        raise ValueError('Native sound IDs are not unique')
    ASSETS.mkdir(parents=True, exist_ok=True)
    for filename in sorted({sound['file'] for sound in sounds}):
        source = RETAIL / filename
        if not source.is_file():
            raise FileNotFoundError(f'Missing extracted sound resource: {source}')
        destination = ASSETS / filename
        if source.resolve() != destination.resolve():
            shutil.copyfile(source, destination)
    (ASSETS / 'sound-index.json').write_text(json.dumps(sounds, indent=2) + '\n', encoding='utf-8')
    print(f'Prepared {len(sounds)} sound records from static PE tables; no native code executed.')


if __name__ == '__main__':
    main()
