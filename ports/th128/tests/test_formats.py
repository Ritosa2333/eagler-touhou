import struct
import sys
import unittest
import zlib
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from th128_assets.formats import (
    FormatError, decode_texture, encode_png, parse_anm, parse_ecl,
    parse_music, validate_archive_name,
)


def anm_fixture():
    binary = bytearray(160)
    struct.pack_into('<IHH', binary, 0, 7, 1, 1)
    struct.pack_into('<HHH', binary, 10, 1, 1, 1)
    struct.pack_into('<I', binary, 16, 96)
    struct.pack_into('<I', binary, 28, 140)
    struct.pack_into('<H', binary, 32, 1)
    struct.pack_into('<I', binary, 64, 76)
    struct.pack_into('<iI', binary, 68, 12, 112)
    struct.pack_into('<Iffff', binary, 76, 17, 0, 0, 1, 1)
    binary[96:108] = b'title/a.png\0'
    struct.pack_into('<HHhHi', binary, 112, 3, 12, 0, 0, 17)
    struct.pack_into('<HHhH', binary, 124, 65535, 0, 0, 0)
    struct.pack_into('<4sHHHHI', binary, 140, b'THTX', 0, 1, 1, 1, 4)
    binary[156:160] = bytes([10, 20, 30, 255])
    return bytes(binary)


def ecl_fixture():
    binary = bytearray(80)
    struct.pack_into('<4sHHI', binary, 0, b'SCPT', 1, 0, 36)
    struct.pack_into('<I', binary, 16, 1)
    struct.pack_into('<I', binary, 36, 48)
    binary[40:45] = b'Main\0'
    struct.pack_into('<4sIII', binary, 48, b'ECLH', 0, 0, 0)
    struct.pack_into('<iHHHBBI', binary, 64, 0, 1, 16, 0, 255, 0, 0)
    return bytes(binary)


class FormatTests(unittest.TestCase):
    def test_archive_names(self):
        self.assertEqual(validate_archive_name('st_a1_1.ecl'), 'st_a1_1.ecl')
        for name in ('../x', '..\\x', 'C:x', '/x', 'CON', 'nul.txt', 'x. ', 'a/b', 'x:y', '', 'x\n'):
            with self.subTest(name=name), self.assertRaises(FormatError):
                validate_archive_name(name)

    def test_texture_channels(self):
        self.assertEqual(decode_texture(1, 1, 1, bytes([3, 2, 1, 4])), bytes([1, 2, 3, 4]))
        self.assertEqual(decode_texture(5, 1, 1, struct.pack('<H', 0xF123)), bytes([17, 34, 51, 255]))
        self.assertEqual(decode_texture(3, 1, 1, struct.pack('<H', 0xF800)), bytes([255, 0, 0, 255]))
        for arguments in ((1, 2, 2, b'x'), (99, 1, 1, b'xxxx'), (1, 99999, 99999, b'')):
            with self.assertRaises(FormatError):
                decode_texture(*arguments)

    def test_png(self):
        image = encode_png(1, 1, bytes([1, 2, 3, 4]))
        self.assertTrue(image.startswith(b'\x89PNG\r\n\x1a\n'))
        position = image.index(b'IDAT')
        length = struct.unpack_from('>I', image, position - 4)[0]
        self.assertEqual(zlib.decompress(image[position + 4:position + 4 + length]), bytes([0, 1, 2, 3, 4]))

    def test_anm_indexes_and_keeps_raw_commands(self):
        archive = parse_anm(anm_fixture())
        self.assertEqual(archive['version'], 7)
        self.assertEqual(archive['entries'][0]['name'], 'title/a.png')
        self.assertEqual(archive['sprites'][0]['sourceId'], 17)
        self.assertEqual(archive['scripts'][0]['sourceId'], 12)
        self.assertEqual(archive['scripts'][0]['commands'][0]['opcode'], 3)
        self.assertEqual(archive['scripts'][0]['commands'][0]['raw'], '11000000')

    def test_anm_rejects_truncation_and_invalid_offsets(self):
        for binary in (anm_fixture()[:155], b'', b'bad!' * 40):
            with self.assertRaises(FormatError):
                parse_anm(binary)
        damaged = bytearray(anm_fixture())
        struct.pack_into('<I', damaged, 36, 4)
        with self.assertRaises(FormatError):
            parse_anm(damaged)

    def test_aliased_anm_script_bodies_are_rejected_before_expansion(self):
        binary = bytearray(116)
        struct.pack_into('<IHH', binary, 0, 7, 0, 2)
        struct.pack_into('<I', binary, 16, 80)
        struct.pack_into('<iIiI', binary, 64, 0, 96, 1, 96)
        binary[80:86] = b'a.anm\0'
        struct.pack_into('<HHhHi', binary, 96, 3, 12, 0, 0, 17)
        struct.pack_into('<HHhH', binary, 108, 65535, 0, 0, 0)
        with self.assertRaises(FormatError):
            parse_anm(bytes(binary))

    def test_anm_has_an_aggregate_decoded_command_budget(self):
        with self.assertRaises(FormatError):
            parse_anm(anm_fixture(), max_commands=0)
        damaged = bytearray(anm_fixture())
        struct.pack_into('<H', damaged, 114, 1)
        with self.assertRaises(FormatError):
            parse_anm(damaged)

    def test_ecl_indexes_without_claiming_semantics(self):
        program = parse_ecl(ecl_fixture())
        self.assertEqual(program['subroutines'][0]['name'], 'Main')
        self.assertEqual(program['subroutines'][0]['commands'][0]['opcode'], 1)
        self.assertEqual(program['opcodeHistogram'], {'1': 1})
        with self.assertRaises(FormatError):
            parse_ecl(ecl_fixture()[:-1])

    def test_ecl_declared_include_offset_is_validated(self):
        binary = bytearray(ecl_fixture())
        struct.pack_into('<I', binary, 8, 0xFFFFFFFF)
        with self.assertRaises(FormatError):
            parse_ecl(bytes(binary))

    def test_music_metadata_and_bounds(self):
        record = struct.pack('<16sIIIIHHIIHHI', b'th128_00.wav', 16, 24, 4, 16, 1, 2, 44100, 176400, 4, 16, 0)
        tracks = parse_music(record + bytes(17), 32)
        self.assertEqual(tracks[0]['offset'], 16)
        self.assertEqual(tracks[0]['loopStartBytes'], 4)
        self.assertEqual(tracks[0]['pcmBytes'], 16)
        with self.assertRaises(FormatError):
            parse_music(record, 31)
        with self.assertRaises(FormatError):
            parse_music(record + b'x', 32)


if __name__ == '__main__':
    unittest.main()
