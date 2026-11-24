import struct

from .formats import require


TABLE_OFFSET = 0x268
RECORD_SIZE = 0x34
MAX_SIZE = 4 * 1024 * 1024
MAX_GROUPS = 256
MAX_RECORDS = 65536


def decode_shot(data, *, max_size=MAX_SIZE, max_groups=MAX_GROUPS, max_records=MAX_RECORDS):
    for value, cap, name in ((max_size, MAX_SIZE, 'size'), (max_groups, MAX_GROUPS, 'groups'),
                             (max_records, MAX_RECORDS, 'records')):
        require(type(value) is int and 1 <= value <= cap, f'invalid shot {name} budget')
    require(isinstance(data, (bytes, bytearray, memoryview)), 'shot data must be bytes-like')
    size = data.nbytes if isinstance(data, memoryview) else len(data)
    require(TABLE_OFFSET <= size <= max_size, 'shot header truncated or file size budget exceeded')
    raw = bytes(data)
    tag, count = struct.unpack_from('<HH', raw)
    require(tag == 3, 'unsupported shot header tag: only actual tag 3 layout is confirmed')
    require(1 <= count <= max_groups, 'shot group count exceeds budget or is empty')
    table_end = TABLE_OFFSET + count * 8
    require(table_end <= size, 'truncated shot group table')
    entries = [struct.unpack_from('<II', raw, TABLE_OFFSET + index * 8) for index in range(count)]
    for offset, unknown in entries:
        require(table_end <= offset < size, 'shot group offset outside record area')
    offsets = sorted({offset for offset, unknown in entries})
    limits = dict(zip(offsets, offsets[1:] + [size]))
    groups, record_count = [], 0
    tails = {}
    for offset, unknown in entries:
        cursor, limit, records = offset, limits[offset], []
        while True:
            require(cursor < limit, 'shot group has no bounded negative-byte terminator')
            period = struct.unpack_from('<b', raw, cursor)[0]
            if period < 0:
                break
            require(period > 0, 'zero shot period would divide by zero in the native scheduler')
            require(RECORD_SIZE <= limit - cursor, 'truncated or overlapping shot record')
            record_count += 1
            require(record_count <= max_records, 'shot record count exceeds budget')
            record = raw[cursor:cursor + RECORD_SIZE]
            records.append({'offset': cursor, 'raw': record, 'period': period,
                            'phase': struct.unpack_from('<b', record, 1)[0],
                            'payloadRaw': record[2:0x24],
                            'callbackIds': list(struct.unpack_from('<4I', record, 0x24))})
            cursor += RECORD_SIZE
        tail_key = (cursor, limit)
        if tail_key not in tails:
            tails[tail_key] = raw[cursor:limit]
        groups.append({'offset': offset, 'unknownWord': unknown, 'records': records,
                       'terminatorOffset': cursor, 'tailRaw': tails[tail_key]})
    return {'layout': 'th128-sht52-tag3', 'tagWord': tag, 'groupCount': count,
            'speedFloats': list(struct.unpack_from('<4f', raw, 0x10)),
            'headerRaw': raw[:TABLE_OFFSET], 'tableRaw': raw[TABLE_OFFSET:table_end],
            'raw': raw, 'groups': groups, 'recordCount': record_count}
