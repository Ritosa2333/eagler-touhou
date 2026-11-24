import hashlib
import json
import tempfile
import unittest
from pathlib import Path
import sys
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))

from build_standalone import LOCALE_FILES, MMDC_V1_HEADER, locale_payload, validate_checkpoint_soundfont
from pf_extract import decode
from stage_locale import (
    CHINESE_COUNTS, CHINESE_LOCALES, CHINESE_PACK, CHINESE_ROWS, CONTINUE_TOKEN,
    build_chinese_pack, chinese_pack_metadata, chinese_records, parse_chinese_pack, record_cells,
    sjis_font_cell, validate_chinese_overlay,
)


class RleTests(unittest.TestCase):
    def test_raw(self):
        self.assertEqual(decode(b'aa\x03', False), b'aa\x03')

    def test_literals(self):
        self.assertEqual(decode(b'abc', True), b'abc')

    def test_repeat(self):
        self.assertEqual(decode(b'aa\x03b', True), b'aaaaab')

    def test_zero_count(self):
        self.assertEqual(decode(b'aa\x00b', True), b'aab')

    def test_chained_repeat(self):
        self.assertEqual(decode(b'aa\x02a\x01b', True), b'aaaaaab')

    def test_truncated_repeat(self):
        with self.assertRaises(ValueError):
            decode(b'aa', True)


class LocaleAuditTests(unittest.TestCase):
    def test_font_slots(self):
        self.assertEqual(sjis_font_cell(bytes.fromhex('8140')), (1, 0x21))
        self.assertEqual(sjis_font_cell(bytes.fromhex('819f')), (2, 0x21))
        self.assertEqual(sjis_font_cell(bytes.fromhex('ea9a')), (0x53, 0x7a))

    def test_record_cells_keep_byte_offsets(self):
        self.assertEqual(list(record_cells(b'A\x81\x40B')), [(0, b'A'), (1, b'\x81\x40'), (3, b'B')])
        with self.assertRaisesRegex(ValueError, 'byte 1'):
            list(record_cells(b'A\x81'))

    def test_extra_missing_last_line_feed_preserves_indices(self):
        ja = (b' ' * 38 + b'\r\n') * 62
        last = b' ' * 34 + b'\x81\x40\x81\x40'
        cn = ja[:-40] + last + b'\r'
        rows = list(chinese_records('STAGE5.TXT', cn, ja))
        self.assertEqual((len(rows), rows[-1]), (62, last))
        with self.assertRaisesRegex(ValueError, 'final record suffix'):
            list(chinese_records('STAGE5.TXT', cn + b'\n', ja))

    def test_ending_trailer_preserves_indices(self):
        ja = (b' ' * 44 + b'\r\n') * 51
        cn = ja + b' '
        self.assertEqual(len(list(chinese_records('END1.TXT', cn, ja))), 51)
        with self.assertRaisesRegex(ValueError, 'trailing byte'):
            list(chinese_records('END1.TXT', cn[:-1] + b'X', ja))


class ChinesePackTests(unittest.TestCase):
    def setUp(self):
        self.variants = {lang: {name: [''] * count for name, count in CHINESE_COUNTS.items()}
                         for lang in CHINESE_LOCALES}
        for lang, text in zip(CHINESE_LOCALES, ('简体中文', '繁體中文'), strict=True):
            self.variants[lang]['STAGE0.TXT'][0] = text
            self.variants[lang]['STAGE3.TXT'][84] = f'{CONTINUE_TOKEN}次'
        self.pack = build_chinese_pack(self.variants)

    def manifest(self, pack=None):
        assets = {} if pack is None else {CHINESE_PACK: chinese_pack_metadata(pack)}
        return json.dumps({'version': 1, 'assets': assets}).encode()

    def test_exact_indices_and_blank_records(self):
        self.assertEqual(parse_chinese_pack(self.pack), self.variants)
        self.assertEqual(CHINESE_ROWS, 1754)
        self.assertEqual(CHINESE_COUNTS['MUSIC.TXT'], 15 * 20)
        self.assertEqual(CHINESE_COUNTS['END3.TXT'], 12)
        validate_chinese_overlay(self.manifest(self.pack), self.pack)
        validate_chinese_overlay(self.manifest(), None)

    def test_pack_rejects_wrong_structure(self):
        for malformed in (b'{"format":"soew-th02-glyph-transcription-draft"}\n', self.pack[:-1], self.pack + b'\n',
                          b'\xef\xbb\xbf' + self.pack, self.pack.replace(b'\n', b'\r\n'),
                          self.pack.replace(b'STAGE0.TXT\t48', b'STAGE0.TXT\t047', 1),
                          self.pack.replace(b'STAGE0.TXT', b'END2.TXT', 1),
                          self.pack.replace(b'1\t\n', b'0\t\n', 1),
                          self.pack.replace(b'locale\tzh-Hant', b'locale\tzh-Hans'),
                          self.pack.split(b'locale\tzh-Hant')[0]):
            with self.subTest(prefix=malformed[:30]), self.assertRaises(ValueError):
                parse_chinese_pack(malformed)

    def test_pack_rejects_bad_unicode_text_and_placeholders(self):
        for text in (b'\xff', b'\xc0\xaf', b'\xed\xa0\x80', b'\x00', b'\t', b'{unknown}', b'{continues}',
                     '\u202e'.encode(), '\U00020000'.encode(), ('漢' * 19).encode()):
            with self.subTest(text=text), self.assertRaises(ValueError):
                parse_chinese_pack(self.pack.replace(b'1\t\n', b'1\t' + text + b'\n', 1))
        for token in (b'', b'{CONTINUES}', b'{continues}{continues}'):
            with self.subTest(token=token), self.assertRaises(ValueError):
                parse_chinese_pack(self.pack.replace(b'{continues}', token, 1))

    def test_manifest_rejects_missing_modified_and_duplicate_entries(self):
        wrong_hash = chinese_pack_metadata(self.pack)['sha256'].encode()
        original = self.manifest(self.pack)
        for manifest, pack in ((self.manifest(), self.pack), (original, None),
                               (original, self.pack.replace('繁'.encode(), '漢'.encode(), 1)),
                               (original.replace(wrong_hash, b'0' * 64), self.pack),
                               (original.replace(b'"version": 1', b'"version": 1, "version": 1'), self.pack),
                               (original.replace(b'"format":', b'"size": 1, "format":'), self.pack),
                               (original.replace(b'chinese-v1.txt', b'Chinese-v1.txt'), self.pack)):
            with self.subTest(manifest=manifest[:70], pack_present=pack is not None), self.assertRaises(ValueError):
                validate_chinese_overlay(manifest, pack)

    def test_packager_accepts_optional_exact_name_only(self):
        with tempfile.TemporaryDirectory() as temporary, patch('build_standalone.is_reparse', return_value=False):
            locale = Path(temporary)
            for folder, names in LOCALE_FILES.items():
                (locale / folder).mkdir()
                for name in names:
                    (locale / folder / name).write_bytes(b'legacy asset')
            manifest = locale / 'manifest.json'
            manifest.write_bytes(self.manifest())
            self.assertEqual(len(locale_payload(locale)), 23)
            sidecar = locale / CHINESE_PACK
            sidecar.write_bytes(self.pack)
            with self.assertRaises(ValueError):
                locale_payload(locale)
            manifest.write_bytes(self.manifest(self.pack))
            self.assertIn((f'content/locale/{CHINESE_PACK}', self.pack), locale_payload(locale))
            sidecar.unlink()
            with self.assertRaises(ValueError):
                locale_payload(locale)
            (locale / CHINESE_PACK.upper()).write_bytes(self.pack)
            with self.assertRaises(RuntimeError):
                locale_payload(locale)


class StandaloneCheckpointTests(unittest.TestCase):
    def test_final_font_matches_every_private_checkpoint(self):
        full, slim = b'full test font', b'slim test font'
        full_sha = hashlib.sha256(full)
        slim_sha = hashlib.sha256(slim)
        full_record = MMDC_V1_HEADER + bytes(32) + full_sha.digest()
        slim_record = MMDC_V1_HEADER + bytes(32) + slim_sha.digest()
        with patch('build_standalone.SOUNDFONT_SHA', full_sha.hexdigest()), \
             patch('build_standalone.SLIM_SOUNDFONT_SHA', slim_sha.hexdigest()):
            validate_checkpoint_soundfont(b'public app', slim)
            validate_checkpoint_soundfont(b'public app', full)
            validate_checkpoint_soundfont(full_record, full)
            validate_checkpoint_soundfont(slim_record, slim)
            for app, font in ((full_record, slim), (slim_record, full), (full_record + slim_record, full)):
                with self.assertRaisesRegex(RuntimeError, 'does not match'):
                    validate_checkpoint_soundfont(app, font)


if __name__ == '__main__':
    unittest.main()
