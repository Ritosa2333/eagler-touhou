import json
import sys
import tempfile
import unittest
from unittest.mock import patch
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from th128_assets.formats import FormatError
from th128_assets.pipeline import atomic_json, generation_directory, parse_listing, validate_duplicate_audit, validate_locations, verify_extracted
from th128_assets.pipeline import prepare, snapshot, TOOL_HASHES
from th128_assets.pipeline import PROJECT, validate_report_destination


class PipelineTests(unittest.TestCase):
    def test_analysis_reports_cannot_replace_the_private_catalog(self):
        with self.assertRaises(FormatError):
            validate_report_destination(PROJECT / 'local/catalog.json', [])
        self.assertEqual(validate_report_destination(PROJECT / 'docs/report-test.json', []),
                         (PROJECT / 'docs/report-test.json').resolve())

    def test_real_archive_identical_duplicate_rows_are_audited(self):
        listing = parse_listing('Name Size Stored\nse_cardget.wav 222864 222864\nse_cardget.wav 222864 222864\n')
        self.assertEqual(dict(listing), {'se_cardget.wav': 222864})
        self.assertEqual(listing.duplicates, ['se_cardget.wav'])
        with self.assertRaises(FormatError):
            parse_listing('Name Size Stored\na.anm 1 1\na.anm 2 2\n')

    def test_expansion_limits_count_all_physical_duplicate_records(self):
        with self.assertRaises(FormatError):
            parse_listing('Name Size Stored\n' + 'a.wav 134217728 134217728\n' * 9)
        with self.assertRaises(FormatError):
            parse_listing('Name Size Stored\n' + 'a.wav 1 1\n' * 4097)

    def test_listing_is_strict_and_case_insensitive_unique(self):
        self.assertEqual(parse_listing('Name Size Stored\ntitle.anm 120 50\nst_a1_1.ecl 10 8\n'),
                         {'title.anm': 120, 'st_a1_1.ecl': 10})
        for listing in ('Name Size Stored\n../x 1 1', 'Name Size Stored\na.anm 1 1\nA.anm 2 2',
                        'Name Size Stored\nbroken row', 'Name Size Stored\n', 'Name Size Stored\na 0 0'):
            with self.assertRaises(FormatError):
                parse_listing(listing)

    def test_original_and_output_must_not_overlap(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            original = root / 'original'
            original.mkdir()
            validate_locations(original, root / 'project' / 'local')
            for output in (original, original / 'nested', root):
                with self.assertRaises(FormatError):
                    validate_locations(original, output)

    def test_duplicate_audit_uses_actual_file_lengths_not_native_status(self):
        listing = parse_listing('Name Size Stored\na.wav 4 4\na.wav 4 4\n')
        audit = [{'name': 'a.wav', 'identical': True, 'entries': [
            {'index': 0, 'bytes': 4, 'sha256': 'a' * 64}, {'index': 1, 'bytes': 4, 'sha256': 'a' * 64}]}]
        validate_duplicate_audit(audit, listing)
        audit[0]['entries'][0]['bytes'] = 1
        with self.assertRaises(FormatError):
            validate_duplicate_audit(audit, listing)

    def test_extraction_must_match_listing_exactly(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / 'a.anm').write_bytes(b'1234')
            verify_extracted(root, {'a.anm': 4})
            with self.assertRaises(FormatError):
                verify_extracted(root, {'a.anm': 5})
            (root / 'unexpected').write_bytes(b'x')
            with self.assertRaises(FormatError):
                verify_extracted(root, {'a.anm': 4})

    def test_generation_junction_cannot_redirect_publication(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            output, original = root / 'local', root / 'original'
            output.mkdir()
            original.mkdir()
            with patch.object(Path, 'resolve', autospec=True,
                              side_effect=lambda path: original if path == output / 'generations' else path):
                with self.assertRaises(FormatError):
                    generation_directory(output)
            self.assertFalse((output / 'generations').exists())

    def test_source_reparse_directories_are_rejected_before_descent(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            redirected = root / 'redirected'
            redirected.mkdir()
            (redirected / 'outside.dat').write_bytes(b'external fixture')
            with patch('th128_assets.pipeline.is_reparse_point', create=True,
                       side_effect=lambda path: path.name == 'redirected'):
                with self.assertRaises(FormatError):
                    snapshot(root)

    def test_publication_rechecks_generation_destination_after_conversion(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary).resolve()
            original, tools, output = root / 'original', root / 'tools', root / 'local'
            original.mkdir()
            tools.mkdir()
            for name in TOOL_HASHES:
                (tools / name).write_bytes(b'tool fixture')
            real_generation_directory = generation_directory
            calls = []

            def checked_destination(directory):
                calls.append(directory)
                if len(calls) > 1:
                    raise FormatError('generation junction changed during conversion')
                return real_generation_directory(directory)

            with patch('th128_assets.pipeline.digest', side_effect=lambda path: TOOL_HASHES[path.name]), \
                    patch('th128_assets.pipeline.snapshot', return_value={}), \
                    patch('th128_assets.pipeline.prepare_variant', return_value={'title': 'fixture', 'totals': {}}), \
                    patch('th128_assets.pipeline.generation_directory', side_effect=checked_destination):
                with self.assertRaises(FormatError):
                    prepare(original, tools, output, ['ja'])
            self.assertFalse((output / 'catalog.json').exists())

    def test_atomic_catalog_publication(self):
        with tempfile.TemporaryDirectory() as temporary:
            catalog = Path(temporary) / 'catalog.json'
            atomic_json(catalog, {'variants': {'ja': 'previous'}})
            with self.assertRaises(TypeError):
                atomic_json(catalog, {'variants': object()})
            self.assertEqual(json.loads(catalog.read_text())['variants']['ja'], 'previous')
            atomic_json(catalog, {'variants': {'zh': 'next'}})
            self.assertEqual(json.loads(catalog.read_text())['variants']['zh'], 'next')


if __name__ == '__main__':
    unittest.main()
