"""Inspect one TH08 ANM script from the user's own PBGZ archive."""
from __future__ import annotations
import argparse
import struct
from pathlib import Path

NAMES={-1:'End',1:'Delete',2:'Static',3:'Sprite',4:'Jmp',5:'JmpDec',6:'Pos',7:'Scale',8:'Alpha',9:'Color',10:'FlipX',11:'FlipY',12:'Rotate',13:'AngularVelocity',14:'ScaleGrowth',15:'AlphaTimeLinear',16:'Blend',17:'PosTimeLinear',18:'PosTimeDecel',19:'PosTimeDecel2',20:'Stop',21:'InterruptLabel',23:'StopHide',24:'PosMode',26:'AddU',27:'AddV',28:'Visible',29:'ScaleTimeLinear',32:'PosTime',33:'ColorTime',34:'AlphaTime',35:'RotateTime',36:'ScaleTime',79:'Wait',80:'UScroll',81:'VScroll',82:'BlendMode',84:'Color2',85:'Alpha2',86:'Color2Time',87:'Alpha2Time',89:'ReturnFromInterrupt'}

def crypt(data:bytes,key:int,step:int,block:int,limit:int)->bytes:
    out=bytearray(len(data));size=len(data);untouched=(size%block if size%block<block//4 else 0)+(size&1)
    remaining=size-untouched;cursor=0
    while remaining>0 and limit>0:
        amount=min(block,remaining);linear=0
        for parity in (1,2):
            for pos in range(amount-parity,-1,-2):
                out[cursor+pos]=data[cursor+linear]^key;key=(key+step)&255;linear+=1
        cursor+=amount;remaining-=amount;limit-=amount
    out[cursor:]=data[cursor:];return bytes(out)

class Lzss:
    def __init__(self): self.dictionary=bytearray(8192)
    def decode(self,data:bytes,capacity:int)->bytes:
        cursor=0;head=1;mask=0x80;byte=0;out=bytearray()
        def bit():
            nonlocal cursor,mask,byte
            if mask==0x80: byte=data[cursor] if cursor<len(data) else 0;cursor+=cursor<len(data)
            value=1 if byte&mask else 0;mask>>=1
            if not mask: mask=0x80
            return value
        def bits(n):
            value=0
            for _ in range(n): value=(value<<1)|bit()
            return value
        def put(v):
            nonlocal head
            if len(out)>=capacity: raise ValueError('LZSS expansion overflow')
            out.append(v);self.dictionary[head]=v;head=(head+1)&8191
        while True:
            if bit(): put(bits(8))
            else:
                offset=bits(13)
                if not offset: return bytes(out)
                for i in range(bits(4)+3): put(self.dictionary[(offset+i)&8191])

def unwrap(data:bytes)->bytes:
    if len(data)<4 or data[:3]!=b'edz': return data
    tags=[0x4d,0x54,0x41,0x4a,0x45,0x57,0x2d,0x2a]
    params=[(0x1b,0x37,0x40,0x2800),(0x51,0xe9,0x40,0x3000),(0xc1,0x51,0x1400,0x2000),(0x03,0x19,0x1400,0x7800),(0xab,0xcd,0x200,0x1000),(0x12,0x34,0x400,0x2800),(0x35,0x97,0x80,0x2800),(0x99,0x37,0x400,0x1000)]
    return crypt(data[4:],*params[tags.index(data[3])]) if data[3] in tags else data

def archive_file(path:Path,name:str)->bytes:
    data=path.read_bytes()
    if len(data)>64*1024*1024 or data[:4]!=b'PBGZ': raise ValueError('Not a bounded TH08 PBGZ archive')
    header=crypt(data[4:16],0x1b,0x37,12,0x400);count,offset,unpacked=struct.unpack('<III',header);count-=123456;offset-=345678;unpacked-=567891
    if not 0<count<=100000 or not 16<=offset<len(data) or unpacked>64*1024*1024: raise ValueError('Invalid archive header')
    codec=Lzss();table=codec.decode(crypt(data[offset:],0x3e,0x9b,0x80,0x400),unpacked)
    entries=[];cursor=0
    for _ in range(count):
        end=table.index(0,cursor);entry_name=table[cursor:end].decode('ascii');cursor=end+1
        start,size,unknown=struct.unpack_from('<III',table,cursor);cursor+=12;entries.append([entry_name,start,size,unknown,0])
    for i,e in enumerate(entries): e[4]=(entries[i+1][1] if i+1<len(entries) else offset)-e[1]
    for entry_name,start,size,_,compressed in entries:
        if entry_name.lower()==name.lower():
            decoded=codec.decode(data[start:start+compressed],size)
            if len(decoded)!=size: raise ValueError('Archive payload length mismatch')
            return unwrap(decoded)
    raise FileNotFoundError(name)

def script(data:bytes,index:int):
    base=0;seen=0
    while True:
        if len(data)-base<64: raise ValueError('Truncated ANM')
        ns,nt=struct.unpack_from('<II',data,base);next_entry=struct.unpack_from('<I',data,base+56)[0];span=next_entry or len(data)-base
        for i in range(nt):
            offset=struct.unpack_from('<I',data,base+64+ns*4+i*8+4)[0]
            if seen==index:
                cursor=base+offset
                while True:
                    op,size,time,mask=struct.unpack_from('<hHhH',data,cursor)
                    raw=data[cursor+8:cursor+size];args=[]
                    for j in range(0,len(raw)//4*4,4):
                        u=struct.unpack_from('<I',raw,j)[0];s=struct.unpack_from('<i',raw,j)[0];f=struct.unpack_from('<f',raw,j)[0];args.append((u,s,f))
                    yield op,size,time,mask,args
                    if op==-1:return
                    if size<8:raise ValueError('Invalid ANM instruction')
                    cursor+=size
            seen+=1
        if not next_entry:return
        base+=next_entry

def main():
    p=argparse.ArgumentParser();p.add_argument('archive',type=Path);p.add_argument('anm');p.add_argument('script',type=int);args=p.parse_args()
    data=archive_file(args.archive,args.anm)
    for op,size,time,mask,values in script(data,args.script):
        fields=' '.join(f'a{i}=0x{u:08x}/{s}/{f:g}' for i,(u,s,f) in enumerate(values))
        print(f'time={time:4} op={op:3} {NAMES.get(op,"?"):<18} size={size:2} mask=0x{mask:04x} {fields}')

if __name__=='__main__': main()
