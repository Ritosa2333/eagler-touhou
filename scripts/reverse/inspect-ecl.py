"""Pinned original ECL VM and movement dispatch evidence; no runtime dependency."""
import hashlib,json,pathlib,struct,sys
TITLE=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(TITLE.parent/'tools/python'))
import pefile,capstone
exe=TITLE.parent/'th12/th12.exe';digest=hashlib.sha256(exe.read_bytes()).hexdigest()
assert digest=='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
pe=pefile.PE(str(exe));base=pe.OPTIONAL_HEADER.ImageBase;data=lambda a,n:pe.get_data(a-base,n)
low_index=data(0x468d30,90);low=struct.unpack('<62I',data(0x468c38,62*4))
high_index=data(0x419890,356);high=struct.unpack('<143I',data(0x419654,143*4))
entries={op:low[low_index[op]] for op in range(90)}
entries.update({op:high[high_index[op-256]] for op in range(256,612)})
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
selected=list(range(40,90))+list(range(256,330))+list(range(400,456))+list(range(500,612))
out=TITLE/'artifacts/reverse/ecl';out.mkdir(parents=True,exist_ok=True)
for op in selected:
    start=entries[op];end=min([a for a in entries.values() if a>start]+[start+500]);end=min(end,start+1200)
    text='\n'.join(f'{i.address:08x} {i.mnemonic:10} {i.op_str}' for i in cs.disasm(data(start,end-start),start))+'\n'
    (out/f'op-{op}.asm').write_text(text)
(out/'targets.json').write_text(json.dumps({'exeSha256':digest,'entries':{op:hex(a) for op,a in entries.items()}},indent=2)+'\n')
print(json.dumps({op:hex(entries[op]) for op in [42,43,44,45,50,51,56,57,86,87,308,309,310,311,434]}))
