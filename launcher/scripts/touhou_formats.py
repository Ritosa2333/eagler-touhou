"""Small, dependency-free Touhou resource readers used by host assembly.

This module intentionally implements only the formats and operations the Web
release pipeline needs. It is not a general replacement for thtk.
"""

from __future__ import annotations

from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path
from struct import Struct, pack, unpack_from
import binascii
import zlib


_ANM06_HEADER = Struct("<13I2H2I")
_THTX_HEADER = Struct("<4s4H I")


class _Pbg3BitReader:
    def __init__(self, data: bytes, offset: int = 0):
        self.data = data
        self.byte_index = offset
        self.bit_mask = 0x80
        self.current = 0

    def read_bit(self) -> int:
        if self.bit_mask == 0x80:
            if self.byte_index >= len(self.data):
                raise ValueError("truncated PBG3 bit stream")
            self.current = self.data[self.byte_index]
            self.byte_index += 1
        value = 1 if self.current & self.bit_mask else 0
        self.bit_mask >>= 1
        if self.bit_mask == 0:
            self.bit_mask = 0x80
        return value

    def read_bits(self, count: int) -> int:
        value = 0
        for _ in range(count):
            value = (value << 1) | self.read_bit()
        return value

    def read_varint(self) -> int:
        header = (self.read_bit() << 1) | self.read_bit()
        return self.read_bits((header + 1) * 8)

    def read_string(self, maximum: int = 256) -> str:
        raw = bytearray()
        for _ in range(maximum):
            value = self.read_bits(8)
            if value == 0:
                return raw.decode("shift_jis")
            raw.append(value)
        raise ValueError("unterminated PBG3 filename")


def _pbg3_decompress(source: bytes, expected_size: int, expected_checksum: int) -> bytes:
    dictionary = bytearray(0x2000)
    dictionary_head = 1
    output = bytearray()
    byte_index = 0
    bit_mask = 0
    current = 0
    checksum = 0

    def read_bit() -> int:
        nonlocal byte_index, bit_mask, current, checksum
        if bit_mask == 0:
            if byte_index >= len(source):
                raise ValueError("truncated PBG3 compressed stream")
            current = source[byte_index]
            byte_index += 1
            checksum = (checksum + current) & 0xFFFFFFFF
            bit_mask = 0x80
        value = 1 if current & bit_mask else 0
        bit_mask >>= 1
        return value

    def read_bits(count: int) -> int:
        value = 0
        for _ in range(count):
            value = (value << 1) | read_bit()
        return value

    def write_byte(value: int) -> None:
        nonlocal dictionary_head
        if len(output) >= expected_size:
            raise ValueError("PBG3 stream exceeds declared size")
        output.append(value)
        dictionary[dictionary_head] = value
        dictionary_head = (dictionary_head + 1) & 0x1FFF

    while True:
        if read_bit():
            write_byte(read_bits(8))
            continue
        match_offset = read_bits(13)
        if match_offset == 0:
            break
        match_length = read_bits(4) + 3
        for index in range(match_length):
            write_byte(dictionary[(match_offset + index) & 0x1FFF])

    if len(output) != expected_size:
        raise ValueError(f"PBG3 size mismatch: expected {expected_size}, got {len(output)}")
    if checksum != expected_checksum:
        raise ValueError(f"PBG3 checksum mismatch: expected {expected_checksum:#x}, got {checksum:#x}")
    return bytes(output)


def extract_pbg3_entry(archive_path: Path | str, wanted: str) -> bytes:
    """Extract one file from the TH06 PBG3 archive format."""

    archive_path = Path(archive_path)
    archive = archive_path.read_bytes()
    reader = _Pbg3BitReader(archive)
    if reader.read_bits(32) != 0x50424733:  # bytes spell PBG3 on disk
        raise ValueError(f"{archive_path}: invalid PBG3 header")
    count = reader.read_varint()
    table_offset = reader.read_varint()
    if not 0 < count <= 65536 or not 0 < table_offset < len(archive):
        raise ValueError(f"{archive_path}: invalid PBG3 table")

    table = _Pbg3BitReader(archive, table_offset)
    entries: list[tuple[str, int, int, int]] = []
    previous_offset = -1
    for _ in range(count):
        table.read_varint()  # upstream bookkeeping fields are not needed here
        table.read_varint()
        checksum = table.read_varint()
        data_offset = table.read_varint()
        size = table.read_varint()
        name = table.read_string()
        if not 0 <= data_offset < table_offset or data_offset <= previous_offset or size <= 0:
            raise ValueError(f"{archive_path}: invalid PBG3 entry layout")
        entries.append((name, data_offset, size, checksum))
        previous_offset = data_offset

    normalized = wanted.replace("\\", "/").lower()
    for index, (name, data_offset, size, checksum) in enumerate(entries):
        if name.replace("\\", "/").lower() != normalized:
            continue
        next_offset = entries[index + 1][1] if index + 1 < len(entries) else table_offset
        if next_offset <= data_offset:
            raise ValueError(f"{archive_path}: invalid PBG3 compressed span")
        return _pbg3_decompress(archive[data_offset:next_offset], size, checksum)
    raise FileNotFoundError(f"{wanted} not found in {archive_path}")


def _lzss_decompress(source: bytes, expected_size: int, label: str) -> bytes:
    dictionary = bytearray(8192)
    dictionary_head = 1
    output = bytearray()
    byte_index = 0
    bit_mask = 0x80

    def read_bit() -> int:
        nonlocal byte_index, bit_mask
        if byte_index >= len(source):
            raise ValueError(f"truncated {label} LZSS stream")
        value = 1 if source[byte_index] & bit_mask else 0
        bit_mask >>= 1
        if bit_mask == 0:
            bit_mask = 0x80
            byte_index += 1
        return value

    def read_bits(count: int) -> int:
        value = 0
        for _ in range(count):
            value = (value << 1) | read_bit()
        return value

    while len(output) < expected_size:
        if read_bit():
            value = read_bits(8)
            output.append(value)
            dictionary[dictionary_head] = value
            dictionary_head = (dictionary_head + 1) & 0x1FFF
        else:
            offset = read_bits(13)
            if offset == 0:
                break
            length = read_bits(4) + 3
            for index in range(length):
                value = dictionary[(offset + index) & 0x1FFF]
                output.append(value)
                dictionary[dictionary_head] = value
                dictionary_head = (dictionary_head + 1) & 0x1FFF
        if len(output) > expected_size:
            raise ValueError(f"{label} LZSS stream exceeds declared size")
    if len(output) != expected_size:
        raise ValueError(f"{label} size mismatch: expected {expected_size}, got {len(output)}")
    return bytes(output)


def extract_pbg4_entry(archive_path: Path | str, wanted: str) -> bytes:
    """Extract one file from the TH07 PBG4 archive format."""

    archive_path = Path(archive_path)
    archive = archive_path.read_bytes()
    if len(archive) < 16:
        raise ValueError("invalid PBG4 header")
    magic, count, header_offset, header_size = unpack_from("<4sIII", archive)
    if magic != b"PBG4" or not 0 < count < 100000 or not 16 <= header_offset < len(archive):
        raise ValueError("invalid PBG4 header")
    header = _lzss_decompress(archive[header_offset:], header_size, "PBG4")
    cursor = 0
    entries: list[tuple[str, int, int]] = []
    for _ in range(count):
        end = header.index(0, cursor)
        name = header[cursor:end].decode("shift_jis")
        cursor = end + 1
        data_offset, size, _magic_value = unpack_from("<III", header, cursor)
        cursor += 12
        entries.append((name, data_offset, size))
    normalized = wanted.replace("\\", "/").lower()
    for index, (name, offset, size) in enumerate(entries):
        if name.replace("\\", "/").lower() != normalized:
            continue
        end_offset = entries[index + 1][1] if index + 1 < len(entries) else header_offset
        return _lzss_decompress(archive[offset:end_offset], size, "PBG4")
    raise FileNotFoundError(f"{wanted} not found in {archive_path}")


_TH08_DECRYPT_PARAMS = (
    (0x5D, 0x1B, 0x37, 0x0040, 0x2800),
    (0x74, 0x51, 0xE9, 0x0040, 0x3000),
    (0x71, 0xC1, 0x51, 0x1400, 0x2000),
    (0x8A, 0x03, 0x19, 0x1400, 0x7800),
    (0x95, 0xAB, 0xCD, 0x0200, 0x1000),
    (0xB7, 0x12, 0x34, 0x0400, 0x2800),
    (0x9D, 0x35, 0x97, 0x0080, 0x2800),
    (0xAA, 0x99, 0x37, 0x0400, 0x1000),
)


def _th08_decrypt(source: bytes, xor_value: int, xor_increment: int, chunk_size: int, max_bytes: int) -> bytes:
    size = len(source)
    num_unencrypted = size % chunk_size if size % chunk_size < chunk_size // 4 else 0
    num_unencrypted += size & 1
    encrypted_size = size - num_unencrypted
    output = bytearray(size)
    source_pos = 0
    output_pos = 0
    remaining = encrypted_size
    budget = max_bytes
    while remaining > 0 and budget > 0:
        current_chunk = min(chunk_size, remaining)
        out = output_pos + current_chunk - 1
        for _ in range((current_chunk + 1) // 2):
            output[out] = source[source_pos] ^ xor_value
            source_pos += 1
            out -= 2
            xor_value = (xor_value + xor_increment) & 0xFF
        out = output_pos + current_chunk - 2
        for _ in range(current_chunk // 2):
            output[out] = source[source_pos] ^ xor_value
            source_pos += 1
            out -= 2
            xor_value = (xor_value + xor_increment) & 0xFF
        remaining -= current_chunk
        output_pos += current_chunk
        budget -= current_chunk
    tail = remaining + num_unencrypted
    if tail:
        output[output_pos : output_pos + tail] = source[source_pos : source_pos + tail]
    return bytes(output)


def _th08_try_decrypt_resource(source: bytes) -> bytes:
    if len(source) < 4 or source[:3] != bytes((0x85 - 0x20, 0xA4 - 0x40, 0xDA - 0x60)):
        return source
    for index, (key, xor_value, xor_increment, chunk_size, max_bytes) in enumerate(_TH08_DECRYPT_PARAMS):
        expected_key = (key - (index << 4) - 0x10) & 0xFF
        if source[3] == expected_key:
            return _th08_decrypt(source[4:], xor_value, xor_increment, chunk_size, max_bytes)
    return source


def extract_pbgz_entry(archive_path: Path | str, wanted: str) -> bytes:
    """Extract one file from the TH08 PBGZ archive format."""

    archive_path = Path(archive_path)
    archive = archive_path.read_bytes()
    if len(archive) < 16 or archive[:4] != b"PBGZ":
        raise ValueError("invalid TH08 PBGZ header")
    encoded_header = _th08_decrypt(archive[4:16], 0x1B, 0x37, 12, 0x400)
    encoded_count, encoded_table_offset, encoded_table_size = unpack_from("<iii", encoded_header)
    count = encoded_count - 123456
    table_offset = encoded_table_offset - 345678
    table_size = encoded_table_size - 567891
    if count <= 0 or not 16 <= table_offset < len(archive) or table_size <= 0:
        raise ValueError("invalid TH08 PBGZ metadata")
    table = _lzss_decompress(
        _th08_decrypt(archive[table_offset:], 0x3E, 0x9B, 0x80, 0x400),
        table_size,
        "TH08",
    )
    entries: list[tuple[str, int, int]] = []
    cursor = 0
    for _ in range(count):
        end = table.index(0, cursor)
        name = table[cursor:end].decode("shift_jis")
        cursor = end + 1
        data_offset, decompressed_size, _metadata = unpack_from("<III", table, cursor)
        cursor += 12
        entries.append((name, data_offset, decompressed_size))
    normalized = wanted.replace("\\", "/").lower()
    wanted_basename = normalized.rsplit("/", 1)[-1]
    for index, (name, data_offset, decompressed_size) in enumerate(entries):
        normalized_name = name.replace("\\", "/").lower()
        if normalized_name != normalized and normalized_name.rsplit("/", 1)[-1] != wanted_basename:
            continue
        end_offset = entries[index + 1][1] if index + 1 < len(entries) else table_offset
        compressed = archive[data_offset:end_offset]
        return _th08_try_decrypt_resource(_lzss_decompress(compressed, decompressed_size, "TH08"))
    raise FileNotFoundError(f"{wanted} not found in {archive_path}")


@dataclass(frozen=True)
class AnmTexture:
    name: str
    width: int
    height: int
    format: int
    pixels: bytes


def _read_c_string(data: bytes, offset: int) -> str:
    if not 0 <= offset < len(data):
        raise ValueError("ANM string offset outside file")
    end = data.find(b"\0", offset)
    if end < 0:
        raise ValueError("unterminated ANM string")
    return data[offset:end].decode("ascii")


def iter_anm_v2_textures(anm: bytes):
    """Yield embedded TH07 ANM v2 textures.

    Struct layout and ARGB4444 channel semantics follow upstream thtk/thanm.
    """

    entry_offset = 0
    while entry_offset < len(anm):
        if entry_offset + _ANM06_HEADER.size > len(anm):
            raise ValueError("truncated ANM header")
        header = _ANM06_HEADER.unpack_from(anm, entry_offset)
        version = header[10]
        if version != 2:
            raise ValueError(f"unsupported ANM version {version}; expected TH07 v2")
        name = _read_c_string(anm, entry_offset + header[7])
        thtx_offset = header[12]
        has_data = header[13]
        if has_data:
            absolute = entry_offset + thtx_offset
            if absolute + _THTX_HEADER.size > len(anm):
                raise ValueError("truncated THTX header")
            magic, zero, pixel_format, width, height, size = _THTX_HEADER.unpack_from(anm, absolute)
            if magic != b"THTX" or zero != 0:
                raise ValueError("invalid THTX header")
            start = absolute + _THTX_HEADER.size
            end = start + size
            if end > len(anm):
                raise ValueError("truncated THTX payload")
            yield AnmTexture(name=name, width=width, height=height, format=pixel_format, pixels=anm[start:end])
        next_offset = header[15]
        if next_offset == 0:
            return
        if next_offset < _ANM06_HEADER.size:
            raise ValueError("invalid ANM next offset")
        entry_offset += next_offset


def extract_anm_v2_texture(anm: bytes, wanted: str) -> AnmTexture:
    normalized = wanted.replace("\\", "/").lower()
    for texture in iter_anm_v2_textures(anm):
        if texture.name.replace("\\", "/").lower() == normalized:
            return texture
    raise FileNotFoundError(f"{wanted} not found in ANM")


def texture_rgba(texture: AnmTexture) -> bytes:
    pixels = texture.width * texture.height
    if texture.format == 1:  # BGRA8888
        if len(texture.pixels) != pixels * 4:
            raise ValueError("BGRA8888 payload size mismatch")
        out = bytearray(len(texture.pixels))
        for index in range(pixels):
            b, g, r, a = texture.pixels[index * 4 : index * 4 + 4]
            out[index * 4 : index * 4 + 4] = bytes((r, g, b, a))
        return bytes(out)
    if texture.format == 5:  # ARGB4444 stored as 0xGB 0xAR
        if len(texture.pixels) != pixels * 2:
            raise ValueError("ARGB4444 payload size mismatch")
        out = bytearray(pixels * 4)
        for index in range(pixels):
            gb = texture.pixels[index * 2]
            ar = texture.pixels[index * 2 + 1]
            g4 = gb >> 4
            b4 = gb & 0x0F
            a4 = ar >> 4
            r4 = ar & 0x0F
            out[index * 4 : index * 4 + 4] = bytes((r4 * 17, g4 * 17, b4 * 17, a4 * 17))
        return bytes(out)
    raise ValueError(f"unsupported ANM texture format {texture.format}")


def encode_rgba_png(width: int, height: int, rgba: bytes) -> bytes:
    """Encode an 8-bit RGBA PNG with only the Python standard library."""

    if width <= 0 or height <= 0 or len(rgba) != width * height * 4:
        raise ValueError("invalid RGBA image dimensions")

    def chunk(kind: bytes, payload: bytes) -> bytes:
        body = kind + payload
        return len(payload).to_bytes(4, "big") + body + (binascii.crc32(body) & 0xFFFFFFFF).to_bytes(4, "big")

    scanlines = b"".join(b"\0" + rgba[row * width * 4 : (row + 1) * width * 4] for row in range(height))
    ihdr = width.to_bytes(4, "big") + height.to_bytes(4, "big") + bytes((8, 6, 0, 0, 0))
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", zlib.compress(scanlines, 9)) + chunk(b"IEND", b"")


def decode_rgba_png(data: bytes) -> tuple[int, int, bytes]:
    """Decode the non-interlaced 8-bit RGB/RGBA PNG subset used by fixtures."""

    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("invalid PNG signature")
    offset = 8
    width = height = color_type = bit_depth = None
    compressed = bytearray()
    while offset + 12 <= len(data):
        size = int.from_bytes(data[offset : offset + 4], "big")
        kind = data[offset + 4 : offset + 8]
        payload = data[offset + 8 : offset + 8 + size]
        if offset + 12 + size > len(data):
            raise ValueError("truncated PNG chunk")
        if kind == b"IHDR":
            width = int.from_bytes(payload[0:4], "big")
            height = int.from_bytes(payload[4:8], "big")
            bit_depth, color_type, compression, filtering, interlace = payload[8:13]
            if bit_depth != 8 or color_type not in (2, 6) or compression or filtering or interlace:
                raise ValueError("unsupported PNG layout")
        elif kind == b"IDAT":
            compressed.extend(payload)
        elif kind == b"IEND":
            break
        offset += 12 + size
    if not width or not height or bit_depth != 8 or color_type not in (2, 6):
        raise ValueError("missing PNG IHDR")
    channels = 4 if color_type == 6 else 3
    stride = width * channels
    raw = zlib.decompress(bytes(compressed))
    if len(raw) != height * (stride + 1):
        raise ValueError("PNG scanline size mismatch")
    rows: list[bytearray] = []
    cursor = 0
    for _ in range(height):
        filter_type = raw[cursor]
        cursor += 1
        source = raw[cursor : cursor + stride]
        cursor += stride
        row = bytearray(stride)
        previous = rows[-1] if rows else bytearray(stride)
        for index, value in enumerate(source):
            left = row[index - channels] if index >= channels else 0
            up = previous[index]
            upper_left = previous[index - channels] if index >= channels else 0
            if filter_type == 0:
                decoded = value
            elif filter_type == 1:
                decoded = value + left
            elif filter_type == 2:
                decoded = value + up
            elif filter_type == 3:
                decoded = value + ((left + up) // 2)
            elif filter_type == 4:
                estimate = left + up - upper_left
                pa = abs(estimate - left)
                pb = abs(estimate - up)
                pc = abs(estimate - upper_left)
                predictor = left if pa <= pb and pa <= pc else up if pb <= pc else upper_left
                decoded = value + predictor
            else:
                raise ValueError(f"unsupported PNG filter {filter_type}")
            row[index] = decoded & 0xFF
        rows.append(row)
    if color_type == 6:
        return width, height, b"".join(rows)
    rgba = bytearray(width * height * 4)
    output = 0
    for row in rows:
        for index in range(0, len(row), 3):
            rgba[output : output + 4] = bytes((row[index], row[index + 1], row[index + 2], 255))
            output += 4
    return width, height, bytes(rgba)


def extract_th07_png(archive_path: Path | str, anm_entry: str, texture_name: str) -> bytes:
    """Extract one embedded TH07 ANM texture as a normal RGBA PNG."""

    texture = extract_anm_v2_texture(extract_pbg4_entry(archive_path, anm_entry), texture_name)
    return encode_rgba_png(texture.width, texture.height, texture_rgba(texture))


def extract_pe_icon(executable_path: Path | str) -> bytes:
    """Rebuild the first Windows ICO group from a PE32/PE32+ executable.

    Host assembly only needs the public icon container, not a general PE
    resource editor.  The implementation is deliberately small and
    fail-closed, and preserves every image referenced by the selected icon
    group instead of rasterizing or resizing it.
    """

    path = Path(executable_path)
    data = path.read_bytes()
    if len(data) < 0x40 or data[:2] != b"MZ":
        raise ValueError(f"{path}: invalid PE DOS header")
    pe_offset = unpack_from("<I", data, 0x3C)[0]
    if pe_offset + 24 > len(data) or data[pe_offset : pe_offset + 4] != b"PE\0\0":
        raise ValueError(f"{path}: invalid PE signature")
    coff = pe_offset + 4
    section_count = unpack_from("<H", data, coff + 2)[0]
    optional_size = unpack_from("<H", data, coff + 16)[0]
    optional = coff + 20
    if optional + optional_size > len(data):
        raise ValueError(f"{path}: truncated PE optional header")
    magic = unpack_from("<H", data, optional)[0]
    if magic == 0x10B:
        data_directory_offset = 96
        directory_count_offset = 92
    elif magic == 0x20B:
        data_directory_offset = 112
        directory_count_offset = 108
    else:
        raise ValueError(f"{path}: unsupported PE optional-header magic {magic:#x}")
    if optional_size < data_directory_offset + 24:
        raise ValueError(f"{path}: PE optional header has no resource directory")
    directory_count = unpack_from("<I", data, optional + directory_count_offset)[0]
    if directory_count <= 2:
        raise ValueError(f"{path}: PE image has no resource directory")
    resource_rva, resource_size = unpack_from("<II", data, optional + data_directory_offset + 16)
    if not resource_rva or not resource_size:
        raise ValueError(f"{path}: PE image has no resources")

    sections: list[tuple[int, int, int, int]] = []
    section_table = optional + optional_size
    if section_table + section_count * 40 > len(data):
        raise ValueError(f"{path}: truncated PE section table")
    for index in range(section_count):
        entry = section_table + index * 40
        virtual_size, rva, raw_size, raw_offset = unpack_from("<IIII", data, entry + 8)
        sections.append((rva, max(virtual_size, raw_size), raw_offset, raw_size))

    def rva_to_offset(rva: int, size: int) -> int:
        for section_rva, virtual_span, raw_offset, raw_size in sections:
            relative = rva - section_rva
            if 0 <= relative and relative + size <= raw_size and relative < virtual_span:
                offset = raw_offset + relative
                if offset + size <= len(data):
                    return offset
        raise ValueError(f"{path}: resource RVA {rva:#x}+{size:#x} has no raw backing")

    root = rva_to_offset(resource_rva, min(resource_size, 16))
    resources: dict[tuple[int, int, int], bytes] = {}

    def walk(relative: int, ids: tuple[int, ...], depth: int) -> None:
        directory = root + relative
        if directory + 16 > len(data):
            raise ValueError(f"{path}: truncated PE resource directory")
        named_count, id_count = unpack_from("<HH", data, directory + 12)
        count = named_count + id_count
        if directory + 16 + count * 8 > len(data):
            raise ValueError(f"{path}: truncated PE resource entries")
        for index in range(count):
            name_value, child_value = unpack_from("<II", data, directory + 16 + index * 8)
            if name_value & 0x80000000:
                # Icons in Touhou releases use numeric resource identifiers.
                # Named resources are irrelevant to this narrow extractor.
                continue
            resource_id = name_value & 0xFFFF
            child_relative = child_value & 0x7FFFFFFF
            next_ids = (*ids, resource_id)
            if child_value & 0x80000000:
                if depth >= 3:
                    raise ValueError(f"{path}: PE resource tree exceeds expected depth")
                walk(child_relative, next_ids, depth + 1)
                continue
            if len(next_ids) != 3:
                continue
            data_entry = root + child_relative
            if data_entry + 16 > len(data):
                raise ValueError(f"{path}: truncated PE resource data entry")
            payload_rva, payload_size = unpack_from("<II", data, data_entry)
            payload_offset = rva_to_offset(payload_rva, payload_size)
            resources[next_ids] = data[payload_offset : payload_offset + payload_size]

    walk(0, (), 1)
    groups = sorted((key, payload) for key, payload in resources.items() if key[0] == 14)
    if not groups:
        raise ValueError(f"{path}: GROUP_ICON resource not found")
    (group_type, _group_id, group_language), group = groups[0]
    if group_type != 14 or len(group) < 6:
        raise ValueError(f"{path}: invalid GROUP_ICON resource")
    reserved, kind, count = unpack_from("<HHH", group)
    if reserved != 0 or kind != 1 or count <= 0 or len(group) < 6 + count * 14:
        raise ValueError(f"{path}: invalid GROUP_ICON directory")

    images: list[tuple[tuple[int, int, int, int, int, int, int], bytes]] = []
    for index in range(count):
        width, height, colors, entry_reserved, planes, bits, declared_size, icon_id = unpack_from(
            "<BBBBHHIH", group, 6 + index * 14
        )
        candidates = [
            payload
            for (resource_type, resource_id, language), payload in resources.items()
            if resource_type == 3 and resource_id == icon_id and language == group_language
        ]
        if not candidates:
            candidates = [
                payload
                for (resource_type, resource_id, _language), payload in resources.items()
                if resource_type == 3 and resource_id == icon_id
            ]
        if not candidates:
            raise ValueError(f"{path}: ICON resource #{icon_id} referenced by group is missing")
        payload = candidates[0]
        if declared_size != len(payload):
            raise ValueError(f"{path}: ICON resource #{icon_id} size mismatch")
        images.append(((width, height, colors, entry_reserved, planes, bits, declared_size), payload))

    offset = 6 + len(images) * 16
    directory = bytearray(pack("<HHH", 0, 1, len(images)))
    body = bytearray()
    for (width, height, colors, entry_reserved, planes, bits, size), payload in images:
        directory.extend(pack("<BBBBHHII", width, height, colors, entry_reserved, planes, bits, size, offset))
        body.extend(payload)
        offset += size
    return bytes(directory + body)


def _main() -> None:
    parser = ArgumentParser(description="Small pure-Python Touhou archive/ANM utility")
    subcommands = parser.add_subparsers(dest="command", required=True)
    extract = subcommands.add_parser("extract-th07-texture", help="extract one TH07 ANM texture as PNG")
    extract.add_argument("--archive", type=Path, required=True)
    extract.add_argument("--anm", required=True)
    extract.add_argument("--texture", required=True)
    extract.add_argument("--output", type=Path, required=True)
    pbg4 = subcommands.add_parser("extract-pbg4-entry", help="extract one TH07 PBG4 archive entry")
    pbg4.add_argument("--archive", type=Path, required=True)
    pbg4.add_argument("--entry", required=True)
    pbg4.add_argument("--output", type=Path, required=True)
    pbgz = subcommands.add_parser("extract-pbgz-entry", help="extract one TH08 PBGZ archive entry")
    pbgz.add_argument("--archive", type=Path, required=True)
    pbgz.add_argument("--entry", required=True)
    pbgz.add_argument("--output", type=Path, required=True)
    pe_icon = subcommands.add_parser("extract-pe-icon", help="extract the first Windows icon group from a PE executable")
    pe_icon.add_argument("--executable", type=Path, required=True)
    pe_icon.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.command == "extract-th07-texture":
        data = extract_th07_png(args.archive, args.anm, args.texture)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
    elif args.command == "extract-pbg4-entry":
        data = extract_pbg4_entry(args.archive, args.entry)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
    elif args.command == "extract-pbgz-entry":
        data = extract_pbgz_entry(args.archive, args.entry)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
    elif args.command == "extract-pe-icon":
        data = extract_pe_icon(args.executable)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)


if __name__ == "__main__":
    _main()

