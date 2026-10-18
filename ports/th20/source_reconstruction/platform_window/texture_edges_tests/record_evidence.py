from pathlib import Path
import csv,hashlib,json,re
here=Path(__file__).resolve().parent
root=here.parents[2]
selected=[0x451640,0x451700,0x4517c0,0x451880,0x451910]
ranges={}
with (root/'analysis/ghidra/function_ranges.csv').open(encoding='utf-8-sig') as source:
    for row in csv.DictReader(source):
        va=int(row['function_entry'],16)
        if va in selected:ranges.setdefault(va,[]).append((int(row['range_start'],16),int(row['range_end_inclusive'],16)))
chunks={va:[] for va in selected}
with (root/'analysis/binary/disassembly.asm').open(encoding='utf-8') as source:
    for line in source:
        if not re.match(r'^[0-9a-f]{8} ',line):continue
        address=int(line[:8],16)
        for entry,parts in ranges.items():
            if any(begin<=address<=end for begin,end in parts):chunks[entry].append(line);break
evidence=here/'evidence';evidence.mkdir(exist_ok=True)
for va,lines in chunks.items():(evidence/f'{va:08x}.asm').write_text(''.join(lines),encoding='utf-8')
report=json.loads((here/'cpu_validation.json').read_text())
report['source_hashes']={str(path.relative_to(root)).replace('\\','/'):hashlib.sha256(path.read_bytes()).hexdigest()
    for path in [root/'source_reconstruction/sprite_renderer/texture_edges.hpp',root/'source_reconstruction/sprite_renderer/texture_edges.cpp',here/'compare.cpp']}
report['original_addresses']=[f'0x{va:08x}' for va in selected]
report['production_uses_original_machine_code']=False
report['full_game_equivalence_proved']=False
(here/'cpu_validation.json').write_text(json.dumps(report,indent=2)+'\n')
print('Recorded source hashes and documentary assembly for five recovered routines')
