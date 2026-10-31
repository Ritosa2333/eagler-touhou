"""Read-only TH12 retail instruction inspector; never rewrites the executable."""
import argparse, pathlib, sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools/python'))
import pefile, capstone
parser=argparse.ArgumentParser()
parser.add_argument('--start',type=lambda s:int(s,0))
parser.add_argument('--size',type=lambda s:int(s,0),default=512)
parser.add_argument('--find',default='')
args=parser.parse_args()
pe=pefile.PE(str(ROOT/'th12/th12.exe'))
cs=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);cs.skipdata=True
if args.start:
    data=pe.get_data(args.start-pe.OPTIONAL_HEADER.ImageBase,args.size)
    for i in cs.disasm(data,args.start):print(f'{i.address:08x} {i.mnemonic:9} {i.op_str}')
else:
    s=pe.sections[0];instructions=list(cs.disasm(s.get_data(),pe.OPTIONAL_HEADER.ImageBase+s.VirtualAddress))
    for n,i in enumerate(instructions):
        if args.find in i.op_str:
            print('---')
            for x in instructions[max(0,n-7):n+10]:print(f'{x.address:08x} {x.mnemonic:9} {x.op_str}')
