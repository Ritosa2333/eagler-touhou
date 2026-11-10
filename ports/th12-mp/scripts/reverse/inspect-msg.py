"""Read-only retail MSG instruction dispatch evidence. No candidate dependency."""
import pathlib,sys,hashlib,struct,json
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile,capstone
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest();assert digest=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));data=lambda a,n:pe.get_data(a-0x400000,n)
entries=struct.unpack('<28I',data(0x420d94,112));cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
out=TITLE/'artifacts/reverse/msg';out.mkdir(parents=True,exist_ok=True)
for op,start in enumerate(entries):
 end=min([a for a in entries if a>start]+[0x420ced]);end=min(end,start+1400)
 (out/f'op-{op}.asm').write_text('\n'.join(f'{i.address:08x} {i.mnemonic:10} {i.op_str}' for i in cs.disasm(data(start,end-start),start))+'\n')
(out/'targets.json').write_text(json.dumps({'exeSha256':digest,'entry':'0x41fcc0','targets':{op:hex(a) for op,a in enumerate(entries)}},indent=2)+'\n')
print({op:hex(entries[op]) for op in range(28)})
