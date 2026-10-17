"""Extract documented source evidence and DATA constants, never executable bytes."""
from pathlib import Path
import argparse, csv, hashlib, json, re, struct
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
EXPECTED = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
SELECTED = [0x41c020,0x41c320,0x41ccf0,0x41d350,0x41d0c0,0x41cc90,
  0x41e050,0x41abe0,0x41ae70,0x41ab30,0x41a1b0,0x419c00,0x41a280,
  0x41c3e0,0x41c730,0x41c1a0,0x41a2c0,0x41d0f0,0x41d0a0,0x41ca70,
  0x41db40,0x41db60,0x41db80,0x41dba0,0x41dbc0,
  0x4117a0,0x411570,0x40ff90,0x40c300,0x41cab0,0x41e030,
  0x412da0,0x41de50,0x41de90,0x41de70,0x41df30,0x41df50,
  0x41ded0,0x41df10,0x41deb0,0x41def0,0x41df70,0x41df90,
  0x41dfb0,0x41dff0,0x41dfd0,0x41e010,0x411700,0x41ca90,0x41cad0,0x41caf0,
  0x401090,0x40aa10,0x418ac0,0x4d8990,0x4d8da0,0x56b3f0,0x4d88c0,
  0x4d9e30,0x4ba8b0,0x40bcf0,0x40bc60,0x40b780,0x4dd840,0x4dbce0,
  0x4ddb20,0x4dbd70,0x4dda30,0x4bfb00,0x445ab0,0x4da1f0,0x4dd490,
  0x41dce0,0x4da120,0x4dda60,0x4193e0,0x4199a0,0x419a50,0x419760,0x4193c0,
  0x41bfc0,0x44bb10,0x4de040,0x4d9210,0x4d9c40,0x4d97f0,0x4d8740,
  0x416d20,0x414820,0x4de1f0,0x4dd600,0x4dc0f0,
  0x4dc510,0x4dc5f0,0x4dc8b0,0x4d9120,0x4dd1c0,0x4d8e90,0x4dd2e0,0x4d8f80,0x4dd400,0x4d9040,0x4dc7c0,0x4daba0,0x4d9ea0,
  0x4da540,0x4ddf80,0x4d9db0,0x4ac0b0,0x4abf30,0x4abb80,0x4abc90,0x4ac1a0,0x4abad0,0x4ac080,0x4abd50,0x4ac0e0,0x4ac170,0x4ac200,0x4ac270,0x451520,0x4abb60,0x4ac260,0x4ac230,0x4abb20,0x44dde0]
def main():
    ap=argparse.ArgumentParser();ap.add_argument('exe',type=Path);args=ap.parse_args()
    data=args.exe.read_bytes()
    assert hashlib.sha256(data).hexdigest()==EXPECTED
    pe=struct.unpack_from('<I',data,0x3c)[0]
    count=struct.unpack_from('<H',data,pe+6)[0]
    sec=pe+24+struct.unpack_from('<H',data,pe+20)[0]
    def offset(va):
        for i in range(count):
            _,rva,n,pos=struct.unpack_from('<IIII',data,sec+i*40+8)
            if rva<=va-0x400000<rva+n:return pos+va-0x400000-rva
        raise ValueError(hex(va))
    def unpack(fmt,va):return struct.unpack_from(fmt,data,offset(va))
    strings={'already_running':0x56cd20,'direct3d_failed':0x56cacc,
      'format_default':0x56cbf8,'missing_texture_cap':0x56cc28,'small_texture':0x56cc78,
      'format_unsupported':0x56ccc8,'device_failed':0x56cba4,'backbuffer_size':0x56cbdc,
      'audio_stop_tag':0x571b14,'snapshot_16bit_unsupported':0x571b2c,
      'archive_open_failed':0x571974,'version_data_missing':0x571944}
    head=['#pragma once','// Recovered read-only DATA constants. No instruction bytes.',
      'namespace th20::source::platform_window::data {']
    for name,va in strings.items():
        start=offset(va);s=data[start:data.index(0,start)]
        head.append('inline constexpr char '+name+'[] = "'+''.join(f'\\x{x:02x}' for x in s)+f'"; // {va:#x}')
    for name,va in [('font_probe_gothic',0x56c820),('font_probe_mincho',0x56c82c)]:
        start=offset(va); units=[]
        while True:
            unit=struct.unpack_from('<H',data,start)[0];start+=2
            if not unit:break
            units.append(unit)
        head.append('inline constexpr wchar_t '+name+'[] = L"'+''.join(f'\\x{x:04x}' for x in units)+'";')
    for name,va,n in [('mode_controls',0x5ae128,10),('mode_scales',0x5ae1d0,10),
       ('navigation_controls',0x5ae1f8,10),('resolution_candidates',0x5ae150,32)]:
        values=unpack('<'+'i'*n,va)
        head.append(f'inline constexpr int {name}[{n}] = '+'{'+','.join(map(str,values))+'};'+f' // {va:#x}')
    for va in [0x56cdac,0x56cda8,0x56cd94,0x56cd90,0x56cda4,0x56cdb0,0x56cdb8,
               0x56d7c0,0x56e0f4,0x571ea0,0x56e0f0,0x56f00c,0x570618,0x570614]:
        head.append(f'inline constexpr float value_{va:08x} = {unpack("<f",va)[0]!r}f;')
    for va in [0x56c468,0x56c470,0x56cd50,0x56cd58,0x56cd60,0x56cd78,0x56cd80,0x56cd88]:
        head.append(f'inline constexpr double value_{va:08x} = {unpack("<d",va)[0]!r};')
    head+=['}',''];(HERE/'data_constants.hpp').write_text('\n'.join(head),encoding='utf-8')
    ranges={}
    with (ROOT/'analysis/ghidra/function_ranges.csv').open(encoding='utf-8-sig') as f:
        for row in csv.DictReader(f):
            entry=int(row['function_entry'],16)
            if entry in SELECTED:ranges.setdefault(entry,[]).append((int(row['range_start'],16),int(row['range_end_inclusive'],16)))
    evid=HERE/'evidence';evid.mkdir(exist_ok=True)
    chunks={va:[] for va in SELECTED}
    with (ROOT/'analysis/binary/disassembly.asm').open(encoding='utf-8') as f:
        for line in f:
            if not re.match(r'^[0-9a-f]{8} ',line):continue
            va=int(line[:8],16)
            for entry,parts in ranges.items():
                if any(a<=va<=b for a,b in parts):chunks[entry].append(line);break
    for entry,lines in chunks.items():(evid/f'{entry:08x}.asm').write_text(''.join(lines),encoding='utf-8')
    report={'specimen_sha256':EXPECTED,'addresses':[f'0x{x:08x}' for x in SELECTED],
      'note':'Selected callees may be inlined by source, not separate recovered functions.'}
    (HERE/'evidence_manifest.json').write_text(json.dumps(report,indent=2)+'\n')
if __name__=='__main__':main()
