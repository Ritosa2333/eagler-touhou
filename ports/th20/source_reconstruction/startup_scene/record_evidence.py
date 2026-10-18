"""Record documentary disassembly and hashes; never embeds instructions in a target."""
from pathlib import Path
import csv,hashlib,json,re
root=Path(__file__).resolve().parents[2]
groups={
 'game_session': [0x401180,0x422e40,0x422f20,0x423050,0x423320,0x40bbc0,0x4989f0,0x488720,0x41df50,0x464230,0x464250,0x478e80,0x40c300,0x4be220,0x4bdd60,0x4bccc0,0x4bccf0],
 'text_renderer':[0x46a170,0x46a8e0,0x46aba0,0x46a6b0,0x46ac80,0x46aef0,0x46b0d0,0x46a090,0x46a100,0x46c080,0x471060,0x46c930,0x46b6e0,0x46b7f0,0x46b9a0,0x46ba20,0x46bb90,0x46d300,0x4708e0,0x470ed0,0x470300,0x4704a0,0x4704b0,0x4704f0,0x470510,0x470530,0x4a0aa0,0x4790e0,0x416140,0x4162f0,0x401060,0x437950,0x46d170,0x470680,0x470920,0x470180,0x4156e0,0x470620,0x46ca40,0x46d090,0x46ce00,0x4710f0,0x4abf30,0x4ac160,0x470ad0,0x488b00,0x414200,0x415640,0x415120,0x4153d0,0x416360,0x417ab0,0x416420,0x415830,0x4145d0,0x416cf0,0x413c50,0x413be0,0x46c210,0x46b390,0x470b80,0x44ac20,0x44fff0,0x46a1c0,0x470670,0x46c990,0x46cc30,0x46cc80,0x46cd40,0x46cd80,0x46ce90,0x46cf00,0x46cfa0,0x4703c0],
 'startup_scene': [0x4d7ea0,0x4d7ef0,0x4d7fd0,0x4d80f0,0x4d8160,0x4d8220,0x4d82c0,0x4d8350,0x4d8560,0x4d85a0,0x4d85b0,0x4d85c0,0x4dd730,0x4515e0,0x4a7700],
 'startup_scene/named_spawn_tests':[0x450c70,0x450cb0,0x446de0,0x40c6b0,0x44c8a0,0x44c8c0,0x449490],
}
source_lists={
 'game_session':['session.hpp','session.cpp','CMakeLists.txt','cpu_compare.cpp'],
 'text_renderer':['text.hpp','text.cpp','ascii.cpp','line_data.cpp','job_layout.cpp','format.cpp','fps.cpp','bitmap.hpp','bitmap.cpp','raster.hpp','raster.cpp','dynamic.cpp','job_lifecycle.cpp','deferred_queue.hpp','deferred_queue.cpp','CMakeLists.txt','oracle/compare.cpp','oracle/test_environment.hpp','oracle/CMakeLists.txt'],
 'startup_scene':['startup.hpp','startup.cpp','data_constants.hpp','CMakeLists.txt','extract_evidence.py','record_evidence.py'],
 'startup_scene/named_spawn_tests':['../../sprite_renderer/named_spawn.hpp','../../sprite_renderer/named_spawn.cpp','compare.cpp','CMakeLists.txt'],
}
selected=set(sum(groups.values(),[]));ranges={}
with (root/'analysis/ghidra/function_ranges.csv').open(encoding='utf-8-sig') as f:
 for row in csv.DictReader(f):
  va=int(row['function_entry'],16)
  if va in selected:ranges.setdefault(va,[]).append((int(row['range_start'],16),int(row['range_end_inclusive'],16)))
chunks={va:[] for va in selected}
with (root/'analysis/binary/disassembly.asm').open(encoding='utf-8') as f:
 for line in f:
  if not re.match(r'^[0-9a-f]{8} ',line):continue
  address=int(line[:8],16)
  for va,parts in ranges.items():
   if any(a<=address<=b for a,b in parts):chunks[va].append(line);break
for group,addresses in groups.items():
 directory=root/'source_reconstruction'/group;evidence=directory/'evidence';evidence.mkdir(exist_ok=True)
 for va in addresses:(evidence/f'{va:08x}.asm').write_text(''.join(chunks[va]),encoding='utf-8')
 source_hashes={str((directory/name).resolve().relative_to(root)).replace('\\','/'):hashlib.sha256((directory/name).read_bytes()).hexdigest() for name in source_lists[group]}
 report_file=directory/'cpu_validation.json'
 if report_file.exists():
  report=json.loads(report_file.read_text());report['source_hashes']=source_hashes;report_file.write_text(json.dumps(report,indent=2)+'\n')
 state={'status':'compiled_source_module','full_game_equivalence':False,'original_va':[f'0x{x:08x}' for x in addresses],'source_hashes':source_hashes,'cpu_validation':'cpu_validation.json' if report_file.exists() else None}
 (directory/'module_status.json').write_text(json.dumps(state,indent=2)+'\n')
 print(group,len(addresses),'evidence entries',len(source_hashes),'source hashes')
