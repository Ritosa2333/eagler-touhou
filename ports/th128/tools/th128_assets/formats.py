import math
import re
import struct
import zlib
from collections import Counter


class FormatError(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise FormatError(message)


def span(data, offset, size, limit=None):
    boundary = len(data) if limit is None else min(limit, len(data))
    require(0 <= offset <= boundary and 0 <= size <= boundary - offset,
            f'out-of-bounds data at {offset:#x}, size={size}, boundary={boundary:#x}')
    return data[offset:offset + size]


def integer(data, offset, kind='<I', limit=None):
    return struct.unpack(kind, span(data, offset, struct.calcsize(kind), limit))[0]


def cstring(data, offset, limit):
    require(0 <= offset < limit <= len(data), 'string offset outside entry')
    ending = data.find(b'\0', offset, min(limit, offset + 4096))
    require(ending >= offset, f'unterminated string at {offset:#x}')
    try:
        return data[offset:ending].decode('ascii'), ending + 1
    except UnicodeDecodeError as error:
        raise FormatError(f'non-ASCII resource identifier at {offset:#x}') from error


def validate_archive_name(name):
    require(isinstance(name, str) and bool(re.fullmatch(r'[A-Za-z0-9_][A-Za-z0-9_.-]{0,199}', name)),
            f'unsafe archive name: {name!r}')
    require(not name.endswith('.') and '..' not in name, f'unsafe archive name: {name!r}')
    stem = name.split('.')[0].upper()
    require(stem not in {'CON', 'PRN', 'AUX', 'NUL', 'CONIN$', 'CONOUT$'}
            and not re.fullmatch(r'(COM|LPT)[0-9]', stem), f'Windows device name: {name!r}')
    return name


def dimensions(width, height):
    require(0 < width <= 8192 and 0 < height <= 8192 and width * height <= 16777216,
            f'unsupported texture dimensions {width}x{height}')


def decode_texture(texture_format, width, height, payload):
    dimensions(width, height)
    byte_depth = {1: 4, 3: 2, 5: 2, 7: 1, 0xFFFF: 4}.get(texture_format)
    require(byte_depth is not None, f'unsupported texture format {texture_format}')
    require(len(payload) == width * height * byte_depth, 'texture payload size mismatch')
    if texture_format == 0xFFFF:
        return bytes(payload)
    output = bytearray(width * height * 4)
    if texture_format == 1:
        output[0::4] = payload[2::4]
        output[1::4] = payload[1::4]
        output[2::4] = payload[0::4]
        output[3::4] = payload[3::4]
    elif texture_format == 7:
        output[0::4] = output[1::4] = output[2::4] = payload
        output[3::4] = b'\xff' * (width * height)
    else:
        target = 0
        for (packed,) in struct.iter_unpack('<H', payload):
            if texture_format == 5:
                channels = ((packed >> 8) & 15, (packed >> 4) & 15, packed & 15, (packed >> 12) & 15)
                output[target:target + 4] = bytes(channel * 17 for channel in channels)
            else:
                red, green, blue = (packed >> 11) & 31, (packed >> 5) & 63, packed & 31
                output[target:target + 4] = bytes(((red << 3) | (red >> 2),
                                                  (green << 2) | (green >> 4),
                                                  (blue << 3) | (blue >> 2), 255))
            target += 4
    return bytes(output)


def encode_png(width, height, rgba):
    dimensions(width, height)
    require(len(rgba) == width * height * 4, 'RGBA size mismatch')

    def chunk(kind, payload):
        return struct.pack('>I', len(payload)) + kind + payload + struct.pack('>I', zlib.crc32(kind + payload))

    stride = width * 4
    scanlines = b''.join(b'\0' + rgba[offset:offset + stride] for offset in range(0, len(rgba), stride))
    return (b'\x89PNG\r\n\x1a\n'
            + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 6, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(scanlines, 6)) + chunk(b'IEND', b''))


def parse_anm(data, *, max_commands=200000):
    require(len(data) >= 64, 'truncated ANM header')
    entries, sprites, scripts = [], [], []
    command_count = 0
    base = 0
    while True:
        span(data, base, 64)
        version = integer(data, base)
        require(version == 7, f'unsupported ANM version {version}')
        sprite_count = integer(data, base + 4, '<H')
        script_count = integer(data, base + 6, '<H')
        require(len(entries) < 4096 and len(sprites) + sprite_count <= 65536 and script_count <= 4096,
                'ANM decoded-object budget exceeded')
        next_offset = integer(data, base + 36)
        end = base + next_offset if next_offset else len(data)
        require(base + 64 <= end <= len(data), 'invalid next ANM entry')
        table_end = base + 64 + sprite_count * 4 + script_count * 8
        span(data, base + 64, table_end - base - 64, end)
        name, _ = cstring(data, base + integer(data, base + 16), end)
        entry_index = len(entries)
        entry = {'index': entry_index, 'offset': base, 'name': name,
                 'width': integer(data, base + 10, '<H'), 'height': integer(data, base + 12, '<H'),
                 'xOffset': integer(data, base + 20, '<H'), 'yOffset': integer(data, base + 22, '<H'),
                 'format': integer(data, base + 14, '<H'), 'texture': None}
        texture_offset = integer(data, base + 28)
        if integer(data, base + 32, '<H'):
            texture_base = base + texture_offset
            require(texture_base >= table_end, 'texture overlaps ANM tables')
            texture_header = span(data, texture_base, 16, end)
            magic, reserved, texture_format, width, height, size = struct.unpack('<4sHHHHI', texture_header)
            require(magic == b'THTX' and reserved == 0, 'invalid THTX header')
            dimensions(width, height)
            byte_depth = {1: 4, 3: 2, 5: 2, 7: 1, 0xFFFF: 4}.get(texture_format)
            require(byte_depth is not None, f'unsupported texture format {texture_format}')
            required_size = width * height * byte_depth
            require(size >= required_size, 'short declared THTX payload')
            span(data, texture_base + 16, size, end)
            entry['texture'] = {'format': texture_format, 'width': width, 'height': height,
                                'data': span(data, texture_base + 16, required_size, end)}
        for sprite_number in range(sprite_count):
            offset = base + integer(data, base + 64 + sprite_number * 4)
            require(offset >= table_end, 'sprite overlaps ANM tables')
            source_id, position_x, position_y, width, height = struct.unpack('<Iffff', span(data, offset, 20, end))
            require(all(math.isfinite(value) for value in (position_x, position_y, width, height)),
                    'non-finite sprite geometry')
            sprites.append({'index': len(sprites), 'sourceId': source_id, 'entry': entry_index,
                            'x': position_x, 'y': position_y, 'width': width, 'height': height})
        script_table = base + 64 + sprite_count * 4
        records = [struct.unpack('<iI', span(data, script_table + number * 8, 8, end))
                   for number in range(script_count)]
        starts = [base + relative for _, relative in records]
        require(len(starts) == len(set(starts)), 'aliased ANM script bodies are not supported')
        require(all(table_end <= start < end for start in starts), 'invalid ANM script offset')
        boundaries = sorted({end, *starts, *( [base + texture_offset] if table_end <= base + texture_offset <= end else [])})
        limits = dict(zip(boundaries, boundaries[1:]))
        for source_id, relative_start in records:
            start = base + relative_start
            limit = limits[start]
            commands, position = [], start
            while True:
                header = span(data, position, 8, limit)
                opcode, size, time, mask = struct.unpack('<HHhH', header)
                if opcode == 65535:
                    break
                require(size >= 8, f'invalid ANM command size at {position:#x}')
                payload = span(data, position + 8, size - 8, limit)
                command_count += 1
                require(command_count <= max_commands, 'ANM aggregate command budget exceeded')
                commands.append({'offset': position - start, 'opcode': opcode, 'time': time,
                                 'mask': mask, 'raw': payload.hex()})
                position += size
            scripts.append({'index': len(scripts), 'sourceId': source_id, 'entry': entry_index,
                            'offset': start, 'commands': commands})
        entries.append(entry)
        if not next_offset:
            break
        base = end
    return {'version': 7, 'entries': entries, 'sprites': sprites, 'scripts': scripts}


def parse_ecl(data, *, max_commands=200000):
    require(len(data) >= 36 and data[:4] == b'SCPT' and integer(data, 4, '<H') == 1,
            'unsupported or truncated ECL header')
    require(integer(data, 8) == 36, 'unsupported ECL include-section offset')
    table = 36 + integer(data, 6, '<H')
    count = integer(data, 16)
    require(count <= 4096, 'excessive ECL subroutine count')
    span(data, table, count * 4)
    offsets = [integer(data, table + number * 4) for number in range(count)]
    require(len(set(offsets)) == len(offsets), 'duplicate ECL subroutine offsets')
    ordered_offsets = sorted([*offsets, len(data)])
    limits = dict(zip(ordered_offsets, ordered_offsets[1:]))
    names_end = min(offsets, default=len(data))
    name_position = table + count * 4
    names = []
    for _ in offsets:
        name, name_position = cstring(data, name_position, names_end)
        names.append(name)
    require(len(set(names)) == len(names), 'duplicate ECL subroutine names')
    includes, position = {'ANIM': [], 'ECLI': []}, 36
    while position < table:
        require(position + 8 <= table, 'truncated ECL includes')
        kind = span(data, position, 4).decode('ascii', errors='replace')
        require(kind in includes, f'unknown ECL include section {kind!r}')
        include_count = integer(data, position + 4)
        require(include_count <= 4096, 'excessive ECL includes')
        position += 8
        for _ in range(include_count):
            name, position = cstring(data, position, table)
            includes[kind].append(validate_archive_name(name))
        position = (position + 3) & ~3
    subroutines, histogram, command_count = [], Counter(), 0
    for name, offset in zip(names, offsets):
        require(offset >= name_position and span(data, offset, 4) == b'ECLH', 'invalid ECL subroutine')
        limit = limits[offset]
        span(data, offset, 16, limit)
        position, commands = offset + 16, []
        while position < limit:
            time, opcode, size, mask, rank, parameters, stack = struct.unpack('<iHHHBBI', span(data, position, 16, limit))
            require(size >= 16 and size % 4 == 0, f'invalid ECL command size at {position:#x}')
            payload = span(data, position + 16, size - 16, limit)
            command_count += 1
            require(command_count <= max_commands, 'ECL aggregate command budget exceeded')
            commands.append({'offset': position - offset - 16, 'time': time, 'opcode': opcode,
                             'mask': mask, 'rankMask': rank, 'parameterCount': parameters,
                             'stackOffset': stack, 'raw': payload.hex()})
            histogram[str(opcode)] += 1
            position += size
        subroutines.append({'name': name, 'offset': offset, 'commands': commands})
    return {'format': 'SCPT/1', 'includes': includes, 'subroutines': subroutines,
            'opcodeHistogram': dict(sorted(histogram.items(), key=lambda pair: int(pair[0]))),
            'executed': False}


def parse_music(data, archive_size):
    require(bool(data), 'empty music table')
    tail_size = len(data) % 52
    require(not tail_size or not any(data[-tail_size:]), 'nonzero incomplete music record')
    tracks, names = [], set()
    for offset in range(0, len(data) - tail_size, 52):
        record = span(data, offset, 52)
        if not any(record):
            continue
        fields = struct.unpack('<16sIIIIHHIIHHI', record)
        try:
            name = fields[0].split(b'\0', 1)[0].decode('ascii')
        except UnicodeDecodeError as error:
            raise FormatError('non-ASCII music filename') from error
        validate_archive_name(name)
        require(bool(re.fullmatch(r'th128_[0-9]{2}\.wav', name)), f'unexpected TH128 track: {name}')
        require(name not in names, 'duplicate music track')
        names.add(name)
        start, nominal, loop_start, length, encoding, channels, rate, average, alignment, bits, _ = fields[1:]
        require(encoding == 1 and channels in (1, 2) and bits == 16 and 0 < rate <= 192000,
                f'unsupported PCM format for {name}')
        require(alignment == channels * bits // 8 and average == rate * alignment,
                f'invalid PCM alignment/rate for {name}')
        require(start <= archive_size and 0 < length <= archive_size - start, f'music outside archive: {name}')
        require(0 <= loop_start < length and loop_start % alignment == 0 and length % alignment == 0,
                f'invalid loop bounds for {name}')
        tracks.append({'name': name, 'offset': start, 'nominalBytes': nominal, 'pcmBytes': length,
                       'loopStartBytes': loop_start, 'loopStart': loop_start / average,
                       'loopEnd': length / average, 'sampleRate': rate, 'channels': channels,
                       'bits': bits, 'blockAlign': alignment})
    require(bool(tracks), 'music table has no tracks')
    return tracks
