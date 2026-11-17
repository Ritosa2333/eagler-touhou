"""Decode the title artwork using uth05win's DAT and Pi file algorithms."""
from pathlib import Path
import argparse,struct
from PIL import Image

def archive_entry(path,name):
    data=path.read_bytes();size=struct.unpack_from('<H',data)[0];key=data[6]
    if size>len(data)-16 or size%32:raise ValueError('Invalid DAT table')
    table=bytearray(data[16:16+size])
    for i,value in enumerate(table):table[i]=value^key;key=(key-table[i])&255
    for at in range(0,size,32):
        entry=table[at:at+32]
        if not entry[0]:break
        if entry[3:16].split(b'\0')[0].decode('ascii')!=name:continue
        mode=struct.unpack_from('<H',entry)[0];length=struct.unpack_from('<H',entry,16)[0];offset=struct.unpack_from('<I',entry,20)[0]
        if offset+length>len(data):raise ValueError('Invalid DAT entry')
        raw=bytes(c^entry[2] for c in data[offset:offset+length])
        if mode==0xf388:return raw
        if mode!=0x9595:raise ValueError('Unsupported DAT compression')
        result=bytearray();i=0;last=None
        while i<len(raw):
            value=raw[i];i+=1
            if value!=last or i==len(raw) or last is None:result.append(value);last=value
            else:result.extend([value]*(raw[i]+1));i+=1
            if len(result)>2000000:raise ValueError('DAT entry exceeds limit')
        return bytes(result)
    raise ValueError('Missing DAT entry: '+name)

def pi_image(data):
    if data[:2]!=b'Pi':raise ValueError('Invalid Pi image')
    at=data.index(26);at=data.index(0,at)+1;palette_present=not(data[at]&128);at+=3
    if data[at]!=4:raise ValueError('Pi image must have 16 colors')
    at+=5;at+=2+int.from_bytes(data[at:at+2],'big')
    width,height=struct.unpack_from('>HH',data,at);at+=4
    if width<2 or height<1 or width*height>1024*1024 or not palette_present:raise ValueError('Invalid title dimensions or palette')
    palette=[(v>>4)*17 for v in data[at:at+48]];at+=48
    stream=data[at:];bit_at=0
    def bits(n=1):
        nonlocal bit_at
        value=0
        for _ in range(n):
            if bit_at>=len(stream)*8:raise ValueError('Truncated Pi stream')
            value=value*2+((stream[bit_at//8]>>(7-bit_at%8))&1);bit_at+=1
        return value
    table=[[(i-j+16)%16 for j in range(16)] for i in range(16)]
    def color(last):
        index=bits(1) if bits() else 2+bits(1) if not bits() else 4+bits(2) if not bits() else 8+bits(3)
        value=table[last].pop(index);table[last].insert(0,value);return value
    def length():
        if not bits():return 1
        if not bits():return 2+bits()
        if not bits():return 4+bits(2)
        if not bits():return 8+bits(3)
        base,n=16,4
        while bits():
            base*=2;n+=1
            if n>24:raise ValueError('Invalid Pi run')
        return base+bits(n)
    end=width*(height+2);out=bytearray(end);cursor=width*2;last=color(0);first=last;second=color(last);last=second
    out[:cursor]=bytes([first,second])*width;previous=6
    while cursor<end:
        position=bits(2)
        if position==3:position+=bits()
        if position==previous:
            previous=6
            while True:
                first=color(last);last=first;second=color(last);last=second
                if cursor+2>end:raise ValueError('Invalid Pi literal')
                out[cursor:cursor+2]=bytes([first,second]);cursor+=2
                if cursor>=end or not bits():break
        else:
            count=length()*2;back=(-4 if out[cursor-1]!=out[cursor-2] else -2) if position==0 else [-1,-width,-width*2,-width+1,-width-1][position]
            if cursor+back<0 or cursor+count>end:raise ValueError('Invalid Pi backreference')
            for i in range(count):out[cursor+i]=out[cursor+back+i]
            cursor+=count;last=out[cursor-1];previous=position
    image=Image.frombytes('P',(width,height),bytes(out[width*2:]));image.putpalette(palette+[0]*(768-len(palette)));return image.convert('RGB')

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--archive',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);args=parser.parse_args()
    image=pi_image(archive_entry(args.archive,'OP1.PI'));args.output.parent.mkdir(parents=True,exist_ok=True)
    image.save(args.output,'WEBP',quality=55,method=6)
