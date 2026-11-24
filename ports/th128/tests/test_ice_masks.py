import hashlib
import json
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from th128_assets.formats import FormatError
from th128_assets.ice_masks import read_ice_masks

class IceMaskPreparationTests(unittest.TestCase):
    def test_unpinned_executables_are_rejected(self):
        for payload in (b'',b'MZ'+bytes(1024),bytes(1048577)):
            with self.assertRaises(FormatError):read_ice_masks(payload)

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'local original executable required')
    def test_prepared_masks_match_original_tables_and_existing_generation(self):
        executable=Path(os.environ['TH128_BULLET_EXE']);before=hashlib.sha256(executable.read_bytes()).hexdigest()
        report=read_ice_masks(executable.read_bytes());self.assertEqual(report['sourceExeSha256'],before)
        root=Path(__file__).resolve().parents[1];catalog=json.loads((root/'local/catalog.json').read_text(encoding='utf8'))
        directory=(root/'local'/catalog['variants']['ja']['manifest']).parent
        prepared=json.loads((directory/'animations/ice-masks.json').read_text(encoding='utf8'))
        self.assertEqual(report,prepared);self.assertEqual(sum(len(bytes.fromhex(profile['hex'])) for profile in report['profiles']),1228)
        self.assertEqual(hashlib.sha256(executable.read_bytes()).hexdigest(),before)

if __name__=='__main__':unittest.main()
