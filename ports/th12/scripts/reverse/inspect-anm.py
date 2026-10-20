"""Read-only TH12 ANM dispatch/parameter/geometry evidence, not a player dependency."""
import pathlib,sys,json,struct,hashlib
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile,capstone
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest()
if digest!='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417':raise ValueError('Pinned TH12 1.00b required')
pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase;data=lambda a,n:pe.get_data(a-base,n)
targets=struct.unpack('<112I',data(0x458730,112*4));handlers={op-1:hex(va) for op,va in enumerate(targets)}
out=TITLE/'artifacts/reverse';out.mkdir(parents=True,exist_ok=True)
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
selected=[1,2,3,40,41,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,75,76,77,78,79,80,81,82,83,85,87,88,89,90,91,92,93,94,95,96,97,100,102,103,104,105,110]
lines=[]
for op in selected:
 start=targets[op+1];end=min([v for v in targets if v>start]+[start+256]);end=min(end,start+600)
 lines.append(f'; opcode {op}, {start:#x}..{end:#x}')
 lines.extend(f'{i.address:08x} {i.mnemonic:9} {i.op_str}' for i in cs.disasm(data(start,end-start),start));lines.append('')
(out/'anm-opcodes.asm').write_text('\n'.join(lines),encoding='utf-8')
report={'schema':'th12-anm-static-evidence/1','originalExeSha256':digest,'wholeGameDeterminism':False,'dispatch':'0x45569d..0x4556aa','table':'0x458730','opcodeTargets':handlers}
(out/'anm-opcode-targets.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({op:handlers[op] for op in selected}))
