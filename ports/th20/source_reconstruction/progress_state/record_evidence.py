"""Document original instruction ranges and hashes; never embed executable bytes."""
from pathlib import Path
import csv, hashlib, json, re

directory=Path(__file__).resolve().parent
root=directory.parents[1]
addresses=[0x50af20,0x50e4a0,0x50e500,0x50e6a0,0x50e540,0x50e6e0,
 0x50ef00,0x486e10,0x50f090,0x4beb60,0x463f20,0x50fc90,0x4bd460,
 0x464100,0x4640a0,0x4640c0,0x4640e0,0x463fb0,0x463fe0,0x464530,
 0x50eb70,0x50eeb0,0x50f5f0,0x50f6b0,0x539550,0x5399a0,0x5399d0,
 0x539060,0x539180,0x5394c0,0x539960,0x539a10,0x4103c0,0x50f3b0,
 0x50fb60,0x50e5d0,0x50e990,0x50adc0,0x50fce0,0x50fc10,0x50f660,
 0x50ab10,0x50ad50,0x50ae50,0x50eb30,0x51c920,0x51bc10,0x4bd610,0x4bd6b0,
 0x51b970,0x51ba10,0x51c7e0,0x51c880,0x51b0c0,0x51b160,0x50fc50,0x488700]
ranges={}
with (root/'analysis/ghidra/function_ranges.csv').open(encoding='utf-8-sig') as file:
 for row in csv.DictReader(file):
  address=int(row['function_entry'],16)
  if address in addresses:ranges.setdefault(address,[]).append((int(row['range_start'],16),int(row['range_end_inclusive'],16)))
chunks={address:[] for address in addresses}
with (root/'analysis/binary/disassembly.asm').open(encoding='utf-8') as file:
 for line in file:
  if not re.match(r'^[0-9a-f]{8} ',line):continue
  address=int(line[:8],16)
  for entry,parts in ranges.items():
   if any(start<=address<=end for start,end in parts):chunks[entry].append(line);break
evidence=directory/'evidence';evidence.mkdir(exist_ok=True)
for address,lines in chunks.items():(evidence/f'{address:08x}.asm').write_text(''.join(lines),encoding='utf-8')
sources=['profile.hpp','records.hpp','records.cpp','spell_defaults.hpp','compression.hpp','compression.cpp',
 'file_codec.hpp','file_codec.cpp','manager.hpp','manager.cpp','menu_choices.cpp','profile_access.cpp','metadata_verify.cpp','oracle/completion_cases.inc','CMakeLists.txt','oracle/compare.cpp','oracle/CMakeLists.txt']
sources += ['../runtime_core/worker.hpp','../runtime_core/worker.cpp','../runtime_state/state.cpp',
 '../archive/archive.hpp','../archive/archive.cpp','../archive/resource_manager.hpp','../archive/resource_manager.cpp',
 '../game_session/session.hpp','../game_session/session.cpp','../platform_services/services.cpp','../stage_completion/progress.hpp','../stage_completion/progress.cpp','../stage_completion/playtime.hpp','../stage_completion/playtime.cpp']
hashes={str((directory/name).resolve().relative_to(root)).replace('\\','/'):hashlib.sha256((directory/name).read_bytes()).hexdigest() for name in sources}
report_path=directory/'cpu_validation.json'
report=json.loads(report_path.read_text(encoding='utf-8'));report['source_hashes']=hashes
report['validation_scope']='Original isolated functions and load/save bodies versus source real worker execution; thread-launch timing is not validated.'
report_path.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
status={'status':'compiled_source_module','full_game_equivalence':False,'original_va':[f'0x{address:08x}' for address in addresses],
 'source_hashes':hashes,'cpu_validation':'cpu_validation.json','unrecovered_game_symbols':[],
 'limitations':['Original thread-launch ABI and arbitrary scheduling interleavings not compared','Malformed original undefined inputs are bounded','Disk-full and allocation-failure paths not equivalence-tested']}
(directory/'module_status.json').write_text(json.dumps(status,indent=2)+'\n',encoding='utf-8')
print(len(addresses),'original evidence entries;',report['passed'],'checks;',report['failed'],'failed')
