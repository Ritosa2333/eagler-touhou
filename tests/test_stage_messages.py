import importlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import unittest

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))


class StageMessageTests(unittest.TestCase):
    def parser(self):
        return importlib.import_module('th128_assets.messages').parse_stage_messages

    def test_xor_stream_and_original_entry_words_decode_without_changing_raw_bytes(self):
        text='Test'.encode('ascii')+b'\0\0\0\0';key=0x77;step=7;encrypted=[]
        for value in text:
            encrypted.append(value^key);key=(key+step)&255;step=(step+16)&255
        payload=struct.pack('<IIIHBB',1,12,256,0,17,8)+bytes(encrypted)+struct.pack('<HBB',4,0,0)
        result=self.parser()(payload)
        self.assertEqual(result['entries'][0]['flags'],256)
        self.assertEqual(result['entries'][0]['commands'][0]['text'],'Test')
        self.assertEqual(result['entries'][0]['commands'][0]['raw'],bytes(encrypted).hex())
        self.assertEqual(result['entries'][0]['commands'][-1]['opcode'],0)

    def test_all_actual_route_messages_have_closed_entries_and_supported_command_shapes(self):
        catalog=json.loads((ROOT/'local/catalog.json').read_text(encoding='utf8'))
        for variant,record in catalog['variants'].items():
            directory=(ROOT/'local'/record['manifest']).parent
            for file in sorted((directory/'raw').glob('st_*.msg')):
                with self.subTest(variant=variant,name=file.name):
                    parsed=self.parser()(file.read_bytes(),encoding='cp932' if variant=='ja' else 'gbk')
                    self.assertGreater(len(parsed['entries']),0)
                    for entry in parsed['entries']:
                        self.assertEqual(entry['commands'][-1]['opcode'],0)
                        self.assertGreater(len(entry['commands']),1)
                        self.assertTrue(any(command['opcode']==17 for command in entry['commands'])
                            or all(command['opcode'] in (0,10,21,22) for command in entry['commands']))

    def test_truncation_offsets_and_oversized_strings_reject_before_reading_outside_source(self):
        from th128_assets.formats import FormatError
        parse=self.parser()
        for value in (b'',bytes(4),struct.pack('<III',1,400,256),struct.pack('<IIIHBB',1,12,256,0,17,250)):
            with self.subTest(value=value.hex()),self.assertRaises(FormatError):parse(value)

    def test_all_route_dialogue_text_matches_independent_pinned_thtk_dump(self):
        tool=ROOT.parents[1]/'work/tools/thtk-12/thtk-bin-12/thmsg.exe'
        self.assertTrue(tool.is_file())
        temporary=ROOT.parents[1]/'work/message-tool-checks';temporary.mkdir(exist_ok=True)
        catalog=json.loads((ROOT/'local/catalog.json').read_text(encoding='utf8'))
        for variant,record in catalog['variants'].items():
            directory=(ROOT/'local'/record['manifest']).parent;encoding='cp932' if variant=='ja' else 'gbk'
            for file in sorted((directory/'raw').glob('st_*.msg')):
                with self.subTest(variant=variant,name=file.name):
                    target=temporary/f'{variant}-{file.stem}.txt'
                    dumped=subprocess.run([str(tool),'-d128',str(file),str(target)],capture_output=True,check=False)
                    self.assertEqual(dumped.returncode,0,dumped.stderr)
                    expected=[line[4:].decode(encoding) for line in target.read_bytes().splitlines() if line.startswith(b'\t17;')]
                    parsed=self.parser()(file.read_bytes(),encoding=encoding)
                    actual=[command['text'] for entry in parsed['entries'] for command in entry['commands'] if command['opcode']==17]
                    self.assertEqual(actual,expected)


if __name__=='__main__':unittest.main()
