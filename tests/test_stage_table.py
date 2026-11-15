import importlib.util
import json
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from research.stage_table import extract_stage_table


class StageTableTests(unittest.TestCase):
    @unittest.skipUnless(importlib.util.find_spec('pefile'),'optional pefile is unavailable')
    def test_original_table_preserves_cross_named_backgrounds(self):
        original=Path('E:/gee/妖精大战争/th128.exe')
        catalog=Path(__file__).resolve().parents[1]/'local/catalog.json'
        if not original.is_file() or not catalog.is_file():
            self.skipTest('optional original content unavailable')
        data=json.loads(catalog.read_text(encoding='utf-8'))
        local=catalog.parent
        manifest=json.loads((local/data['variants']['ja']['manifest']).read_text(encoding='utf-8'))
        result=extract_stage_table(original,{record['name'] for record in manifest['files']})
        self.assertEqual(len(result['stages']),16)
        stage=result['stages'][1]
        self.assertEqual(stage['id'],2)
        self.assertEqual(stage['ecl'],'st_a1_2.ecl')
        self.assertEqual(stage['background'],'stage_b2.std')
        self.assertEqual(stage['message'],'st_a1_2.msg')
        self.assertEqual(result['stages'][-1]['ecl'],'st_ex.ecl')
        self.assertEqual(result['stages'][-1]['music'],['th128_10.wav','th128_11.wav'])


if __name__=='__main__':
    unittest.main()
