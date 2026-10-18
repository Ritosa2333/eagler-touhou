"""Read module and thread metadata from this source build's Windows minidump."""
import json
import struct
import sys
from pathlib import Path

data = Path(sys.argv[1]).read_bytes()
signature, version, count, directory = struct.unpack_from('<4I', data)
assert signature == 0x504d444d
streams = {kind: (size, rva) for kind, size, rva in
           (struct.unpack_from('<3I', data, directory + i * 12) for i in range(count))}
modules = []
if 4 in streams:
    _, rva = streams[4]
    for i in range(struct.unpack_from('<I', data, rva)[0]):
        off = rva + 4 + i * 108
        base, size, checksum, stamp, name = struct.unpack_from('<Q4I', data, off)
        length = struct.unpack_from('<I', data, name)[0]
        filename = data[name + 4:name + 4 + length].decode('utf-16-le')
        modules.append({'base': hex(base), 'size': size, 'path': filename})
print(json.dumps({'modules': modules}, indent=2))
