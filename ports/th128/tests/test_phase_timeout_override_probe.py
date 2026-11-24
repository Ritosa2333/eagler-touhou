import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class TimeoutOverrideProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original421_changes_only_the_selected_time_target_pointer(self):
        report=importlib.import_module('phase_timeout_override_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        for sample in report['cases']:
            for index,words in enumerate(sample['records']):
                self.assertEqual(words,[0xa5a5a5a5]*3+[sample['stringPointer'] if index==sample['slot'] else 0xa5a5a5a5])


if __name__=='__main__':unittest.main()
