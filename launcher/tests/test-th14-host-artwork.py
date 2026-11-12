"""Exercise the default TH14 artwork path and the real preparer CLI."""
from pathlib import Path
from io import BytesIO
from tempfile import TemporaryDirectory
import hashlib
import importlib.util
import json
import subprocess
import sys
import unittest

from PIL import Image

PROJECT = Path(__file__).resolve().parents[1]
SCRIPT = PROJECT / 'scripts/prepare-host-artwork.py'
sys.path.insert(0, str(SCRIPT.parent))
spec = importlib.util.spec_from_file_location('th14_host_artwork', SCRIPT)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)

def picture(format='JPEG', color=(120, 60, 30)):
    output = BytesIO()
    Image.new('RGB', (7, 5), color).save(output, format=format)
    return output.getvalue()

class Th14Artwork(unittest.TestCase):
    def test_default_card_without_retail_reader(self):
        with TemporaryDirectory(prefix='th14-artwork-') as directory:
            root = Path(directory).resolve()
            card = root / 'public/assets/th14-card.jpg'
            card.parent.mkdir(parents=True)
            data = picture()
            card.write_bytes(data)
            previous = module.__file__
            module.__file__ = str(root / 'scripts/prepare-host-artwork.py')
            try:
                report = module.prepare_host_artwork(root / 'output', ['th14'], {})
            finally:
                module.__file__ = previous
            self.assertEqual(report['games'], ['th14'])
            self.assertEqual(len(report['files']), 1)
            entry = report['files'][0]
            self.assertEqual(entry['file'], 'th14-card.jpg')
            self.assertEqual(entry['source'], 'bundled-card')
            self.assertEqual(entry['sha256'], hashlib.sha256(data).hexdigest())
            self.assertEqual((root / 'output/th14-card.jpg').read_bytes(), data)

    def test_real_cli_selects_th14_and_preserves_override(self):
        with TemporaryDirectory(prefix='th14-artwork-cli-') as directory:
            root = Path(directory).resolve()
            override = root / 'override'
            override.mkdir()
            data = picture(color=(20, 100, 180))
            (override / 'th14-card.jpg').write_bytes(data)
            result = subprocess.run([sys.executable, str(SCRIPT), '--games=th14',
                                     '--output=' + str(root / 'output'), '--override-dir=' + str(override)],
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            report = json.loads(result.stdout)
            self.assertEqual(report['files'][0]['source'], 'override')
            self.assertEqual((root / 'output/th14-card.jpg').read_bytes(), data)
        listed = subprocess.run([sys.executable, str(SCRIPT), '--list-games'], capture_output=True, text=True)
        self.assertEqual(listed.returncode, 0, listed.stderr)
        self.assertEqual(json.loads(listed.stdout)['files']['th14'], ['th14-card.jpg'])

    def test_jpeg_validation_rejects_wrong_format_and_corruption(self):
        for data in [b'\xff\xd8\xffbroken', picture(format='PNG')]:
            with self.subTest(data=data[:16]):
                with self.assertRaises(ValueError):
                    module._validate_artwork('th14-card.jpg', data)

if __name__ == '__main__':
    unittest.main()
