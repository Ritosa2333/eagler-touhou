"""Read-only TH12 player evidence; output is never a gameplay golden."""
import pathlib,struct,sys
root=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'tools/python'))
import capstone,pefile
if len(sys.argv)>1:
    pe=pefile.PE(str(root/'th12/th12.exe'))
    cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
    start=int(sys.argv[1],0);size=int(sys.argv[2],0)
    for i in cs.disasm(pe.get_data(start-pe.OPTIONAL_HEADER.ImageBase,size),start):
        print(f'{i.address:08x} {i.mnemonic:9} {i.op_str}')
else:
    for path in sorted((root/'th12-eagler/local/retail').glob('pl*.sht')):
        b=path.read_bytes();groups=struct.unpack_from('<H',b,2)[0]
        print(path.name,'header',struct.unpack_from('<HH7fii',b),'positions')
        print([struct.unpack_from('<2f',b,40+i*8) for i in range(20)])
        for i in range(groups):
            off,metadata=struct.unpack_from('<II',b,0x268+i*8);shots=[]
            while b[off]<128:
                shots.append(struct.unpack_from('<bbh6fbbhhh4I',b,off));off+=52
            print('group',i,hex(metadata),shots)
