import importlib
import importlib.util
import math
import os
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class Geometry526UpdateProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('geometry526_update_probe'),
                             'bounded original526 update and genuine D3DX helper evidence is missing')
        return importlib.import_module('geometry526_update_probe')

    def test_only_declared_readonly_import_slots_can_transfer_to_original_mapped_code(self):
        module = self.module()
        entry, target, slot = 0x500000, 0x510000, 0x520000
        chunks = [(entry, b'\xff\x15' + struct.pack('<I', slot) + b'\xc3'), (target, b'\xc3')]
        data = [module.datum('import', slot, struct.pack('<I', target), writable=False)]
        rejected = module.emulate(chunks, entry, data)
        self.assertEqual(rejected['stop']['reason'], 'unsupported_transfer')
        result = module.emulate(chunks, entry, data, bindings={entry: (slot, target)})
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['importsExecuted'], [{'instruction': '0x00500000',
                                                     'slot': '0x00520000', 'target': '0x00510000'}])
        with self.assertRaises(ValueError):
            module.emulate(chunks, entry, [module.datum('import', slot, struct.pack('<I', target))],
                           bindings={entry: (slot, target)})
        with self.assertRaises(ValueError):
            module.emulate(chunks, entry, data, bindings={entry: (slot, target + 1)})
        with self.assertRaises(ValueError):
            module.emulate(chunks, entry, data, bindings={entry: (slot, entry)})

    @unittest.skipUnless(os.environ.get('TH128_D3DX_DLL'), 'read-only original32-bit D3DX DLL opt-in')
    def test_actual_sse2_dispatch_setter_and_normalize_return_without_assigned_api_results(self):
        report = self.module().probe_normalize(os.environ['TH128_D3DX_DLL'])
        self.assertEqual(report['dll']['sha256'], report['dll']['sha256After'])
        self.assertEqual(report['setter']['stop']['reason'], 'returned')
        self.assertEqual(report['resolvedTarget'], '0x101a4013')
        self.assertFalse(report['windowsDllLoaded'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']), 6)
        for sample in report['cases']:
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 12)
            result = struct.unpack('<3f', bytes.fromhex(sample['outputAfter']))
            if sum(value * value for value in sample['input']) < 1.4210854715202004e-14:
                self.assertEqual(result, (0, 0, 0))
            else:
                self.assertAlmostEqual(math.sqrt(sum(value * value for value in result)), 1, places=6)

    @unittest.skipUnless(os.environ.get('TH128_D3DX_DLL') and os.environ.get('TH128_BULLET_EXE'),
                         'pinned original EXE and32-bit D3DX DLL opt-in')
    def test_complete_geometry_update_reaches_real_boundary_and_all_strips_copy_current_vertices(self):
        module = self.module()
        report = module.probe(os.environ['TH128_BULLET_EXE'], os.environ['TH128_D3DX_DLL'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertFalse(report['allocationExecuted'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertGreaterEqual(len(report['updateCases']), 4)
        for sample in report['updateCases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041722d'})
            self.assertEqual(sample['copy']['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 0)
            columns, rows = sample['input']['columns'], sample['input']['rows']
            vertices = bytes.fromhex(sample['verticesAfter'])
            strips = bytes.fromhex(sample['stripsAfter'])
            self.assertEqual(len(vertices), columns * rows * 28)
            for column in range(columns - 1):
                for row in range(rows):
                    start = (column * rows + row) * 56
                    self.assertEqual(strips[start:start + 28],
                                     vertices[(column * rows + row) * 28:(column * rows + row + 1) * 28])
                    self.assertEqual(strips[start + 28:start + 56],
                                     vertices[((column + 1) * rows + row) * 28:((column + 1) * rows + row + 1) * 28])


if __name__ == '__main__':
    unittest.main()
