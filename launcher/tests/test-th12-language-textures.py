"""Resource-free regression for thcrap sprite-level PNG composition."""
import importlib.util
import struct
import tempfile
import unittest
from pathlib import Path
from PIL import Image

spec = importlib.util.spec_from_file_location('textures', Path(__file__).parents[1] / 'scripts/th12-language-textures.py')
textures = importlib.util.module_from_spec(spec)
spec.loader.exec_module(textures)

class SpritePatchTests(unittest.TestCase):
    def test_empty_sprite_keeps_its_edge_when_transparent_padding_overlaps(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            data = bytearray(160)
            struct.pack_into('<IHH', data, 0, 7, 2, 0)
            struct.pack_into('<HHH', data, 10, 4, 2, 1)
            struct.pack_into('<I', data, 28, 112)
            struct.pack_into('<II', data, 64, 72, 92)
            struct.pack_into('<Iffff', data, 72, 0, 0, 0, 2, 2)
            struct.pack_into('<Iffff', data, 92, 1, 1, 0, 3, 2)
            data[112:116] = b'THTX'
            struct.pack_into('<HHHI', data, 118, 1, 4, 2, 32)
            data[128:160] = bytes([0, 0, 255, 128]) * 8
            (root / 'a.anm').write_bytes(data)
            patch = Image.new('RGBA', (4, 2))
            patch.paste((0, 255, 0, 255), (2, 0, 3, 2))
            patch.save(root / 'patch.png')
            textures.prepare(dict(input=str(root / 'a.anm'), png=str(root / 'patch.png'), out=str(root / 'out.png'), index=0, name='overlap'))
            with Image.open(root / 'out.png') as output:
                self.assertEqual(output.getpixel((1, 0)), (255, 0, 0, 128))
                self.assertEqual(output.getpixel((2, 0)), (0, 255, 0, 255))
                self.assertEqual(output.getpixel((3, 0)), (0, 0, 0, 0))

    def test_empty_sprite_preserved_and_changed_sprite_overwritten(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            data = bytearray(160)
            struct.pack_into('<IHH', data, 0, 7, 2, 0)
            struct.pack_into('<HHH', data, 10, 4, 2, 1)
            struct.pack_into('<I', data, 28, 112)
            struct.pack_into('<II', data, 64, 72, 92)
            struct.pack_into('<Iffff', data, 72, 0, 0, 0, 2, 2)
            struct.pack_into('<Iffff', data, 92, 1, 2, 0, 2, 2)
            data[112:116] = b'THTX'
            struct.pack_into('<HHHI', data, 118, 1, 4, 2, 32)
            data[128:160] = bytes([0, 0, 255, 128]) * 8
            (root / 'a.anm').write_bytes(data)
            patch = Image.new('RGBA', (4, 2))
            patch.paste((0, 255, 0, 255), (2, 0, 3, 2))
            patch.save(root / 'patch.png')
            textures.prepare(dict(input=str(root / 'a.anm'), png=str(root / 'patch.png'), out=str(root / 'out.png'), index=0, name='fixture'))
            with Image.open(root / 'out.png') as output:
                self.assertEqual(output.getpixel((0, 0)), (255, 0, 0, 128))
                self.assertEqual(output.getpixel((2, 0)), (0, 255, 0, 255))
                self.assertEqual(output.getpixel((3, 0)), (0, 0, 0, 0))

if __name__ == '__main__':
    unittest.main()
