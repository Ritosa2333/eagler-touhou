"""Pinned retail Bomb case dispatch and callback assembly, explicit research only."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import capstone,pefile
exe=TITLE.parent/'th12/th12.exe';pe=pefile.PE(str(exe));cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
targets={0x407010:0x4072d0,0x4072d0:0x4073b0,0x4073b0:0x407580,0x407580:0x407670,
0x407780:0x407950,0x407950:0x4079d0,0x4079d0:0x407a30,0x407a30:0x407ae0,
0x408120:0x4082e0,0x4082e0:0x408510,0x408510:0x408580,0x408580:0x4086b0,0x4086b0:0x4089c0,
0x4089c0:0x408b90,0x408b90:0x408c10,0x408c10:0x408c30,0x408c30:0x408cb0,
0x408cb0:0x408f00,0x408f00:0x409220,0x409220:0x409460,
0x46a5f0:0x46a970,0x46a970:0x46abe0,0x46abe0:0x46ac60,
0x469750:0x469d30,0x469d30:0x469f50}
out=TITLE/'local/reverse/bombs';out.mkdir(parents=True,exist_ok=True)
for a,b in targets.items():
 text='\n'.join(f'{i.address:08x} {i.mnemonic:10} {i.op_str}' for i in cs.disasm(pe.get_data(a-0x400000,b-a),a))+'\n';(out/f'{a:08x}.asm').write_text(text)
profiles=[]
for i in range(12):profiles.append({'index':i,'words':[hex(x) for x in struct.unpack('<6I',pe.get_data(0xaf160+i*24,24))]})
(out/'manifest.json').write_text(json.dumps({'exeSha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'bounds':'Reviewed entry addresses, explicit ranges; function boundaries may contain adjacent helpers','effectProfiles':profiles},indent=2)+'\n');print(out)
