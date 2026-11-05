"""Read-only TH20 record inspection; never rewrites a replay or infers sync."""
import argparse, hashlib, json, struct
from pathlib import Path

def decrypt(data, key, step, block):
    data=bytearray(data); source=bytes(data); n=len(data)
    remaining=n-(n&1)-(n%block if n%block<block//4 else 0); cursor=0
    while remaining>0:
        count=min(remaining,block); inp=cursor
        for start in (count-1,count-2):
            for offset in range(start,-1,-2):
                data[cursor+offset]=source[inp]^key; inp+=1; key=(key+step)&255
        remaining-=count;cursor+=count
    return data

def decode(data, size):
    dictionary=bytearray(8192); output=bytearray(); bit=0; cursor=1
    def read(count):
        nonlocal bit
        v=0
        for _ in range(count):
            v=(v<<1)|((data[bit//8]>>(7-bit%8))&1 if bit<len(data)*8 else 0);bit+=1
        return v
    def emit(value):
        nonlocal cursor
        output.append(value);dictionary[cursor]=value;cursor=(cursor+1)&8191
        if len(output)>size:raise ValueError('LZSS exceeds declared size')
    while len(output)<size:
        if read(1):emit(read(8))
        else:
            offset=read(13)
            if not offset:break
            for i in range(read(4)+3):emit(dictionary[(offset+i)&8191])
    if len(output)!=size:raise ValueError('Truncated LZSS')
    return output

def inspect(path):
    b=path.read_bytes(); u32=lambda data,o:struct.unpack_from('<I',data,o)[0]
    i32=lambda data,o:struct.unpack_from('<i',data,o)[0]
    if b[:4]!=b't20r' or len(b)<48:raise ValueError('Not TH20 replay')
    packed_size=u32(b,40);unpacked_size=u32(b,44)
    if packed_size>len(b)-48 or unpacked_size>32*1024*1024:raise ValueError('Invalid sizes')
    decoded=decode(decrypt(decrypt(b[48:48+packed_size],0x5c,0xe1,0x400),0x7d,0x3a,0x100),unpacked_size)
    count=u32(decoded,0xd4);offset=256;stages=[]
    for _ in range(count):
        frames=u32(decoded,offset+8);n=u32(decoded,offset+12)
        if offset+0x2a0+n>len(decoded) or frames*6>n:raise ValueError('Invalid stage bounds')
        table=offset+0x70
        stages.append(dict(stage=i32(decoded,offset),seed=u32(decoded,offset+4),frames=frames,
            score=struct.unpack_from('<Q',decoded,table)[0],lives=i32(decoded,table+0xb8),
            power=i32(decoded,table+0x30),specialItems=u32(decoded,table+0x44),overlayMeter=u32(decoded,table+0x4c),
            playerStones=list(struct.unpack_from('<4I',decoded,table+0x1c))))
        offset+=0x2a0+n
    extensions=[];tail=48+packed_size
    while tail+12<=len(b):
        if b[tail:tail+4]!=b'USER':raise ValueError('Invalid USER extension')
        n=u32(b,tail+4)
        if n<12 or tail+n>len(b):raise ValueError('Invalid extension size')
        tag=b[tail+8:tail+12]
        entry={'tagHex':tag.hex(),'size':n}
        if tag==b'PRAC':entry['params']=json.loads(b[tail+12:tail+n].split(b'\0')[0])
        extensions.append(entry);tail+=n
    return dict(path=str(path.resolve()),sha256=hashlib.sha256(b).hexdigest(),
        character=u32(decoded,0xd8),stones=list(struct.unpack_from('<4I',decoded,0xdc)),
        difficulty=i32(decoded,0xf0),finishedStage=i32(decoded,0xf4),
        finalScore=struct.unpack_from('<Q',decoded,0x18)[0],scoreDisplayMultiplier=10,
        stages=stages,extensions=extensions)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('replay',type=Path);p.add_argument('--output',type=Path)
    a=p.parse_args();result=inspect(a.replay);text=json.dumps(result,ensure_ascii=False,indent=2)
    if a.output:a.output.write_text(text+'\n',encoding='utf-8')
    print(text)
