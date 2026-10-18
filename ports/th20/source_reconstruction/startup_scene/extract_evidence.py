from pathlib import Path
import argparse,csv,hashlib,json,re,struct
here=Path(__file__).resolve().parent;root=here.parents[1]
expected='a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
selected=[0x4d7ea0,0x4d7ef0,0x4d7fd0,0x4d80b0,0x4d80f0,0x4d8160,0x4d8220,0x4d82c0,
          0x4d8350,0x4d8560,0x4d85a0,0x4d85b0,0x4d85c0,0x4dd730,0x4515e0,0x4a7700,0x425cc0,
          0x50fce0,0x50fc10,0x40bbc0,0x488720,0x41df50,0x4be220,0x4bdd60]
def main():
    ap=argparse.ArgumentParser();ap.add_argument('specimen',type=Path);args=ap.parse_args();data=args.specimen.read_bytes()
    assert hashlib.sha256(data).hexdigest()==expected
    header=struct.unpack_from('<I',data,60)[0];section=header+24+struct.unpack_from('<H',data,header+20)[0]
    def offset(va):
        for index in range(struct.unpack_from('<H',data,header+6)[0]):
            _,rva,size,start=struct.unpack_from('<IIII',data,section+index*40+8)
            if rva<=va-0x400000<rva+size:return start+va-0x400000-rva
        raise ValueError(hex(va))
    lines=['#pragma once','// Read-only original DATA, no machine instructions.','namespace th20::source::startup::data {']
    for name,va in [('audio_format_error',0x57183c),('text_renderer_error',0x571800)]:
        at=offset(va);text=data[at:data.index(0,at)]
        lines.append('inline constexpr char '+name+'[]="'+''.join(f'\\x{b:02x}' for b in text)+'";')
    for name,va in [('text_x',0x56cda8),('text_y',0x570388)]:
        value=struct.unpack_from('<f',data,offset(va))[0];lines.append(f'inline constexpr float {name}={value!r}f;')
    lines.append('}');(here/'data_constants.hpp').write_text('\n'.join(lines)+'\n',encoding='utf-8')
    ranges={}
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
    evidence=here/'evidence';evidence.mkdir(exist_ok=True)
    for va,lines in chunks.items():(evidence/f'{va:08x}.asm').write_text(''.join(lines),encoding='utf-8')
    (here/'evidence_manifest.json').write_text(json.dumps({'specimen_sha256':expected,'addresses':[f'0x{x:08x}' for x in selected]},indent=2)+'\n')
if __name__=='__main__':main()
